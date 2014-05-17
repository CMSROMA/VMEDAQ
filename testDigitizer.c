#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/time.h> 
#include <time.h> 

#include "modules_config.h"
#include "V1742_lib.h"
#include "V814_lib_CAENComm.h"
#include "V262_CAENComm.h"
#include "V513_CAENComm.h"
#include "adc792_lib_CAENComm.h"

#include <fstream>
#include <iostream>
#include <vector>

int main(int argc, char** argv)
{
  int handleV1742;
  int ret = 1-CAEN_DGTZ_OpenDigitizer((CAEN_DGTZ_ConnectionType) 0,1,0,0x500000,&handleV1742);
  if (ret != 1)
    {
      printf("Error opening V1742... STOP!\n");
      return(1);
    }

  int handleV814;
  ret*=(1-CAENComm_OpenDevice((CAENComm_ConnectionType) 0,1,0,V814_0_BA,&handleV814));
  if (ret != 1) { 
    printf("Error opening DISCR 814... STOP!\n");
    return(1); 
  }

  int handleV792;
  ret*=(1-CAENComm_OpenDevice((CAENComm_ConnectionType) 0,1,0,V792_0_BA,&handleV792));
  if (ret != 1) { 
    printf("Error opening ADC 792... STOP!\n");
    return(1); 
  }

  int handleV513;
  ret*=(1-CAENComm_OpenDevice((CAENComm_ConnectionType) 0,1,0,V513_0_BA,&handleV513));
  if (ret != 1) { 
    printf("Error opening I/O 513... STOP!\n");
    return(1); 
  }

  /* int handleV262; */
  /* ret*=(1-CAENComm_OpenDevice((CAENComm_ConnectionType) 0,1,0,0x00380000,&handleV262)); */
  /* if (ret != 1) {  */
  /*   printf("Error opening I/O V262... STOP!\n"); */
  /*   return(1);  */
  /* } */

  printf("V513 register initialization\n");
  ret *= init_V513_CAENCOMM(handleV513);
  /* status_init *= busy_V513(BHandle,DAQ_BUSY_ON); */
  if (ret != 1)
    {
      printf("Error initializing V513... STOP!\n");
      return(1);
    }

  printf("V1742 digitizer initialization\n");
  ret*=(1-init_V1742(handleV1742));
  if (ret != 1)
    {
      printf("Error initializing V1742... STOP!\n");
      return(1);
    }
  
  printf("V814 discriminator initialization\n");
  ret *= cvt_V814_CAENCOMM_init(handleV814);

  if (ret != 1) { 
    printf("Error in DISCR 814 initialization... STOP!\n");
    return(1); 
  }

  printf("V792 ADC initialization\n");
  ret *= init_adc792_CAENCOMM(handleV792); //Initialize the first card
  if (ret != 1) 
    {
      printf("Error in ADC 792 initialization... STOP!\n");
      return(1);
    }
  

  int triggerMask = (1 << 1); //put to 1 bit 1 veto beam triggers;
  write_Register_V513_CAENCOMM(handleV513,triggerMask);
  /* sleep(10); */

  int i=0;
  int adc792Words;
  short daq_status;
  int hm_evt_read=1;

  vector<int> my_adc792_OD, my_adc792_WD;
  while(i<100)
    {
      printf("%d\n",i);
      bool trigger=false;
      ret *= reset_busy_V513_CAENCOMM(handleV513,triggerMask);
      ret *= clear_strobe_V513_CAENCOMM(handleV513); 
      while(!trigger)
	{
	  trigger = trigger_V513_CAENCOMM(handleV513);
	}
      my_adc792_OD.clear();
      my_adc792_WD.clear();
      //my_adc792_OD = read_adc792(BHandle,daq_status); 
      //	  my_adc792_OD = readFastadc792(BHandle,0,daq_status); 
      my_adc792_OD = readFastNadc792_CAENCOMM(handleV792,daq_status,hm_evt_read,my_adc792_WD); 
      //	  if(!my_adc792_OD.size())  cout<<" Warning:: QDC0 Read :: "<< my_adc792_OD.size()<<" "<< my_adc792_WD.size()<<endl;
      if (daq_status != 1) 
	{
	  printf("\nError reading ADC 792... STOP!\n");
	  return(1);
	}
      adc792Words = my_adc792_OD.size();
      ++i;
    }
  
  /* printf("V262 IO register initialization\n"); */
  /* ret *=OutCh_V262_CAENCOMM(handleV262,1,1);  */
  /* printf("V262: trigger beam is %d\n",1); */
  /* if (ret != 1)  */
  /*   { */
  /*     printf("Error setting the beam trigger veto... STOP!\n"); */
  /*     return(1); */
  /*   } */

  /* printf("read digitizer\n"); */
  /* ret*=(1-read_V1742(handleV1742)); */
  /* ret*=(1-stop_V1742(handleV1742)); */
}
