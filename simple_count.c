#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/time.h> 
#include <time.h> 

//Bridge!
#include "vme_bridge.h"

#include "main_acquisition.h" 
#include "scaler560_lib.h"
#include "v1718_lib.h"

#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

int main(int argc, char** argv)
{
  short daq_status=1;
  short status_init=1;
  uint32_t count_time; //count time (assuming msec for the moment)
  int32_t BHandle(0);

  for ( int i(1); i < argc; i++) {
    /* Check for a switch (leading "-"). */
    if (argv[i][0] == '-') {
      /* Use the next character to decide what to do. */
      switch (argv[i][1]) {
      case 't':count_time = atoi(argv[++i]);
	break;
      }
    }
  }


  /* Bridge VME initialization */
  status_init = bridge_init(BHandle);
  bridge_deinit(BHandle);
  status_init = bridge_init(BHandle);

  /* VME deinitialization */
  printf("***** VME initialization *******\n");
  if (status_init != 1) 
    {
      printf("VME Initialization error ... STOP!\n");
      return(1);
    }
  
  /* Modules initialization */
  if(V1718)
    {
      printf("\nBridge initialization and trigger vetoed\n");
      status_init *= init_1718(BHandle);
      status_init *= init_pulser_1718(BHandle) ;
      status_init *= set_configuration_1718(BHandle);
      status_init *= print_configuration_1718(BHandle);
      status_init *= init_scaler_1718(BHandle) ;
      /* status_init *= setbusy_1718(BHandle,DAQ_BUSY_ON); */
      if (status_init != 1) { return(1); }
      
    } 
  else {
    printf("No TRIGGER module is present:: EXIT!\n");
    return(1); 
  }

  /* Scaler560 Initialisation */
  if(SCALER560) {
    printf("\n Initialization of SCALER 560\n");
    status_init *= init_scaler560(BHandle);
    if (status_init != 1) 
      {
	printf("Error in SCALER 560 initialization... STOP!\n");
	return(1);
      }
  }
  else {
    printf("No SCALER module is present:: EXIT!\n");
    return(1); 
  }
  

  printf("\nVME and modules initialization completed\n____ Start counting for %d msec _____\n",count_time);
  
  usleep(count_time*1e3);

  std::cout << "++++++ Final statitistics +++++++" << std::endl;
  vector<uint32_t> tmpscaD;
  tmpscaD.clear();
  tmpscaD = read_scaler560Vec(BHandle,daq_status); 
  if(!tmpscaD.size())  cout<<" Warning:: Scaler Read :: "<< tmpscaD.size() << std::endl;
  for (unsigned int i(0);i<tmpscaD.size();++i)
    std::cout << "V560:: channel " << i << " has " << tmpscaD[i] << " counts" << std::endl;

  if (daq_status != 1) 
    {
      printf("Error reading SCALER 560... STOP!\n");
      return(1);
    }

  /* VME deinitialization */
  bridge_deinit(BHandle);

  printf("\nVME and modules deinitialization completed\nData acquisition stopped\n");

  return(0);
}


