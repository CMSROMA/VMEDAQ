#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/time.h> 
#include <time.h> 
#include <math.h>

//Bridge!
#include "vme_bridge.h"

#include "main_v1718.h" 
#include "v1718_lib.h"
#include <fstream>
#include <iostream>
#include <vector>


#define NCOUNTMAX 100000000

using namespace std;

int main(int argc, char** argv)
{

  unsigned long  max_evts=100000;
  int nevent = 0;
  short trigger_OK, daq_status, status_init;
  int i;
  double rate;
  int32_t BHandle(0);
  int n_value = 0; int p_value = 50;
  char* f_value = "dumb";

  
  double tempo_start=0. , tempo_now =0., tempo_last=0.;
  double time_start=0. , time_now =0., time_last=0.;
  double   elapsed_seconds=0, delta_seconds=0;
  struct timeval tempo1;
  
  FILE *fp;
 
  
  //a.out -a 3 -b 5.6 -c "I am a string" -d 222 111
  for (i = 1; i < argc; i++) {
    /* Check for a switch (leading "-"). */
    if (argv[i][0] == '-') {
      /* Use the next character to decide what to do. */
      switch (argv[i][1]) {
      case 'n':n_value = atoi(argv[++i]);
	break;
      case 'p':p_value = atoi(argv[++i]);
	break;
      case 'f':f_value = argv[++i];
	break;
      }
    }
  }
  
  printf("n_evts(max) = %d\n", n_value);

  /*   
       if file storage is requested...
  */
  if (f_value != NULL) {
    printf("file = \"%s\"\n", f_value);
    fp = fopen(f_value,"w");
  }
  else{
    fp = fopen("DataStructure.txt", "w");
  }
  if (fp==NULL) {
    printf("Cannot open the file!\n");
    return 1;
  }


  if(n_value) max_evts = n_value;
  
  /* Bridge VME initialization */
  status_init = bridge_init(BHandle);
  bridge_deinit(BHandle);
  status_init = bridge_init(BHandle);
  
  /* VME deinitialization */
  printf("\n\n VME initialization\n");
  if (status_init != 1) 
    {
      printf("VME Initialization error ... STOP!\n");
      return(1);
    }
  
  /* Modules initialization */
  else if(V1718)
    {
      printf("\n Bridge initialization and trigger vetoed\n");
      status_init *= init_1718(BHandle);
      status_init *= init_scaler_1718(BHandle) ;
      status_init *= init_pulser_1718(BHandle) ;
      status_init *= clearbusy_1718(BHandle);
      if (status_init != 1) { return(1); }      
    } 
  else {
    printf("\n No TRIGGER module is present:: EXIT!\n");
    return(1); 
  }
  
  
  
  printf("\n VME and modules initialization completed \n\n Start data acquisition\n");
  
  
  /* Output file initialization  */
  
  bool trigger;
  /*  Start counting: check that the scaler's channels are empty */
  
  gettimeofday(&tempo1, NULL);
  tempo_start = ((double)tempo1.tv_sec) + ((double)tempo1.tv_usec)/1000000;
  tempo_last = tempo_start;
  time_start = tempo_start;
  time_last = tempo_start;
  
  /* Start of the event collection cycle */
  while(nevent<(int)max_evts)
    {
      trigger_OK = 0;
      daq_status = 1;
      
      if (V1718) {
	trigger = false;
	while(!trigger){
	  //daq_status = read_trig_1718(BHandle,&trigger);    
	  daq_status = trigger_scaler_1718(BHandle,&trigger);    
	}
      }
      if(daq_status!=1){
	printf("\nError while on polling... STOP!\n");
	return(1);
      }
      
            
      gettimeofday(&tempo1, NULL);
      tempo_now = ((double)tempo1.tv_sec) + ((double)tempo1.tv_usec)/1000000;
      
      elapsed_seconds = tempo_now-tempo_start;
      delta_seconds   = tempo_now-tempo_last;
      tempo_last = tempo_now;
      /*      printf("\nSeconds: %lf  delta seconds = %lf\n",
	     elapsed_seconds,delta_seconds);
      */    
      nevent++;
      daq_status = read_scaler_1718(BHandle);
      
      fprintf(fp," %d   %f \n",nevent,delta_seconds);

      if((nevent-(p_value*((int)(nevent/p_value))))==0)
	{
	  gettimeofday(&tempo1, NULL);
	  time_now = ((double)tempo1.tv_sec) + 
	    ((double)tempo1.tv_usec)/1000000;
	  elapsed_seconds = time_now-time_start;
	  delta_seconds   = time_now-time_last;
	  time_last = time_now;

	  if(delta_seconds) rate = ((double)p_value)/delta_seconds;
	  printf("\n Event number =%d. El time (s): %f. Freq (Hz): %lf \n",
		 nevent,delta_seconds,rate);
	}
      if(!access("acq.stop",F_OK)) {
	   cout<<"Stopped run from acq.stop : deleting acq.stop file"<<endl;
	   nevent = max_evts;
	   remove("acq.stop");
      }
      daq_status = clearbusy_1718(BHandle);
    }

  
  
  /* Output File finalization */  
  printf("\n Closing output file!\n");
  fclose(fp);
  /*--------------------------------------------------------------------------*/
  
  /* VME deinitialization */
  bridge_deinit(BHandle);
  
  printf("\n VME and modules deinitialization completed \n\n Data acquisition stopped\n");
  
  return(0);
}

// Read FAST (no decoding)
unsigned short writeFastEvent(vector<int> wriD, ofstream *Fouf)
{
  
  /*
    Event FORMAT:
    Evt Size
    Channel Mask
    Evt number
    Trigger time
    And for the non zero channels
    N. of ch words (+1, tot charge)
  */
  
  unsigned short status = 1;
  int size = wriD.size();
  int myD[size];
  for(int dum=0; dum<size; dum++) {myD[dum] = wriD[dum];}
  Fouf->write((char *) &size,sizeof(int));
  Fouf->write((char *) myD,wriD.size()*sizeof(int));
  return (status);
  
}
