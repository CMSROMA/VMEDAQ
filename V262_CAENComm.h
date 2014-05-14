#include <fstream>
#include "modules_config.h"
#include <vector>

#ifndef _DEF_V262_CAENCOMM
#define _DEF_V262_CAENCOMM
using namespace std;

unsigned short PulseCh_V262_CAENCOMM(int32_t BHandle,unsigned short channel);
unsigned short OutCh_V262_CAENCOMM(int32_t BHandle,unsigned short channel, unsigned short level);

//#define V262_CAENCOMM_ADDRESS    V262_0_BA /* fissato dai rotary switches sulla scheda */
#define V262_CAENCOMM_debug            0

#endif
