#include "CAENVMElib.h"
#include "CAENVMEtypes.h" 
#include "CAENVMEoslib.h"
#include "CAENComm.h"
#include "V262_CAENComm.h"

unsigned short PulseCh_V262_CAENCOMM(int32_t BHandle, unsigned short channel)
{
  int status(1),caenst;
  unsigned long address;
  uint16_t DataLong;

  address =  0x08; /* address of the write register */
  DataLong = (0x1) << channel; 
  //status = vme_write_dt(address, point2data, V262_CAENCOMM_AM,V262_CAENCOMM_DS);
  caenst = CAENComm_Write16(BHandle,address,DataLong);
  status *= (1-caenst); 
  // DataLong = 0x0 ;
  // //  status = vme_write_dt(address, point2data, V262_CAENCOMM_AM,V262_CAENCOMM_DS);
  // caenst = CAENComm_Write16(BHandle,address,&DataLong);
  // status *= (1-caenst); 
  return(status);
}


unsigned short OutCh_V262_CAENCOMM(int32_t BHandle, unsigned short channel, unsigned short level)
{
  int status(1),caenst;
  unsigned long address;
  uint16_t DataLong;

  address =  0x06; /* address of the write register */
  DataLong = level << channel; 
  //status = vme_write_dt(address, point2data, V262_CAENCOMM_AM,V262_CAENCOMM_DS);
  caenst = CAENComm_Write16(BHandle,address,DataLong);
  printf("AAA %d\n",caenst);
  status *= (1-caenst); 
  // DataLong = 0x0 ;
  // //  status = vme_write_dt(address, point2data, V262_CAENCOMM_AM,V262_CAENCOMM_DS);
  // caenst = CAENComm_Write16(BHandle,address,&DataLong);
  // status *= (1-caenst); 
  return(status);
}
