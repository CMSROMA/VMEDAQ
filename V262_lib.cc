#include "CAENVMElib.h"
#include "CAENVMEtypes.h" 
#include "CAENVMEoslib.h"
#include "V262.h"

unsigned short PulseCh_V262(int32_t BHandle, unsigned short channel)
{
  unsigned short status(1),caenst;
  unsigned long address, DataLong;

  address = V262_ADDRESS + 0x08; /* address of the write register */
  DataLong = (0x1) << channel; 
  //status = vme_write_dt(address, point2data, V262_AM,V262_DS);
  caenst = CAENVME_WriteCycle(BHandle,address,&DataLong,cvA24_U_DATA,cvD16);
  status *= (1-caenst); 
  // DataLong = 0x0 ;
  // //  status = vme_write_dt(address, point2data, V262_AM,V262_DS);
  // caenst = CAENVME_WriteCycle(BHandle,address,&DataLong,cvA24_U_DATA,cvD16);
  // status *= (1-caenst); 
  return(status);
}


unsigned short OutCh_V262(int32_t BHandle, unsigned short channel, unsigned short level)
{
  unsigned short status(1),caenst;
  unsigned long address, DataLong;

  address = V262_ADDRESS + 0x06; /* address of the write register */
  DataLong = level << channel; 
  //status = vme_write_dt(address, point2data, V262_AM,V262_DS);
  caenst = CAENVME_WriteCycle(BHandle,address,&DataLong,cvA24_U_DATA,cvD16);
  status *= (1-caenst); 
  // DataLong = 0x0 ;
  // //  status = vme_write_dt(address, point2data, V262_AM,V262_DS);
  // caenst = CAENVME_WriteCycle(BHandle,address,&DataLong,cvA24_U_DATA,cvD16);
  // status *= (1-caenst); 
  return(status);
}
