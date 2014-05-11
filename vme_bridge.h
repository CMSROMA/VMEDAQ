#include "CAENVMElib.h"
#include "CAENVMEtypes.h" 
#include "CAENVMEoslib.h"

#define VME_DEVICE_ID 1 //Beware this number can flip from 0 to 1 after reboot. Change it in case of problems connecting
#define VME_LINK_ID 0

int bridge_init(int32_t &BHandle);
int bridge_deinit(int32_t BHandle);
