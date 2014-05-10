#include "CAENVMElib.h"
#include "CAENVMEtypes.h" 
#include "CAENVMEoslib.h"

#define VME_DEVICE_ID 1
#define VME_LINK_ID 1

int bridge_init(int32_t &BHandle);
int bridge_deinit(int32_t BHandle);
