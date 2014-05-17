#include <vector>
#include "modules_config.h"

unsigned short init_tdc1190(int32_t BHandle);
//unsigned short read_tdc1190(int32_t BHandle);
short opwriteTDC(int32_t BHandle, unsigned long addr, unsigned short data);
unsigned short opreadTDC(int32_t BHandle, unsigned long addr, unsigned short *data);
std::vector<int> readEventTDC(int32_t BHandle, int idB, int status);
std::vector<int> readNEventsTDC(int32_t BHandle, int idB, int status, int nevents, std::vector<int> &outW);
std::vector<int> readFastNEventsTDC(int32_t BHandle, int idB, int status, int nevents, std::vector<int> &outW, bool t1290);

#define NUMBOARDS	        1
#define TDC1190_ADDRESS       	V1290_0_BA
#define TDC1190_ADDRESS2 	0xA0000000
#define TDC1190_CHANNEL         32

#define MICROHANDREG            0x1030
#define MICROREG                0x102E
#define STATUSREGADD            0x1002
#define CONREGADD               0x1000

#define td1190_debug              0
