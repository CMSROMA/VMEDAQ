#ifndef  _V560_HEADER_
#define  _V560_HEADER_
#include "CAENVMElib.h"
#include "CAENVMEtypes.h" 
#include "CAENVMEoslib.h"

#include <vector>
#include "modules_config.h"

#define  V560_ADDRESS   V560_0_BA /* fissato dai rotary switches sulla scheda */
#define  V560_CHANNEL    16      /* contatori a 32 bit */

#define V560_REG_STATUS       0x58
#define V560_REG_VETO_CLEAR   0x54
#define V560_CLEAR_VME_INTER  0x0c//MM
#define V560_SCALE_INCREMENT  0x56//MM
#define V560_ManEMod_TYPE     0xFC//MM
#define V560_FIXEDCODE        0xFA//MM
#define V560_REG_VETO_SET     0x52
#define V560_REG_CLEAR        0x50
#define V560_REG_TEST         0x56
#define V560_REG_COUNTER0     0x10
#define V560_REG_COUNTER1     0x14
#define V560_REG_COUNTER2     0x18
#define V560_REG_COUNTER3     0x1c
#define V560_REG_COUNTER4     0x20
#define V560_REG_COUNTER5     0x24
#define V560_REG_COUNTER6     0x28
#define V560_REG_COUNTER7     0x2c
#define V560_REG_COUNTER8     0x30
#define V560_REG_COUNTER9     0x34
#define V560_REG_COUNTER10    0x38
#define V560_REG_COUNTER11    0x3c
#define V560_REG_COUNTER12    0x40
#define V560_REG_COUNTER13    0x44
#define V560_REG_COUNTER14    0x48
#define V560_REG_COUNTER15    0x4c


int init_scaler560(int32_t BHandle);
int read_scaler560(int32_t BHandle, int *pScalerData);
std::vector<uint32_t> read_scaler560Vec(int32_t BHandle, int status);
int read_scaler560_single(int32_t BHandle, int *pScalerData, int nchan);
int write_scaler560(int32_t BHandle, int *pScalerData);

#endif
