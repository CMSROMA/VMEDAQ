/* #define WaveDump_Release        "3.5.3_20130419" */
/* #define WaveDump_Release_Date   "Apr 2013" */
/* #define DBG_TIME */

#include "V1742_lib.h"
#include "vme_bridge.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "X742CorrectionRoutines.h"

#define DEFAULT_CONFIG_FILE  "/home/cmsdaq/DAQ/VMEDAQ/V1742_config.txt"

#define MAX_CH  64          /* max. number of channels */
#define MAX_SET 8           /* max. number of independent settings */

#define MAX_GW  1000        /* max. number of generic write commads */


#define VME_INTERRUPT_LEVEL      1
#define VME_INTERRUPT_STATUS_ID  0xAAAA
#define INTERRUPT_MODE           CAEN_DGTZ_IRQ_MODE_ROAK
#define INTERRUPT_TIMEOUT        200  // ms

        
/* ###########################################################################
   Typedefs
   ###########################################################################
*/

typedef struct WaveDumpConfig_t {
  int LinkType;
  int LinkNum;
  int ConetNode;
  uint32_t BaseAddress;
  int Nch;
  int Nbit;
  float Ts;
  int NumEvents;
  int RecordLength;
  unsigned int PostTrigger;
  int InterruptNumEvents; 
  int TestPattern;
  int DesMode;
  int TriggerEdge;
  int FPIOtype;

  CAEN_DGTZ_TriggerMode_t ExtTriggerMode;

  uint8_t EnableMask;

  CAEN_DGTZ_TriggerMode_t ChannelTriggerMode[MAX_SET];

  uint32_t DCoffset[MAX_SET];
  int32_t  DCoffsetGrpCh[MAX_SET][MAX_SET];
  uint32_t Threshold[MAX_SET];
  uint8_t GroupTrgEnableMask[MAX_SET];
  
  uint32_t FTDCoffset[MAX_SET];
  uint32_t FTThreshold[MAX_SET];

  CAEN_DGTZ_TriggerMode_t	FastTriggerMode;
  uint32_t	 FastTriggerEnabled;

  int GWn;
  uint32_t GWaddr[MAX_GW];
  uint32_t GWdata[MAX_GW];

  /* OUTFILE_FLAGS OutFileFlags; */
  
  int useCorrections;
  
} WaveDumpConfig_t;


/* ###########################################################################
   Statics
   ###########################################################################
*/

static WaveDumpConfig_t  WDcfg;

/* static int v1742_handle=0; */

static CAEN_DGTZ_BoardInfo_t       BoardInfo;

static DataCorrection_t Table_gr0;
static DataCorrection_t Table_gr1;

/* ###########################################################################
   Error messages
   ###########################################################################
*/

typedef enum  {
  ERR_NONE= 0,
  ERR_CONF_FILE_NOT_FOUND,
  ERR_DGZ_OPEN,
  ERR_BOARD_INFO_READ,
  ERR_INVALID_BOARD_TYPE,
  ERR_DGZ_PROGRAM,
  ERR_MALLOC,
  ERR_RESTART,
  ERR_INTERRUPT,
  ERR_READOUT,
  ERR_EVENT_BUILD,
  ERR_HISTO_MALLOC,
  ERR_UNHANDLED_BOARD,
  ERR_MISMATCH_EVENTS,
  ERR_FREE_BUFFER,
  /* ERR_OUTFILE_WRITE, */

  ERR_DUMMY_LAST,
} ERROR_CODES;


/* ###########################################################################
 *  Macros
 *  ########################################################################### */

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

/* ###########################################################################
 *  Functions
 *  ########################################################################### */
/*! \fn      static long get_time()
 *   \brief   Get time in milliseconds
 *
 *   \return  time in msec
 */
static long get_time()
{
    long time_ms;
    struct timeval t1;
    struct timezone tz;
    gettimeofday(&t1, &tz);
    time_ms = (t1.tv_sec) * 1000 + t1.tv_usec / 1000;
    return time_ms;
}


/*! \fn      int GetMoreBoardInfo(CAEN_DGTZ_BoardInfo_t BoardInfo,  WaveDumpConfig_t *WDcfg)
*   \brief   calculate num of channels, num of bit and sampl period according to the board type
*
*   \param   BoardInfo   Board Type
*   \param   WDcfg       pointer to the config. struct
*   \return  0 = Success; -1 = unknown board type
*/
int GetMoreBoardInfo(int handle)
{
  CAEN_DGTZ_DRS4Frequency_t freq;
  int ret;
  switch(BoardInfo.FamilyCode) {
  case CAEN_DGTZ_XX724_FAMILY_CODE: WDcfg.Nbit = 14; WDcfg.Ts = 10.0; break;
  case CAEN_DGTZ_XX720_FAMILY_CODE: WDcfg.Nbit = 12; WDcfg.Ts = 4.0;  break;
  case CAEN_DGTZ_XX721_FAMILY_CODE: WDcfg.Nbit =  8; WDcfg.Ts = 2.0;  break;
  case CAEN_DGTZ_XX731_FAMILY_CODE: WDcfg.Nbit =  8; WDcfg.Ts = 2.0;  break;
  case CAEN_DGTZ_XX751_FAMILY_CODE: WDcfg.Nbit = 10; WDcfg.Ts = 1.0;  break;
  case CAEN_DGTZ_XX761_FAMILY_CODE: WDcfg.Nbit = 10; WDcfg.Ts = 0.25;  break;
  case CAEN_DGTZ_XX740_FAMILY_CODE: WDcfg.Nbit = 12; WDcfg.Ts = 16.0; break;
  case CAEN_DGTZ_XX742_FAMILY_CODE: 
    WDcfg.Nbit = 12; 
    if ((ret = CAEN_DGTZ_GetDRS4SamplingFrequency(handle, &freq)) != CAEN_DGTZ_Success) return CAEN_DGTZ_CommError;
    switch (freq) {
    case CAEN_DGTZ_DRS4_1GHz:
      WDcfg.Ts = 1.0;
      break;
    case CAEN_DGTZ_DRS4_2_5GHz:
      WDcfg.Ts = (float)0.4;
      break;
    case CAEN_DGTZ_DRS4_5GHz:
      WDcfg.Ts = (float)0.2;
      break;
    }
    break;
  default: return -1;
  }
  if (((BoardInfo.FamilyCode == CAEN_DGTZ_XX751_FAMILY_CODE) ||
       (BoardInfo.FamilyCode == CAEN_DGTZ_XX731_FAMILY_CODE) ) && WDcfg.DesMode)
    WDcfg.Ts /= 2;
	
  switch(BoardInfo.FamilyCode) {
  case CAEN_DGTZ_XX724_FAMILY_CODE:
  case CAEN_DGTZ_XX720_FAMILY_CODE:
  case CAEN_DGTZ_XX721_FAMILY_CODE:
  case CAEN_DGTZ_XX751_FAMILY_CODE:
  case CAEN_DGTZ_XX761_FAMILY_CODE:
  case CAEN_DGTZ_XX731_FAMILY_CODE:
    switch(BoardInfo.FormFactor) {
    case CAEN_DGTZ_VME64_FORM_FACTOR:
    case CAEN_DGTZ_VME64X_FORM_FACTOR:
      WDcfg.Nch = 8;
      break;
    case CAEN_DGTZ_DESKTOP_FORM_FACTOR:
    case CAEN_DGTZ_NIM_FORM_FACTOR:
      WDcfg.Nch = 4;
      break;
    }
    break;
  case CAEN_DGTZ_XX740_FAMILY_CODE:
    switch( BoardInfo.FormFactor) {
    case CAEN_DGTZ_VME64_FORM_FACTOR:
    case CAEN_DGTZ_VME64X_FORM_FACTOR:
      WDcfg.Nch = 64;
      break;
    case CAEN_DGTZ_DESKTOP_FORM_FACTOR:
    case CAEN_DGTZ_NIM_FORM_FACTOR:
      WDcfg.Nch = 32;
      break;
    }
    break;
  case CAEN_DGTZ_XX742_FAMILY_CODE:
    switch( BoardInfo.FormFactor) {
    case CAEN_DGTZ_VME64_FORM_FACTOR:
    case CAEN_DGTZ_VME64X_FORM_FACTOR:
      WDcfg.Nch = 36;
      break;
    case CAEN_DGTZ_DESKTOP_FORM_FACTOR:
    case CAEN_DGTZ_NIM_FORM_FACTOR:
      WDcfg.Nch = 16;
      break;
    }
    break;
  default:
    return -1;
  }
  return 0;
}


/*! \fn      int ProgramDigitizer(int handle, WaveDumpConfig_t WDcfg)
 *   \brief   configure the digitizer according to the parameters read from
 *            the cofiguration file and saved in the WDcfg data structure
 *
 *   \param   handle   Digitizer handle
 *   \param   WDcfg:   WaveDumpConfig data structure
 *   \return  0 = Success; negative numbers are error codes
 */
int ProgramDigitizer(int handle)
{
  int i,j, ret = 0;

  /* reset the digitizer */
  ret |= CAEN_DGTZ_Reset(handle);
  if (ret != 0) {
    printf("Error: Unable to reset digitizer.\nPlease reset digitizer manually then restart the program\n");
    return -1;
  }
  /* execute generic write commands */
  for(i=0; i<WDcfg.GWn; i++)
    ret |= CAEN_DGTZ_WriteRegister(handle, WDcfg.GWaddr[i], WDcfg.GWdata[i]);

  // Set the waveform test bit for debugging
  if (WDcfg.TestPattern)
    ret |= CAEN_DGTZ_WriteRegister(handle, CAEN_DGTZ_BROAD_CH_CONFIGBIT_SET_ADD, 1<<3);
  // custom setting for X742 boards
  if (BoardInfo.FamilyCode == CAEN_DGTZ_XX742_FAMILY_CODE) {
    ret |= CAEN_DGTZ_SetFastTriggerDigitizing(handle,(CAEN_DGTZ_EnaDis_t)WDcfg.FastTriggerEnabled);
    ret |= CAEN_DGTZ_SetFastTriggerMode(handle,(CAEN_DGTZ_TriggerMode_t)WDcfg.FastTriggerMode);
  }
  if ((BoardInfo.FamilyCode == CAEN_DGTZ_XX751_FAMILY_CODE) || (BoardInfo.FamilyCode == CAEN_DGTZ_XX731_FAMILY_CODE)) {
    ret |= CAEN_DGTZ_SetDESMode(handle, ( CAEN_DGTZ_EnaDis_t) WDcfg.DesMode);
  }
  ret |= CAEN_DGTZ_SetRecordLength(handle, WDcfg.RecordLength);
  ret |= CAEN_DGTZ_SetPostTriggerSize(handle, (uint32_t) WDcfg.PostTrigger);
  if(BoardInfo.FamilyCode != CAEN_DGTZ_XX742_FAMILY_CODE)
    ret |= CAEN_DGTZ_GetPostTriggerSize(handle, &WDcfg.PostTrigger);
  ret |= CAEN_DGTZ_SetIOLevel(handle, (CAEN_DGTZ_IOLevel_t) WDcfg.FPIOtype);
  if( WDcfg.InterruptNumEvents > 0) {
      // Interrupt handling
      if( ret |= CAEN_DGTZ_SetInterruptConfig( handle, CAEN_DGTZ_ENABLE,
                                               VME_INTERRUPT_LEVEL, VME_INTERRUPT_STATUS_ID,
                                               WDcfg.InterruptNumEvents, INTERRUPT_MODE)!= CAEN_DGTZ_Success) {
          printf( "\nError configuring interrupts. Interrupts disabled\n\n");
          WDcfg.InterruptNumEvents = 0;
      }
      printf ("Interrupt enabled\n");
  }
  ret |= CAEN_DGTZ_SetMaxNumEventsBLT(handle, WDcfg.NumEvents);
  ret |= CAEN_DGTZ_SetAcquisitionMode(handle, CAEN_DGTZ_SW_CONTROLLED);
  ret |= CAEN_DGTZ_SetExtTriggerInputMode(handle, WDcfg.ExtTriggerMode);

  if ((BoardInfo.FamilyCode == CAEN_DGTZ_XX740_FAMILY_CODE) || (BoardInfo.FamilyCode == CAEN_DGTZ_XX742_FAMILY_CODE)){
    ret |= CAEN_DGTZ_SetGroupEnableMask(handle, WDcfg.EnableMask);
    for(i=0; i<(WDcfg.Nch/8); i++) {
      if (WDcfg.EnableMask & (1<<i)) {
	if (BoardInfo.FamilyCode == CAEN_DGTZ_XX742_FAMILY_CODE) {
	  for(j=0; j<8; j++) {
	    if (WDcfg.DCoffsetGrpCh[i][j] != -1)
	      ret |= CAEN_DGTZ_SetChannelDCOffset(handle,(i*8)+j, WDcfg.DCoffsetGrpCh[i][j]);
	    else
	      ret |= CAEN_DGTZ_SetChannelDCOffset(handle,(i*8)+j, WDcfg.DCoffset[i]);
	  }
	}
	else {
	  ret |= CAEN_DGTZ_SetGroupDCOffset(handle, i, WDcfg.DCoffset[i]);
	  ret |= CAEN_DGTZ_SetGroupSelfTrigger(handle, WDcfg.ChannelTriggerMode[i], (1<<i));
	  ret |= CAEN_DGTZ_SetGroupTriggerThreshold(handle, i, WDcfg.Threshold[i]);
	  ret |= CAEN_DGTZ_SetChannelGroupMask(handle, i, WDcfg.GroupTrgEnableMask[i]);
	} 
	ret |= CAEN_DGTZ_SetTriggerPolarity(handle, i, (CAEN_DGTZ_TriggerPolarity_t) WDcfg.TriggerEdge);
                
      }
    }
  } else {
    ret |= CAEN_DGTZ_SetChannelEnableMask(handle, WDcfg.EnableMask);
    for(i=0; i<WDcfg.Nch; i++) {
      if (WDcfg.EnableMask & (1<<i)) {
	ret |= CAEN_DGTZ_SetChannelDCOffset(handle, i, WDcfg.DCoffset[i]);
	ret |= CAEN_DGTZ_SetChannelSelfTrigger(handle, WDcfg.ChannelTriggerMode[i], (1<<i));
	ret |= CAEN_DGTZ_SetChannelTriggerThreshold(handle, i, WDcfg.Threshold[i]);
	ret |= CAEN_DGTZ_SetTriggerPolarity(handle, i, (CAEN_DGTZ_TriggerPolarity_t) WDcfg.TriggerEdge);
      }
    }
  }
  if (BoardInfo.FamilyCode == CAEN_DGTZ_XX742_FAMILY_CODE) {
    for(i=0; i<(WDcfg.Nch/8); i++) {
      ret |= CAEN_DGTZ_SetGroupFastTriggerDCOffset(handle,i,WDcfg.FTDCoffset[i]);
      ret |= CAEN_DGTZ_SetGroupFastTriggerThreshold(handle,i,WDcfg.FTThreshold[i]);
    }
  }
    
  if (ret)
    printf("Warning: errors found during the programming of the digitizer.\nSome settings may not be executed\n");

  return 0;
}

/*! \brief   Write the correction table of a x742 boards into the output files
 *
 *   \param   Filename of output file
 *   \param   DataCorrection table
 */
void SaveCorrectionTable(char *outputFileName, DataCorrection_t tb) {
  char fnStr[1000];
  int ch,i,j;
  FILE *outputfile;

  strcpy(fnStr, outputFileName);
  strcat(fnStr, "_cell.txt");
  printf("Saving correction table cell values to %s\n", fnStr);
  outputfile = fopen(fnStr, "w");
  for(ch=0; ch<MAX_X742_CHANNELS+1; ch++) {
    fprintf(outputfile, "Calibration values from cell 0 to 1024 for channel %d:\n\n", ch);
    for(i=0; i<1024; i+=8) {
      for(j=0; j<8; j++)
	fprintf(outputfile, "%d\t", tb.cell[ch][i+j]);
      fprintf(outputfile, "cell = %d to %d\n", i, i+7);
    }
  }
  fclose(outputfile);

  strcpy(fnStr, outputFileName);
  strcat(fnStr, "_nsample.txt");
  printf("Saving correction table nsamples values to %s\n", fnStr);
  outputfile = fopen(fnStr, "w");
  for(ch=0; ch<MAX_X742_CHANNELS+1; ch++) {
    fprintf(outputfile, "Calibration values from cell 0 to 1024 for channel %d:\n\n", ch);
    for(i=0; i<1024; i+=8) {
      for(j=0; j<8; j++)
	fprintf(outputfile, "%d\t", tb.nsample[ch][i+j]);
      fprintf(outputfile, "cell = %d to %d\n", i, i+7);
    }
  }
  fclose(outputfile);

  strcpy(fnStr, outputFileName);
  strcat(fnStr, "_time.txt");
  printf("Saving correction table time values to %s\n", fnStr);
  outputfile = fopen(fnStr, "w");
  fprintf(outputfile, "Calibration values (ps) from cell 0 to 1024 :\n\n");
  for(i=0; i<1024; i+=8) {
    for(ch=0; ch<8; ch++)
      fprintf(outputfile, "%09.3f\t", tb.time[i+ch]);
    fprintf(outputfile, "cell = %d to %d\n", i, i+7);
  }
  fclose(outputfile);
}

int writeEventToOutputBuffer_V1742(std::vector<unsigned int> *eventBuffer, CAEN_DGTZ_EventInfo_t *EventInfo, CAEN_DGTZ_X742_EVENT_t *Event)
{
  int gr,ch;

  //          ====================================================
  //          |           V1742 Raw Event Data Format            |
  //          ====================================================

  //                       31  -  28 27  -  16 15   -   0
  //            Word[0] = [ 1010  ] [Event Tot #Words  ] //Event Header (5 words)
  //            Word[1] = [     Board Id    ] [ Pattern]  
  //            Word[2] = [      #channels readout     ]
  //            Word[3] = [        Event counter       ]
  //            Word[4] = [      Trigger Time Tag      ]
  //            Word[5] = [ 1000  ] [    Ch0   #Words  ] // Ch0 Data (2 + #samples words)
  //            Word[6] = [    Ch0  #Gr    ] [ Ch0 #Ch ] 
  //            Word[7] = [ Ch0 Corr. samples  (float) ]
  //                ..  = [ Ch0 Corr. samples  (float) ]
  // Word[5+Ch0 #Words] = [ 1000  ] [    Ch1   #Words  ] // Ch1 Data (2 + #samples words)
  // Word[6+Ch0 #Words] = [    Ch1  #Gr    ] [ Ch1 #Ch ]
  // Word[7+Ch0 #Words] = [ Ch1 Corr. samples  (float) ]
  //               ...   = [          .....             ]

  eventBuffer->clear();
  eventBuffer->resize(5);
  (*eventBuffer)[0]=0xA0000005; 
  (*eventBuffer)[1]=((EventInfo->BoardId)<<26)+EventInfo->Pattern;
  (*eventBuffer)[2]=0;
  (*eventBuffer)[3]=EventInfo->EventCounter;
  (*eventBuffer)[4]=EventInfo->TriggerTimeTag;

  //  printf("EVENT 1742 %d %d\n",EventInfo->EventCounter,EventInfo->TriggerTimeTag);
  for (gr=0;gr<(WDcfg.Nch/8);gr++) {
    if (Event->GrPresent[gr]) {
      for(ch=0; ch<9; ch++) {
	int Size = Event->DataGroup[gr].ChSize[ch];
	if (Size <= 0) {
	  continue;
	}

	// Channel Header for this event
 	uint32_t ChHeader[2];
	ChHeader[0] = (8<<28) + (2 + Size); //Number of words written for this channel
	ChHeader[1] = (gr<<16)+ch;

	//Starting pointer
	int start_ptr=eventBuffer->size();

	//Allocating necessary space for this channel
	eventBuffer->resize(eventBuffer->size() + 2 + Size);
	memcpy(&((*eventBuffer)[start_ptr]), &ChHeader[0], 2 * sizeof(unsigned int));

	//Beware the datas are float (because they are corrected...) but copying them here bit by bit. Should remember this for reading them out
	memcpy(&((*eventBuffer)[start_ptr+2]), Event->DataGroup[gr].DataChannel[ch], Size * sizeof(unsigned int));

	//Update event size and #channels
	(*eventBuffer)[0]+=(Size+2);
	(*eventBuffer)[2]++;
      }
    }
  }
  
  return 0;
}

/*! \fn      int ParseConfigFile(FILE *f_ini, WaveDumpConfig_t *WDcfg) 
 *   \brief   Read the configuration file and set the WaveDump paremeters
 *            
 *   \param   f_ini        Pointer to the config file
 *   \param   WDcfg:   Pointer to the WaveDumpConfig data structure
 *   \return  0 = Success; negative numbers are error codes
 */
int ParseConfigFile(FILE *f_ini) 
{
  char str[1000], str1[1000];
  int i,j, ch=-1, val, Off=0, tr = -1;

  /* Default settings */
  WDcfg.RecordLength = (1024*16);
  WDcfg.PostTrigger = 80;
  WDcfg.NumEvents = 1023;
  WDcfg.EnableMask = 0xFF;
  WDcfg.GWn = 0;
  WDcfg.ExtTriggerMode = CAEN_DGTZ_TRGMODE_ACQ_ONLY;
  WDcfg.InterruptNumEvents = 0;
  WDcfg.TestPattern = 0;
  WDcfg.TriggerEdge = 0;
  WDcfg.DesMode = 0;
  WDcfg.FastTriggerMode = CAEN_DGTZ_TRGMODE_DISABLED;
  WDcfg.FastTriggerEnabled = 0; 
  WDcfg.FPIOtype = 0;
  /* strcpy(WDcfg.GnuPlotPath, GNUPLOT_DEFAULT_PATH); */
  for(i=0; i<MAX_SET; i++) {
    WDcfg.DCoffset[i] = 0;
    WDcfg.Threshold[i] = 0;
    WDcfg.ChannelTriggerMode[i] = CAEN_DGTZ_TRGMODE_DISABLED;
    WDcfg.GroupTrgEnableMask[i] = 0;
    for(j=0; j<MAX_SET; j++) WDcfg.DCoffsetGrpCh[i][j] = -1;
    WDcfg.FTThreshold[i] = 0;
    WDcfg.FTDCoffset[i] =0;
  }

  WDcfg.useCorrections = -1;


  /* read config file and assign parameters */
  while(!feof(f_ini)) {
    int read;
    char *res;
    // read a word from the file
    read = fscanf(f_ini, "%s", str);
    if( !read || (read == EOF) || !strlen(str))
      continue;
    // skip comments
    if(str[0] == '#') {
      res = fgets(str, 1000, f_ini);
      continue;
    }
      
    if (strcmp(str, "@ON")==0) {
      Off = 0;
      continue;
    }
    if (strcmp(str, "@OFF")==0)
      Off = 1;
    if (Off)
      continue;
      
      
    // Section (COMMON or individual channel)
    if (str[0] == '[') {
      if (strstr(str, "COMMON")) {
	ch = -1;
	continue; 
      }
      if (strstr(str, "TR")) {
	sscanf(str+1, "TR%d", &val);
	if (val < 0 || val >= MAX_SET) {
	  printf("%s: Invalid channel number\n", str);
	} else {
	  tr = val;
	}
      } else {
	sscanf(str+1, "%d", &val);
	if (val < 0 || val >= MAX_SET) {
	  printf("%s: Invalid channel number\n", str);
	} else {
	  ch = val;
	}
      }
      continue;
    }
		
    // OPEN: read the details of physical path to the digitizer
    if (strstr(str, "OPEN")!=NULL) {
      read = fscanf(f_ini, "%s", str1);
      if (strcmp(str1, "USB")==0)
	WDcfg.LinkType = CAEN_DGTZ_USB;
      else if (strcmp(str1, "PCI")==0)
	WDcfg.LinkType = CAEN_DGTZ_PCI_OpticalLink;
      else {
	printf("%s %s: Invalid connection type\n", str, str1);
	return -1; 
      }
      read = fscanf(f_ini, "%d", &WDcfg.LinkNum);
      if (WDcfg.LinkType == CAEN_DGTZ_USB)
	WDcfg.ConetNode = 0;
      else
	read = fscanf(f_ini, "%d", &WDcfg.ConetNode);
      read = fscanf(f_ini, "%x", &WDcfg.BaseAddress);
      continue;
    }

    // Generic VME Write (address offset + data, both exadecimal)
    if ((strstr(str, "WRITE_REGISTER")!=NULL) && (WDcfg.GWn < MAX_GW)) {
      read = fscanf(f_ini, "%x", (int *)&WDcfg.GWaddr[WDcfg.GWn]);
      read = fscanf(f_ini, "%x", (int *)&WDcfg.GWdata[WDcfg.GWn]);
      WDcfg.GWn++;
      continue;
    }

    // Acquisition Record Length (number of samples)
    if (strstr(str, "RECORD_LENGTH")!=NULL) {
      read = fscanf(f_ini, "%d", &WDcfg.RecordLength);
      continue;
    }

    // Correction Level (mask)
    if (strstr(str, "CORRECTION_LEVEL")!=NULL) {
      read = fscanf(f_ini, "%s", str1);
      if( strcmp(str1, "AUTO") == 0 )
	WDcfg.useCorrections = -1;
      else
	WDcfg.useCorrections = atoi(str1);
      continue;
    }

    // Test Pattern
    if (strstr(str, "TEST_PATTERN")!=NULL) {
      read = fscanf(f_ini, "%s", str1);
      if (strcmp(str1, "YES")==0)
	WDcfg.TestPattern = 1;
      else if (strcmp(str1, "NO")!=0)
	printf("%s: invalid option\n", str);
      continue;
    }

    // Trigger Edge
    if (strstr(str, "TRIGGER_EDGE")!=NULL) {
      read = fscanf(f_ini, "%s", str1);
      if (strcmp(str1, "FALLING")==0)
	WDcfg.TriggerEdge = 1;
      else if (strcmp(str1, "RISING")!=0)
	printf("%s: invalid option\n", str);
      continue;
    }

    // External Trigger (DISABLED, ACQUISITION_ONLY, ACQUISITION_AND_TRGOUT)
    if (strstr(str, "EXTERNAL_TRIGGER")!=NULL) {
      read = fscanf(f_ini, "%s", str1);
      if (strcmp(str1, "DISABLED")==0)
	WDcfg.ExtTriggerMode = CAEN_DGTZ_TRGMODE_DISABLED;
      else if (strcmp(str1, "ACQUISITION_ONLY")==0)
	WDcfg.ExtTriggerMode = CAEN_DGTZ_TRGMODE_ACQ_ONLY;
      else if (strcmp(str1, "ACQUISITION_AND_TRGOUT")==0)
	WDcfg.ExtTriggerMode = CAEN_DGTZ_TRGMODE_ACQ_AND_EXTOUT;
      else
	printf("%s: Invalid Parameter\n", str);
      continue;
    }

    // Max. number of events for a block transfer (0 to 1023)
    if (strstr(str, "MAX_NUM_EVENTS_BLT")!=NULL) {
      read = fscanf(f_ini, "%d", &WDcfg.NumEvents);
      continue;
    }

    /* // GNUplot path */
    /* if (strstr(str, "GNUPLOT_PATH")!=NULL) { */
    /* 	read = fscanf(f_ini, "%s", WDcfg.GnuPlotPath); */
    /* 	continue; */
    /* } */

    // Post Trigger (percent of the acquisition window)
    if (strstr(str, "POST_TRIGGER")!=NULL) {
      read = fscanf(f_ini, "%d", &WDcfg.PostTrigger);
      continue;
    }

    // DesMode (Double sampling frequency for the Mod 731 and 751)
    if (strstr(str, "ENABLE_DES_MODE")!=NULL) {
      read = fscanf(f_ini, "%s", str1);
      if (strcmp(str1, "YES")==0)
	WDcfg.DesMode = 1;
      else if (strcmp(str1, "NO")!=0)
	printf("%s: invalid option\n", str);
      continue;
    }

    /* // Output file format (BINARY or ASCII) */
    /* if (strstr(str, "OUTPUT_FILE_FORMAT")!=NULL) { */
    /* 	read = fscanf(f_ini, "%s", str1); */
    /* 	if (strcmp(str1, "BINARY")==0) */
    /* 		WDcfg.OutFileFlags|= OFF_BINARY; */
    /* 	else if (strcmp(str1, "ASCII")!=0) */
    /* 		printf("%s: invalid output file format\n", str1); */
    /* 	continue; */
    /* } */

    /* // Header into output file (YES or NO) */
    /* if (strstr(str, "OUTPUT_FILE_HEADER")!=NULL) { */
    /* 	read = fscanf(f_ini, "%s", str1); */
    /* 	if (strcmp(str1, "YES")==0) */
    /* 		WDcfg.OutFileFlags|= OFF_HEADER; */
    /* 	else if (strcmp(str1, "NO")!=0) */
    /* 		printf("%s: invalid option\n", str); */
    /* 	continue; */
    /* } */

    //    Interrupt settings (request interrupt when there are at least N events to read; 0=disable interrupts (polling mode))
    if (strstr(str, "USE_INTERRUPT")!=NULL) {
    	read = fscanf(f_ini, "%d", &WDcfg.InterruptNumEvents);
    	continue;
    }
		
    if (!strcmp(str, "FAST_TRIGGER")) {
      read = fscanf(f_ini, "%s", str1);
      if (strcmp(str1, "DISABLED")==0)
	WDcfg.FastTriggerMode = CAEN_DGTZ_TRGMODE_DISABLED;
      else if (strcmp(str1, "ACQUISITION_ONLY")==0)
	WDcfg.FastTriggerMode = CAEN_DGTZ_TRGMODE_ACQ_ONLY;
      else
	printf("%s: Invalid Parameter\n", str);
      continue;
    }
		
    if (strstr(str, "ENABLED_FAST_TRIGGER_DIGITIZING")!=NULL) {
      read = fscanf(f_ini, "%s", str1);
      if (strcmp(str1, "YES")==0)
	WDcfg.FastTriggerEnabled= 1;
      else if (strcmp(str1, "NO")!=0)
	printf("%s: invalid option\n", str);
      continue;
    }
		
    // DC offset (percent of the dynamic range, -50 to 50)
    if (!strcmp(str, "DC_OFFSET")) {
      float dc;
      read = fscanf(f_ini, "%f", &dc);
      if (tr != -1) {
	// 				WDcfg.FTDCoffset[tr] = dc;
	WDcfg.FTDCoffset[tr*2] = (uint32_t)dc;
	WDcfg.FTDCoffset[tr*2+1] = (uint32_t)dc;
	continue;
      }
      val = (int)((dc+50) * 65535 / 100);
      if (ch == -1)
	for(i=0; i<MAX_SET; i++)
	  WDcfg.DCoffset[i] = val;
      else
	WDcfg.DCoffset[ch] = val;
      continue;
    }
		
    if (strstr(str, "GRP_CH_DC_OFFSET")!=NULL) {
      float dc[8];
      read = fscanf(f_ini, "%f,%f,%f,%f,%f,%f,%f,%f", &dc[0], &dc[1], &dc[2], &dc[3], &dc[4], &dc[5], &dc[6], &dc[7]);
      for(i=0; i<MAX_SET; i++) {
	val = (int)((dc[i]+50) * 65535 / 100); 
	WDcfg.DCoffsetGrpCh[ch][i] = val;
      }
      continue;
    }

    // Threshold
    if (strstr(str, "TRIGGER_THRESHOLD")!=NULL) {
      read = fscanf(f_ini, "%d", &val);
      if (tr != -1) {
	//				WDcfg.FTThreshold[tr] = val;
	WDcfg.FTThreshold[tr*2] = val;
	WDcfg.FTThreshold[tr*2+1] = val;

	continue;
      }
      if (ch == -1)
	for(i=0; i<MAX_SET; i++)
	  WDcfg.Threshold[i] = val;
      else
	WDcfg.Threshold[ch] = val;
      continue;
    }

    // Group Trigger Enable Mask (hex 8 bit)
    if (strstr(str, "GROUP_TRG_ENABLE_MASK")!=NULL) {
      read = fscanf(f_ini, "%x", &val);
      if (ch == -1)
	for(i=0; i<MAX_SET; i++)
	  WDcfg.GroupTrgEnableMask[i] = val & 0xFF;
      else
	WDcfg.GroupTrgEnableMask[ch] = val & 0xFF;
      continue;
    }

    // Channel Auto trigger (DISABLED, ACQUISITION_ONLY, ACQUISITION_AND_TRGOUT)
    if (strstr(str, "CHANNEL_TRIGGER")!=NULL) {
      CAEN_DGTZ_TriggerMode_t tm;
      read = fscanf(f_ini, "%s", str1);
      if (strcmp(str1, "DISABLED")==0)
	tm = CAEN_DGTZ_TRGMODE_DISABLED;
      else if (strcmp(str1, "ACQUISITION_ONLY")==0)
	tm = CAEN_DGTZ_TRGMODE_ACQ_ONLY;
      else if (strcmp(str1, "ACQUISITION_AND_TRGOUT")==0)
	tm = CAEN_DGTZ_TRGMODE_ACQ_AND_EXTOUT;
      else {
	printf("%s: Invalid Parameter\n", str);
	continue;
      }
      if (ch == -1)
	for(i=0; i<MAX_SET; i++)
	  WDcfg.ChannelTriggerMode[i] = tm;
      else
	WDcfg.ChannelTriggerMode[ch] = tm;
      continue;
    }

    // Front Panel LEMO I/O level (NIM, TTL)
    if (strstr(str, "FPIO_LEVEL")!=NULL) {
      read = fscanf(f_ini, "%s", str1);
      if (strcmp(str1, "TTL")==0)
	WDcfg.FPIOtype = 1;
      else if (strcmp(str1, "NIM")!=0)
	printf("%s: invalid option\n", str);
      continue;
    }

    // Channel Enable (or Group enable for the V1740) (YES/NO)
    if (strstr(str, "ENABLE_INPUT")!=NULL) {
      read = fscanf(f_ini, "%s", str1);
      if (strcmp(str1, "YES")==0) {
	if (ch == -1)
	  WDcfg.EnableMask = 0xFF;
	else
	  WDcfg.EnableMask |= (1 << ch);
	continue;
      } else if (strcmp(str1, "NO")==0) {
	if (ch == -1)
	  WDcfg.EnableMask = 0x00;
	else
	  WDcfg.EnableMask &= ~(1 << ch);
	continue;
      } else {
	printf("%s: invalid option\n", str);
      }
      continue;
    }

    printf("%s: invalid setting\n", str);
  }
  return 0;
}

int init_V1742(int handle)
{

  /* WaveDumpRun_t      WDrun; */
  int ret=CAEN_DGTZ_Success;
  /* int  handle; */
  ERROR_CODES ErrCode= ERR_NONE;

  char ConfigFileName[100];
  /* int isVMEDevice= 0; */
  int MajorNumber; 

  /* v1742_eventPtr=NULL; */

  /* int nCycles= 0; */
  FILE *f_ini;

  
  printf("**************************************************************\n");
  printf("                        Initialise V1742\n");
  printf("**************************************************************\n");
  
  /* *************************************************************************************** */
  /* Open and parse configuration file                                                       */
  /* *************************************************************************************** */
  strcpy(ConfigFileName, DEFAULT_CONFIG_FILE);
  printf("Opening Configuration File %s\n", ConfigFileName);
  f_ini = fopen(ConfigFileName, "r");

  if (f_ini == NULL ) {
    ErrCode = ERR_CONF_FILE_NOT_FOUND;
    return ErrCode;
  }



  ParseConfigFile(f_ini);
  fclose(f_ini);


  /* /\* *************************************************************************************** *\/ */
  /* /\* Open the digitizer and read the board information                                       *\/ */
  /* /\* *************************************************************************************** *\/ */
  /* isVMEDevice = WDcfg.BaseAddress ? 1 : 0; */
  
  /* /\* HACK, the function to load the correction table is a CAENComm function, so we first open the */
  /*    device with CAENComm lib, read the the correction table and suddenly close the device. *\/ */

  /* if(WDcfg.useCorrections != -1) { // use Corrections Manually */
  /*   //Beware: Get Device ID from the vme_bridge.h and not from config file */
  /*   if (ret = (CAEN_DGTZ_ErrorCode) CAENComm_OpenDevice((CAENComm_ConnectionType)WDcfg.LinkType,VME_DEVICE_ID,WDcfg.ConetNode,WDcfg.BaseAddress,&handle)) { */
  /*     ErrCode = ERR_DGZ_OPEN; */
  /*     return ErrCode; */
  /*   } */
    
  /*   if (ret = (CAEN_DGTZ_ErrorCode) LoadCorrectionTables(handle, &Table_gr0, 0, CAEN_DGTZ_DRS4_5GHz)) */
  /*     return -1; */
    
  /*   if (ret = (CAEN_DGTZ_ErrorCode) LoadCorrectionTables(handle, &Table_gr1, 1, CAEN_DGTZ_DRS4_5GHz)) */
  /*     return -1; */

  /*   if (ret = (CAEN_DGTZ_ErrorCode) CAENComm_CloseDevice(handle)) */
  /*     return -1; */
    
  /*   SaveCorrectionTable("table0", Table_gr0); */
  /*   SaveCorrectionTable("table1", Table_gr1); */
  /*   // write tables to file */
  /* } */


  /* printf("Opening device %d %d %d %X %d\n",(CAEN_DGTZ_ConnectionType) WDcfg.LinkType,VME_DEVICE_ID, WDcfg.ConetNode, WDcfg.BaseAddress, handle); */

  /* //Beware: Get Device ID from the vme_bridge.h and not from config file */
  /* //ret = CAEN_DGTZ_OpenDigitizer( (CAEN_DGTZ_ConnectionType) WDcfg.LinkType, VME_DEVICE_ID, WDcfg.ConetNode, WDcfg.BaseAddress, &handle); */
  /* //  ret = CAEN_DGTZ_OpenDigitizer( (CAEN_DGTZ_ConnectionType) WDcfg.LinkType, VME_DEVICE_ID, WDcfg.ConetNode, WDcfg.BaseAddress, &handle); */
  /* ret = CAEN_DGTZ_OpenDigitizer( (CAEN_DGTZ_ConnectionType) 0, 2, 0, 0x500000, &handle); */

  /* printf("Open status %d\n",ret); */

  /* if (ret) { */
  /*   ErrCode = ERR_DGZ_OPEN; */
  /*   return ErrCode; */
  /* } */

  ret = CAEN_DGTZ_GetInfo(handle, &BoardInfo);


  if (ret) {
    ErrCode = ERR_BOARD_INFO_READ;
    return ErrCode;
  }
  printf("Connected to CAEN Digitizer Model %s\n", BoardInfo.ModelName);
  printf("ROC FPGA Release is %s\n", BoardInfo.ROC_FirmwareRel);
  printf("AMC FPGA Release is %s\n", BoardInfo.AMC_FirmwareRel);
  
  // Check firmware rivision (DPP firmwares cannot be used with WaveDump */
  sscanf(BoardInfo.AMC_FirmwareRel, "%d", &MajorNumber);
  if (MajorNumber >= 128) {
    printf("This digitizer has a DPP firmware\n");
    ErrCode = ERR_INVALID_BOARD_TYPE;
    return ErrCode;
  }
  
  // get num of channels, num of bit, num of group of the board */
  ret = (CAEN_DGTZ_ErrorCode) GetMoreBoardInfo(handle);

  if (ret) {
    ErrCode = ERR_INVALID_BOARD_TYPE;
    return ErrCode;
  }
  
  if( WDcfg.useCorrections == -1 ) { // use automatic corrections
    ret = CAEN_DGTZ_LoadDRS4CorrectionData(handle,CAEN_DGTZ_DRS4_5GHz);
    ret = CAEN_DGTZ_EnableDRS4Correction(handle);
  }

  // mask the channels not available for this model
  if ((BoardInfo.FamilyCode != CAEN_DGTZ_XX740_FAMILY_CODE) && (BoardInfo.FamilyCode != CAEN_DGTZ_XX742_FAMILY_CODE)){
    WDcfg.EnableMask &= (1<<WDcfg.Nch)-1;
  } else {
    WDcfg.EnableMask &= (1<<(WDcfg.Nch/8))-1;
  }
  if ((BoardInfo.FamilyCode == CAEN_DGTZ_XX751_FAMILY_CODE) && WDcfg.DesMode) {
    WDcfg.EnableMask &= 0xAA;
  }
  if ((BoardInfo.FamilyCode == CAEN_DGTZ_XX731_FAMILY_CODE) && WDcfg.DesMode) {
    WDcfg.EnableMask &= 0x55;
  }

  /* // Set plot mask */
  /* if ((BoardInfo.FamilyCode != CAEN_DGTZ_XX740_FAMILY_CODE) && (BoardInfo.FamilyCode != CAEN_DGTZ_XX742_FAMILY_CODE)){ */
  /*   WDrun.ChannelPlotMask = WDcfg.EnableMask; */
  /* } else { */
  /*   WDrun.ChannelPlotMask = (WDcfg.FastTriggerEnabled == 0) ? 0xFF: 0x1FF; */
  /* } */
  
  /* *************************************************************************************** */
  /* program the digitizer                                                                   */
  /* *************************************************************************************** */
  ret = (CAEN_DGTZ_ErrorCode) ProgramDigitizer(handle);
  if (ret) {
    ErrCode = ERR_DGZ_PROGRAM;
    return ErrCode;
  }
  /* printf("%d\n",ret); */

  /* // Select the next enabled group for plotting */
  /* if ((WDcfg.EnableMask) && (WDcfg.Nch>8)) */
  /*   if( ((WDcfg.EnableMask>>WDrun.GroupPlotIndex)&0x1)==0 ) */
  /*     GoToNextEnabledGroup(&WDrun, WDcfg); */
  
  // Read again the board infos, just in case some of them were changed by the programming
  // (like, for example, the TSample and the number of channels if DES mode is changed)
  ret = CAEN_DGTZ_GetInfo(handle, &BoardInfo);
  if (ret) {
    ErrCode = ERR_BOARD_INFO_READ;
    return ErrCode;
  }
  ret = (CAEN_DGTZ_ErrorCode) GetMoreBoardInfo(handle);
  if (ret) {
    ErrCode = ERR_INVALID_BOARD_TYPE;
    return ErrCode;
  }
  
  
  //if (WDcfg.TestPattern) CAEN_DGTZ_DisableDRS4Correction(handle);
  //else CAEN_DGTZ_EnableDRS4Correction(handle);

  /* if (WDrun.Restart && WDrun.AcqRun) */

  CAEN_DGTZ_SWStartAcquisition(handle);

  printf("**************************************************************\n");
  printf("         Initialise V1742 completed. Starting run \n");
  printf("**************************************************************\n");

  return 0;
}

/* else */
/*     printf("[s] start/stop the acquisition, [q] quit, [SPACE] help\n"); */
/* WDrun.Restart = 0; */
/* PrevRateTime = get_time(); */
/* *************************************************************************************** */
/* Readout Loop                                                                            */
/* *************************************************************************************** */
/* while(!WDrun.Quit) { */
    
/* // Check for keyboard commands (key pressed) */
/* CheckKeyboardCommands(handle, &WDrun, WDcfg, BoardInfo); */
/* if (WDrun.Restart) { */
/*     CAEN_DGTZ_SWStopAcquisition(handle); */
/*     CAEN_DGTZ_FreeReadoutBuffer(&v1742_buffer); */
/*     ClosePlotter(); */
/*     PlotVar = NULL; */
/*     if(WDcfg.Nbit == 8) */
/*         CAEN_DGTZ_FreeEvent(handle, (void**)&Event8); */
/*     else */
/* 			if (BoardInfo.FamilyCode != CAEN_DGTZ_XX742_FAMILY_CODE) { */
/* 				CAEN_DGTZ_FreeEvent(handle, (void**)&Event16); */
/* 			} */
/* 			else { */
/* 			    CAEN_DGTZ_FreeEvent(handle, (void**)&Event742); */
/* 			} */
/*     f_ini = fopen(ConfigFileName, "r"); */
/*     ParseConfigFile(f_ini, WDcfg); */
/*     fclose(f_ini); */
/*     goto Restart; */
/* } */
/* if (WDrun.AcqRun == 0) */
/*     continue; */
    
/* /\* Send a software trigger *\/ */
/* if (WDrun.ContinuousTrigger) { */
/*     CAEN_DGTZ_SendSWtrigger(handle); */
/* } */
    

int read_V1742(int handle, unsigned int nevents, std::vector<V1742_Event_t>& events)
//int read_V1742(int handle)
{
  /* printf("Start read\n"); */
  CAEN_DGTZ_ErrorCode ret=CAEN_DGTZ_Success;
  ERROR_CODES ErrCode= ERR_NONE;


  int i;
  //, Nb=0, Ne=0;
  uint32_t BufferSize, NumEvents,Nb=0,Ne=0;

  CAEN_DGTZ_EventInfo_t       EventInfo;
  
  char *v1742_buffer;
  char *v1742_eventPtr;
  
  v1742_buffer=NULL;


  uint32_t AllocatedSize;

  CAEN_DGTZ_UINT16_EVENT_t    *Event16=NULL; /* generic event struct with 16 bit data (10, 12, 14 and 16 bit digitizers */
  CAEN_DGTZ_UINT8_EVENT_t     *Event8=NULL; /* generic event struct with 8 bit data (only for 8 bit digitizers) */ 
  CAEN_DGTZ_X742_EVENT_t       *Event742=NULL;  /* custom event struct with 8 bit data (only for 8 bit digitizers) */

  
  /* Read data from the board */
  // Allocate memory for the event data and readout buffer

  if(WDcfg.Nbit == 8)
    ret = CAEN_DGTZ_AllocateEvent(handle, (void**)&Event8);
  else {
    if (BoardInfo.FamilyCode != CAEN_DGTZ_XX742_FAMILY_CODE) {
      ret = CAEN_DGTZ_AllocateEvent(handle, (void**)&Event16);
    }
    else {
      ret = CAEN_DGTZ_AllocateEvent(handle, (void**)&Event742);
    }
  }

  if (ret != CAEN_DGTZ_Success) {
    ErrCode = ERR_MALLOC;
    return ErrCode;
  }

  /* printf("allocated event %d\n",ret); */

  ret = CAEN_DGTZ_MallocReadoutBuffer(handle, &v1742_buffer, &AllocatedSize); /* WARNING: This malloc must be done after the digitizer programming */
  /* printf("malloc %d %d\n",AllocatedSize,ret); */
  if (ret) {
    ErrCode = ERR_MALLOC;
    return ErrCode;
  }

  /* /\* //Wait for interrupt (if enabled) *\/ */
  /* if (WDcfg.InterruptNumEvents > 0) { */
  /* 	int32_t boardId; */
  /* 	int VMEHandle; */
  /* 	int InterruptMask = (1 << VME_INTERRUPT_LEVEL); */
  
  /* 	// Interrupt handling */
  /* 	ret = CAEN_DGTZ_VMEIRQWait ((CAEN_DGTZ_ConnectionType) WDcfg.LinkType, WDcfg.LinkNum, WDcfg.ConetNode , InterruptMask, INTERRUPT_TIMEOUT, &VMEHandle); */
  /* 	if (ret == CAEN_DGTZ_Timeout)  // No active interrupt requests */
  /* 	  goto InterruptTimeout; */
  /* 	if (ret != CAEN_DGTZ_Success)  { */
  /* 	  ErrCode = ERR_INTERRUPT; */
  /* 	  return ErrCode; */
  /* 	} */
  /* 	// Interrupt Ack */
  /* 	ret = CAEN_DGTZ_VMEIACKCycle(VMEHandle, VME_INTERRUPT_LEVEL, &boardId); */
  /* 	if ((ret != CAEN_DGTZ_Success) || (boardId != VME_INTERRUPT_STATUS_ID)) { */
  /* 	  goto InterruptTimeout; */
  /* 	} else { */
  /* 	  if (INTERRUPT_MODE == CAEN_DGTZ_IRQ_MODE_ROAK) */
  /* 	    ret = CAEN_DGTZ_RearmInterrupt(handle); */
  /* 	} */
  /* } */
  
  BufferSize = 0;
  NumEvents = 0;

  while (nevents != NumEvents)
    {
      ret = CAEN_DGTZ_ReadData(handle, CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT, v1742_buffer, &BufferSize);
      if (ret) {
	
	ErrCode = ERR_READOUT;
	return ErrCode;
      }
      
      NumEvents = 0;
      if (BufferSize != 0) {
	ret = CAEN_DGTZ_GetNumEvents(handle, v1742_buffer, BufferSize, &NumEvents);
	if (ret) {
	  ErrCode = ERR_READOUT;
	  return ErrCode;
	}
      }
    }
  

      
  /* if (nevents != NumEvents) */
  /*   { */
  /*     ErrCode = ERR_MISMATCH_EVENTS; */
  /*     return ErrCode; */
  /*   } */
  
  /* Nb += BufferSize;  */
  /* Ne += NumEvents; */
  /* CurrentTime=get_time(); */
  /* ElapsedTime=CurrentTime-PrevRateTime; */
  
  //      
  
  /* /\* nCycles++; *\/ */
  /* if (Ne%10==0) { */
  /* /\*   if (Nb == 0) *\/ */
  /* /\*     if (ret == CAEN_DGTZ_Timeout) printf ("Timeout...\n"); else printf("No data...\n"); *\/ */

  
  /* /\*   nCycles= 0; *\/ */
  /* /\*   Nb = 0; *\/ */
  /* /\*   Ne = 0; *\/ */
  /* 	PrevRateTime = CurrentTime; */
  /* } */
  
  /* Analyze data */
  for(i = 0; i < (int)NumEvents; i++) {
    /* Get one event from the readout buffer */
    ret = CAEN_DGTZ_GetEventInfo(handle, v1742_buffer, BufferSize, i, &EventInfo, &v1742_eventPtr);
    if (ret) {
      ErrCode = ERR_EVENT_BUILD;
      return ErrCode;
    }
    /* decode the event */
    if (WDcfg.Nbit == 8) 
      ret = CAEN_DGTZ_DecodeEvent(handle, v1742_eventPtr, (void**)&Event8);
    else if (BoardInfo.FamilyCode != CAEN_DGTZ_XX742_FAMILY_CODE) {
      ret = CAEN_DGTZ_DecodeEvent(handle, v1742_eventPtr, (void**)&Event16);
    }
    else {
      ret = CAEN_DGTZ_DecodeEvent(handle, v1742_eventPtr, (void**)&Event742);
      if(WDcfg.useCorrections != -1) { // if manual corrections
	ApplyDataCorrection( 0, WDcfg.useCorrections, CAEN_DGTZ_DRS4_5GHz, &(Event742->DataGroup[0]), &Table_gr0);
	ApplyDataCorrection( 1, WDcfg.useCorrections, CAEN_DGTZ_DRS4_5GHz, &(Event742->DataGroup[1]), &Table_gr1);
	  }
      events.push_back(V1742_Event_t(EventInfo,*Event742));
    }
    
    if (ret) {
      ErrCode = ERR_EVENT_BUILD;
      return ErrCode;
    }    
  }  


      //      printf("%d %d\n",Nb,Ne);
      //      sleep(1);
  
  /* //Freeing V1742 memory  after read */
  free(v1742_buffer);
  //  free(v1742_eventPtr);
  delete(Event742);

  // Test what happens when enable this. Do we need to malloc again? To be checked
  /* ret = CAEN_DGTZ_FreeReadoutBuffer(&v1742_buffer); */
  /* if (ret) { */
  /*   ErrCode = ERR_FREE_BUFFER; */
  /*   return ErrCode; */
  /* } */
  
  return 0;
  
}

int stop_V1742(int handle)
{
  /* stop the acquisition */
  CAEN_DGTZ_SWStopAcquisition(handle);
  //  CAEN_DGTZ_FreeReadoutBuffer(&v1742_buffer);
  //  CAEN_DGTZ_CloseDigitizer(handle);
  
  return 0;
}
