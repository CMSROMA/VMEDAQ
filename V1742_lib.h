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

#ifndef _WAVEDUMP_H_
#define _WAVEDUMP_H_

#include <vector>
#include <CAENDigitizer.h>


int init_V1742();
int read_V1742(unsigned int nevents, std::vector<CAEN_DGTZ_X742_EVENT_t>& events);
int stop_V1742();

#endif /* _WAVEDUMP__H */
