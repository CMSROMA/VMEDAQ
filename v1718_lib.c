#include "CAENVMElib.h"
#include "CAENVMEtypes.h" 
#include "CAENVMEoslib.h"

#include "v1718_lib.h"
#include <iostream>
#include <map>

using namespace std;


/*----------------------------------------------------------------------*/
int init_1718(int32_t BHandle) {

  int status,caenst;
  unsigned short Mask=0;
  unsigned int DataShort=0;

  /*----------   setting the IO reg section of the V1718  -----------*/
  
  
  status = 1;
  caenst = CAENVME_SystemReset(BHandle);
  status *= (1-caenst);
  DataShort = 0x200; /* timeout=50us - FIFO mode */
  caenst = CAENVME_WriteRegister(BHandle,cvVMEControlReg,DataShort);
  status *= (1-caenst); 
  caenst = CAENVME_ReadRegister(BHandle,cvVMEControlReg,&DataShort);
  status *= (1-caenst); 
  printf("Control Reg config = %x \n",DataShort);

  /*  setting the output lines */

  caenst = CAENVME_SetOutputConf(BHandle,cvOutput0,cvDirect,
				 cvActiveHigh,cvManualSW);
  status *= (1-caenst); 
  caenst = CAENVME_SetOutputConf(BHandle,cvOutput1,cvDirect,
				 cvActiveHigh,cvManualSW);
  status *= (1-caenst); 
  /*
  status *= CAENVME_SetOutputConf(BHandle,cvOutput2,cvDirect,
				 cvActiveHigh,cvManualSW);
  status *= CAENVME_SetOutputConf(BHandle,cvOutput3,cvDirect,
				 cvActiveHigh,cvManualSW);
  status *= CAENVME_SetOutputConf(BHandle,cvOutput4,cvDirect,
				 cvActiveHigh,cvManualSW);
  */

  /* setting which output line must be pulsed  */
  

  //  Mask = cvOut0Bit + cvOut1Bit + cvOut2Bit + cvOut3Bit + cvOut4Bit;
  Mask = cvOut0Bit + cvOut1Bit;
  caenst = CAENVME_SetOutputRegister(BHandle,Mask);
  status *= (1-caenst);
  
  //setting the input lines
  caenst = CAENVME_SetInputConf(BHandle,cvInput0,cvDirect,cvActiveHigh);
  status *= (1-caenst);
  caenst = CAENVME_SetInputConf(BHandle,cvInput1,cvDirect,cvActiveHigh);
  status *= (1-caenst);

  /* status *= set_configuration_1718(BHandle); */
  return status;

}
  /*---------------   end setting IO reg section of V1718 ----------------*/

int init_scaler_1718(int32_t BHandle) {
  unsigned int DataShort=0; 
  int status, caenst;
  unsigned short limit=1023, AutoReset=1;
  printf("Init SCALER v1718\n");
  caenst = CAENVME_SetScalerConf(BHandle,limit,AutoReset,
				 cvInputSrc0,cvManualSW,cvManualSW);
  status = 1-caenst;

  caenst = CAENVME_ReadRegister(BHandle, cvScaler0, &DataShort);
  status *= 1-caenst;
  printf("Scaler Reg config = %x \n",DataShort);

  caenst = CAENVME_EnableScalerGate(BHandle); 

  status *= 1-caenst;
  return status;
}
  /*---------------   end setting IO reg section of V1718 ----------------*/

int init_pulser_1718(int32_t BHandle) {
  unsigned char Period=20, Width=10,PulseNo=1;
  int status, caenst;
  CVTimeUnits Unit;
  CVIOSources Start, Reset;

  printf("Init PULSER v1718\n");
  Unit = cvUnit1600ns;
  Start = cvManualSW;
  Reset = cvManualSW;

  caenst = CAENVME_SetPulserConf(BHandle,cvPulserA,Period,Width,
				Unit ,PulseNo,Start,Reset);
  status = 1-caenst;
  caenst = CAENVME_GetPulserConf(BHandle,cvPulserA,&Period,&Width,
				 &Unit,&PulseNo,&Start,&Reset);
  status *= 1-caenst;
  printf("Pulser config: Pulser= %d Period %d Width %d Units %d Npulse %d\n", 
	 cvPulserA,Period,Width,Unit,PulseNo);
  status *= 1-caenst;
  return status;
}

int reset_nim_scaler_1718(int32_t BHandle) {
  int status = 1; int caenst;

  unsigned short Mask=0;
  Mask = cvOut3Bit;
  caenst = CAENVME_SetOutputRegister(BHandle,Mask);
  status *= (1-caenst);
  caenst = CAENVME_ClearOutputRegister(BHandle,Mask);
  status *= (1-caenst);
  return status;
}


/*------------------------------------------------------------------------*/

int read_scaler_1718(int32_t BHandle) {
  
  unsigned int DataShort=0;
  int status, caenst;
  caenst = CAENVME_ReadRegister(BHandle, cvScaler1, &DataShort);
  //printf("Scaler value = %d \n",DataShort);
  status = (1-caenst); 
  caenst = CAENVME_ResetScalerCount(BHandle);
  status *= (1-caenst); 
  return status;
}
 /*------------------------------------------------------------------------*/

int trigger_scaler_1718(int32_t BHandle, bool *ptrig) {
  
  unsigned int DataShort=0;
  int status, caenst;
  bool trigger = false;
  caenst = CAENVME_ReadRegister(BHandle, cvScaler1, &DataShort);
  status = (1-caenst); 
  if(DataShort){ /*ma se ci metti maggiore o uguale a 1 invece che ==?Solo per evitare l'incastro inizale..funziona.. lasciamo cosi' .. 4aprile2011*/
    if(DataShort>1){
      cout<<"HEY: Warning: scaler > 1"<<endl;
      *ptrig=true;
      return 999;
    }
    trigger = true;
    caenst = CAENVME_ResetScalerCount(BHandle);
    status *= (1-caenst); 
  }
  *ptrig = trigger;
  return status;
  
}

int print_configuration_1718(int32_t BHandle)
{
  unsigned int DataShort=0;
  int status=1, caenst;

  std::map<std::string,CVRegisters> mainRegisters;
  mainRegisters["StatusReg"]=cvStatusReg;
  mainRegisters["VMEControlReg"]=cvVMEControlReg;
  mainRegisters["VMEIRQEnaReg"]=cvVMEIRQEnaReg;
  mainRegisters["InputReg"]=cvInputReg;
  mainRegisters["OutReg"]=cvOutRegSet;
  mainRegisters["InMuxReg"]=cvInMuxRegSet;
  mainRegisters["OutMuxReg"]=cvOutMuxRegSet;
  mainRegisters["PulserATime"]=cvPulserA0;
  mainRegisters["PulserAPulses"]=cvPulserA1;
  mainRegisters["PulserBTime"]=cvPulserB0;
  mainRegisters["Scaler0Conf"]=cvScaler0;
  mainRegisters["Scaler1Conf"]=cvScaler1;
  

  std::cout << "+++++ V1718 CONFIGURATION +++++++++" << std::endl;
  for (std::map<std::string,CVRegisters>::const_iterator myReg=mainRegisters.begin();myReg!=mainRegisters.end();++myReg)
    {
      caenst = CAENVME_ReadRegister(BHandle, myReg->second, &DataShort);
      status *= (1-caenst); 
      printf("%s = %X\n",myReg->first.c_str(),DataShort);
    }
  std::cout << "+++++ V1718 END CONFIGURATION +++++++++" << std::endl;

  return status;
}


int set_configuration_1718(int32_t BHandle)
{
  unsigned int DataShort=0;
  int status=1, caenst;

  std::map<CVRegisters,unsigned int> registerValues;

  /* mainRegisters["StatusReg"]=cvStatusReg; */
  /* mainRegisters["VMEControlReg"]=cvVMEControlReg; */
  /* mainRegisters["VMEIRQEnaReg"]=cvVMEIRQEnaReg; */
  /* registerValues["InputReg"]=cvInputReg; */
  /* registerValues["OutReg"]=cvOutRegSet; */

  registerValues[cvInMuxRegSet]=0x488; //SCALER HIT SOURCE=1,PULSER A START SOURCE=IN0,PULSER B START SOURCE=IN1,
  registerValues[cvOutMuxRegSet]=0x3FB; //OUT0=OUTREGISTER, OUT1=PULSERA,OUT2,OUTREGISTER,OUT3=OUTREGISTER,OUT4=OUTREGISTER

  /* registerValues["PulserATime"]=cvPulserA0; */
  /* registerValues["PulserAPulses"]=cvPulserA1; */
  /* registerValues["PulserBTime"]=cvPulserB0; */
  registerValues[cvScaler0]=0xFFF    ; //AUTORESET=1,ENDLIMIT=1023

  /* registerValues["Scaler1Conf"]=cvScaler1; */
  

  std::cout << "+++++ V1718 SET CONFIGURATION +++++++++" << std::endl;
  for (std::map<CVRegisters,unsigned int>::const_iterator myReg=registerValues.begin();myReg!=registerValues.end();++myReg)
    {
      caenst = CAENVME_WriteRegister(BHandle, myReg->first, myReg->second);
      status *= (1-caenst); 
      /* printf("%s = %X\n",myReg->first.c_str(),DataShort); */
    }
  std::cout << "+++++ V1718 END SET CONFIGURATION +++++++++" << std::endl;

  return status;
}


 /*------------------------------------------------------------------------*/


int setbusy_1718(int32_t BHandle,int command) {

  int status = 1; int caenst;
  unsigned short Mask=0;
  /* Mask = cvOut3Bit+cvOut4Bit; */
  /* caenst = CAENVME_PulseOutputRegister(BHandle,Mask); */
  /* status *= (1-caenst); */
  Mask = cvOut0Bit+cvOut2Bit;
  if(command==DAQ_BUSY_ON)
    {
      caenst = CAENVME_SetOutputRegister(BHandle,Mask+cvOut4Bit);
      status *= (1-caenst);
    }
  else
    {
      caenst = CAENVME_ClearOutputRegister(BHandle,Mask);
      status *= (1-caenst);
    }
  caenst = CAENVME_EnableScalerGate(BHandle);  //Maybe use a better function to avoid disabling the scaler gate....
  status *= 1-caenst;
  return status;
}

 /*------------------------------------------------------------------------*/


int clearbusy_new_1718(int32_t BHandle) {

  int status = 1; int caenst=0;

  caenst = CAENVME_StartPulser(BHandle,cvPulserA);
  status = (1-caenst); 
  return status;
}

/*---------------------------------------------------------------*/
int read_trig_1718(int32_t BHandle, bool *ptrig) {

  int status = 1;
  int caenst;
  unsigned int DataShort=0;
  int ch0 =0, ch1=0;

  unsigned short Mask=0;
  Mask = cvOut0Bit + cvOut1Bit;

  bool trig =false;

  int ncy=0,ncycles=1000000000;
  while(!trig && ncy<ncycles){ 
    caenst = CAENVME_ReadRegister(BHandle,cvInputReg, &DataShort);
    status *= (1-caenst); 
    //    int_to_binary(DataShort&0xff);
    ch0= (int) (DataShort & cvIn0Bit);
    ch1= (int) (DataShort & cvIn1Bit)>1;
    //    printf("Input reg :  %i  \n",(DataShort&0xff));
    if(ch0 || ch1) trig = true;
    ncy++;     
    /* usleep(1); */
  }
  if(ncy == ncycles) {printf("TIMEOUT of V1718!!!!\n");}
  /*
    reset of the input register after finding the trigger
   */
  DataShort = 0;
  caenst = CAENVME_WriteRegister(BHandle,cvInputReg,DataShort);
  status *= (1-caenst); 

  caenst = CAENVME_ReadRegister(BHandle,cvInputReg, &DataShort);
  status *= (1-caenst); 
  //    int_to_binary(DataShort&0xff);
  ch0= (int) (DataShort & cvIn0Bit);
  ch1= (int) (DataShort & cvIn1Bit)>1;
  //  printf("ch0= %i  ch1 = %i \n",ch0,ch1);

  *ptrig = trig;
  return status;
}
