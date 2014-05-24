#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/time.h> 
#include <time.h> 
#include <iostream> 

#include "CAENVMElib.h"
#include "CAENVMEtypes.h" 
#include "CAENVMEoslib.h"
#include "tdc1190_lib.h"

using namespace std;

std::vector<unsigned long> addrs;

/*------------------------------------------------------------------*/

unsigned short init_tdc1190(int32_t BHandle)
{
  unsigned long address;
  unsigned short data;
  short  status; int caenst;
  int WindowWidth =  0x8;    /* Value * 25ns : 0x8 --> 200ns*/
  int WindowOffset = -0x4;    /* Time Window Offset 100ns, TDC Gate is [-100,100] ns*/
  unsigned long trMode = 0x0000;


  addrs.clear();
  addrs.push_back(TDC1190_ADDRESS);
  if(NUMBOARDS >1) addrs.push_back(TDC1190_ADDRESS2);

  //Initialize all the boards
  for(int iBo = 0; iBo<NUMBOARDS; iBo++) {
    status = 1;
    /* 0 passo: card reset*/
    /* Edge detection rising by default after reset*/
    
    //software Clear
    address = addrs.at(iBo) + 0x1016;
    data = 0x0;
    
    //  status *= vme_write_dt(address,&data,AD32,D16);
    caenst = CAENVME_WriteCycle(BHandle,address,&data,cvA24_U_DATA,cvD16);
    status *= 1-caenst;
    if(td1190_debug) printf("Issued a Software Clear. Status:%d\n",status);
    
    sleep(1); /* wait until the inizialization is complete */


    
    /* 0 step: debugging. Printout the status register */
    

    /* //Load defaults */
    /* status *=opwriteTDC(BHandle, addrs.at(iBo), 0x0500); */

    /* I step: set TRIGGER Matching mode via OPCODE 00xx */
    if(td1190_debug) printf("Going to set Tr mode :%d\n",(int)trMode);
    status *=opwriteTDC(BHandle, addrs.at(iBo), trMode);
    printf("TDC1290::Setting trigger matching. Status:%d\n",status);
    
    /* I step: set Edge detection via OPCODE 22xx */
    data = 0x2200;
    status *=opwriteTDC(BHandle, addrs.at(iBo), data);
    data = 0x2; 
    status *=opwriteTDC(BHandle, addrs.at(iBo), data);

    /* I step: set Time Reso via OPCODE 24xx */
    data =   0x2400;
    status *=opwriteTDC(BHandle, addrs.at(iBo), data);
    data = 0x3; //25 ps resolution
    status *=opwriteTDC(BHandle, addrs.at(iBo), data);

    printf("TDC1290::Setting Time Reso 25ps. Status:%d\n",status);
    
    /* II step: set TRIGGER Window Width to value n */
    data = 0x1000;
    status *=opwriteTDC(BHandle, addrs.at(iBo), data); 
    status *=opwriteTDC(BHandle, addrs.at(iBo), WindowWidth);
    
    /* III step: set TRIGGER Window Offset to value -n */
    data = 0x1100;
    status *=opwriteTDC(BHandle, addrs.at(iBo), data); 
    status *=opwriteTDC(BHandle, addrs.at(iBo), WindowOffset);

    printf("TDC1290::Setting Time Window Width %d and Offset %d (values in 25ns bins). Status:%d\n",WindowWidth, WindowOffset,status);

    /* IV step: enable channels*/
    data = 0x4300;
    status *=opwriteTDC(BHandle, addrs.at(iBo), data);

    unsigned int enabledChannels=0;
    data = 0x4000;
    status *=opwriteTDC(BHandle, addrs.at(iBo), data);
    enabledChannels |= ( 1 << (data & 0xFF) );
    data = 0x4001;
    status *=opwriteTDC(BHandle, addrs.at(iBo), data);
    enabledChannels |= ( 1 << (data & 0xFF) );
    data = 0x4002;
    status *=opwriteTDC(BHandle, addrs.at(iBo), data);
    enabledChannels |= ( 1 << (data & 0xFF) );
    data = 0x4003;
    status *=opwriteTDC(BHandle, addrs.at(iBo), data);
    enabledChannels |= ( 1 << (data & 0xFF) );
    data = 0x4004;
    status *=opwriteTDC(BHandle, addrs.at(iBo), data);
    enabledChannels |= ( 1 << (data & 0xFF) );
    data = 0x4005;
    status *=opwriteTDC(BHandle, addrs.at(iBo), data);
    enabledChannels |= ( 1 << (data & 0xFF) );
    data = 0x4006;
    status *=opwriteTDC(BHandle, addrs.at(iBo), data);
    enabledChannels |= ( 1 << (data & 0xFF) );
    data = 0x4007;
    status *=opwriteTDC(BHandle, addrs.at(iBo), data);
    enabledChannels |= ( 1 << (data & 0xFF) );

    printf("TDC1290::Channels %X are enabled. Status:%d\n", enabledChannels, status);

    //Max 128 hits per event
    /* data = 0x3300; */
    /* status *=opwriteTDC(BHandle, addrs.at(iBo), data);  */
    /* data = 0x8; */
    /* status *=opwriteTDC(BHandle, addrs.at(iBo), data);  */

    /* III-bis step: enable BLT mode */
    /* data = 0x1100; */
    /* status *=opwriteTDC(BHandle, addrs.at(iBo), 0x1100);  */
    /* status *=opwriteTDC(BHandle, addrs.at(iBo), WindowOffset); */

    
    /* IV step: Enable trigger time subtraction */
    data = 0x1400;
    status *=opwriteTDC(BHandle, addrs.at(iBo), 0x1400);
    
    //Disable tr time subtr
    //  status *=opwriteTDC(BHandle,0x1500); 

    /* //Setting Event Aligned at 1 events */
    /* address = addrs.at(iBo) + 0x1018;  data=0x1; */
    /* //  status *= vme_write_dt(address,&data,AD32,D16); */
    /* caenst = CAENVME_WriteCycle(BHandle,address,&data,cvA24_U_DATA,cvD16); */
    /* status = 1-caenst; */
    
    if(td1190_debug) printf("Settings Status:: %d\n",status);

    /* //status *= vme_read_dt(address,&data,AD32,D16); */
    address = addrs.at(iBo) + 0x1000;
    /* caenst = CAENVME_ReadCycle(BHandle,address,&data,cvA24_U_DATA,cvD16); */
    /* status = 1-caenst; */

    /* data |= 0x0001; */
    /* data |= 0x0010; */

    /* caenst = CAENVME_WriteCycle(BHandle,address,&data,cvA24_U_DATA,cvD16); */
    /* status = 1-caenst; */
    /* if(td1190_debug) printf("Let's see..:%d\n",status); */

    /* address = addrs.at(iBo) + 0x1024; */
    /* data = 0x1; */

    /* caenst = CAENVME_WriteCycle(BHandle,address,&data,cvA24_U_DATA,cvD16); */
    /* status = 1-caenst; */
    /* if(td1190_debug) printf("Let's see again..:%d\n",status); */

    caenst = CAENVME_ReadCycle(BHandle,address,&data,cvA24_U_DATA,cvD16);
    status *= 1-caenst;
    printf("TDC1290:: Reading status register. Status:%u\t%X\n",status,(unsigned int)data);
    

    /* /\* TDC Event Reset *\/ */
    /* address = addrs.at(iBo) + 0x1018;  data=0x1; */
    /* //  status *= vme_write_dt(address,&data,AD32,D16); */
    /* caenst = CAENVME_WriteCycle(BHandle,address,&data,cvA24_U_DATA,cvD16); */
    /* status = 1-caenst; */
    if(td1190_debug) printf("Setup Done :: %d\t. Ready for acquisition.\n",status);
  }

  return (status);
}

/*------------------------------------------------------------*/

short opwriteTDC(int32_t BHandle, unsigned long addr, unsigned short data) 
/* It allows to write an opcode or an operand  */
{
  unsigned long address;
  unsigned short rdata;
  unsigned long time=0;
  short status=1; int caenst;
  const int TIMEOUT = 10000;

  /* Check the Write OK bit */
  do {
    address = addr + MICROHANDREG; 
    //status *= vme_read_dt(address, &rdata,AD32,D16);
    caenst = CAENVME_ReadCycle(BHandle,address,&rdata,cvA24_U_DATA,cvD16);
    status = 1-caenst;
    time++;
   } while ((rdata != 0x1) && ((int)time < TIMEOUT));

  if ((int)time == TIMEOUT) {
    printf("\n TDC opwrite failed: TIMEOUT! \n");  
    status = 0;
    return(status);
   }
   /* usleep(10000);    /\* 10ms sleep before sending real data *\/ */

  address = addr + MICROREG; /* OPCODE register */
  //status *= vme_write_dt(address, &data,AD32,D16);
  caenst = CAENVME_WriteCycle(BHandle,address,&data,cvA24_U_DATA,cvD16);
  status = 1-caenst;

  return (status);
}

/*----------------------------------------------------------------------*/

unsigned short opreadTDC(int32_t BHandle, unsigned long addr,
			 unsigned short *p2data) 
/* It allows to read the data; CAEN MANUAL like  */
{
  unsigned long address;
  unsigned short rdata;
  int time=0;
  unsigned short status=1; int caenst;
  const int TIMEOUT = 10000;

  /* Wait the DATA READY bit set to 1 */
  do {
    address = addr + MICROHANDREG; 
    //status *= vme_read_dt(address, &rdata,AD32,D16);
    caenst = CAENVME_ReadCycle(BHandle,address,&rdata,cvA24_U_DATA,cvD16);
    status = 1-caenst ;
    time++;
   } while (!(rdata & 0x2) && (time < TIMEOUT));

  if (time == TIMEOUT) {
    printf("\n TDC opread failed: TIMEOUT! \n");  
    status = 0;
    return(status);
   }

  address = addr + MICROREG; /* OPCODE register */
  //status *= vme_read_dt(address,p2data,AD32,D16);
  caenst = CAENVME_ReadCycle(BHandle,address,p2data,cvA24_U_DATA,cvD16);
  status = 1-caenst;
  return(status);
}

/*-------------------------------------------------------------*/

vector<int> readEventTDC(int32_t BHandle, int idB, int status) {
  unsigned long data,address,geo_add,measurement,channel,trailing;
  double tdc_time;
  int dataReady = 0; int TRG_matched = 0, evt_num = 0, tra_stat = 0;
  int caenst; int TIMEOUT = 50000;
  vector<int> outD;
  /* 
     Events Words
     1) Header
     2) Tdc evt, ch, time
     3) Trailer + error
  */
  int tdc_hea = 0; int tdc_mea = 0;
  int tdc_tra = 0; int glb_hea = 0, glb_tra = 0;
  int nTry (0);

  if(idB<0 || idB>NUMBOARDS-1) {
    cout<<" Accssing Board number"<<idB<<" while only "<<NUMBOARDS<<" are initialized!!! Check your configuration!"<<endl;
    status = 0;
    return outD;
  }

  /*
    First of all check the Status Register to see 
    if there's Data Ready
  */
  status = 1;
  do {
    address =  addrs.at(idB) + STATUSREGADD;
    //status *= vme_read_dt(address,&data,AD32,D16);
    caenst = CAENVME_ReadCycle(BHandle,address,&data,cvA24_U_DATA,cvD16);
    status = 1-caenst;
    if((data>>0) & 0x1) { 
      dataReady = 1;
    }
    if((data>>3) & 0x1) { TRG_matched = 1; }
    nTry++;
  } while (!dataReady && nTry<TIMEOUT);
  if(nTry > TIMEOUT-10) printf("Timeout!!!! :%d\n",nTry);
  
  /* Wait the DATA READY bit set to 1 */
  if(dataReady) {
    address= addrs.at(idB) + 0x0;
    //status *= vme_read_dt(address,&data,AD32,D32);
    caenst = CAENVME_ReadCycle(BHandle,address,&data,cvA24_U_DATA,cvD32);
    status = 1-caenst;
    if(td1190_debug) printf("Read New event!\n");

    if( (data>>27) & 0x8 ) {
      glb_hea = 1;
      geo_add = data>>27;
      evt_num = data>>5 & 0x3fffff;
      outD.push_back((int) evt_num);
    }

    /*Check if trailer is found. Then loop until trailer is found and
      start over*/
    if( (data>>27) & 0x10 ) glb_tra = 1;

    while(status && !glb_tra) 
      {
	//status *= vme_read_dt(address,&data,AD32,D32);
	caenst = CAENVME_ReadCycle(BHandle,address,&data,cvA24_U_DATA,cvD32);
	status = 1-caenst;
	glb_tra = (data>>27) & 0x10 ;
	tdc_tra = (data>>27) & 0x3 ;
	tdc_hea = (data>>27) & 0x1 ;
	tdc_mea = (data>>27) & 0x0 ;

	if(glb_tra == 0x10) {
	  /* Global Trailer!*/
	  if(td1190_debug) printf("Global trailer:: %X\n",(unsigned int)data);
	  tra_stat = data>>24 & 0x1;
	  outD.push_back((int) tra_stat);
	} else if(tdc_tra == 0x3) {
	  /* TDC Trailer!*/
	  if(td1190_debug) printf("TDC trailer:: %X\n",(unsigned int)data);
	} else if(tdc_hea) {
	  /* TDC Header!*/
	  if(td1190_debug) printf("TDC header:: %X\n",(unsigned int)data);
	} else if(!tdc_mea) {
	  /* TDC measurement!*/
	  measurement = data & 0x3ffff;
	  channel = (data>>19) & 0x7f;
	  trailing = (data>>26) & 0x1;
	  tdc_time = (double)measurement/10;
	  //	  if(td1190_debug)
	  if(td1190_debug) 
	    printf("TDC header:: %d %d %d\n",(int)evt_num,(int)channel,(int)measurement);
	  outD.push_back((int) evt_num);
	  outD.push_back((int) channel);
	  outD.push_back((int) measurement);
	}
	
      }
  }

  return outD;
}

/*-------------------------------------------------------------*/

vector<int> readNEventsTDC(int32_t BHandle, int idB, int status, int nevents, vector<int> &outW) {
  unsigned int address,geo_add,measurement,channel,trailing;
  int data;

  double tdc_time;
  int dataReady = 0; int TRG_matched = 0, evt_num = 0, tra_stat = 0;
  int caenst; int TIMEOUT = 10000;
  vector<int> outD;
  /* 
     Events Words
     1) Header
     2) Tdc evt, ch, time
     3) Trailer + error
  */
  int tdc_hea = 0; int tdc_mea = 0;
  int tdc_tra = 0; int glb_hea = 0, glb_tra = 0;
  int nTry (0);

  if(idB<0 || idB>NUMBOARDS-1) {
    cout<<" Accssing Board number"<<idB<<" while only "<<NUMBOARDS<<" are initialized!!! Check your configuration!"<<endl;
    status = 0;
    return outD;
  }

  /*Access to control REGISTER
  address= addrs.at(idB) + CONREGADD;
  status *= vme_read_dt(address,&data,AD32,D16);

  if(td1190_debug) printf("Access to control register. Status:%d\n",status);
  */

  /*
    First of all check the Status Register to see 
    if there's Data Ready
  */
  status = 1;
  do {
    address =  addrs.at(idB) + STATUSREGADD;
    //status *= vme_read_dt(address,&data,AD32,D16);
    caenst = CAENVME_ReadCycle(BHandle,address,&data,cvA24_U_DATA,cvD16);
    status = 1-caenst;
    if((data>>0) & 0x1) { 
      dataReady = 1;
    }
    if((data>>3) & 0x1) { TRG_matched = 1; }
    nTry++;
  } while (!dataReady && nTry<TIMEOUT);
  if(nTry > TIMEOUT-10) printf("Timeout!!!! :%d\n",nTry);
  
  /* Wait the DATA READY bit set to 1 */
  int tmpev(0),tmpW(0);
  if(dataReady) {
    while(tmpev<nevents) {
      tmpW = 0;
      address= addrs.at(idB);
      //status *= vme_read_dt(address,&data,AD32,D32);
      caenst = CAENVME_ReadCycle(BHandle,address,&data,cvA24_U_DATA,cvD32);
      status = 1-caenst;
      /* std::cout << "GGG " << (data>>27) << std::endl; */
      /* glb_hea = 0; */
      /* if( (data>>27) & 0x8 ) { */
      glb_hea = 1;
      geo_add = data & 0x1F;
      evt_num = data>>5 & 0x3fffff;
      outD.push_back( (0xA << 28) | ((int) evt_num & 0xFFFFFFF ));
      tmpW++;
      if(td1190_debug) printf("Read new event %d\n",evt_num);
      /* } */
      
      //      cout<<"First read::  "<<glb_hea<<" "<<evt_num<<endl;
      
      /*Check if trailer is found. Then loop until trailer is found and
	start over*/
      glb_tra = 0;
      if( (data>>27) & 0x10 ) glb_tra = 1;


      while(status && !glb_tra) 
	{
	  //status *= vme_read_dt(address,&data,AD32,D32);
	  caenst = CAENVME_ReadCycle(BHandle,address,&data,cvA24_U_DATA,cvD32);
	  status = 1-caenst;
	  glb_tra = (data>>27) & 0x10 ;
	  tdc_tra = (data>>27) & 0x3 ;
	  tdc_hea = (data>>27) & 0x1 ;
	  tdc_mea = (data>>27) & 0x0 ;
	  
	  if(glb_tra == 0x10) {
	    /* Global Trailer!*/
	    if(td1190_debug) printf("Global trailer:: %X\n",(unsigned int)data);
	    tra_stat = data>>24 & 0x1;
	    outD.push_back(data);
	    tmpW++;
	  } else if(tdc_tra == 0x3) {
	    /* TDC Trailer!*/
	    if(td1190_debug) printf("TDC trailer:: %X\n",(unsigned int)data);
	  } else if(tdc_hea) {
	    /* TDC Header!*/
	    if(td1190_debug) printf("TDC header:: %X\n",(unsigned int)data);
	  } else if(!tdc_mea) {
	    /* TDC measurement!*/
	    measurement = data & 0x1fffff;
	    channel = (data>>21) & 0x1f;
	    trailing = (data>>26) & 0x1;
	    tdc_time = (double)measurement/10;
	    //	  if(td1190_debug)
	    if(td1190_debug) printf("TDC measurement:: %d %d %d\n",(int)evt_num,(int)channel,(int)measurement);
	    /* outD.push_back((int) evt_num); tmpW++; */
	    /* outD.push_back((int) channel); tmpW++; */
	    /* outD.push_back((int) measurement); tmpW++; */
	    outD.push_back(data);
	    tmpW++;
	  }
	  
	}
      outW.push_back(tmpW);
      tmpev++;
    }//multiple events block
  }

  return outD;
}


/*-------------------------------------------------------------*/

vector<int> readFastNEventsTDC(int32_t BHandle, int idB, int status, int nevents, vector<int> &outW, bool t1290) {
  unsigned long data,address,geo_add,measurement,channel,trailing;
  double tdc_time;
  int dataReady = 0; int TRG_matched = 0, evt_num = 0, tra_stat = 0;
  int caenst; int TIMEOUT = 10000;
  vector<int> outD;
  /* 
     Events Words
     1) Header
     2) Tdc evt, ch, time
     3) Trailer + error
  */
  int nbytes_tran = 99999;
  int tdc_tst = 0;
  int glb_tra = 0;
  int nTry (0);
  int wr; int maxW = 75;
  /* if(idB == 1) maxW =  75; //Second TDC has only 16 channels */

  int dataV[maxW*nevents];
  if(idB<0 || idB>NUMBOARDS-1) {
    cout<<" Accssing Board number"<<idB<<" while only "<<NUMBOARDS<<" are initialized!!! Check your configuration!"<<endl;
    status = 0;
    return outD;
  }

  /*Access to control REGISTER
  address= addrs.at(idB) + CONREGADD;
  status *= vme_read_dt(address,&data,AD32,D16);

  if(td1190_debug) printf("Access to control register. Status:%d\n",status);
  */

  /*
    First of all check the Status Register to see 
    if there's Data Ready
  */
  status = 1;
  /* do { */
  address =  addrs.at(idB) + STATUSREGADD;
  //status *= vme_read_dt(address,&data,AD32,D16);
  caenst = CAENVME_ReadCycle(BHandle,address,&data,cvA24_U_DATA,cvD16);
  status = 1-caenst;
  if((data>>0) & 0x1) { 
    dataReady = 1;
  }
  if((data>>3) & 0x1) { TRG_matched = 1; }
  
  if (td1190_debug)
    std::cout <<  "TDC trigger matched:" << TRG_matched << std::endl;

  /* Wait the DATA READY bit set to 1 */
  int tmpW(0), idV(0);
  if(dataReady) {

    if (td1190_debug)
      std::cout <<  "TDC DREADY 1"<< std::endl;

    tmpW = 0;
    address= addrs.at(idB) + 0x0;

    //Vector reset
    idV = 0; while(idV<(maxW)*nevents) { dataV[idV] = 0; idV++; }
    //    wr = maxW*4*nevents; 
    wr = sizeof(dataV);
    
    //Performing BLT access (assuming BERR is enabled, check nbytes_transf=0 to exit the read cycle
    int nWordsRead=0;
    while(nbytes_tran>0 && nWordsRead<wr)
      {
	caenst = CAENVME_FIFOBLTReadCycle(BHandle,address,dataV+nWordsRead,wr,
				      cvA24_U_DATA,cvD32,&nbytes_tran);
	status *= (1-caenst); 
	if(td1190_debug) printf("Read events with size %d %d %X\n",nbytes_tran,sizeof(dataV),dataV[nWordsRead]);
	nWordsRead+=nbytes_tran/4;
      }
    
    
    //Vector dump into output
    idV = 0; while(idV<std::min(nWordsRead,wr)) {

      /* if (td1190_debug) */
      /* 	std::cout << "TDC MEB word " << idV << ":" << dataV[idV] << std::endl; */
      geo_add = dataV[idV]>>27;
      tdc_tst = geo_add & 0x1f ;

      std::cout << "TDC WORD TYPE " << tdc_tst << std::endl;
      if(tdc_tst == 0x18)  {
	idV++;  
	continue;
      }

      /* if(td1190_debug)  */
      /* 	printf("Found word: %d %lx :: %lx %d\n",idV,dataV[idV],geo_add,tdc_tst);  */


      if(tdc_tst == 0x8 ) {
	tmpW = 0;
	evt_num = dataV[idV]>>5 & 0x3fffff;
	/* if(td1190_debug) */
	/*   printf("Found GLB header: %d %d %lx :: %d\n",idB,idV,dataV[idV],evt_num); */

	outD.push_back((int) evt_num);
	if(td1190_debug) 
	  cout<<evt_num<<endl;
	tmpW++;
      }

      /*
	Check if trailer is found. Then loop until trailer is found and
	start over
      */

      if(tdc_tst == 0x10) {
	/* Global Trailer!*/
	if(td1190_debug)
	  printf("Global trailer:: %d %X\n",idB,(unsigned int)dataV[idV]);
	tra_stat = dataV[idV]>>24 & 0x1;
	outD.push_back((int) tra_stat);
	if(td1190_debug) 
	  cout<<"trail "<<tra_stat<<endl;
	tmpW++;
	outW.push_back(tmpW);
      } 

      if(tdc_tst == 0x3) {
	/* TDC Trailer!*/
	if(td1190_debug)
	  printf("TDC trailer:: %d %X\n",idB,(unsigned int)dataV[idV]);

      } 

      if(tdc_tst == 0x1) {
	/* TDC Header!*/
	if(td1190_debug)
	  printf("TDC header:: %d %X\n",idB,(unsigned int)dataV[idV]);
      } 

      if(tdc_tst == 0x0) {

	/* TDC measurement!*/
	trailing = (dataV[idV]>>26) & 0x1; //1190 & 1290
	if(!t1290) {
	  measurement = dataV[idV] & 0x3ffff;
	  channel = (dataV[idV]>>19) & 0x7f;
	} else {
	  measurement = dataV[idV] & 0x1fffff;
	  channel = (dataV[idV]>>21) & 0x1f;
	}
	tdc_time = (double)measurement/10;

	if(td1190_debug)
	  printf("TDC %d Measurement:: %d %d %d\n",(int)idB,(int)evt_num,(int)channel,(int)measurement);
	/* outD.push_back((int) evt_num); tmpW++; */
	/* outD.push_back((int) channel); tmpW++; */
	/* outD.push_back((int) measurement); tmpW++; */
	if(td1190_debug)
	  cout<<"eve:: "<<evt_num<<" "<<channel<<" "<<measurement<<endl;
      }

      outD.push_back((int)dataV[idV]); 
      idV++;  
    }

  }

  //Protection against timeout.
  /* usleep(20); */
  return outD;
}



