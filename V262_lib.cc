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
#include "V262.h"

// /*----------------------------------------------------------------*/

// unsigned short init_V262(int32_t BHandle)
// {
// /* 
//    reset dell IO register 
// */
//   unsigned long address, DataLong;
//   unsigned long strobe_polarity;
//   unsigned long chan_register;
//   short status(1),caenst;
//   int ii;
//   /* reset del modulo */
//   if(V262_debug) printf("Entering init_V262 \n");

//   address = V262_ADDRESS + io_shift.reset;
//   DataLong = 0xff ;
//   //  status = vme_write_dt(address, poi2data, V262_AM, V262_DS);
//   caenst = CAENVME_WriteCycle(BHandle,address,&DataLong,cvA24_U_DATA,cvD16);
//   status *= (1-caenst); 

//   if(V262_debug) printf("IO reset: status = %d \n",status);
  
//   /* setting del registro di STROBE */

//   address = V262_ADDRESS + io_shift.strobe_reg ;
//   strobe_polarity = 0x0;    /* strobe positivo */
//   DataLong = strobe_polarity;
//   //  status *= vme_write_dt(address, poi2data, V262_AM, V262_DS);
//   caenst = CAENVME_WriteCycle(BHandle,address,&DataLong,cvA24_U_DATA,cvD16);
//   status *= (1-caenst); 

//   /* setting of the IO channels */
//   for(ii=0;ii<V262_CHANNEL;ii++){
//     //    chan_register = IO_CHAN_DIRECTION[ii] +
//     chan_register = V262_CHAN_DIRECTION[ii] + V262_CHAN_POLARITY*0x2 +
//     V262_CHAN_INPUT_MODE*0x4 + V262_CHAN_TRANSFER_MODE[ii]*0x8;
//     DataLong = chan_register;
//     address = V262_ADDRESS + 0x10 + 2*ii;
    
//     //  status *= vme_write_dt(address, poi2data, V262_AM, V262_DS);
//     caenst = CAENVME_WriteCycle(BHandle,address,&DataLong,cvA24_U_DATA,cvD16);
//     status *= (1-caenst); 
//     printf("chan=%d \t address%d \t reg=%x\t status= %d \n",
// 	   ii,(unsigned int)address,(unsigned int)chan_register,status);
//   }

//   if (status != 1) 
//    {
//      printf("Errore inizializzazione IO REGISTER... esco!\n");
//    }
//   if(V262_debug) printf("init_V262 exit status = %d \n",status);
//   return(status);
// }

// /*----------------------------------------------------------------*/

// /* 
//     Reset the DAQ sending a pulse on the 0 channel of the IO
// */
// unsigned short reset_daq(int32_t BHandle)
// {
//   unsigned short status(1),caenst;
//   unsigned long address, DataLong;

//   address = V262_ADDRESS + 0x04; /* address of the write register */
//   DataLong = 0x1; 
//   //status = vme_write_dt(address, point2data, V262_AM,V262_DS);
//   caenst = CAENVME_WriteCycle(BHandle,address,&DataLong,cvA24_U_DATA,cvD16);
//   status *= (1-caenst); 
//   DataLong = 0x0 ;
//   //  status = vme_write_dt(address, point2data, V262_AM,V262_DS);
//   caenst = CAENVME_WriteCycle(BHandle,address,&DataLong,cvA24_U_DATA,cvD16);
//   status *= (1-caenst); 
//   return(status);
// }

// /*----------------------------------------------------------------*/

// /* 
//     Read the channel 2
// */
// unsigned short read_trig(int32_t BHandle)
// {
//   unsigned short status(1),caenst;
//   unsigned long address, DataLong;

//   address = V262_ADDRESS + 0x04; /* address of the write register */
//   //  status = vme_read_dt(address, &value, V262_AM,V262_DS);
//   caenst = CAENVME_ReadCycle(BHandle,address,&DataLong,cvA24_U_DATA,cvD16);
//   status *= (1-caenst); 

//   address = V262_ADDRESS + 0x48;
//   //  status = vme_write_dt(address, &value, V262_AM,V262_DS);
//   caenst = CAENVME_WriteCycle(BHandle,address,&DataLong,cvA24_U_DATA,cvD16);
//   status *= (1-caenst); 

//   return(status);
// }

// /*----------------------------------------------------------------

//     set of the busy bit ( channel # 0 ) of the IO register
// */

// unsigned short busy_V262(int32_t BHandle, int ordine)
// {
//   int status(1),caenst;
//   unsigned long address, DataLong;

//   address = V262_ADDRESS + 0x04; /* address of the write register */
//   if(ordine==DAQ_BUSY_ON)
//     { DataLong = 0x2; }  /*  2nd bit of the word written in the write register : chan #1 on */
//   else
//     {DataLong = 0x0 ;}
//   //  status = vme_write_dt(address, point2data, V262_AM,V262_DS);
//   caenst = CAENVME_WriteCycle(BHandle,address,&DataLong,cvA24_U_DATA,cvD16);
//   status *= (1-caenst); 
//   //  printf("%d %x %x\n",status,address,value);

//   return(status);
// }

// /*----------------------------------------------------------------

// legge i registri dell'I/O  e mette il risultato in un vettore i cui 
// elementi sono i valori di ciascun canale e il cui address e' dato in 
// ingresso. Ritorna 1 se la lettura e' ok
// */

// vector<int> read_V262(int32_t BHandle, unsigned short *p_IO_value)
// {
//   vector<int> myD; myD.clear();
//   unsigned short  status(1),caenst;
//   unsigned long address, ValueRegister;

//   address = V262_ADDRESS + 0x04; /*E' il registro dove e' conservato il 
//                                     contenuto di ciascun canale */

//   //  status = vme_read_dt(address, &valueRegister,IOREG_AM,IOREG_DS);
//   caenst = CAENVME_ReadCycle(BHandle,address,&ValueRegister,cvA24_U_DATA,cvD16);
//   status *= (1-caenst); 
//   if(V262_debug) printf("(read_ioreg) Reg Val :: %x\n",
// 			 (unsigned int)ValueRegister);
//   myD.push_back((int)ValueRegister);
//   return myD;
// }



// /*----------------------------------------------------------------

// legge i registri dell'I/O  e mette il risultato in un vettore i cui 
// elementi sono i valori di ciascun canale e il cui address e' dato in 
// ingresso. Ritorna 1 se la lettura e' ok
// */

// unsigned short read_V262_old(int32_t BHandle, unsigned short *p_IO_value)
// {
//   unsigned short  status(1), ii, caenst;
//   unsigned long address, DataLong;
//   int mask = 1;

//   address = V262_ADDRESS + 0x04; /*E' il registro dove e' conservato il 
// 				    contenuto di ciascun canale */

//   //  status = vme_read_dt(address, &valueRegister,V262_AM,V262_DS);
//   caenst = CAENVME_ReadCycle(BHandle,address,&DataLong,cvA24_U_DATA,cvD16);
//   status *= (1-caenst); 
//   if(V262_debug) printf("(read_V262) Reg Val :: %x\n",(unsigned int)DataLong);

//   for(ii=0;ii<V262_CHANNEL;ii++)
//     {
//       *(p_IO_value+ii)=(DataLong & mask)!=0; 
//       mask=mask*2;
//     }

//   return 1 ;
// }


// /*----------------------------------------------------------------*/

// /*
//  test the trigger bit of the IO (strobe bit): returns 1=OK or 0=Error
// */
// unsigned short trigger_V262(int32_t BHandle)
// {
//   unsigned long strobe_address, strobeRegister;
//   unsigned short status(1),caenst;
//   //  printf("Entro in trigger_V262 \n");
   
//   strobe_address = V262_ADDRESS+ 0x06; 
//   /* Mette in strobeRegister il contenuto del registro di status */
//   //  status = vme_read_dt(strobe_address, &strobeRegister, V262_AM,V262_DS); 
//   caenst = CAENVME_ReadCycle(BHandle,strobe_address,&strobeRegister,
// 			     cvA24_U_DATA,cvD16);
//   status *= (1-caenst); 
//   //  printf("%x\n",strobeRegister);
//   if ( (strobeRegister & 0x4) != 0)
//     status = 1;       
//   else
//     status = 0;       

//   /*
//   printf("Letto strobeRegister= %lx \n", strobeRegister);
//   printf("Esco in trigger_V262: status = %d \n", status);
//   */
//   return(status);
// }

// /*----------------------------------------------------------------*/

// /* Pulisce il bit che dice se e' arrivato uno strobe o meno */
// unsigned short clear_strobe_V262(int32_t BHandle)
// {
//   unsigned short status(1),caenst;
//   unsigned long address,DataLong;

//   address = V262_ADDRESS+ 0x44; 
//   /* V262_ADDRESS+ 0x44 individua il registro per pulire lo strobe. */
  
//   DataLong = 0xff ;
//   //  status= vme_write_dt(address, &data, V262_AM,V262_DS); 
//   caenst = CAENVME_WriteCycle(BHandle,address,&DataLong,cvA24_U_DATA,cvD16);
//   status *= (1-caenst); 
//   return(status);
// }

// /*----------------------------------------------------------------*/


/* 
    Reset the DAQ sending a pulse on the 0 channel of the IO
*/
unsigned short PulseCh_V262(int32_t BHandle, unsigned short channel)
{
  unsigned short status(1),caenst;
  unsigned long address, DataLong;

  address = V262_ADDRESS + 0x10*channel + 0x08; /* address of the write register */
  DataLong = 0x1; 
  //status = vme_write_dt(address, point2data, V262_AM,V262_DS);
  caenst = CAENVME_WriteCycle(BHandle,address,&DataLong,cvA24_U_DATA,cvD16);
  status *= (1-caenst); 
  // DataLong = 0x0 ;
  // //  status = vme_write_dt(address, point2data, V262_AM,V262_DS);
  // caenst = CAENVME_WriteCycle(BHandle,address,&DataLong,cvA24_U_DATA,cvD16);
  // status *= (1-caenst); 
  return(status);
}


unsigned short OutCh_V262(int32_t BHandle, unsigned short channel, unsigned short level)
{
  unsigned short status(1),caenst;
  unsigned long address, DataLong;

  address = V262_ADDRESS + 0x10*channel + 0x06; /* address of the write register */
  DataLong = level; 
  //status = vme_write_dt(address, point2data, V262_AM,V262_DS);
  caenst = CAENVME_WriteCycle(BHandle,address,&DataLong,cvA24_U_DATA,cvD16);
  status *= (1-caenst); 
  // DataLong = 0x0 ;
  // //  status = vme_write_dt(address, point2data, V262_AM,V262_DS);
  // caenst = CAENVME_WriteCycle(BHandle,address,&DataLong,cvA24_U_DATA,cvD16);
  // status *= (1-caenst); 
  return(status);
}
