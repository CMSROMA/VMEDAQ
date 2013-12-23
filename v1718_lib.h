#include "CAENVMElib.h"
#include "CAENVMEtypes.h" 
#include "CAENVMEoslib.h"

int init_1718(int32_t BHandle);
int init_scaler_1718(int32_t BHandle);
int init_pulser_1718(int32_t BHandle);
int trigger_scaler_1718(int32_t BHandle, bool *ptrig);
int read_scaler_1718(int32_t BHandle);
int clearbusy_1718(int32_t BHandle);
int clearbusy_new_1718(int32_t BHandle);
int read_trig_1718(int32_t BHandle, bool *trig);
