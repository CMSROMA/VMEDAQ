#include <vector>

#ifndef _DEF_V488A_
#define _DEF_V488A_

using namespace std;

unsigned short init_tdcV488A(int32_t BHandle);
unsigned short dump_tdcV488A(int32_t BHandle);
unsigned short readEventV488A(int32_t BHandle,short unsigned int *);
vector<int> readFastEventV488A(int32_t BHandle, short daq_status);
vector<int> readFastNEventV488A(int32_t BHandle, short daq_status, int nevents, vector<int> &outW);
vector<int> readBltNEventV488A(int32_t BHandle, short daq_status, int nevents, vector<int> &outW);

#define  V488A_ADDRESS   0xE0000 /* fissato dai rotary switches sulla scheda */
#define  V488A_CHANNEL   8       /* contatori a 32 bit */

#define V488A_REG_HF          0x1E
#define V488A_REG_RESET       0x1C
#define V488A_REG_CONTROL     0x1A
#define V488A_REG_OUTBUF      0x18
#define V488A_REG_FF          0x16
#define V488A_REG_RANGE       0x14
#define V488A_REG_THRH        0x12
#define V488A_REG_THRL        0x10
#define V488A_REG_INTERRUPT   0x00

#define V488A_THRL            0x01
#define V488A_THRH            0xc7
#define V488A_TIME_RANGE      0xe0 //e0 200ps/count, d0 125 ps/count, c0 100 ps/count, b0 71 ps/count

#define V488A_DEBUG            0

#endif
