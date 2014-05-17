#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/time.h> 
#include <time.h> 
#include "CAENVMElib.h"
#include "CAENComm.h"
#include "CAENVMEtypes.h" 
#include "CAENVMEoslib.h"
#include "adc792_lib_CAENComm.h"
#include <iostream>
#include <vector>

using namespace std;

std::vector<unsigned long> adcaddrs;

void print_adc792_CAENCOMM_debug_word(uint32_t word)
{
  short dt_type = word>>24 & 0x7;
  if (dt_type==0)
    {
      // adc_chan = data>>17 & 0xF; //For 792N [bit 17-20]
      short adc_chan = word>>16 & 0x1F; //For 792 [bit 16-20]
      
      unsigned int adc_value = word & 0xFFF; // adc data [bit 0-11]
      bool adc_overflow = (word>>12) & 0x1; // overflow bit [bit 12]
      bool adc_underthreshold = (word>>13) & 0x1; // under threshold bit [bit 13]
      std::cout << "raw " << word << "\tchannel " << adc_chan << "\tvalue " << adc_value << "\toverflow " << adc_overflow << "\tunderthreshold " << adc_underthreshold << std::endl;
    }
}

void check_adc792_CAENCOMM_status_afterRead(int32_t BHandle)
{
  uint32_t address =  0;
  uint16_t data;
  int status=1;

  int caenst = CAENComm_Read16(BHandle,address+adc792_CAENCOMM_shift.statusreg2,&data);
  status *= (1-caenst);

  if(status != 1) {
    printf("Could not read statusreg2\n");
  }
  
  bool full = data &  adc792_CAENCOMM_bitmask.full; 
  bool empty = data &  adc792_CAENCOMM_bitmask.empty;	

   if(full || !empty || status!=1 ) { 
     std::cout << "FULL " << full << " !EMPTY " << !empty << " STATUS " << status << std::endl;
     //Try a dataReset
     int dtRst=dataReset792_CAENCOMM(BHandle);
     if (dtRst!=1)
       {
	 //Now try a full software reset
	 softReset792_CAENCOMM(BHandle);
	 //Reinialize the module
	 init_adc792_CAENCOMM(BHandle);
       }
   }

}

unsigned short init_adc792_CAENCOMM(int32_t BHandle) {

  int status=1;
  unsigned long address;
  uint16_t  DataLong;
  int nZS = 1; int caenst;

  adcaddrs.clear();
  adcaddrs.push_back(V792N_CAENCOMM_ADDRESS);
  if(NUMADBOARDS >= 2) adcaddrs.push_back(V792N_CAENCOMM_ADDRESS2);
  if(NUMADBOARDS >= 3) adcaddrs.push_back(V792N_CAENCOMM_ADDRESS3);

  /* //Initialize all the boards */
  /* if (idB>adcaddrs.size()-1) */
  /*   { */
  /*     std::cout << "ADC CARD requested " << idB << " not available" << std::endl; */
  /*     return 2; */
  /*   } */

  /* QDC Reset */
  address =  0x1000;
  caenst = CAENComm_Read16(BHandle,address,&DataLong);
  status *= (1-caenst); 
  printf("HELLO!!\n");
  if(status != 1) {
    printf("Error READING V792N_CAENCOMM firmware -> address= %ld \n",address);
    return status;
  }
  else {
    if(adc792_CAENCOMM_debug) printf("V792N_CAENCOMM firmware is version:  %d \n",DataLong);
  }

    //Bit set register
    address =  0x1006;
    DataLong = 0x80; //Issue a software reset. To be cleared with bit clear register access
    caenst = CAENComm_Write16(BHandle,address,DataLong);
    status *= (1-caenst); 
    caenst = CAENComm_Read16(BHandle,address,&DataLong);
    status *= (1-caenst); 
    if(status != 1) {
      printf("ERROR: Bit Set Register read: %d\n", DataLong);
      return status;
    }
    
    //Control Register: enable BLK_end
    address =  0x1010;
    DataLong = 0x4; //Sets bit 2 to 1 [enable blkend]

    caenst = CAENComm_Write16(BHandle,address,DataLong);
    status *= (1-caenst); 
    caenst = CAENComm_Read16(BHandle,address,&DataLong);
    status *= (1-caenst); 
    if(status != 1) {
      printf("ERROR: Bit Set Register read: %d\n", DataLong);
      return status;
    }
    
    //Bit clear register
    address =  0x1008;
    DataLong = 0x80; //Release the software reset. 
    caenst = CAENComm_Write16(BHandle,address,DataLong);
    status *= (1-caenst); 
    caenst = CAENComm_Read16(BHandle,address,&DataLong);
    status *= (1-caenst); 
    if(status != 1) {
      printf("ERROR: Bit Clear Register read: %d\n", DataLong);
      return status;
    }
    
    //Bit set register 2
    //Enable/disable zero suppression
    if(nZS) {
      address =  0x1032;
      DataLong = 0x1018; //Disable Zero Suppression + disable overfl
      caenst = CAENComm_Write16(BHandle,address,DataLong);
      status *= (1-caenst); 
      if(status != 1) {
	printf("ERROE: Could not disable ZS: %d\n", DataLong);
	return status;
      }
    } else {
      address =  0x1032;
      DataLong = 0x1008; //Enable Zero Suppression + disable overfl
      caenst = CAENComm_Write16(BHandle,address,DataLong);
      status *= (1-caenst); 
      if(status != 1) {
	printf("ERROR: Could not enable ZS: %d\n", DataLong);
	return status;
      }
    }
    
    //Set the thresholds.
    for(int i=0; i< V792N_CAENCOMM_CHANNEL; i++) {
      //      address =  0x1080 +4*i; //every 4 for V792N_CAENCOMM
      address =  0x1080 +2*i; //every 2 for V792
      if(adc792_CAENCOMM_debug) printf("Channel %d Address : %ld\n",i,address);
      DataLong = 0x0; //Threshold
      caenst = CAENComm_Write16(BHandle,address,DataLong);
      status *= (1-caenst); 
      if(adc792_CAENCOMM_debug) printf("Iped register read: %d\n", DataLong);
      if(status != 1) {
	printf("ERROR: Threshold register read: %d\n", DataLong);
	return status;
      }
    }
    
    //Set the Iped value to XX value [180, defaults; >60 for coupled channels]
    address =  0x1060;
    //  status = vme_read_dt(address, &DataLong, AD32, D16);
    caenst = CAENComm_Read16(BHandle,address,&DataLong);
    status *= (1-caenst); 
    //    if(adc792_CAENCOMM_debug) printf("Iped register read: %d\n", DataLong);
    // DataLong = 0xFF; //iped value
    DataLong = 0x60; //iped value
    caenst = CAENComm_Write16(BHandle,address,DataLong);
    status *= (1-caenst); 
    printf("Iped register read: %d\n", DataLong);
    
    
     if(status != 1) {
      printf("ERROR: Iped register read: %d\n", DataLong);
      return status;
    }

  return status;
}


unsigned short dataReset792_CAENCOMM(int32_t BHandle)
{
  unsigned short status = 1;
  unsigned short caenst;
  unsigned int rst=0x4;
  caenst = CAENComm_Write16(BHandle,V792N_CAENCOMM_BIT_SET2,rst);
  status *= (1-caenst); 
  caenst = CAENComm_Write16(BHandle,V792N_CAENCOMM_BIT_CLEAR2,rst);
  status *= (1-caenst); 
  return status;
}


unsigned short softReset792_CAENCOMM(int32_t BHandle)
{
  unsigned short status = 1;
  unsigned short caenst;
  unsigned int rst=0x80;
  caenst = CAENComm_Write16(BHandle,V792N_CAENCOMM_BIT_SET1,rst);
  status *= (1-caenst); 
  caenst = CAENComm_Write16(BHandle,V792N_CAENCOMM_BIT_CLEAR1,rst);
  status *= (1-caenst); 
  return status;
}

vector<int> readFastNadc792_CAENCOMM(int32_t BHandle, short int& status, int nevts, vector<int> &outW)
{
  /*
    Implements Block Transfer Readout of V792N_CAENCOMM 
    returns vector with output
  */

  int nbytes_tran = 0;
  vector<int> outD; outD.clear();
  status = 1; 
  int caenst;

  //AS  unsigned long dataV[34];
  uint32_t* dataV; //each event is composed of 34x32bit words
  dataV=new uint32_t[1000];
  unsigned int wr;
  unsigned long adc792_CAENCOMM_rdy, adc792_CAENCOMM_busy;
  unsigned int ncha(32), idV; //no ZS reading all 32 channels

  /* if(idB<0 || idB>NUMADBOARDS-1) { */
  /*   cout<<" Accssing Board number"<<idB<<" while only "<<NUMADBOARDS<<" are initialized!!! Check your configuration!"<<endl; */
  /*   status = 0; */
  /*   return outD; */
  /* } */

  /*
    check if the fifo has something inside: use status register 1
  */  
  uint16_t data;
  unsigned long address;
  address =  adc792_CAENCOMM_shift.statusreg1;
  caenst = CAENComm_Read16(BHandle,address,&data);
  status *= (1-caenst); 

  /* if(adc792_CAENCOMM_debug) printf("ST (str1) :: %i, %lx, %lx \n",status,address,data); */
  adc792_CAENCOMM_rdy = data & adc792_CAENCOMM_bitmask.rdy;
  adc792_CAENCOMM_busy = data & adc792_CAENCOMM_bitmask.busy;
  int tmpW;
  unsigned long full,empty;

  /* //Check the status register to check the MEB status */
  /* address =  adc792_CAENCOMM_shift.statusreg2; */
  /* caenst = CAENComm_Read16(BHandle,address,&data); */
  /* status *= (1-caenst);  */

  /* full = data &  adc792_CAENCOMM_bitmask.full; */
  /* empty = data &  adc792_CAENCOMM_bitmask.empty; */

  //Event counter register
  if(adc792_CAENCOMM_debug)  cout<<" rdy:: "<<adc792_CAENCOMM_rdy<<" busy:: "<<adc792_CAENCOMM_busy<<endl;

  if(adc792_CAENCOMM_rdy == 1 && adc792_CAENCOMM_busy == 0) {
    /*
      Data Ready and board not busy
    */
    tmpW = 0;
    full = 0; empty = 0;
    //Read the Event Header
    address = 0;

    //Vector reset
    idV = 0; while(idV<(ncha+2)*nevts) { dataV[idV] = 0; idV++; }
    //    wr = (ncha+2)*4*nevts; 
    wr = (ncha+2)*sizeof(dataV)*nevts;

    printf("HELLO %d\n",wr);
    caenst =  CAENComm_BLTRead(BHandle,0x0,dataV,wr,&nbytes_tran);
    status *= (1-caenst); 
    printf("%d\n",nbytes_tran);
    //Vector dump into output
    idV = 0; while(idV<(ncha+2)*nevts) { 
      outD.push_back((int)dataV[idV]); 
      //   if (adc792_CAENCOMM_debug) print_adc792_CAENCOMM_debug_word(dataV[idV]);
      idV++;  
    }
    delete dataV;

    /*
    int ntry = 100, nt = 0;
    while(!empty && nt<ntry) {
      //Check the status register to check the MEB status
      address = adcaddrs.at(idB);
      caenst = CAENVME_BLTReadCycle(BHandle,address,dataV,(ncha+2)*4,
				    cvA24_U_DATA,cvD32,&nbytes_tran);
      status *= (1-caenst); 

      //Vector dump into output
      idV = 0; while(idV<(ncha+2)) { 
	outD.push_back((int)dataV[idV]); 
	if (adc792_CAENCOMM_debug) print_adc792_CAENCOMM_debug_word(dataV[idV]);
	idV++;  
      }

      //Check the status register to check the MEB status
      address =  adc792_CAENCOMM_shift.statusreg2;
      caenst = CAENComm_Read16(BHandle,address,&data);
      status *= (1-caenst); 
      
      full = data &  adc792_CAENCOMM_bitmask.full;
      empty = data &  adc792_CAENCOMM_bitmask.empty;
      nt++;
    }
    if(nt) cout<<" Warning:: needed "<<nt<<" add read to clean up MEB"<<endl;
    */

    check_adc792_CAENCOMM_status_afterRead(BHandle);
  }

  return outD;
}

int find_adc792_CAENCOMM_eventSize(std::vector<int>& events,unsigned int evtStart)
{
  short dt_type_boe = events.at(evtStart)>>24 & 0x7;
  if (dt_type_boe != 2)
    {
      std::cout << "ADC 792:: NOT AT BEGIN OF EVENT. Data are probably corrupted" << std::endl;
      return -1;
    }

  int channelsReadout =  events.at(evtStart)>>8 & 0x3F;  

  short dt_type_eoe = events.at(evtStart+channelsReadout+1)>>24 & 0x7;
  if (dt_type_eoe != 4)
    {
      std::cout << "ADC 792:: NOT AT END OF EVENT. Data are probably corrupted" << std::endl;
      return -1;
    }
  
  if (adc792_CAENCOMM_debug)
    {
      int evtNum=events.at(evtStart+channelsReadout+1) & 0xFFFFFF;
      std::cout << "ADC 792:: EVENT " << evtNum << " has " << channelsReadout << " channels readout"<< std::endl; 
    }

  return channelsReadout+2;
}
