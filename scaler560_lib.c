#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "scaler560_lib.h"
#include <iostream>
#include <time.h>
#include <sys/time.h>
#include <vector>

using namespace std;

#define scaler560_debug 0
/*-----------------------------------------------------------------------------------*/
  
int init_scaler560(int32_t BHandle){

  int caenst=0,status=1;
  unsigned long address;
  unsigned short DataShort, *pDataShort;
  unsigned long  DataLong,  *pDataLong;

 /* 
    reset dello scaler V560
 */
 pDataShort = &DataShort ;
 pDataLong = &DataLong ;

address = V560_ADDRESS + V560_ManEMod_TYPE ;
 // status = vme_read_dt(address, pDataLong, AD32, D16);
  caenst = CAENVME_ReadCycle(BHandle,address,&DataLong,cvA32_U_DATA,cvD16);
  status = (1-caenst); 
 
 if( status != 1)
   {
     printf("Error reading V560  Manufacturer and Module Type reg-> address=%lx \n",address);
     return(1);
   }
 else{
   printf("V560 Manufacturer and Module Type = %lX \n",DataLong);
 }


 address = V560_ADDRESS + V560_REG_STATUS;
 // status = vme_read_dt(address, pDataLong, AD32, D16);
  caenst = CAENVME_ReadCycle(BHandle,address,&DataLong,cvA32_U_DATA,cvD16);
  status = (1-caenst); 
 
 if( status != 1)
   {
     printf("Error reading V560 status reg-> address=%lx \n",address);
     return(1);
   }
 else{
   printf("V560 status register = %lX \n",DataLong);
 }

 

 address = V560_ADDRESS + V560_REG_CLEAR;
 caenst = status*CAENVME_ReadCycle(BHandle,address,&DataLong,cvA32_U_DATA,cvD16);
 status = (1-caenst);

 address = V560_ADDRESS + V560_REG_VETO_CLEAR;
 //status = vme_read_dt(address, pDataLong, AD32, D16);
 caenst = CAENVME_ReadCycle(BHandle,address,&DataLong,cvA32_U_DATA,cvD16);
 status = (1-caenst);

 //prova del 9 febbraio MM
 // address = V560_ADDRESS + V560_CLEAR_VME_INTER;
 //caenst = status*CAENVME_ReadCycle(BHandle,address,&DataLong,cvA32_U_DATA,cvD16);
 //status = (1-caenst);


   if(status == 0)
     {
       printf("V560 error resetting->  address=%lx \n",address);
       return(2);
     }
 printf("V560 reset and clear\n"); 

 return status;
}

/*------------------------------------------------------------*/

int read_scaler560(int32_t BHandle,int *pdata){

  int status =0;
  int caenst=1;
  
  caenst = CAENVME_ReadCycle(BHandle, V560_ADDRESS + V560_REG_COUNTER0,
			     pdata,cvA32_U_DATA,cvD32);
  caenst += CAENVME_ReadCycle(BHandle,V560_ADDRESS + V560_REG_COUNTER1,
			      (pdata+1), cvA32_U_DATA,cvD32);
  caenst += CAENVME_ReadCycle(BHandle,V560_ADDRESS + V560_REG_COUNTER2,
			      (pdata+2), cvA32_U_DATA,cvD32);
  caenst += CAENVME_ReadCycle(BHandle, V560_ADDRESS + V560_REG_COUNTER3,
			      (pdata+3),cvA32_U_DATA,cvD32);
  caenst += CAENVME_ReadCycle(BHandle, V560_ADDRESS + V560_REG_COUNTER4,
			      (pdata+4),cvA32_U_DATA,cvD32); 
  caenst += CAENVME_ReadCycle(BHandle, V560_ADDRESS + V560_REG_COUNTER5,
			      (pdata+5),  cvA32_U_DATA,cvD32);
  caenst += CAENVME_ReadCycle(BHandle, V560_ADDRESS + V560_REG_COUNTER6,
			      (pdata+6), cvA32_U_DATA,cvD32);
  caenst += CAENVME_ReadCycle(BHandle, V560_ADDRESS + V560_REG_COUNTER7,
			      (pdata+7),cvA32_U_DATA,cvD32); 
  caenst += CAENVME_ReadCycle(BHandle,  V560_ADDRESS + V560_REG_COUNTER8,
			      (pdata+8), cvA32_U_DATA,cvD32);
  caenst += CAENVME_ReadCycle(BHandle, V560_ADDRESS + V560_REG_COUNTER9,
			      (pdata+9), cvA32_U_DATA,cvD32);
  caenst += CAENVME_ReadCycle(BHandle, V560_ADDRESS + V560_REG_COUNTER10,
			      (pdata+10),cvA32_U_DATA,cvD32);
  caenst += CAENVME_ReadCycle(BHandle, V560_ADDRESS + V560_REG_COUNTER11,
			      (pdata+11),cvA32_U_DATA,cvD32);
  caenst += CAENVME_ReadCycle(BHandle, V560_ADDRESS + V560_REG_COUNTER12,
			      (pdata+12),cvA32_U_DATA,cvD32);
  caenst += CAENVME_ReadCycle(BHandle, V560_ADDRESS + V560_REG_COUNTER13,
			      (pdata+13),cvA32_U_DATA,cvD32);
  caenst += CAENVME_ReadCycle(BHandle, V560_ADDRESS + V560_REG_COUNTER14,
			      (pdata+14),cvA32_U_DATA,cvD32);
  caenst += CAENVME_ReadCycle(BHandle, V560_ADDRESS + V560_REG_COUNTER15,
			      (pdata+15),cvA32_U_DATA,cvD32);
  
  status = (1-caenst);
  
  if(status!=1){
    printf("Error reading scaler V560\n");
  }
  
  if (scaler560_debug)
    {
      for (int i(0);i<16;++i)
	std::cout << "V560:: channel " << i << " has " << *(pdata+i) << " counts" << std::endl;
    }
  
  return status;
}

/*------------------------------------------------------------*/

vector<uint32_t> read_scaler560Vec(int32_t BHandle, int status){

  int caenst=1;
  vector<uint32_t> data;
  data.clear();
  unsigned long address,DataLong;
  for(int i=0; i<V560_CHANNEL; i++) {    
    address=   V560_ADDRESS + V560_REG_COUNTER0 + i*0x04 ;
    caenst *= CAENVME_ReadCycle(BHandle,address,&DataLong,cvA32_U_DATA,cvD32);
    data.push_back((int)DataLong);
  }  

  status = (1-caenst);
  
  if(status!=1){
    printf("Error reading scaler V560\n");
  }
  
  if (scaler560_debug)
    {
      for (unsigned int i(0);i<data.size();++i)
	std::cout << "V560:: channel " << i << " has " << data[i] << " counts" << std::endl;
    }

  return data;
}

/*------------------------------------------------------------*/

int read_scaler560_single(int32_t BHandle, int *pdata, int nchan){


  int caenst=0,status =1;
  unsigned long address,DataLong;

  if( ( nchan<0 ) || ( nchan>V560_CHANNEL ) ) {
    return 0;
  }
  address=   V560_ADDRESS + V560_REG_COUNTER0 + nchan*0x04 ;
  // status *= vme_read_dt( address,  pdata, AD32, D32);
  caenst += CAENVME_ReadCycle(BHandle,address,&DataLong,cvA32_U_DATA,cvD16);
  status = (1-caenst);
  if(status!=1){
    printf("Error reading scaler V560; chan = %d\n", nchan);
  }
  
  return status;
}

/*------------------------------------------------------------*/

int write_scaler560(int32_t BHandle,int *pdata){
  
  int status =0;
  int caenst=1;
  unsigned long address,DataLong;
  address = V560_ADDRESS + V560_SCALE_INCREMENT;
  caenst = CAENVME_WriteCycle(BHandle,address,&DataLong,cvA32_U_DATA,cvD16);
  //printf("V560 scruittura forse %d  \n",DataLong); 
 
  
   status = (1-caenst);
  
  if(status!=1){
    printf("Error reading scaler V560\n");
  }
  
  return status;
}
