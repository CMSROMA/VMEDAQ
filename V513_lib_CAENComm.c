#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/time.h> 
#include <time.h>
#include <vector>
#include <iostream>

#include "CAENVMElib.h"
#include "CAENVMEtypes.h" 
#include "CAENVMEoslib.h"
#include "CAENComm.h"
#include "V513_CAENComm.h"

/*----------------------------------------------------------------*/

unsigned short init_V513_CAENCOMM(int32_t BHandle)
{
/* 
   reset dell IO register 
*/
  unsigned long address; 
  uint16_t DataLong;
  unsigned long strobe_polarity;
  unsigned long chan_register;
  short status(1),caenst;
  int ii;
  /* reset del modulo */
  if(V513_CAENCOMM_debug) printf("Entering init_V513_CAENCOMM \n");

  address = V513_CAENCOMM_ADDRESS + io_shift.reset;
  DataLong = 0xff ;
  //  status = vme_write_dt(address, poi2data, V513_CAENCOMM_AM, V513_CAENCOMM_DS);
  caenst = CAENComm_Write16(BHandle,address,DataLong);
  status *= (1-caenst); 

  if(V513_CAENCOMM_debug) printf("IO reset: status = %d \n",status);
  
  /* setting del registro di STROBE */

  address = V513_CAENCOMM_ADDRESS + io_shift.strobe_reg ;
  strobe_polarity = 0x0;    /* strobe positivo */
  DataLong = strobe_polarity;
  //  status *= vme_write_dt(address, poi2data, V513_CAENCOMM_AM, V513_CAENCOMM_DS);
  caenst = CAENComm_Write16(BHandle,address,DataLong);
  status *= (1-caenst); 

  /* setting of the IO channels */
  for(ii=0;ii<V513_CAENCOMM_CHANNEL;ii++){
    //    chan_register = IO_CHAN_DIRECTION[ii] +
    chan_register = V513_CAENCOMM_CHAN_DIRECTION[ii] + V513_CAENCOMM_CHAN_POLARITY*0x2 +
    V513_CAENCOMM_CHAN_INPUT_MODE*0x4 + V513_CAENCOMM_CHAN_TRANSFER_MODE[ii]*0x8;
    DataLong = chan_register;
    address = V513_CAENCOMM_ADDRESS + 0x10 + 2*ii;
    
    //  status *= vme_write_dt(address, poi2data, V513_CAENCOMM_AM, V513_CAENCOMM_DS);
    caenst = CAENComm_Write16(BHandle,address,DataLong);
    status *= (1-caenst); 
    printf("chan=%d \t address%d \t reg=%x\t status= %d \n",
	   ii,(unsigned int)address,(unsigned int)chan_register,status);
  }

  if (status != 1) 
   {
     printf("Errore inizializzazione IO REGISTER... esco!\n");
   }
  if(V513_CAENCOMM_debug) printf("init_V513_CAENCOMM exit status = %d \n",status);
  return(status);
}

/*----------------------------------------------------------------*/

/* 
    Reset the DAQ sending a pulse on the 0 channel of the IO
*/
unsigned short reset_busy_V513_CAENCOMM(int32_t BHandle, int triggerMask)
{
  unsigned short status(1),caenst;
  unsigned long address;
  uint16_t DataLong;

  address = V513_CAENCOMM_ADDRESS + 0x04; /* address of the write register */
  //  DataLong = triggerMask;
  DataLong = 0x3;
  //status = vme_write_dt(address, point2data, V513_CAENCOMM_AM,V513_CAENCOMM_DS);
  caenst = CAENComm_Write16(BHandle,address,DataLong);
  status *= (1-caenst); 
  //  DataLong = triggerMask;
  DataLong = 0x2;
  //  status = vme_write_dt(address, point2data, V513_CAENCOMM_AM,V513_CAENCOMM_DS);
  caenst = CAENComm_Write16(BHandle,address,DataLong);
  status *= (1-caenst); 
  return(status);
}

unsigned short write_Register_V513_CAENCOMM(int32_t BHandle, int triggerMask)
{
  unsigned short status(1),caenst;
  unsigned long address;
  uint16_t DataLong;

  address = V513_CAENCOMM_ADDRESS + 0x04; /* address of the write register */
  DataLong = triggerMask;
  //status = vme_write_dt(address, point2data, V513_CAENCOMM_AM,V513_CAENCOMM_DS);
  caenst = CAENComm_Write16(BHandle,address,DataLong);
  status *= (1-caenst); 
  return(status);
}

/*----------------------------------------------------------------*/

/* 
    Read the channel 2
*/
unsigned short read_trig(int32_t BHandle)
{
  unsigned short status(1),caenst;
  unsigned long address;
  uint16_t DataLong;

  address = V513_CAENCOMM_ADDRESS + 0x04; /* address of the write register */
  //  status = vme_read_dt(address, &value, V513_CAENCOMM_AM,V513_CAENCOMM_DS);
  caenst = CAENComm_Read16(BHandle,address,&DataLong);
  status *= (1-caenst); 

  address = V513_CAENCOMM_ADDRESS + 0x48;
  //  status = vme_write_dt(address, &value, V513_CAENCOMM_AM,V513_CAENCOMM_DS);
  caenst = CAENComm_Write16(BHandle,address,DataLong);
  status *= (1-caenst); 

  return(status);
}

/*----------------------------------------------------------------

    set of the busy bit ( channel # 0 ) of the IO register
*/

unsigned short busy_V513_CAENCOMM(int32_t BHandle, int ordine)
{
  int status(1),caenst;
  unsigned long address;
  uint16_t DataLong;

  address = V513_CAENCOMM_ADDRESS + 0x04; /* address of the write register */
  if(ordine==DAQ_BUSY_ON)
    { DataLong = 0x1; }  /*  2nd bit of the word written in the write register : chan #1 on */
  else
    {DataLong = 0x0 ;}
  //  status = vme_write_dt(address, point2data, V513_CAENCOMM_AM,V513_CAENCOMM_DS);
  caenst = CAENComm_Write16(BHandle,address,DataLong);
  status *= (1-caenst); 
  //  printf("%d %x %x\n",status,address,value);

  return(status);
}

/*----------------------------------------------------------------

legge i registri dell'I/O  e mette il risultato in un vettore i cui 
elementi sono i valori di ciascun canale e il cui address e' dato in 
ingresso. Ritorna 1 se la lettura e' ok
*/

vector<int> read_V513_CAENCOMM(int32_t BHandle, unsigned short *p_IO_value)
{
  vector<int> myD; myD.clear();
  unsigned short  status(1),caenst;
  unsigned long address;
  uint16_t ValueRegister;

  address = V513_CAENCOMM_ADDRESS + 0x04; /*E' il registro dove e' conservato il 
                                    contenuto di ciascun canale */

  //  status = vme_read_dt(address, &valueRegister,IOREG_AM,IOREG_DS);
  caenst = CAENComm_Read16(BHandle,address,&ValueRegister);
  status *= (1-caenst); 
  if(V513_CAENCOMM_debug) printf("(read_ioreg) Reg Val :: %x\n",
			 (unsigned int)ValueRegister);
  myD.push_back((int)ValueRegister);
  return myD;
}



/*----------------------------------------------------------------

legge i registri dell'I/O  e mette il risultato in un vettore i cui 
elementi sono i valori di ciascun canale e il cui address e' dato in 
ingresso. Ritorna 1 se la lettura e' ok
*/

unsigned short read_V513_CAENCOMM_old(int32_t BHandle, unsigned short *p_IO_value)
{
  unsigned short  status(1), ii, caenst;
  unsigned long address;
  uint16_t DataLong;
  int mask = 1;

  address = V513_CAENCOMM_ADDRESS + 0x04; /*E' il registro dove e' conservato il 
				    contenuto di ciascun canale */

  //  status = vme_read_dt(address, &valueRegister,V513_CAENCOMM_AM,V513_CAENCOMM_DS);
  caenst = CAENComm_Read16(BHandle,address,&DataLong);
  status *= (1-caenst); 
  if(V513_CAENCOMM_debug) printf("(read_V513_CAENCOMM) Reg Val :: %x\n",(unsigned int)DataLong);

  for(ii=0;ii<V513_CAENCOMM_CHANNEL;ii++)
    {
      *(p_IO_value+ii)=(DataLong & mask)!=0; 
      mask=mask*2;
    }

  return 1 ;
}


/*----------------------------------------------------------------*/

/*
 test the trigger bit of the IO (strobe bit): returns 1=OK or 0=Error
*/
unsigned short trigger_V513_CAENCOMM(int32_t BHandle)
{
  unsigned long strobe_address;
  uint16_t strobeRegister;
  unsigned short status(1),caenst;
  //  printf("Entro in trigger_V513_CAENCOMM \n");
   
  strobe_address = V513_CAENCOMM_ADDRESS+ 0x06; 
  /* Mette in strobeRegister il contenuto del registro di status */
  //  status = vme_read_dt(strobe_address, &strobeRegister, V513_CAENCOMM_AM,V513_CAENCOMM_DS); 
  caenst = CAENComm_Read16(BHandle,strobe_address,&strobeRegister);
  status *= (1-caenst); 
  //  printf("%x\n",strobeRegister);
  if ( (strobeRegister & 0x4) != 0)
    status = 1;       
  else
    status = 0;       

  /*
  printf("Letto strobeRegister= %lx \n", strobeRegister);
  printf("Esco in trigger_V513_CAENCOMM: status = %d \n", status);
  */
  return(status);
}

/*----------------------------------------------------------------*/

/* Pulisce il bit che dice se e' arrivato uno strobe o meno */
unsigned short clear_strobe_V513_CAENCOMM(int32_t BHandle)
{
  unsigned short status(1),caenst;
  unsigned long address;
  uint16_t DataLong;

  address = V513_CAENCOMM_ADDRESS+ 0x44; 
  /* V513_CAENCOMM_ADDRESS+ 0x44 individua il registro per pulire lo strobe. */
  
  DataLong = 0xff ;
  //  status= vme_write_dt(address, &data, V513_CAENCOMM_AM,V513_CAENCOMM_DS); 
  caenst = CAENComm_Write16(BHandle,address,DataLong);
  status *= (1-caenst); 
  return(status);
}

/*----------------------------------------------------------------*/


/* 
    Reset the DAQ sending a pulse on the 0 channel of the IO
*/
unsigned short PulseCh1_V513_CAENCOMM(int32_t BHandle)
{
  unsigned short status(1),caenst;
  unsigned long address;
  uint16_t DataLong;

  address = V513_CAENCOMM_ADDRESS + 0x04; /* address of the write register */
  DataLong = 0x2; 
  //status = vme_write_dt(address, point2data, V513_CAENCOMM_AM,V513_CAENCOMM_DS);
  caenst = CAENComm_Write16(BHandle,address,DataLong);
  status *= (1-caenst); 
  DataLong = 0x0 ;
  //  status = vme_write_dt(address, point2data, V513_CAENCOMM_AM,V513_CAENCOMM_DS);
  caenst = CAENComm_Write16(BHandle,address,DataLong);
  status *= (1-caenst); 
  return(status);
}
