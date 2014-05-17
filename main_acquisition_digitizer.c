#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

//I'd like to get rid of those!!!
#include "my_vmeio.h"
#include "my_vmeint.h"

//Bridge!
#include "CAENComm.h"
#include "vme_bridge.h"

#include "time_utils.h"

#include "main_acquisition.h" 
#include "tdc1190_lib.h"
#include "adc265_lib.h"
#include "adc792_lib.h"
//#include "v495_lib.h"
#include "tdcV488A_lib.h"
#include "scaler560_lib.h"
#include "v1718_lib.h"
#include "V513.h"
#include "V814_lib.h"
#include "V1742_lib.h"
#include "V262.h"
#include <fstream>
#include <iostream>
#include <vector>

#define update_scaler 0
using namespace std;




int main(int argc, char** argv)
{

  unsigned long  max_evts=9000;
  int nevent = 0;
  short daq_status, status_init;
  int i; 
  double rate;
  int32_t BHandle(0);

  int n_value = 0; int p_value = 50;
  //float b_value = 0.0;
  char* f_value = "dumb";

  ofstream myOut;

  int d_value = 0;

  float pedestal_freq=0;
  bool beam_trigger=0;

  ref_date.tm_hour = 0;
  ref_date.tm_min = 0;
  ref_date.tm_sec = 0;
  ref_date.tm_year = 114;
  ref_date.tm_mon = 4;
  ref_date.tm_mday = 1;
  ref_time=mktime(&ref_date);

  for (i = 1; i < argc; i++) {
    /* Check for a switch (leading "-"). */
    if (argv[i][0] == '-') {
      /* Use the next character to decide what to do. */
      switch (argv[i][1]) {
      case 'n': n_value = atoi(argv[++i]);
	break;
      case 'p':p_value = atoi(argv[++i]);
	break;
      case 'f':f_value = argv[++i];
	break;
      case 'd':d_value = atoi(argv[++i]);
	break;
      case 'b': beam_trigger = true;
	break;
      case 'r': pedestal_freq = atof(argv[++i]);
	break;
      }
    }
  }

  //Check options
  if (beam_trigger && pedestal_freq>0)
    {
      printf("Cannot have beam and pedestals at the same time:: EXIT!\n");
      return(1); 
    }

  if (pedestal_freq>0 && !IO262)
    {
      printf("Cannot have pedestals without IO262:: EXIT!\n");
      return(1); 
    }


  struct timeval tv,tv1,tv2,tv3;
  int    n_microseconds = 0, n_microseconds_dt = 0, elapsed_microseconds_dt=0, delta_micro_seconds=0; 


  printf("n_evts(max) = %d\n", n_value);
  if (f_value != NULL) printf("file = \"%s\"\n", f_value);
  if(n_value) max_evts = n_value;


  int handleV1742;
  /* int ret = CAENComm_OpenDevice((CAENComm_ConnectionType) 0,1,0,0x500000,&handle); */
  /* ret = CAENComm_CloseDevice(handle); */
  int ret = 1-CAEN_DGTZ_OpenDigitizer((CAEN_DGTZ_ConnectionType) 0,1,0,0x500000,&handleV1742);
  //  ret = CAEN_DGTZ_OpenDigitizer((CAEN_DGTZ_ConnectionType) 0,1,0,0x500000,&handle);

  //hack to get VME Handle (normally this handle is 0, can be also hardcoded...)

  CAEN_DGTZ_BoardInfo_t myBoardInfo;
  ret *= 1-CAEN_DGTZ_GetInfo(handleV1742, &myBoardInfo);  
  /* printf("%d %s\n",ret,&myBoardInfo.ModelName); */
  ret *= 1-CAENComm_Info(myBoardInfo.CommHandle, CAENComm_VMELIB_handle ,&BHandle);
  /* printf("%d VME Handle %d\n",ret,BHandle); */

  printf("Opened V1742 and initialize crate: status %d\n",ret);


  /* Bridge VME initialization */
  /* status_init = bridge_init(BHandle);  */
  /* bridge_deinit(BHandle);  */
  /* status_init = bridge_init(BHandle); */

  /* /\* VME deinitialization *\/ */
  /* printf("VME initialization\n"); */
  /* if (status_init != 1)  */
  /*   { */
  /*     printf("VME Initialization error ... STOP!\n"); */
  /*     return(1); */
  /*  } */

  /* Modules initialization */
  status_init=1;
  if(V1718)
    {
      printf("V1718 bridge initialization\n");
      status_init *= init_1718(BHandle);
      status_init *= init_pulser_1718(BHandle) ;
      status_init *= set_configuration_1718(BHandle);
      status_init *= print_configuration_1718(BHandle);
      status_init *= init_scaler_1718(BHandle) ;
      /* status_init *= setbusy_1718(BHandle,DAQ_BUSY_ON); */
      if (status_init != 1) { 
	printf("ERROR: V1718 initialization\n");
	return(1); 
      }
      
    } 
  else {
    printf("\nNo INTERFACE module is present:: EXIT!\n");
    return(1); 
  }

  if(IO513)
    {
      printf("V513 register initialization\n");
      status_init *= init_V513(BHandle);
      status_init *= busy_V513(BHandle,DAQ_BUSY_ON);
      if (status_init != 1) { return(1); }
    } 

  if(DISCR814)
    {
      printf("V814 discriminator initialization\n");
      status_init *= cvt_V814_init(BHandle);
      /* status_init *= busy_V513(BHandle,DAQ_BUSY_ON); */
      if (status_init != 1) { 
	printf("Error in DISCR 814 initialization... STOP!\n");
	return(1); 
      }
    } 
  
  /* Scaler560 Initialisation */
  if(SCALER560) {
    printf("V560 scaler initialization\n");
    status_init *= init_scaler560(BHandle);
    if (status_init != 1) 
      {
	printf("Error in SCALER 560 initialization... STOP!\n");
	return(1);
      }
  }
  
  /* 
     Fast TDC initialization 
     Initializes ALL the TDC 1190 boards specified in tdc1190_lib.h header
  */
  if(TDC1190 || TDC1190_2) 
    {
      printf("V1290 TDC initialization\n");
      status_init *= init_tdc1190(BHandle);
      if(status_init!=1)
	{
	  printf("Error in tdc1190 initialization.... STOP!\n");
	  return(0);
	}
      
    }


  /* ADC 265 initialization */
  if(ADC265) {
    printf("V265 ADC initialization\n");
    status_init *= init_adc265(BHandle);
    if (status_init != 1) 
      {
	printf("Error in ADC 265 initialization... STOP!\n");
	return(1);
      }
  }

  /* Fast TDC initialization */
  if(TDC488A) {
    printf("Initialization of TDCV488A \n");
    status_init *= init_tdcV488A(BHandle);
    if(status_init!=1) {
	printf("Error in tdcV488A initialization.... STOP!\n");
	return(0);
      }
  }
  
  /* ADC 792 initialization */
  if(ADC792) {
    printf("V792 ADC initialization\n");
    status_init *= init_adc792(BHandle,0); //Initialize the first card
    if (status_init != 1) 
      {
	printf("Error in ADC 792 initialization... STOP!\n");
	return(1);
      }
  }


  if (DIG1742)
    {
      printf("V1742 digitizer initialization\n");
      status_init *= 1-init_V1742(handleV1742);
      if (status_init != 1)
      	{
      	  printf("Error initializing V1742... STOP!\n");
      	  return(1);
      	}
    }

  /* IO 262 initialization */
  if (IO262)
    {
      printf("V262 IO register initialization\n");
      status_init *=OutCh_V262(BHandle,0,!beam_trigger); 
      printf("V262: trigger beam is %d\n",beam_trigger);
      if (status_init != 1) 
	{
	  printf("Error setting the beam trigger veto... STOP!\n");
	  return(1);
	}
    }


  printf("================================================\nVME and modules initialization completed\n\nStart data acquisition\n================================================\n");
  

  /* Output file initialization  */
  int start, end;
  int start_adc265;
  int start_adc792, end_adc792;
  int start_adc792_2, end_adc792_2;
  int start_adc792_3, end_adc792_3;
  int start_tdc2, end_tdc2;
  int start_v488, end_v488;
  int start_v560, end_v560;
  int start_hea, end_hea;
  int tdcWords, tdc2Words, board_num, adcWords, adc792Words, headWords;
  int scalWords;
  int adc792Words_2, adc792Words_3, v488Words;
  bool trigger = false; 

  vector<int> myOE;
  vector<int> my_tdc_OD, my_tdc_WD;
  vector<int> my_tdc2_OD, my_tdc2_WD;
  vector<int> my_v488_OD, my_v488_WD; //
  vector<int> my_adc_OD;
  vector<int> my_adc792_OD, my_adc792_WD;
  vector<int> my_adc792_2_OD, my_adc792_2_WD;
  vector<int> my_adc792_3_OD, my_adc792_3_WD;
  vector<uint32_t> my_scal_OD, my_scal_WD, tmpscaD;
  vector<V1742_Event_t> my_dig1742_OD;
  vector<int> my_header_OD;
  vector<unsigned int> my_Dig_Event;

  myOut.open(f_value,ios::out);

  
  int in_evt_read = 10; 
  int hm_evt_read; 

  bool read_boards,read_scaler;

  bool hiScale = true;

  //Clear of header info
  my_header_OD.clear();

  int nreadout=0;

  /*  Start counting: check that the scaler's channels are empty */
  status_init *= reset_nim_scaler_1718(BHandle) ;
  if (status_init != 1) 
    {
      printf("Error resetting the V1718... STOP!\n");
      return(1);
    }

  //Get the start time!
  gettimeofday(&tv1, NULL);
  tv2=tv1;

  sleep(2);
  /* Start of the event collection cycle */
  while(nevent<(int)max_evts)
    {
      board_num = 0;
      daq_status = 1;
      hm_evt_read = in_evt_read;
      //      hm_evt_read = 1;
      hiScale = true;
      trigger = false;

      //nasty... 
      if(nevent<in_evt_read) {hm_evt_read = 1; hiScale = false;}

      /*  START of data acquisition */ 
      if (V1718 && !IO513) {
	daq_status *= read_scaler_1718(BHandle); //reset scaler
	daq_status *= setbusy_1718(BHandle,DAQ_BUSY_OFF); //busy off to latch the busy bit
	if (daq_status != 1) 
	  { 
	    printf("\n Trigger enable(v1718)  problems.. STOP!\n");
	    return(1); 
	  }
      } else if (V1718 && IO513) {
	daq_status *= clear_strobe_V513(BHandle); 
	daq_status *= reset_daq(BHandle);
	daq_status *= busy_V513(BHandle, DAQ_BUSY_OFF);
	if (daq_status != 1) 
	  { 
	    printf("\n Problem in V513 trigger enable\n");
	    return(1); 
	  }
      }



      if (!beam_trigger && IO262 && pedestal_freq>0)
	{
	  float usec_delay=1.E6/(pedestal_freq);
	  usleep(usec_delay);
	  daq_status=PulseCh_V262(BHandle,0);
	}

      /* Wait for the trigger signal from the IO */
      if (V1718 && !IO513) {
	while(!trigger)
	  {
	    daq_status = trigger_scaler_1718(BHandle,&trigger);
	  }
	if(daq_status==999){
	  printf("Consider setting a reset...\n");
	}
	else if(daq_status!=1){
	  printf("\nError %d while on polling... STOP!\n",daq_status);
	  return(1);
	}
	daq_status *= setbusy_1718(BHandle,DAQ_TRIG_ACK); //acknowledge of the trigger to latch the trigger bit

      } else if (V1718 && IO513) {
	while(!trigger)
	  {
	    trigger = trigger_V513(BHandle);    
	  }
      }

      /* Attach a TIMESPAMP to the event */
      gettimeofday(&tv, NULL);
      n_microseconds = timevaldiff(&tv1,&tv);
      n_microseconds_dt = elapsed_microseconds_dt;

      my_header_OD.push_back(n_microseconds);
      my_header_OD.push_back(n_microseconds_dt);
      my_header_OD.push_back(gettimestamp(&tv)); //timestamp(msec wrt to reference time)

      if(d_value) cout<<"Dist btw evts:: "<<n_microseconds<<" Dist btw start/end:: "<<n_microseconds_dt<<endl;
      tv1=tv; //Update last time

      headWords = my_header_OD.size();

      read_boards = false;
      read_scaler = false;
      nevent++;

      if(!(nevent%hm_evt_read)) {
	if((nevent != hm_evt_read) || !hiScale)
	  read_boards = true;
      }

 

      if(read_boards) {
	nreadout++;
	/* read the TDC 1190 */
	if(TDC1190) {
	  my_tdc_OD.clear();
	  my_tdc_WD.clear();
	  my_tdc_OD = readNEventsTDC(BHandle,0,daq_status,hm_evt_read,my_tdc_WD);
	  //my_tdc_OD = readFastNEventsTDC(BHandle,0,daq_status,hm_evt_read,my_tdc_WD,true);
	  if(d_value) 
	    cout<<"Filled Word Vector:: "<<my_tdc_WD.size()<<" "<<my_tdc_OD.size()<<endl;
	  if(daq_status!=1){
	    printf("\nError reading the TDC 1190... STOP!\n");
	    return(1); 
	  }
	  board_num ++;
	  tdcWords = my_tdc_OD.size();
	}
	
	/* read the TDC 1290 */
	if(TDC1190_2) {
	  my_tdc2_OD.clear();
	  my_tdc2_WD.clear();
	  my_tdc2_OD = readFastNEventsTDC(BHandle,1,daq_status,hm_evt_read,my_tdc2_WD,true);
	  if(daq_status!=1){
	    printf("\nError reading the TDC 1290... STOP!\n");
	    return(1); 
	  }
	    cout<<"Filled Word Vector:: "<<my_tdc2_WD.size()<<" "<<my_tdc2_OD.size()<<endl;
	  board_num += 2;
	  tdc2Words = my_tdc2_OD.size();
	}
	
	/* read the TDC V488A */
	if(TDC488A) {
	  my_v488_OD.clear();	 
	  my_v488_WD.clear();
	  my_v488_OD = readBltNEventV488A(BHandle,daq_status,hm_evt_read,my_v488_WD);

	  if(daq_status!=1){
	    printf("\nEvent= %d  Error reading the TDC V488A... STOP!\n",nevent);
	    return(1); 
	  }
	  board_num += 4;
	  v488Words = my_v488_OD.size();
	  /*
	  for(int i=0; i<my_v488_OD.size(); i++) {
	    cout<<" content:: "<<nevent<<" "<<i<<" "<<my_v488_OD.at(i)<<endl;
	  }
	  */
	}
	
	/* read the ADC 265*/
	if(ADC265) {
	  my_adc_OD.clear();
	  my_adc_OD = read_Nadc265(BHandle,hm_evt_read,daq_status); 
	  if (daq_status != 1) 
	    {
	      printf("\nError reading ADC 265... STOP!\n");
	      return(1);
	    }
	  board_num += 8;
	  /* adcWords = my_adc_OD.size(); */
	  /* adcWords = my_adc_OD.size(); */
	}

	/* read the ADC 792*/
	if(ADC792) {
	  my_adc792_OD.clear();
	  my_adc792_WD.clear();
	  //my_adc792_OD = read_adc792(BHandle,daq_status); 
	  //	  my_adc792_OD = readFastadc792(BHandle,0,daq_status); 
	  my_adc792_OD = readFastNadc792(BHandle,0,daq_status,hm_evt_read,my_adc792_WD); 
	  //	  if(!my_adc792_OD.size())  cout<<" Warning:: QDC0 Read :: "<< my_adc792_OD.size()<<" "<< my_adc792_WD.size()<<endl;
	  if (daq_status != 1) 
	    {
	      printf("\nError reading ADC 792... STOP!\n");
	      return(1);
	    }
	  board_num += 16;
	  adc792Words = my_adc792_OD.size();
	  if (d_value) std::cout << "ADC 792:: WORDS " << adc792Words << std::endl;
	}


	/* read the ADC 792 n.2*/
	if(ADC792_2) {
	  my_adc792_2_OD.clear();
	  my_adc792_2_WD.clear();
	  my_adc792_2_OD = readFastNadc792(BHandle,1,daq_status,hm_evt_read,my_adc792_2_WD); 
	  if(!my_adc792_2_OD.size())  cout<<" Warning:: Read :: "<< my_adc792_2_OD.size()<<" "<< my_adc792_2_WD.size()<<endl;
	  if (daq_status != 1) 
	    {
	      printf("\nError reading ADC 792 n.2... STOP!\n");
	      return(1);
	    }
	  board_num += 32;
	  adc792Words_2 = my_adc792_2_OD.size();
	  if (d_value) std::cout << "ADC 792 n.2:: WORDS " << adc792Words_2 << std::endl;
	}

	/* read the ADC 792 n.3*/
	if(ADC792_3) {
	  my_adc792_3_OD.clear();
	  my_adc792_3_WD.clear();
	  my_adc792_3_OD = readFastNadc792(BHandle,2,daq_status,hm_evt_read,my_adc792_3_WD); 
	  if(!my_adc792_3_OD.size())  cout<<" Warning:: Read :: "<< my_adc792_3_OD.size()<<" "<< my_adc792_3_WD.size()<<endl;
	  if (daq_status != 1) 
	    {
	      printf("\nError reading ADC 792 n.3... STOP!\n");
	      return(1);
	    }
	  board_num += 64;
	  adc792Words_3 = my_adc792_3_OD.size();
	  if (d_value) std::cout << "ADC 792 n.3:: WORDS " << adc792Words_3 << std::endl;
	}

	//read the Digitizer 1742
	if(DIG1742) {
	  my_dig1742_OD.clear();
	  printf("Hello\n");
	  daq_status = 1 - read_V1742(handleV1742,hm_evt_read,my_dig1742_OD);
	  if (daq_status != 1)
	    {
	      printf("\nError reading DIGI 1742... STOP!\n");
	      return(1);
	    }

	  board_num += 128;
	}
	
	if(SCALER560 && update_scaler) {
	  
	  my_scal_OD.clear();
	  my_scal_WD.clear();


	  //Needed only if you read each event. In this implementation the 
	  //tmp Vector is no longer needed.

	  //read the SCALER 1 in 100 times
	  if (nreadout%100==0)
	      read_scaler=true;

	  if (read_scaler)
	    {
	      my_scal_WD.push_back(V560_CHANNEL);
	      tmpscaD.clear();
	      tmpscaD = read_scaler560Vec(BHandle,daq_status); 
	      my_scal_OD.insert( my_scal_OD.end(), tmpscaD.begin(), tmpscaD.end() );
	      
	      board_num += 128;
	      scalWords = my_scal_OD.size();
	      //	  cout<<my_scal_OD.at(0)<<" "<<my_scal_OD.at(1)<<" "<<my_scal_OD.at(2)<<" "<<my_scal_OD.at(3)<<" "<<my_scal_OD.at(4)<<endl;
	      if(!my_scal_OD.size())  cout<<" Warning:: Scaler Read :: "<< my_scal_OD.size()<<" "<< my_scal_WD.size()<<endl;
	      if (daq_status != 1) 
		{
		  printf("\nError reading SCALER 560... STOP!\n");
		  return(1);
		}
	    }
	}

	//HEADER with timing will be always present
	board_num += 256;

	start = 0;

	start_adc265 = 0;
	start_adc792 = 0;
	start_adc792_2 = 0;
	start_adc792_3 = 0;
	start_tdc2 = 0;
	start_v488 = 0;
	start_v560 = 0;
	start_hea = 0;

	/* if(hm_evt_read == 1) {	 */

	/*   myOE.clear(); */

	/*   //Write the event in unformatted style */
	/*   myOE.push_back(nevent); */
	  
	/*   //What boards? */
	/*   myOE.push_back(board_num); */
	  
	/*   if(TDC1190) myOE.push_back(tdcWords); */
	/*   if(TDC1190_2) myOE.push_back(tdc2Words); */
	/*   if(TDC488A) { */
	/*     myOE.push_back(v488Words); */
	/*   } */
	/*   if(ADC265) myOE.push_back(adcWords); */
	/*   if(ADC792) myOE.push_back(adc792Words); */
	/*   if(ADC792_2) myOE.push_back(adc792Words_2); */
	/*   if(ADC792_3) myOE.push_back(adc792Words_3); */
	/*   if(SCALER560) myOE.push_back(scalWords); */

	/*   myOE.push_back(headWords); */

	/*   if(TDC1190) myOE.insert( myOE.end(), my_tdc_OD.begin(), my_tdc_OD.end() ); */
	/*   if(TDC1190_2) myOE.insert( myOE.end(), my_tdc2_OD.begin(), my_tdc2_OD.end() ); */
	/*   if(TDC488A) myOE.insert( myOE.end(), my_v488_OD.begin(), my_v488_OD.end() ); */
	/*   if(ADC265) myOE.insert( myOE.end(), my_adc_OD.begin(), my_adc_OD.end() ); */
	/*   if(ADC792) myOE.insert( myOE.end(), my_adc792_OD.begin(), my_adc792_OD.end() ); */
	/*   if(ADC792_2) myOE.insert( myOE.end(), my_adc792_2_OD.begin(), my_adc792_2_OD.end() ); */
	/*   if(ADC792_3) myOE.insert( myOE.end(), my_adc792_3_OD.begin(), my_adc792_3_OD.end() ); */
	/*   if(SCALER560) myOE.insert( myOE.end(), my_scal_OD.begin(), my_scal_OD.end() ); */

	/*   myOE.insert( myOE.end(), my_header_OD.begin(), my_header_OD.end() ); */

	/*   //Clears the HEADER after writing the EVENT */
	/*   my_header_OD.clear(); */
	  
	/*   daq_status *= writeFastEvent(myOE,&myOut); */

	/* } else { */

	for(int ie=0; ie<hm_evt_read; ie++) {
	  printf("DAQ_STATUS %d\n",daq_status);
	  myOE.clear();
	  
	  //Write the event in unformatted style
	  myOE.push_back(nevent-hm_evt_read+ie+1);
	  
	  //What boards?
	  myOE.push_back(board_num);
	  
	  //	    if(ADC265) myOE.push_back(adcWords);
	  int eventSize_adc792=0;
	  int eventSize_adc792_2=0;
	  int eventSize_adc792_3=0;

	  if(ADC265) {
	    if (! (start_adc265 + ADC265_CHANNEL >  my_adc_OD.size()) )
		myOE.push_back(ADC265_CHANNEL);
	    else
	      {
		cout<<"ADC265::ERROR::FIFO DATA ARE CORRUPTED" <<endl;
		myOE.push_back(0);
	      }
	  }

	  if(ADC792) {
	    eventSize_adc792=find_adc792_eventSize(my_adc792_OD,start_adc792);
	    if(eventSize_adc792) {
	      if(d_value) cout<<"This ADC evt has "<<eventSize_adc792<<" words"<<endl;
	      myOE.push_back(eventSize_adc792);
	    }
	  }
	  
	  if(ADC792_2) {
	    eventSize_adc792_2=find_adc792_eventSize(my_adc792_2_OD,start_adc792_2);
	    if(eventSize_adc792_2) {
	      if(d_value) cout<<"This 2nd ADC evt has "<<eventSize_adc792_2<<" words"<<endl;
	      myOE.push_back(eventSize_adc792_2);
	    }
	  }
	  
	  if(ADC792_3) {
	    eventSize_adc792_3=find_adc792_eventSize(my_adc792_3_OD,start_adc792_3);
	    if(eventSize_adc792_3) {
	      if(d_value) cout<<"This 3rd ADC evt has "<<eventSize_adc792_3<<" words"<<endl;
	      myOE.push_back(eventSize_adc792_3);
	    }

	  }

	  int eventSize_dig1742=0;
	  if (DIG1742)
	    {
	      if (ie<(int)my_dig1742_OD.size())
	  	{
		  std::cout << "Filling Buffer V1742 " << ie << std::endl;
	  	  daq_status *= 1-writeEventToOutputBuffer_V1742(&my_Dig_Event,&((my_dig1742_OD[ie]).eventInfo),&((my_dig1742_OD[ie]).event));
	  	  if (my_Dig_Event.size()>0)
	  	    {
	  	      eventSize_dig1742=my_Dig_Event.size();
	  	      cout<<"This V1742 event " << ie << " has "<<eventSize_dig1742<<" words"<<endl;
	  	    }
	  	}
	      else
	  	cout<<"V1742::ERROR::DATA ARE CORRUPTED" <<endl;
	      myOE.push_back(eventSize_dig1742);
	    }

	  
	  if(TDC1190) {
	    if(ie<(int)my_tdc_WD.size()) {
	      myOE.push_back(my_tdc_WD.at(ie));	    
	    }
	  }
	  
	  if(TDC1190_2) {
	    if(ie<(int)my_tdc2_WD.size()) {
	      myOE.push_back(my_tdc2_WD.at(ie));	    
	    }
	  }
	  
	  if(TDC488A)  {
	    if(ie<(int)my_v488_WD.size()) {
	      myOE.push_back(my_v488_WD.at(ie));	    
	    } else {
	      cout<<"nWords:: We are missing an event!!! "<<ie<<" "<<my_v488_WD.size()<<endl;
	      myOE.push_back(1);
	    }
	  }

	  

	  if(SCALER560 && update_scaler) {
	    //Only dump the scaler on the of the event readout'
	    if(my_scal_WD.size()) {
	      if(d_value) cout<<" This scaler evt has "<<my_scal_WD.at(0)<<" words"<<endl;
	      if(ie == hm_evt_read-1) myOE.push_back(my_scal_WD.at(0));
	      else myOE.push_back(0);
	    } 
	    else 
	      myOE.push_back(0);
	  }
	  
	  if(headWords/hm_evt_read != 3) { 
	    cout<<"Problem with the HEADER!! "<<endl;
	    cout<<"HEADER words in the multiple events"<<headWords<<" "<<hm_evt_read<<endl;
	  }
	  myOE.push_back(headWords/hm_evt_read);
	  
	  //Ok, finished writing the Number of words.
	  //Now writing the events
	  
	  
	  if(ADC265) {
	    if (! (start_adc265 + ADC265_CHANNEL >  my_adc_OD.size()) )
	      for(int idum = start_adc265; idum<start_adc265 + ADC265_CHANNEL; idum++)
		{
		  myOE.push_back(my_adc_OD.at(idum));
		}
	    start_adc265+=ADC265_CHANNEL;
	  }

	  if(ADC792) {
	    
	    end_adc792 = start_adc792 + eventSize_adc792;
	    for(int idum = start_adc792; idum<end_adc792; idum++) {
	      myOE.push_back(my_adc792_OD.at(idum));
	    }
	    start_adc792 = end_adc792; //Reset the start position to the end of previuos write
	    }
	  
	  if(ADC792_2 && my_adc792_2_WD.size()) {
	    end_adc792_2 = start_adc792_2 + eventSize_adc792_2;
	    for(int idum = start_adc792_2; idum<end_adc792_2; idum++) {
	      myOE.push_back(my_adc792_2_OD.at(idum));
	    }
	    start_adc792_2 = end_adc792_2; //Reset the start position to the end of previuos write
	  }
	  
	  if(ADC792_3 && my_adc792_3_WD.size()) {
	    end_adc792_3 = start_adc792_3 + eventSize_adc792_3;
	    for(int idum = start_adc792_3; idum<end_adc792_3; idum++) {
	      myOE.push_back(my_adc792_3_OD.at(idum));
	      }
	    start_adc792_3 = end_adc792_3; //Reset the start position to the end of previuos write
	  }

	  if(DIG1742) {
	    myOE.insert(myOE.end(),my_Dig_Event.begin(),my_Dig_Event.end());
	  }


	  if(TDC1190 && my_tdc_WD.size()) {
	    end = start + my_tdc_WD.at(ie); 
	    for(int idum = start; idum<end; idum++) {
	      if(d_value) 
		cout<<" TDC loop:: "<<ie<<" "<<idum<<" "<<my_tdc_OD.at(idum)<<endl;
	      myOE.push_back(my_tdc_OD.at(idum));
	    }
	    start = end; //Reset the start position to the end of previuos write
	    }
	  
	  if(TDC1190_2 && my_tdc2_WD.size()) {
	    end_tdc2 = start_tdc2 + my_tdc2_WD.at(ie); 
	    for(int idum = start_tdc2; idum<end_tdc2; idum++) {
	      if(d_value) 
		cout<<" TDC 2 loop:: "<<ie<<" "<<idum<<" "<<my_tdc2_OD.at(idum)<<endl;
		myOE.push_back(my_tdc2_OD.at(idum));
	    }
	    start_tdc2 = end_tdc2; //Reset the start position to the end of previuos write
	  }
	  
	  if(TDC488A) {
	    if(d_value) 
	      cout<<" TDC488:: "<<ie<<" "<<my_v488_WD.size()<<endl;
	    if(ie<(int)my_v488_WD.size()){
	      end_v488 = start_v488 + my_v488_WD.at(ie); 
		for(int idum = start_v488; idum<end_v488; idum++) {
		  if(d_value) 
		    cout<<" V488 loop:: "<<ie<<" "<<idum<<" "<<my_v488_OD.at(idum)<<endl;
		  myOE.push_back(my_v488_OD.at(idum));
		}
		start_v488 = end_v488; //Reset the start position to the end of previuos write
	    } else {
	      cout<<" eWords:: We are missing an event!!! "<<ie<<" "<<my_v488_WD.size()<<endl;
	      myOE.push_back(40000);
	    }
	  }

	  
	  if(SCALER560 && my_scal_OD.size() && update_scaler) {
	    //Only dump the scaler on the 'FIRST set of events'
	    if(ie == hm_evt_read-1) { 
	      end_v560 = start_v560 + my_scal_WD.at(0); 
	      for(int idum = start_v560; idum<end_v560; idum++) {
		myOE.push_back(my_scal_OD.at(idum));
	      }
	      start_v560 = end_v560; //Reset the start position to the end of previuos write
	    }
	    }
	  
	  //ADD the header info
	  end_hea = start_hea + 3;
	  for(int idum = start_hea; idum<end_hea; idum++) {
	    myOE.push_back( my_header_OD.at(idum) );
	  }
	  start_hea = end_hea;
	  
	  daq_status *= writeFastEvent(myOE,&myOut);
	}
	

	//Clears the HEADER after writing the EVENT
	my_header_OD.clear();
      }
      
      /* if(daq_status!=1){ */
      /* 	printf("\nError writing the Event... STOP!\n"); */
      /* 	return(1);  */
      /* } */
    
      //      if(IO513) daq_status = read_V513_old(BHandle, IO_value);
      
      if((nevent-(p_value*((int)(nevent/p_value))))==0) 
	{
	  delta_micro_seconds = timevaldiff(&tv2,&tv);

	  if(delta_micro_seconds) rate = ((double)p_value)/(double)delta_micro_seconds;
	  printf("_____ Event number: %d El time (s): %f Freq (Hz): %lf ______\n",
		 nevent,(float)delta_micro_seconds/1000000.,rate*1000000.);
          fflush(stdout);

	  if(!access("/home/cmsdaq/DAQ/VMEDAQ/acq.stop",F_OK)) {
	    cout<<"Stopped run from acq.stop : deleting acq.stop file"<<endl;
	    nevent = max_evts;
	    remove("/home/cmsdaq/DAQ/VMEDAQ/acq.stop");
	  }
	  tv2=tv;
	}     
      

      /* /\* Reset the strobe bit and send the reset to the DAQ *\/ */
      /* if (V1718 && !IO513) { */
      /* 	daq_status *= clearbusy_1718(BHandle); */

      /* 	if (daq_status != 1) */
      /* 	  { */
      /* 	    printf("\nerror resetting V1718 -> exiting\n"); */
      /* 	    return(1); */
      /* 	  } */
      /* } else if (V1718 && IO513) { */
      /* 	/\* */
      /* 	  reset the strobe bit and send the reset to  the DAQ         */
      /* 	*\/ */
      /* 	if (daq_status != 1)  */
      /* 	  { */
      /* 	    printf("\nerror resetting IO -> exiting\n"); */
      /* 	    return(1);  */
      /* 	  } */
      /* } */

      gettimeofday(&tv3, NULL);
      elapsed_microseconds_dt = timevaldiff(&tv,&tv3); //time to readout the event

    }

  std::cout << "++++++ Final run statistics +++++++" << std::endl;
  tmpscaD.clear();
  tmpscaD = read_scaler560Vec(BHandle,daq_status); 
  if (daq_status != 1) 
    {
      printf("\nError reading SCALER 560... STOP!\n");
      return(1);
    }
  if(!tmpscaD.size())  cout<<" Warning:: Scaler Read :: "<< tmpscaD.size() << std::endl;
  for (unsigned int i(0);i<tmpscaD.size();++i)
    std::cout << "V560:: channel " << i << " has " << tmpscaD[i] << " counts" << std::endl;
  fflush(stdout);

  if(DIG1742) {
    daq_status = 1 - stop_V1742(handleV1742);
    if (daq_status != 1)
      {
  	printf("\nError stopping DIGI 1742... STOP!\n");
  	return(1);
      }
  }

  /* Output File finalization */  
  printf("\n Closing output file!\n");
  myOut.close();

  /* VME deinitialization */
  bridge_deinit(BHandle);

  printf("\n VME and modules deinitialization completed \n\n Data acquisition stopped\n");
  fflush(stdout);

  return(0);
}


// Write FAST event (no decoding)
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
  //  cout<<"++++++++ New Event of size " << size <<endl;
  for(int dum=0; dum<size; dum++) {
    myD[dum] = wriD[dum];
    // cout<<dum << "\t" << myD[dum] <<endl;
  }
  Fouf->write((char *) &size,sizeof(int));
  Fouf->write((char *) myD,wriD.size()*sizeof(int));
  return (status);

}
