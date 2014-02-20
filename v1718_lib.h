#include "CAENVMElib.h"
#include "CAENVMEtypes.h" 
#include "CAENVMEoslib.h"
#define DAQ_BUSY_ON      0x1
#define DAQ_BUSY_OFF     0x0

int init_1718(int32_t BHandle);
int init_scaler_1718(int32_t BHandle);
int init_pulser_1718(int32_t BHandle);
int trigger_scaler_1718(int32_t BHandle, bool *ptrig);
int print_configuration_1718(int32_t BHandle);
int set_configuration_1718(int32_t BHandle);
int read_scaler_1718(int32_t BHandle);
int reset_nim_scaler_1718(int32_t BHandle);
int setbusy_1718(int32_t BHandle,int command);
int clearbusy_new_1718(int32_t BHandle);
int read_trig_1718(int32_t BHandle, bool *trig);


