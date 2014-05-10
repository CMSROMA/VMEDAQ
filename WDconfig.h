/******************************************************************************
* 
* CAEN SpA - Front End Division
* Via Vetraia, 11 - 55049 - Viareggio ITALY
* +390594388398 - www.caen.it
*
***************************************************************************//**
* \note TERMS OF USE:
* This program is free software; you can redistribute it and/or modify it under
* the terms of the GNU General Public License as published by the Free Software
* Foundation. This program is distributed in the hope that it will be useful, 
* but WITHOUT ANY WARRANTY; without even the implied warranty of 
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. The user relies on the 
* software, documentation and results solely at his own risk.
******************************************************************************/
#ifndef _WDCONFIG_H_
#define _WDCONFIG_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "V1742_lib.h"

#define DEFAULT_WDCONFIG_FILE  "/etc/wavedump/WaveDumpConfig.txt"

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
  int PostTrigger;
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
*  Functions
*  ########################################################################### */

/*! \fn      int ParseConfigFile(FILE *f_ini, WaveDumpConfig_t *WDcfg) 
*   \brief   Read the configuration file and set the WaveDump paremeters
*            
*   \param   f_ini        Pointer to the config file
*   \param   WDcfg:   Pointer to the WaveDumpConfig data structure
*   \return  0 = Success; negative numbers are error codes
*/
int ParseConfigFile(FILE *f_ini, WaveDumpConfig_t *WDcfg);


#endif


