#include <fstream>
#include "modules_config.h"
#include <vector>

#ifndef _DEF_V262_
#define _DEF_V262_
using namespace std;

unsigned short PulseCh_V262(int32_t BHandle,unsigned short channel);
unsigned short OutCh_V262(int32_t BHandle,unsigned short channel, unsigned short level);

#define V262_ADDRESS    V262_0_BA /* fissato dai rotary switches sulla scheda */
#define V262_debug            0

#endif
