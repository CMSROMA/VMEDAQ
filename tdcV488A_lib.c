#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
/*
#include "my_vmeio.h"
#include "universe.h"
#include "my_vmeint.h"
*/

#include "CAENVMElib.h"
#include "CAENVMEtypes.h" 
#include "CAENVMEoslib.h"
#include "tdcV488A_lib.h"
#include "printbit.h"


/*-------------------------------------------------*/
unsigned short init_tdcV488A(int32_t BHandle)
{
  short status=1;
  int caenst=0;
  unsigned long address;
  unsigned long  DataLong;

  /* 
     reset of the V488A TDC
  */
  address = V488A_ADDRESS + V488A_REG_RESET;
  caenst = CAENVME_WriteCycle(BHandle,address,&DataLong,cvA32_U_DATA,cvD16);
  status *= (1-caenst); 

  //  printf("Init of the V488A TDC \n");
  /*
    V488A TDC initialization
  */
  DataLong = 0;
  DataLong = DataLong | 0xf000; // bit 15 = 1/0 common stop/start mode
  DataLong = DataLong | 0xff;   // enable all the 8 channel
  address = V488A_ADDRESS + V488A_REG_CONTROL;
  caenst = CAENVME_WriteCycle(BHandle,address,&DataLong,cvA32_U_DATA,cvD16);
  status *= (1-caenst); 
  if( status != 1)
    {
      printf("Error writing V488A ctrl reg-> address = %lx \n",address);
      return(2);
    }

  address = V488A_ADDRESS + V488A_REG_FF;
  caenst = CAENVME_WriteCycle(BHandle,address,&DataLong,cvA32_U_DATA,cvD16);
  status *= (1-caenst); 

  /*
    setting the maximum signal amplitude accepted by the High and Low Threshold
  */  
  DataLong = V488A_THRH;
  address = V488A_ADDRESS + V488A_REG_THRH;
  caenst = CAENVME_WriteCycle(BHandle,address,&DataLong,cvA32_U_DATA,cvD16);
  status *= (1-caenst); 
  DataLong = V488A_THRL;
  address = V488A_ADDRESS + V488A_REG_THRL;
  caenst = CAENVME_WriteCycle(BHandle,address,&DataLong,cvA32_U_DATA,cvD16);
  status *= (1-caenst); 

  /*
    setting the time range (90 ns -> 770 ns)
  */
  DataLong = V488A_TIME_RANGE;
  address = V488A_ADDRESS + V488A_REG_RANGE;
  caenst = CAENVME_WriteCycle(BHandle,address,&DataLong,cvA32_U_DATA,cvD16);
  status *= (1-caenst); 
  return(status);
}

/*-------------------------------------------------*/
unsigned short readEventV488A(int32_t BHandle,short unsigned int *pV488A_data)
{

  unsigned long address;
  unsigned long  DataLong;
  short int status=1, caenst=0;
  int ii, data, nch;
  int EvCounter=0, TotChannel=0;
  /*
    check that the Output buffer is not Empty
*/
  for(ii=0;ii<V488A_CHANNEL;ii++){ 
    *(pV488A_data+ii)=0; 
  }
 
  if(V488A_DEBUG!=0) {
    printf("\n\n Enter in read TDC V488A \n");
    address = V488A_ADDRESS + V488A_REG_CONTROL;
    //    status = vme_read_dt(address, &DataLong, AD32, D16);
    caenst = CAENVME_ReadCycle(BHandle,address,&DataLong,cvA32_U_DATA,cvD16);
    status *= (1-caenst); 
    if( (status!=1)|| (DataLong & 0x4000 ) ==0) {
      printf("Empty buffer! Register =\n");
      //      printbit_long(DataLong);
      return 0;
    }
  }
  /*
    Read the header of the events
  */
  address = V488A_ADDRESS + V488A_REG_OUTBUF;
  //  status = vme_read_dt(address, &DataLong, AD32, D16);
  caenst = CAENVME_ReadCycle(BHandle,address,&DataLong,cvA32_U_DATA,cvD16);
  status *= (1-caenst);

 
  if(V488A_DEBUG!=0){
    printf("Should be event header \n");
    //    printbit_long(DataLong);
  }
  if( (status!=1) || (DataLong & 0x8000)!=0x8000){
    printf("Error reading V488A -> No Header? Status = %d, Datalong=%lx EvCounter= %d\n",
	   status,DataLong,EvCounter);
     return 0;
  }
  EvCounter = DataLong & 0xfff;
  TotChannel = ( ( DataLong & 0x7000 ) >> 12) + 1;
  if(V488A_DEBUG!=0) printf("EvCounter = %ld ; multiplicity = %d\n",
			    (DataLong&0xfff),TotChannel);
  if(TotChannel>V488A_CHANNEL){
    printf("Error reading V488A -> Nchannel > 8 !! EvCounter= %d\n",EvCounter);
    return 0;
  }
  for(ii=0;ii<TotChannel;ii++){
    //    status = vme_read_dt(address, &DataLong, AD32, D16);
    caenst = CAENVME_ReadCycle(BHandle,address,&DataLong,cvA32_U_DATA,cvD16);
    status *= (1-caenst); 
    if(V488A_DEBUG!=0){
      printf("Row data of the channel %d EvCounter= %d\n",ii,EvCounter);
      //      printbit_long(DataLong);
    }
    if( (status!= 1) || ( (DataLong&0x8000)!=0 ) ){
      printf("\nV488A Read Error > Datalong=%lx HeaderFlag =%ld EvCounter= %d\n",
	     DataLong,(DataLong&0x8000)>>15,EvCounter);
      printf("tot chan= %d Error reading chan %d \n",TotChannel,ii);  
      if(V488A_DEBUG!=0) status = dump_tdcV488A(BHandle);
      //    status = dump_tdcV488A(BHandle);
            status = init_tdcV488A(BHandle);
            return 1;


    }

    nch = (DataLong & 0x7000) >> 12;
    data = DataLong & 0xfff ;
    if(V488A_DEBUG!=0) printf("data= %d  nch = %d\n",data,nch);
    *(pV488A_data+nch) = data;
  }
  return (status);
}

/*-------------------------------------------------*/
vector<int> readFastEventV488A(int32_t BHandle, short daq_status)
{

  unsigned long address;
  unsigned long  DataLong;
  short int status=1, caenst=0;
  int ii, data, nch;
  int EvCounter=0, TotChannel=0;

  vector<int> tmpData;
  tmpData.clear();
  /*
    check that the Output buffer is not Empty
*/

  if(V488A_DEBUG!=0) {
    printf("\n\n Enter in read TDC V488A \n");
    address = V488A_ADDRESS + V488A_REG_CONTROL;
    //    status = vme_read_dt(address, &DataLong, AD32, D16);
    caenst = CAENVME_ReadCycle(BHandle,address,&DataLong,cvA32_U_DATA,cvD16);
    status *= (1-caenst); 
    if( (status!=1)|| (DataLong & 0x4000 ) ==0) {
      printf("Empty buffer! Register =\n");
      //      printbit_long(DataLong);
      daq_status = 0;
      return tmpData;
    }
  }
  /*
    Read the header of the events
  */
  address = V488A_ADDRESS + V488A_REG_OUTBUF;
  //  status = vme_read_dt(address, &DataLong, AD32, D16);
  caenst = CAENVME_ReadCycle(BHandle,address,&DataLong,cvA32_U_DATA,cvD16);
  status *= (1-caenst);

 
  if(V488A_DEBUG!=0){
    printf("Should be event header \n");
    //    printbit_long(DataLong);
  }
  if( (status!=1) || (DataLong & 0x8000)!=0x8000){
    printf("Error reading V488A -> No Header? Status = %d, Datalong=%lx EvCounter= %d\n",
	   status,DataLong,EvCounter);
     daq_status = 0;
     return tmpData;
  }
  EvCounter = DataLong & 0xfff;
  TotChannel = ( ( DataLong & 0x7000 ) >> 12) + 1;

  tmpData.push_back(DataLong);

  if(V488A_DEBUG!=0) printf("EvCounter = %ld ; multiplicity = %d\n",
			    (DataLong&0xfff),TotChannel);
  if(TotChannel>V488A_CHANNEL){
    printf("Error reading V488A -> Nchannel > 8 !! EvCounter= %d\n",EvCounter);
    daq_status = 0;
    return tmpData;
  }
  for(ii=0;ii<TotChannel;ii++){
    //    status = vme_read_dt(address, &DataLong, AD32, D16);
    caenst = CAENVME_ReadCycle(BHandle,address,&DataLong,cvA32_U_DATA,cvD16);
    status *= (1-caenst); 
    if(V488A_DEBUG!=0){
      printf("Row data of the channel %d EvCounter= %d\n",ii,EvCounter);
      //      printbit_long(DataLong);
    }
    if( (status!= 1) || ( (DataLong&0x8000)!=0 ) ){
      printf("\nV488A Read Error > Datalong=%lx HeaderFlag =%ld EvCounter= %d\n",
	     DataLong,(DataLong&0x8000)>>15,EvCounter);
      printf("tot chan= %d Error reading chan %d \n",TotChannel,ii);  
      if(V488A_DEBUG!=0) status = dump_tdcV488A(BHandle);
      //    status = dump_tdcV488A(BHandle);
      status = init_tdcV488A(BHandle);
      daq_status = 1;
      return tmpData;

    }

    nch = (DataLong & 0x7000) >> 12;
    data = DataLong & 0xfff ;
    if(V488A_DEBUG!=0) printf("data= %d  nch = %d\n",data,nch);

    tmpData.push_back(data);
    tmpData.push_back(nch);

  }

  daq_status = status;

  return tmpData;
}

/*-------------------------------------------------*/
unsigned short dump_tdcV488A(int32_t BHandle)
{
  short status=1;
  int caenst=0;
  unsigned long address;
  unsigned long  DataLong;

 
  address = V488A_ADDRESS + V488A_REG_FF;     
  caenst = CAENVME_ReadCycle(BHandle,address,&DataLong,cvA32_U_DATA,cvD16);
  status *= (1-caenst); 
  printf("\n\nFF Reg: %lx", DataLong);	
  address = V488A_ADDRESS + V488A_REG_OUTBUF;
  caenst = CAENVME_ReadCycle(BHandle,address,&DataLong,cvA32_U_DATA,cvD16);	
  status *= (1-caenst); 
  printf("\n\nOutBuf: %lx",DataLong); 
  address = V488A_ADDRESS + V488A_REG_CONTROL;     
  caenst = CAENVME_ReadCycle(BHandle,address,&DataLong,cvA32_U_DATA,cvD16);
  status *= (1-caenst); 
  printf("\n\nCtrl Reg: %lx", DataLong);
  address = V488A_ADDRESS + V488A_REG_RANGE;     
  caenst = CAENVME_ReadCycle(BHandle,address,&DataLong,cvA32_U_DATA,cvD16);
  status *= (1-caenst); 
  printf("\n\nRange Reg: %lx\n\n", DataLong);

  return(status);
}

/*-------------------------------------------------*/
vector<int> readFastNEventV488A(int32_t BHandle, short daq_status, int nevents, vector<int> &outW)
{

  unsigned long address;
  unsigned long  DataLong;
  short int status=1, caenst=0;
  int ii, data, nch;
  unsigned int EvCounter=0, TotChannel=0;

  vector<int> tmpData;
  tmpData.clear();

  
  /*
    check that the Output buffer is not Empty
  */
  if(V488A_DEBUG!=0) {
    printf("\n\n Enter in read TDC V488A \n");
    address = V488A_ADDRESS + V488A_REG_CONTROL;
    caenst = CAENVME_ReadCycle(BHandle,address,&DataLong,cvA32_U_DATA,cvD16);
    status *= (1-caenst); 
    if( (status!=1)|| (DataLong & 0x4000 ) ==0) {
      printf("Empty buffer! Register =\n");
      //      printbit_long(DataLong);
      daq_status = 0;
      return tmpData;
    }
  }

  int tmpev(0),tmpW(0);
  /*
    Read the header of the events
  */
  while(tmpev<nevents) {

    tmpW = 0;
    
    address = V488A_ADDRESS + V488A_REG_OUTBUF;
    caenst = CAENVME_ReadCycle(BHandle,address,&DataLong,cvA32_U_DATA,cvD16);
    status *= (1-caenst);
    
    
    if(V488A_DEBUG!=0){
      printf("Should be event header \n");
      //      printbit_long(DataLong);
    }
    if( (status!=1) || (DataLong & 0x8000)!=0x8000){
      printf("Error reading V488A -> No Header? Status = %d, Datalong=%lx EvCounter= %d\n",
	     status,DataLong,EvCounter);
      daq_status = 0;
      return tmpData;
    }
    EvCounter = DataLong & 0xfff;
    TotChannel = ( ( DataLong & 0x7000 ) >> 12) + 1;
    
    tmpData.push_back(DataLong);
    tmpW++;
    if(V488A_DEBUG!=0) printf("EvCounter = %ld ; multiplicity = %d\n",
			      (DataLong&0xfff),(int)TotChannel);
    if(TotChannel>V488A_CHANNEL){
      printf("Error reading V488A -> Nchannel > 8 !! EvCounter= %d\n",EvCounter);
      daq_status = 0;
      return tmpData;
    }
    for(ii=0;ii<(int)TotChannel;ii++){
      //    status = vme_read_dt(address, &DataLong, AD32, D16);
      caenst = CAENVME_ReadCycle(BHandle,address,&DataLong,cvA32_U_DATA,cvD16);
      status *= (1-caenst); 
      if(V488A_DEBUG!=0){
	printf("Row data of the channel %d EvCounter= %d\n",ii,EvCounter);
	//	printbit_long(DataLong);
      }
      if( (status!= 1) || ( (DataLong&0x8000)!=0 ) ){
	printf("\nV488A Read Error > Datalong=%lx HeaderFlag =%ld EvCounter= %d\n",
	       DataLong,(DataLong&0x8000)>>15,EvCounter);
	printf("tot chan= %d Error reading chan %d \n",TotChannel,ii);  
	if(V488A_DEBUG!=0) status = dump_tdcV488A(BHandle);
	//    status = dump_tdcV488A(BHandle);
	status = init_tdcV488A(BHandle);
	daq_status = 1;
	return tmpData;
	
      }
      
      nch = (DataLong & 0x7000) >> 12;
      data = DataLong & 0xfff ;
      if(V488A_DEBUG!=0) printf("data= %d  nch = %d\n",data,nch);
      
      tmpData.push_back(data); tmpW++;
      tmpData.push_back(nch); tmpW++;
    }
    outW.push_back(tmpW);
    tmpev++;
  }

  daq_status = status;

  return tmpData;
}

/*-------------------------------------------------*/
vector<int> readBltNEventV488A(int32_t BHandle, short daq_status, int nevents, vector<int> &outW)
{

  unsigned long address;
  short int status=1, caenst=0;
  int data, nch;
  unsigned int EvCounter=0, TotChannel=0;

  vector<int> tmpData;
  tmpData.clear();

  int nbytes_tran = 0;
  int maxW = 9; int wr;
  unsigned short dataV[maxW*nevents];
  int tmpW(0), idV(0);
  /*
    Read the header of the events
  */

  idV = 0; while(idV<(maxW)*nevents) { dataV[idV] = 0; idV++; }
  address = V488A_ADDRESS + V488A_REG_OUTBUF;
  wr = maxW*2*nevents; 
  caenst = CAENVME_BLTReadCycle(BHandle,address,dataV,wr,
				cvA32_U_DATA,cvD16,&nbytes_tran);
  status *= (1-caenst);
  daq_status = status;

  if(V488A_DEBUG!=0) printf("rnter:: %d %lx %d\n",nevents,address,TotChannel);

  idV = 0; int nhea(0);
  while(idV<(maxW)*nevents && (nhea<nevents || tmpW<2*(int)TotChannel+1)) { 
    if((dataV[idV] & 0x8000)==0x8000) {
      if(idV != 0)   outW.push_back(tmpW);
      tmpW = 0;
      EvCounter = dataV[idV] & 0xfff;
      if(V488A_DEBUG!=0) printf("Ev:: %d\n",EvCounter);
      TotChannel = ( ( dataV[idV] & 0x7000 ) >> 12) + 1;
      if(V488A_DEBUG!=0) printf("HEADER:: %d %d %d\n",dataV[idV],idV,TotChannel); 
      nhea++;
      tmpData.push_back(dataV[idV]);
      tmpW++;
    } else  {
      if(tmpW<2*(int)TotChannel+1 && TotChannel) {

	nch = (dataV[idV] & 0x7000) >> 12;
	data = dataV[idV] & 0xfff ;
       	if(V488A_DEBUG!=0)
	  printf("data= %d  nch = %d\n",data,nch);
	
	tmpData.push_back(data); tmpW++;
	tmpData.push_back(nch); tmpW++;

      }
    } 
    
    idV++; 
  }
  if(tmpW) 
    outW.push_back(tmpW);

  return tmpData;
}

