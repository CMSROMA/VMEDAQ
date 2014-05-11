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


/* #define VME_INTERRUPT_LEVEL      1 */
/* #define VME_INTERRUPT_STATUS_ID  0xAAAA */
/* #define INTERRUPT_MODE           CAEN_DGTZ_IRQ_MODE_ROAK */
/* #define INTERRUPT_TIMEOUT        200  // ms */

        
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
  /* int InterruptNumEvents; */
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
static int v1742_handle;

static char *v1742_buffer;
static char *v1742_eventPtr;
static CAEN_DGTZ_UINT16_EVENT_t    *Event16=NULL; /* generic event struct with 16 bit data (10, 12, 14 and 16 bit digitizers */
static CAEN_DGTZ_UINT8_EVENT_t     *Event8=NULL; /* generic event struct with 8 bit data (only for 8 bit digitizers) */ 
static CAEN_DGTZ_X742_EVENT_t       *Event742=NULL;  /* custom event struct with 8 bit data (only for 8 bit digitizers) */

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
  /* ERR_INTERRUPT, */
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
/* static long get_time() */
/* { */
/*     long time_ms; */
/* #ifdef WIN32 */
/*     struct _timeb timebuffer; */
/*     _ftime( &timebuffer ); */
/*     time_ms = (long)timebuffer.time * 1000 + (long)timebuffer.millitm; */
/* #else */
/*     struct timeval t1; */
/*     struct timezone tz; */
/*     gettimeofday(&t1, &tz); */
/*     time_ms = (t1.tv_sec) * 1000 + t1.tv_usec / 1000; */
/* #endif */
/*     return time_ms; */
/* } */


/*! \fn      int GetMoreBoardInfo(CAEN_DGTZ_BoardInfo_t BoardInfo,  WaveDumpConfig_t *WDcfg)
*   \brief   calculate num of channels, num of bit and sampl period according to the board type
*
*   \param   BoardInfo   Board Type
*   \param   WDcfg       pointer to the config. struct
*   \return  0 = Success; -1 = unknown board type
*/
int GetMoreBoardInfo()
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
    if ((ret = CAEN_DGTZ_GetDRS4SamplingFrequency(v1742_handle, &freq)) != CAEN_DGTZ_Success) return CAEN_DGTZ_CommError;
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
int ProgramDigitizer()
{
  int i,j, ret = 0;

  /* reset the digitizer */
  ret |= CAEN_DGTZ_Reset(v1742_handle);
  if (ret != 0) {
    printf("Error: Unable to reset digitizer.\nPlease reset digitizer manually then restart the program\n");
    return -1;
  }
  /* execute generic write commands */
  for(i=0; i<WDcfg.GWn; i++)
    ret |= CAEN_DGTZ_WriteRegister(v1742_handle, WDcfg.GWaddr[i], WDcfg.GWdata[i]);

  // Set the waveform test bit for debugging
  if (WDcfg.TestPattern)
    ret |= CAEN_DGTZ_WriteRegister(v1742_handle, CAEN_DGTZ_BROAD_CH_CONFIGBIT_SET_ADD, 1<<3);
  // custom setting for X742 boards
  if (BoardInfo.FamilyCode == CAEN_DGTZ_XX742_FAMILY_CODE) {
    ret |= CAEN_DGTZ_SetFastTriggerDigitizing(v1742_handle,(CAEN_DGTZ_EnaDis_t)WDcfg.FastTriggerEnabled);
    ret |= CAEN_DGTZ_SetFastTriggerMode(v1742_handle,(CAEN_DGTZ_TriggerMode_t)WDcfg.FastTriggerMode);
  }
  if ((BoardInfo.FamilyCode == CAEN_DGTZ_XX751_FAMILY_CODE) || (BoardInfo.FamilyCode == CAEN_DGTZ_XX731_FAMILY_CODE)) {
    ret |= CAEN_DGTZ_SetDESMode(v1742_handle, ( CAEN_DGTZ_EnaDis_t) WDcfg.DesMode);
  }
  ret |= CAEN_DGTZ_SetRecordLength(v1742_handle, WDcfg.RecordLength);
  ret |= CAEN_DGTZ_SetPostTriggerSize(v1742_handle, (uint32_t) WDcfg.PostTrigger);
  if(BoardInfo.FamilyCode != CAEN_DGTZ_XX742_FAMILY_CODE)
    ret |= CAEN_DGTZ_GetPostTriggerSize(v1742_handle, &WDcfg.PostTrigger);
  ret |= CAEN_DGTZ_SetIOLevel(v1742_handle, (CAEN_DGTZ_IOLevel_t) WDcfg.FPIOtype);
  /* if( WDcfg.InterruptNumEvents > 0) { */
  /*     // Interrupt handling */
  /*     if( ret |= CAEN_DGTZ_SetInterruptConfig( v1742_handle, CAEN_DGTZ_ENABLE, */
  /*                                              VME_INTERRUPT_LEVEL, VME_INTERRUPT_STATUS_ID, */
  /*                                              WDcfg.InterruptNumEvents, INTERRUPT_MODE)!= CAEN_DGTZ_Success) { */
  /*         printf( "\nError configuring interrupts. Interrupts disabled\n\n"); */
  /*         WDcfg.InterruptNumEvents = 0; */
  /*     } */
  /* } */
  ret |= CAEN_DGTZ_SetMaxNumEventsBLT(v1742_handle, WDcfg.NumEvents);
  ret |= CAEN_DGTZ_SetAcquisitionMode(v1742_handle, CAEN_DGTZ_SW_CONTROLLED);
  ret |= CAEN_DGTZ_SetExtTriggerInputMode(v1742_handle, WDcfg.ExtTriggerMode);

  if ((BoardInfo.FamilyCode == CAEN_DGTZ_XX740_FAMILY_CODE) || (BoardInfo.FamilyCode == CAEN_DGTZ_XX742_FAMILY_CODE)){
    ret |= CAEN_DGTZ_SetGroupEnableMask(v1742_handle, WDcfg.EnableMask);
    for(i=0; i<(WDcfg.Nch/8); i++) {
      if (WDcfg.EnableMask & (1<<i)) {
	if (BoardInfo.FamilyCode == CAEN_DGTZ_XX742_FAMILY_CODE) {
	  for(j=0; j<8; j++) {
	    if (WDcfg.DCoffsetGrpCh[i][j] != -1)
	      ret |= CAEN_DGTZ_SetChannelDCOffset(v1742_handle,(i*8)+j, WDcfg.DCoffsetGrpCh[i][j]);
	    else
	      ret |= CAEN_DGTZ_SetChannelDCOffset(v1742_handle,(i*8)+j, WDcfg.DCoffset[i]);
	  }
	}
	else {
	  ret |= CAEN_DGTZ_SetGroupDCOffset(v1742_handle, i, WDcfg.DCoffset[i]);
	  ret |= CAEN_DGTZ_SetGroupSelfTrigger(v1742_handle, WDcfg.ChannelTriggerMode[i], (1<<i));
	  ret |= CAEN_DGTZ_SetGroupTriggerThreshold(v1742_handle, i, WDcfg.Threshold[i]);
	  ret |= CAEN_DGTZ_SetChannelGroupMask(v1742_handle, i, WDcfg.GroupTrgEnableMask[i]);
	} 
	ret |= CAEN_DGTZ_SetTriggerPolarity(v1742_handle, i, (CAEN_DGTZ_TriggerPolarity_t) WDcfg.TriggerEdge);
                
      }
    }
  } else {
    ret |= CAEN_DGTZ_SetChannelEnableMask(v1742_handle, WDcfg.EnableMask);
    for(i=0; i<WDcfg.Nch; i++) {
      if (WDcfg.EnableMask & (1<<i)) {
	ret |= CAEN_DGTZ_SetChannelDCOffset(v1742_handle, i, WDcfg.DCoffset[i]);
	ret |= CAEN_DGTZ_SetChannelSelfTrigger(v1742_handle, WDcfg.ChannelTriggerMode[i], (1<<i));
	ret |= CAEN_DGTZ_SetChannelTriggerThreshold(v1742_handle, i, WDcfg.Threshold[i]);
	ret |= CAEN_DGTZ_SetTriggerPolarity(v1742_handle, i, (CAEN_DGTZ_TriggerPolarity_t) WDcfg.TriggerEdge);
      }
    }
  }
  if (BoardInfo.FamilyCode == CAEN_DGTZ_XX742_FAMILY_CODE) {
    for(i=0; i<(WDcfg.Nch/8); i++) {
      ret |= CAEN_DGTZ_SetGroupFastTriggerDCOffset(v1742_handle,i,WDcfg.FTDCoffset[i]);
      ret |= CAEN_DGTZ_SetGroupFastTriggerThreshold(v1742_handle,i,WDcfg.FTThreshold[i]);
    }
  }
    
  if (ret)
    printf("Warning: errors found during the programming of the digitizer.\nSome settings may not be executed\n");

  return 0;
}

/* /\*! \fn      void GoToNextEnabledGroup(WaveDumpRun_t *WDrun, WaveDumpConfig_t *WDcfg) */
/* *   \brief   selects the next enabled group for plotting */
/* * */
/* *   \param   WDrun:   Pointer to the WaveDumpRun_t data structure */
/* *   \param   WDcfg:   Pointer to the WaveDumpConfig_t data structure */
/* *\/ */
/* void GoToNextEnabledGroup(WaveDumpRun_t *WDrun, WaveDumpConfig_t *WDcfg) { */
/*     if ((WDcfg.EnableMask) && (WDcfg.Nch>8)) { */
/*         int orgPlotIndex = WDrun->GroupPlotIndex; */
/*         do { */
/*             WDrun->GroupPlotIndex = (++WDrun->GroupPlotIndex)%(WDcfg.Nch/8); */
/*         } while( !((1 << WDrun->GroupPlotIndex)& WDcfg.EnableMask)); */
/*         if( WDrun->GroupPlotIndex != orgPlotIndex) { */
/*             printf("Plot group set to %d\n", WDrun->GroupPlotIndex); */
/*         } */
/*     } */
/* 	ClearPlot(); */
/* } */

/* /\*! \fn      void CheckKeyboardCommands(WaveDumpRun_t *WDrun) */
/* *   \brief   check if there is a key pressed and execute the relevant command */
/* * */
/* *   \param   WDrun:   Pointer to the WaveDumpRun_t data structure */
/* *   \param   WDcfg:   Pointer to the WaveDumpConfig_t data structure */
/* *   \param   BoardInfo: structure with the board info */
/* *\/ */
/* void CheckKeyboardCommands(int handle, WaveDumpRun_t *WDrun, WaveDumpConfig_t *WDcfg, CAEN_DGTZ_BoardInfo_t BoardInfo) */
/* { */
/*     int c = 0; */

/*     if(!kbhit()) */
/*         return; */

/*     c = getch(); */
/*     if ((c < '9') && (c >= '0')) { */
/*         int ch = c-'0'; */
/*         if ((BoardInfo.FamilyCode == CAEN_DGTZ_XX740_FAMILY_CODE) || (BoardInfo.FamilyCode == CAEN_DGTZ_XX742_FAMILY_CODE)){ */
/*             if ( (BoardInfo.FamilyCode == CAEN_DGTZ_XX742_FAMILY_CODE) && (WDcfg.FastTriggerEnabled == 0) && (ch == 8)) WDrun->ChannelPlotMask = WDrun->ChannelPlotMask ; else WDrun->ChannelPlotMask ^= (1 << ch); */
/*             if (WDrun->ChannelPlotMask & (1 << ch)) */
/*                 printf("Channel %d enabled for plotting\n", ch + WDrun->GroupPlotIndex*8); */
/*             else */
/*                 printf("Channel %d disabled for plotting\n", ch + WDrun->GroupPlotIndex*8); */
/*         } else { */
/*             /\*if( !( WDcfg.EnableMask & (1 << ch))) { */
/*                 printf("Channel %d not enabled for acquisition\n", ch); */
/*             } else { */
/*                 WDrun->ChannelPlotMask ^= (1 << ch); */
/*                 if (WDrun->ChannelPlotMask & (1 << ch)) */
/*                     printf("Channel %d enabled for plotting\n", ch); */
/*                 else */
/*                     printf("Channel %d disabled for plotting\n", ch); */
/*             }*\/ */
/*             WDrun->ChannelPlotMask ^= (1 << ch); */
/*             if (WDrun->ChannelPlotMask & (1 << ch)) */
/*                 printf("Channel %d enabled for plotting\n", ch); */
/*             else */
/*                 printf("Channel %d disabled for plotting\n", ch); */
/*         } */
/*     } else { */
/*         switch(c) { */
/*             case 'g' : */
/*                 // Update the group plot index */
/*                 if ((WDcfg.EnableMask) && (WDcfg.Nch>8)) */
/*                     GoToNextEnabledGroup(WDrun, WDcfg); */
/*                 /\*if ((WDcfg.EnableMask) && (WDcfg.Nch>8)) { */
/*                     int orgPlotIndex = WDrun->GroupPlotIndex; */
/*                     do { */
/*                         WDrun->GroupPlotIndex = (++WDrun->GroupPlotIndex)%(WDcfg.Nch/8); */
/*                     } while( !((1 << WDrun->GroupPlotIndex)& WDcfg.EnableMask)); */
/*                     if( WDrun->GroupPlotIndex != orgPlotIndex) { */
/*                         printf("Plot group set to %d\n", WDrun->GroupPlotIndex); */
/*                     } */
/*                 } */
/* 				ClearPlot();*\/ */
/*                 break; */
/*             case 'q' : */
/*                 WDrun->Quit = 1; */
/*                 break; */
/*             case 'R' : */
/*                 WDrun->Restart = 1; */
/*                 break; */
/*             case 't' : */
/*                 if (!WDrun->ContinuousTrigger) { */
/*                     CAEN_DGTZ_SendSWtrigger(handle); */
/*                     printf("Single Software Trigger issued\n"); */
/*                 } */
/*                 break; */
/*             case 'T' : */
/*                 WDrun->ContinuousTrigger ^= 1; */
/*                 if (WDrun->ContinuousTrigger) */
/*                     printf("Continuous trigger is enabled\n"); */
/*                 else */
/*                     printf("Continuous trigger is disabled\n"); */
/*                 break; */
/*             case 'P' : */
/*                 if (WDrun->ChannelPlotMask == 0) */
/*                     printf("No channel enabled for plotting\n"); */
/*                 else */
/*                     WDrun->ContinuousPlot ^= 1; */
/*                 break; */
/*             case 'p' : */
/*                 if (WDrun->ChannelPlotMask == 0) */
/*                     printf("No channel enabled for plotting\n"); */
/*                 else */
/*                     WDrun->SinglePlot = 1; */
/*                 break; */
/*             case 'f' : */
/*                 WDrun->PlotType = (WDrun->PlotType == PLOT_FFT) ? PLOT_WAVEFORMS : PLOT_FFT; */
/*                 WDrun->SetPlotOptions = 1; */
/*                 break; */
/*             case 'h' : */
/*                 WDrun->PlotType = (WDrun->PlotType == PLOT_HISTOGRAM) ? PLOT_WAVEFORMS : PLOT_HISTOGRAM; */
/*                 WDrun->RunHisto = (WDrun->PlotType == PLOT_HISTOGRAM); */
/*                 WDrun->SetPlotOptions = 1; */
/*                 break; */
/*             case 'w' : */
/*                 if (!WDrun->ContinuousWrite) */
/*                     WDrun->SingleWrite = 1; */
/*                 break; */
/*             case 'W' : */
/*                 WDrun->ContinuousWrite ^= 1; */
/*                 if (WDrun->ContinuousWrite) */
/*                     printf("Continuous writing is enabled\n"); */
/*                 else */
/*                     printf("Continuous writing is disabled\n"); */
/*                 break; */
/*             case 's' : */
/*                 if (WDrun->AcqRun == 0) { */
/*                     printf("Acquisition started\n"); */
/*                     CAEN_DGTZ_SWStartAcquisition(handle); */
/*                     WDrun->AcqRun = 1; */
/*                 } else { */
/*                     printf("Acquisition stopped\n"); */
/*                     CAEN_DGTZ_SWStopAcquisition(handle); */
/*                     WDrun->AcqRun = 0; */
/*                 } */
/*                 break; */
/*             case ' ' : */
/*                 printf("\n                            Bindkey help                                \n"); */
/* 				printf("--------------------------------------------------------------------------\n");; */
/*                 printf("  [q]   Quit\n"); */
/*                 printf("  [R]   Reload configuration file and restart\n"); */
/*                 printf("  [s]   Start/Stop acquisition\n"); */
/*                 printf("  [t]   Send a software trigger (single shot)\n"); */
/*                 printf("  [T]   Enable/Disable continuous software trigger\n"); */
/*                 printf("  [w]   Write one event to output file\n"); */
/*                 printf("  [W]   Enable/Disable continuous writing to output file\n"); */
/*                 printf("  [p]   Plot one event\n"); */
/*                 printf("  [P]   Enable/Disable continuous plot\n"); */
/*                 printf("  [f]   Toggle between FFT and Waveform plot\n"); */
/*                 printf("  [h]   Toggle between Histogram and Waveform plot\n"); */
/*                 printf("  [g]   Change the index of the group to plot (XX740 family)\n"); */
/*                 printf(" [0-7]  Enable/Disable one channel on the plot\n"); */
/*                 printf("        For x740 family this is the plotted group's relative channel index\n"); */
/*                 printf("[SPACE] This help\n"); */
/* 				printf("--------------------------------------------------------------------------\n"); */
/*                 printf("Press a key to continue\n"); */
/*                 getch(); */
/*                 break; */
/*             default :   break; */
/*         } */
/*     } */
/* } */



/* /\*! \brief   Write the event data into the output files */
/* * */
/* *   \param   WDrun Pointer to the WaveDumpRun data structure */
/* *   \param   WDcfg Pointer to the WaveDumpConfig data structure */
/* *   \param   EventInfo Pointer to the EventInfo data structure */
/* *   \param   Event Pointer to the Event to write */
/* *\/ */
/* int WriteOutputFiles(WaveDumpConfig_t *WDcfg, WaveDumpRun_t *WDrun, CAEN_DGTZ_EventInfo_t *EventInfo, void *Event) */
/* { */
/*     int ch, j, ns; */
/*     CAEN_DGTZ_UINT16_EVENT_t  *Event16; */
/*     CAEN_DGTZ_UINT8_EVENT_t   *Event8; */

/*     if (WDcfg.Nbit == 8) */
/*         Event8 = (CAEN_DGTZ_UINT8_EVENT_t *)Event; */
/*     else */
/*         Event16 = (CAEN_DGTZ_UINT16_EVENT_t *)Event; */

/*     for(ch=0; ch<WDcfg.Nch; ch++) { */
/*         int Size = (WDcfg.Nbit == 8) ? Event8->ChSize[ch] : Event16->ChSize[ch]; */
/*         if (Size <= 0) { */
/*             continue; */
/*         } */

/*         // Check the file format type */
/* 	//        if( WDcfg.OutFileFlags& OFF_BINARY) { */
/*         if( 0 ) { */
/*             // Binary file format */
/*             uint32_t BinHeader[6]; */
/*             BinHeader[0] = (WDcfg.Nbit == 8) ? Size + 6*sizeof(*BinHeader) : Size*2 + 6*sizeof(*BinHeader); */
/*             BinHeader[1] = EventInfo->BoardId; */
/*             BinHeader[2] = EventInfo->Pattern; */
/*             BinHeader[3] = ch; */
/*             BinHeader[4] = EventInfo->EventCounter; */
/*             BinHeader[5] = EventInfo->TriggerTimeTag; */
/*             if (!WDrun->fout[ch]) { */
/*                 char fname[100]; */
/*                 sprintf(fname, "wave%d.dat", ch); */
/*                 if ((WDrun->fout[ch] = fopen(fname, "wb")) == NULL) */
/*                     return -1; */
/*             } */
/*             if( WDcfg.OutFileFlags & OFF_HEADER) { */
/*                 // Write the Channel Header */
/*                 if(fwrite(BinHeader, sizeof(*BinHeader), 6, WDrun->fout[ch]) != 6) { */
/*                     // error writing to file */
/*                     fclose(WDrun->fout[ch]); */
/*                     WDrun->fout[ch]= NULL; */
/*                     return -1; */
/*                 } */
/*             } */
/*             if (WDcfg.Nbit == 8) */
/*                 ns = (int)fwrite(Event8->DataChannel[ch], 1, Size, WDrun->fout[ch]); */
/*             else */
/*                 ns = (int)fwrite(Event16->DataChannel[ch] , 1 , Size*2, WDrun->fout[ch]) / 2; */
/*             if (ns != Size) { */
/*                 // error writing to file */
/*                 fclose(WDrun->fout[ch]); */
/*                 WDrun->fout[ch]= NULL; */
/*                 return -1; */
/*             } */
/*         } else { */
/*             // Ascii file format */
/*             if (!WDrun->fout[ch]) { */
/*                 char fname[100]; */
/*                 sprintf(fname, "wave%d.txt", ch); */
/*                 if ((WDrun->fout[ch] = fopen(fname, "w")) == NULL) */
/*                     return -1; */
/*             } */
/*             if( WDcfg.OutFileFlags & OFF_HEADER) { */
/*                 // Write the Channel Header */
/*                 fprintf(WDrun->fout[ch], "Record Length: %d\n", Size); */
/*                 fprintf(WDrun->fout[ch], "BoardID: %2d\n", EventInfo->BoardId); */
/*                 fprintf(WDrun->fout[ch], "Channel: %d\n", ch); */
/*                 fprintf(WDrun->fout[ch], "Event Number: %d\n", EventInfo->EventCounter); */
/*                 fprintf(WDrun->fout[ch], "Pattern: 0x%04X\n", EventInfo->Pattern & 0xFFFF); */
/*                 fprintf(WDrun->fout[ch], "Trigger Time Stamp: %u\n", EventInfo->TriggerTimeTag); */
/*                 fprintf(WDrun->fout[ch], "DC offset (DAC): 0x%04X\n", WDcfg.DCoffset[ch] & 0xFFFF); */
/*             } */
/*             for(j=0; j<Size; j++) { */
/*                 if (WDcfg.Nbit == 8) */
/*                     fprintf(WDrun->fout[ch], "%d\n", Event8->DataChannel[ch][j]); */
/*                 else */
/*                     fprintf(WDrun->fout[ch], "%d\n", Event16->DataChannel[ch][j]); */
/*             } */
/*         } */
/*         if (WDrun->SingleWrite) { */
/*             fclose(WDrun->fout[ch]); */
/*             WDrun->fout[ch]= NULL; */
/*         } */
/*     } */
/*     return 0; */

/* } */

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

int WriteEventToBuffer_V1742(std::vector<unsigned int> *eventBuffer, CAEN_DGTZ_EventInfo_t *EventInfo, CAEN_DGTZ_X742_EVENT_t *Event)
{
  int gr,ch, j, ns;
  char trname[10], flag;
  eventBuffer->clear();
  for (gr=0;gr<(WDcfg.Nch/8);gr++) {
    if (Event->GrPresent[gr]) {
      for(ch=0; ch<9; ch++) {
	int Size = Event->DataGroup[gr].ChSize[ch];
	if (Size <= 0) {
	  continue;
	}
	// Binary file format
	uint32_t BinHeader[6];
	BinHeader[0] = (WDcfg.Nbit == 8) ? Size + 6*sizeof(*BinHeader) : Size*4 + 6*sizeof(*BinHeader);
	BinHeader[1] = EventInfo->BoardId;
	BinHeader[2] = EventInfo->Pattern;
	BinHeader[3] = ch;
	BinHeader[4] = EventInfo->EventCounter;
	BinHeader[5] = EventInfo->TriggerTimeTag;
	eventBuffer->insert(eventBuffer->end(), &BinHeader[0], &BinHeader[5]); 
	eventBuffer->resize(eventBuffer->size() + Size);
	memcpy(&((*eventBuffer)[eventBuffer->size() - Size]), Event->DataGroup[gr].DataChannel[ch], Size * sizeof(unsigned int));
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
  /* WDcfg.InterruptNumEvents = 0; */
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

    // Interrupt settings (request interrupt when there are at least N events to read; 0=disable interrupts (polling mode))
    /* if (strstr(str, "USE_INTERRUPT")!=NULL) { */
    /* 	read = fscanf(f_ini, "%d", &WDcfg.InterruptNumEvents); */
    /* 	continue; */
    /* } */
		
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



int init_V1742()
{

  /* WaveDumpRun_t      WDrun; */
  CAEN_DGTZ_ErrorCode ret=CAEN_DGTZ_Success;
  /* int  v1742_handle; */
  ERROR_CODES ErrCode= ERR_NONE;
  v1742_buffer=NULL;
  v1742_eventPtr=NULL;

  uint32_t AllocatedSize;

  char ConfigFileName[100];
  /* int isVMEDevice= 0; */
  int MajorNumber; 
  /* uint64_t CurrentTime, PrevRateTime, ElapsedTime; */
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

  if(WDcfg.useCorrections != -1) { // use Corrections Manually
    //Beware: Get Device ID from the vme_bridge.h and not from config file
    if (ret = (CAEN_DGTZ_ErrorCode) CAENComm_OpenDevice((CAENComm_ConnectionType)WDcfg.LinkType,VME_DEVICE_ID,WDcfg.ConetNode,WDcfg.BaseAddress,&v1742_handle)) {
      ErrCode = ERR_DGZ_OPEN;
      return ErrCode;
    }
    
    if (ret = (CAEN_DGTZ_ErrorCode) LoadCorrectionTables(v1742_handle, &Table_gr0, 0, CAEN_DGTZ_DRS4_5GHz))
      return -1;
    
    if (ret = (CAEN_DGTZ_ErrorCode) LoadCorrectionTables(v1742_handle, &Table_gr1, 1, CAEN_DGTZ_DRS4_5GHz))
      return -1;

    if (ret = (CAEN_DGTZ_ErrorCode) CAENComm_CloseDevice(v1742_handle))
      return -1;
    
    SaveCorrectionTable("table0", Table_gr0);
    SaveCorrectionTable("table1", Table_gr1);
    // write tables to file
  }

  //Beware: Get Device ID from the vme_bridge.h and not from config file
  ret = CAEN_DGTZ_OpenDigitizer( (CAEN_DGTZ_ConnectionType) WDcfg.LinkType, VME_DEVICE_ID, WDcfg.ConetNode, WDcfg.BaseAddress, &v1742_handle);
  if (ret) {
    ErrCode = ERR_DGZ_OPEN;
    return ErrCode;
  }

  if( WDcfg.useCorrections == -1 ) { // use automatic corrections
    ret = CAEN_DGTZ_LoadDRS4CorrectionData(v1742_handle,CAEN_DGTZ_DRS4_5GHz);
    ret = CAEN_DGTZ_EnableDRS4Correction(v1742_handle);
  }
  ret = CAEN_DGTZ_GetInfo(v1742_handle, &BoardInfo);
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
  ret = (CAEN_DGTZ_ErrorCode) GetMoreBoardInfo();
  if (ret) {
    ErrCode = ERR_INVALID_BOARD_TYPE;
    return ErrCode;
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
  ret = (CAEN_DGTZ_ErrorCode) ProgramDigitizer();
  if (ret) {
    ErrCode = ERR_DGZ_PROGRAM;
    return ErrCode;
  }

  /* // Select the next enabled group for plotting */
  /* if ((WDcfg.EnableMask) && (WDcfg.Nch>8)) */
  /*   if( ((WDcfg.EnableMask>>WDrun.GroupPlotIndex)&0x1)==0 ) */
  /*     GoToNextEnabledGroup(&WDrun, WDcfg); */
  
  // Read again the board infos, just in case some of them were changed by the programming
  // (like, for example, the TSample and the number of channels if DES mode is changed)
  ret = CAEN_DGTZ_GetInfo(v1742_handle, &BoardInfo);
  if (ret) {
    ErrCode = ERR_BOARD_INFO_READ;
    return ErrCode;
  }
  ret = (CAEN_DGTZ_ErrorCode) GetMoreBoardInfo();
  if (ret) {
    ErrCode = ERR_INVALID_BOARD_TYPE;
    return ErrCode;
  }
  
  // Allocate memory for the event data and readout buffer
  if(WDcfg.Nbit == 8)
    ret = CAEN_DGTZ_AllocateEvent(v1742_handle, (void**)&Event8);
  else {
    if (BoardInfo.FamilyCode != CAEN_DGTZ_XX742_FAMILY_CODE) {
      ret = CAEN_DGTZ_AllocateEvent(v1742_handle, (void**)&Event16);
    }
    else {
      ret = CAEN_DGTZ_AllocateEvent(v1742_handle, (void**)&Event742);
    }
  }
  if (ret != CAEN_DGTZ_Success) {
    ErrCode = ERR_MALLOC;
    return ErrCode;
  }
  ret = CAEN_DGTZ_MallocReadoutBuffer(v1742_handle, &v1742_buffer,&AllocatedSize); /* WARNING: This malloc must be done after the digitizer programming */
  if (ret) {
    ErrCode = ERR_MALLOC;
    return ErrCode;
  }
  
  //if (WDcfg.TestPattern) CAEN_DGTZ_DisableDRS4Correction(v1742_handle);
  //else CAEN_DGTZ_EnableDRS4Correction(v1742_handle);

  /* if (WDrun.Restart && WDrun.AcqRun) */

  CAEN_DGTZ_SWStartAcquisition(v1742_handle);

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
/* CheckKeyboardCommands(v1742_handle, &WDrun, WDcfg, BoardInfo); */
/* if (WDrun.Restart) { */
/*     CAEN_DGTZ_SWStopAcquisition(v1742_handle); */
/*     CAEN_DGTZ_FreeReadoutBuffer(&v1742_buffer); */
/*     ClosePlotter(); */
/*     PlotVar = NULL; */
/*     if(WDcfg.Nbit == 8) */
/*         CAEN_DGTZ_FreeEvent(v1742_handle, (void**)&Event8); */
/*     else */
/* 			if (BoardInfo.FamilyCode != CAEN_DGTZ_XX742_FAMILY_CODE) { */
/* 				CAEN_DGTZ_FreeEvent(v1742_handle, (void**)&Event16); */
/* 			} */
/* 			else { */
/* 			    CAEN_DGTZ_FreeEvent(v1742_handle, (void**)&Event742); */
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
/*     CAEN_DGTZ_SendSWtrigger(v1742_handle); */
/* } */
    
/* Wait for interrupt (if enabled) */
/* if (WDcfg.InterruptNumEvents > 0) { */
/*   int32_t boardId; */
/*         int VMEV1742_Handle; */
/*         int InterruptMask = (1 << VME_INTERRUPT_LEVEL); */

/*         BufferSize = 0; */
/*         NumEvents = 0; */
/*         // Interrupt handling */
/*         if (isVMEDevice) */
/*             ret = CAEN_DGTZ_VMEIRQWait (WDcfg.LinkType, WDcfg.LinkNum, WDcfg.ConetNode, InterruptMask, INTERRUPT_TIMEOUT, &VMEV1742_Handle); */
/*         else */
/*             ret = CAEN_DGTZ_IRQWait(v1742_handle, INTERRUPT_TIMEOUT); */
/*         if (ret == CAEN_DGTZ_Timeout)  // No active interrupt requests */
/*             goto InterruptTimeout; */
/*         if (ret != CAEN_DGTZ_Success)  { */
/*             ErrCode = ERR_INTERRUPT; */
/*             goto QuitProgram; */
/*         } */
/*         // Interrupt Ack */
/*         if (isVMEDevice) { */
/*             ret = CAEN_DGTZ_VMEIACKCycle(VMEV1742_Handle, VME_INTERRUPT_LEVEL, &boardId); */
/*             if ((ret != CAEN_DGTZ_Success) || (boardId != VME_INTERRUPT_STATUS_ID)) { */
/*                 goto InterruptTimeout; */
/*             } else { */
/*                 if (INTERRUPT_MODE == CAEN_DGTZ_IRQ_MODE_ROAK) */
/*                     ret = CAEN_DGTZ_RearmInterrupt(v1742_handle); */
/* 				} */
/* 			} */
/* 	} */

int read_V1742(unsigned int nevents, std::vector<V1742_Event_t>& events)
{

  CAEN_DGTZ_ErrorCode ret=CAEN_DGTZ_Success;
  ERROR_CODES ErrCode= ERR_NONE;


  int i;
  //, Nb=0, Ne=0;
  uint32_t BufferSize, NumEvents;


  CAEN_DGTZ_EventInfo_t       EventInfo;
  
  /* Read data from the board */
  ret = CAEN_DGTZ_ReadData(v1742_handle, CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT, v1742_buffer, &BufferSize);
  if (ret) {
    
    ErrCode = ERR_READOUT;
    return ErrCode;
  }
  NumEvents = 0;
  if (BufferSize != 0) {
    ret = CAEN_DGTZ_GetNumEvents(v1742_handle, v1742_buffer, BufferSize, &NumEvents);
    if (ret) {
      ErrCode = ERR_READOUT;
      return ErrCode;
    }
  }
  

  if (nevents != NumEvents)
    {
      ErrCode = ERR_MISMATCH_EVENTS;
      return ErrCode;
    }
    
  /* Nb += BufferSize; */
  /* Ne += NumEvents; */
  /* CurrentTime = get_time(); */
  /* ElapsedTime = CurrentTime - PrevRateTime; */
  
  /* nCycles++; */
  /* if (ElapsedTime > 1000) { */
  /*   if (Nb == 0) */
  /*     if (ret == CAEN_DGTZ_Timeout) printf ("Timeout...\n"); else printf("No data...\n"); */
  /*   else */
  /*     printf("Reading at %.2f MB/s (Trg Rate: %.2f Hz)\n", (float)Nb/((float)ElapsedTime*1048.576f), (float)Ne*1000.0f/(float)ElapsedTime); */
  /*   nCycles= 0; */
  /*   Nb = 0; */
  /*   Ne = 0; */
  /*   PrevRateTime = CurrentTime; */
  /* } */

  /* Analyze data */
  for(i = 0; i < (int)NumEvents; i++) {
    /* Get one event from the readout buffer */
    ret = CAEN_DGTZ_GetEventInfo(v1742_handle, v1742_buffer, BufferSize, i, &EventInfo, &v1742_eventPtr);
    if (ret) {
      ErrCode = ERR_EVENT_BUILD;
      return ErrCode;
    }
    /* decode the event */
    if (WDcfg.Nbit == 8) 
      ret = CAEN_DGTZ_DecodeEvent(v1742_handle, v1742_eventPtr, (void**)&Event8);
    else if (BoardInfo.FamilyCode != CAEN_DGTZ_XX742_FAMILY_CODE) {
      ret = CAEN_DGTZ_DecodeEvent(v1742_handle, v1742_eventPtr, (void**)&Event16);
    }
    else {
      ret = CAEN_DGTZ_DecodeEvent(v1742_handle, v1742_eventPtr, (void**)&Event742);
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

  /* //Freeing V1742 memory  after read */
  // Test what happens when enable this. Do we need to malloc again? To be checked
  /* ret = CAEN_DGTZ_FreeReadoutBuffer(&v1742_buffer); */
  /* if (ret) { */
  /*   ErrCode = ERR_FREE_BUFFER; */
  /*   return ErrCode; */
  /* } */
   
  ErrCode = ERR_NONE;
  return ErrCode;
}

int stop_V1742()
{
  /* stop the acquisition */
  CAEN_DGTZ_SWStopAcquisition(v1742_handle);
  
  //  CAEN_DGTZ_FreeReadoutBuffer(&v1742_buffer);
  CAEN_DGTZ_CloseDigitizer(v1742_handle);
  
  return 0;
}
