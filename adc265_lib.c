#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/time.h> 
#include <time.h> 
/*
#include "my_vmeio.h"
#include "universe.h"
#include "my_vmeint.h"
*/
#include "CAENVMElib.h"
#include "CAENVMEtypes.h" 
#include "CAENVMEoslib.h"

#include "adc265_lib.h"
#include "printbit.h"

using namespace std;

unsigned long adc265_count[ADC265_CHANNEL];
int ADC265_value[ADC265_CHANNEL];

/*------------------------------------------------------------------*/

unsigned short init_adc265(int32_t BHandle)
{
  /* inizializza l'adc265  return = 1 -> OK
                        return = 0 -> failed
*/
  int caenst=0;
  unsigned long address, data, *poi2data;
  unsigned short status=1;
  /* 
     procedura di inizializzazioe dell'adc265 
  */
  poi2data = &data;
  address = ADC265_ADDRESS + adc265_shift.clear;
  data = 0x0;
  //   status = vme_write_dt(address,poi2data,ADC265_AM,ADC265_DS);
  caenst = CAENVME_WriteCycle(BHandle,address,poi2data,cvA24_U_DATA,cvD16);
  status *= (1-caenst); 

  sleep(2);

  return(status);
}

/*------------------------------------------------------------------*/

vector<int> read_adc265(int32_t BHandle, int status)
{
  /* reading of the ADC265   return = 1 -> OK
                             return = 0 -> failed
*/
  vector<int> outD;
  unsigned long address, data, *poi2data;
  unsigned long adc265_rdy, adc265_full;
  int  mm, canale, risultato, adc265_range, caenst;

  status = 1;
  poi2data = &data;
  /*
    check if the fifo has something inside
   */  
  address = ADC265_ADDRESS + adc265_shift.statusreg;
  //    status = vme_read_dt(address,poi2data,ADC265_AM,ADC265_DS);
  caenst = CAENVME_ReadCycle(BHandle,address,poi2data,cvA24_U_DATA,cvD16);
  status *= (1-caenst); 
  adc265_rdy = data & adc265_bitmask.rdy;
  adc265_rdy = (data & adc265_bitmask.rdy)>>15;
  adc265_full = data & adc265_bitmask.full;
  adc265_full = (data & adc265_bitmask.full)>>14;
  if(adc265_rdy == 1)
    {
/*
   Then read the ADC265 data fifo
*/
/* #ifdef ADC265_DEBUG */
/*       printf("+++++++++++++++++++++++++++++++\n"); */
/* #endif */
      address = ADC265_ADDRESS + adc265_shift.datareg;
      for(mm=0;mm<2*ADC265_CHANNEL;mm++)
	{
	  //	  status *= vme_read_dt(address,poi2data,ADC265_AM,ADC265_DS);
	  caenst = CAENVME_ReadCycle(BHandle,address,poi2data,cvA24_U_DATA,cvD16);
	  status *= (1-caenst); 
	  adc265_range =(int) ((data & adc265_bitmask.range)>>12);
	  //	  printf("mm=%d e adc265_range=%d\n",mm,adc265_range);
	  if(adc265_range==0)
	    {
	      canale = (int) ((data & adc265_bitmask.channel)>>13);
	      risultato = (int) (data & adc265_bitmask.convdata);
	      printf("canale=%d  risultato=%d\n",canale,risultato); 
	      ADC265_value[canale] = (int)data;
/* #ifdef ADC265_DEBUG */
/* 	      
/* #endif */
	    }
	}
      for(int iC = 0; iC<8; iC++) {
	outD.push_back(ADC265_value[iC]);
	//	printf("IC:%d VALUE:%d \n",iC,outD[iC]);
      }
    }
  /*
    Clear of the ADC265
   */
  address = ADC265_ADDRESS + adc265_shift.clear;
  data = 0x0;
  //  status *= vme_write_dt(address,poi2data,ADC265_AM,ADC265_DS);
  caenst = CAENVME_WriteCycle(BHandle,address,poi2data,cvA24_U_DATA,cvD16);
  status *= (1-caenst); 
  return outD;
}


/*------------------------------------------------------------------*/

vector<int> read_Nadc265(int32_t BHandle, int nevts, int status)
{
  /* reading of the ADC265   return = 1 -> OK
                             return = 0 -> failed
*/
  vector<int> outD;
  outD.clear();

  unsigned long address, data;
  unsigned long adc265_rdy, adc265_full;
  int  mm, canale, risultato, adc265_range, caenst;

  status = 1;
  /*
    check if the fifo has something inside
   */  
  address = ADC265_ADDRESS + adc265_shift.statusreg;
  //    status = vme_read_dt(address,poi2data,ADC265_AM,ADC265_DS);
  caenst = CAENVME_ReadCycle(BHandle,address,&data,cvA24_U_DATA,cvD16);
  status *= (1-caenst); 
  adc265_rdy = data & adc265_bitmask.rdy;
  adc265_rdy = (data & adc265_bitmask.rdy)>>15;
  adc265_full = data & adc265_bitmask.full;
  adc265_full = (data & adc265_bitmask.full)>>14;
  if(adc265_rdy == 1)
    {
/*
   Then read the ADC265 data fifo
*/
/* #ifdef ADC265_DEBUG */
/*       printf("+++++++++++ ADC 265 DEBUG +++++++++\n"); */
/* #endif */
      address = ADC265_ADDRESS + adc265_shift.datareg;

      for(unsigned int iev(0); iev < nevts; ++iev)
	{
	  for(mm=0;mm < 2 * ADC265_CHANNEL;mm++)
	    {
	      //	  status *= vme_read_dt(address,poi2data,ADC265_AM,ADC265_DS);
	      caenst = CAENVME_ReadCycle(BHandle,address,&data,cvA24_U_DATA,cvD16);
	      status *= (1-caenst); 
	      adc265_range =(int) ((data & adc265_bitmask.range)>>12);
	      //	  printf("mm=%d e adc265_range=%d\n",mm,adc265_range);
	      if(adc265_range==0)
		{
		  canale = (int) ((data & adc265_bitmask.channel)>>13);
		  risultato = (int) (data & adc265_bitmask.convdata);
  		  ADC265_value[canale] = (int)data; 
/* #ifdef ADC265_DEBUG */
/* 		  printf("ADC265::DATA::evbuff=%d channel=%d  value=%d\n",iev,canale,risultato); */
/* #endif */
		}
	    }
	  for(int iC = 0; iC<8; iC++) {
	    outD.push_back(ADC265_value[iC]);
	    //	printf("IC:%d VALUE:%d \n",iC,outD[iC]);
	  }
	}
    }
  /*
    Clear of the ADC265
   */
  address = ADC265_ADDRESS + adc265_shift.clear;
  data = 0x0;
  //  status *= vme_write_dt(address,poi2data,ADC265_AM,ADC265_DS);
  caenst = CAENVME_WriteCycle(BHandle,address,&data,cvA24_U_DATA,cvD16);
  status *= (1-caenst); 
  return outD;
}


/*------------------------------------------------------------------*/

unsigned short read_adc265_simple(int32_t BHandle, int * pAdcData)
{
  /* reading of the ADC265   return = 1 -> OK
                          return = 0 -> failed
*/
  unsigned short status;
  unsigned long address, data;
  unsigned long adc265_rdy, adc265_full;
  int  mm, canale, risultato, adc265_range, caenst;
  
  status = 1;
  /*
    check if the fifo has something inside
  */  
  address = ADC265_ADDRESS + adc265_shift.statusreg;
  //  status = vme_read_dt(address,&data,ADC265_AM,ADC265_DS);
  caenst = CAENVME_ReadCycle(BHandle,address,&data,cvA24_U_DATA,cvD16);
  status *= (1-caenst); 
  adc265_rdy = data & adc265_bitmask.rdy;
  adc265_rdy = (data & adc265_bitmask.rdy)>>15;
  adc265_full = data & adc265_bitmask.full;
  adc265_full = (data & adc265_bitmask.full)>>14;
  if(adc265_rdy == 1)
    {
/*
   Then read the ADC265 data fifo
*/
      address = ADC265_ADDRESS + adc265_shift.datareg;
      for(mm=0;mm<2*ADC265_CHANNEL;mm++)
        {
	  //          printf("\n lettura numero: %i= ",mm);
	  //          status *= vme_read_dt(address,&data,ADC265_AM,ADC265_DS);
	  caenst = CAENVME_ReadCycle(BHandle,address,&data,
				     cvA24_U_DATA,cvD16);
	  status *= (1-caenst); 
	 
	  //          printbit_long(data);
          adc265_range =(int) ((data & adc265_bitmask.range)>>12);
	  if(ADC265_DEBUG==1){  
            printf("\n mm=%d e adc265_range=%d\n",mm,adc265_range);
	  }
 
          if(adc265_range==0)
            {
              canale = (int) ((data & adc265_bitmask.channel)>>13);
              risultato = (unsigned int) (data & adc265_bitmask.convdata);
              ADC265_value[canale] = (int) risultato;
	      //      printf("\n canale=%d  risultato=%d\n",canale,risultato);
            }
	  /*	  else if(adc265_range==1) 
	    { 
	      canale = (int) ((data & adc265_bitmask.channel)>>13); 
	      risultato = (unsigned int) (data & adc265_bitmask.convdata); 
	      risultato = risultato<<3; 
	      ADC265_value[canale] = (int) risultato; 
	      printf("\nRANGE = 15 bit !!!! canale=%d  risultato=%d\n",canale,risultato); 
	      }*/ 
	}
      for(int iC = 0; iC<ADC265_CHANNEL; iC++) {
        if(ADC265_DEBUG ==1){ 
          printf("ADC 265 channel: %d value= %d \n",iC,ADC265_value[iC]);
        }
        *(pAdcData+iC) = ADC265_value[iC];
      }
    }
  /*
    Clear of the ADC265
   */
  address = ADC265_ADDRESS + adc265_shift.clear;
  data = 0x0;
  //  status *= vme_write_dt(address,&data,ADC265_AM,ADC265_DS);
  caenst = CAENVME_WriteCycle(BHandle,address,&data,cvA24_U_DATA,cvD16);
  status *= (1-caenst); 

  return status;
}



