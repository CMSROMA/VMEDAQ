//------------------------------------------------------------------------------  
// Title:        VME I/O procedures for Linux
// Version:      Linux 1.0
// Date:         January 2002                                                                
// Author:       Stefano Coluccini - CAEN SpA - Computing Division
// Platform:     Linux 2.4.x
// Language:     GCC 2.95 and 3.0
//------------------------------------------------------------------------------  
//  Purpose: Provide an API for using the VME bus under Linux   
//  Docs:                                  
//                                         
//------------------------------------------------------------------------------  

#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "my_vmeio.h"
#include "universe.h"
#include "my_vmeint.h"

/*****************************************************************************/
/*                           GLOBAL VARIABLES                                */
/*****************************************************************************/
static int    vme_handle;       /* VME file handle                           */
static int    vme_int_handle;   /* VME interrupt file handle                 */
static char   add_modifier;     /* VME address modifier                      */
static ulong  add_vme = 1;      /* VME base address                          */
static char   data_size;        /* VME data size                             */
static ushort last_mode;        /* Last op is DMA or Programmed ?            */
ushort ivectors[8],en_int[8];   /* interrupt vectors and enables             */


/*****************************************************************************/
/*                           LOCAL FUNCTIONS                                 */
/*****************************************************************************/
static void vme_adjust_window( ulong address, ushort a_type, 
                               ushort d_type, ushort mode    )
{
  ulong tmp_add;
  ulong ctl       = 0x00800000;
  ulong pci_base  = 0xC0000000;
  ulong pci_bound = 0xC0010000;
  ulong to;
  
  switch( a_type )
    {
    case AD16: tmp_add = 0;
      break;
    case AD24: tmp_add = address & 0x00FF0000;
      break;
    case AD32: tmp_add = address & 0xFFFF0000;
      break;
    }
  if( (add_vme != tmp_add)      || 
      (a_type  != add_modifier) || 
      (d_type  != data_size)    ||
      (mode    != last_mode)       )
    {
      
      switch( d_type )
	{
	case D8: 
	  ctl &= ~CTL_VDW;
	  ctl |= CTL_VDW_8;
	  break;
	case D16: 
	  ctl &= ~CTL_VDW;
	  ctl |= CTL_VDW_16;
			  break;
	case D32: 
	  ctl &= ~CTL_VDW;
	  ctl |= CTL_VDW_32;
	  break;
	case D64: 
	  ctl &= ~CTL_VDW;
	  ctl |= CTL_VDW_64;
	  break;
	}
      data_size = d_type;
      
      switch( a_type )
	{
	case AD16:
	  ctl &= ~CTL_VAS;
	  ctl |= CTL_VAS_A16;
	  break;
	case AD24:
	  ctl &= ~CTL_VAS;
	  ctl |= CTL_VAS_A24;
	  break;
	case AD32:
	  ctl &= ~CTL_VAS;
	  ctl |= CTL_VAS_A32;
	  break;
	}
      add_modifier = a_type;
      
      add_vme = tmp_add;
      
      ctl &= ~CTL_PGM;
      ctl &= ~CTL_SUPER;
      
      ctl &= ~CTL_VCT; 
      if( mode == MODE_DMA ) 
	ctl |= CTL_VCT; 
      
      ctl &= ~CTL_EN; // Disable Slave
      ioctl(vme_handle, IOCTL_SET_CTL, ctl);
      
      to = add_vme - pci_base;
      
      ioctl(vme_handle, IOCTL_SET_TO, to);
      ioctl(vme_handle, IOCTL_SET_BD, pci_bound);
      ioctl(vme_handle, IOCTL_SET_BS, pci_base);  // Must set this after BD
      
      ctl |= CTL_EN; // Enable Slave
      ioctl(vme_handle,IOCTL_SET_CTL,ctl);
      
      if( mode == MODE_DMA )
	ioctl(vme_handle, IOCTL_SET_MODE, MODE_DMA);
      else 
	ioctl(vme_handle, IOCTL_SET_MODE, MODE_PROGRAMMED);
      
      last_mode = mode;      
    }
}

/*****************************************************************************/
/*                                 INIT_VME                                  */
/*---------------------------------------------------------------------------*/
/* return         :      1 -> OK,   0 -> init_error                          */
/*---------------------------------------------------------------------------*/
/* Initialize the VME IO library                                             */
/*****************************************************************************/
short init_vme(void)
{
  vme_handle = open("//dev//vme_m1",O_RDWR,0);
  vme_int_handle = open("//dev//vme_int",O_RDWR,0);
  
  if( vme_handle && vme_int_handle )
    return(1);
  else
    return(0);
}

/*****************************************************************************/
/*                                DEINIT_VME                                 */
/*---------------------------------------------------------------------------*/
/* return         :      1 -> OK,   0 -> deinit_error                        */
/*---------------------------------------------------------------------------*/
/* Deinitialize the VME IO library                                           */
/*****************************************************************************/
short deinit_vme(void)
{	
  close(vme_handle);   
  close(vme_int_handle);   

  return(0);
}

/*****************************************************************************/
/*                             VME_READ_DT                                   */
/*---------------------------------------------------------------------------*/
/* parameters     :      address                                             */
/*                       data                                                */
/*                       a_type: 0x39 = USER_A24  0x09 = USER_A32            */
/*                       d_type: 1 = D8, 2 = D16, 4 = D32                    */
/* return         :      0 -> bus error,   1 -> OK                           */
/*---------------------------------------------------------------------------*/
/* VME read cycle                                                            */
/*****************************************************************************/
short vme_read_dt (ulong address, ulong *data, ushort a_type, ushort d_type)
{
  unsigned char  b;
  unsigned short w;
  unsigned long  l;
  int 	         c;
  
  vme_adjust_window(address, a_type, d_type, MODE_PROGRAMMED);
  
  lseek(vme_handle, address, SEEK_SET);

  switch( d_type )
    {
    case D8:
      c = read(vme_handle, &b, 1);
      if( c != 1 ) return(0);
      *data = (ulong)b;
      break;
    case D16:
      c = read(vme_handle, &w, 2);
      if( c != 2 ) return(0);
      *data = (ulong)((w & 0xFF00) >> 8) | ((w & 0x00FF) << 8);
      break;
    case D32:
      c = read(vme_handle, &l, 4);
      if( c != 4 ) return(0);
      *data = (ulong)((l & 0xFF000000) >> 24) | ((l & 0x00FF0000) >>  8) |
	((l & 0x0000FF00) <<  8) | ((l & 0x000000FF) <<  24);
      break;
    }
  
  return(1);
}

/*****************************************************************************/
/*                             VME_WRITE_DT                                  */
/*---------------------------------------------------------------------------*/
/* parameters     :      address                                             */
/*                       data                                                */
/*                       a_type: 39 = USER_A24  09 = USER_A32                */
/*                       d_type: 1 = D8, 2 = D16, 4 = D32                    */
/* return         :      0 -> bus error,   1 -> OK                           */
/*---------------------------------------------------------------------------*/
/* VME write cycle                                                           */
/*****************************************************************************/
short vme_write_dt (ulong address, ulong *data, ushort a_type, ushort d_type)
{
  unsigned char  b;
  unsigned short w;
  unsigned long  l;
  int            c;

  vme_adjust_window(address, a_type, d_type, MODE_PROGRAMMED);
  lseek(vme_handle, address, SEEK_SET);

  switch( d_type )
    {
    case D8:
      b = *data & 0xFF;
      c = write(vme_handle, &b, 1);
      if( c != 1 ) return(0);
      break;
    case D16:
      w = ((*data & 0xFF00) >> 8) | ((*data & 0x00FF) << 8);
      c = write(vme_handle, &w, 2);
      if( c != 2 ) return(0);
      break;
    case D32:
      l = ((*data & 0xFF000000) >> 24) | ((*data & 0x00FF0000) >>  8) |
	((*data & 0x0000FF00) <<  8) | ((*data & 0x000000FF) << 24);
      c = write(vme_handle, &l, 4);
      if( c != 4 ) return(0);
      break;
    }
  
  return(1);
}

/*****************************************************************************/
/*                              VME_READ_BLK                                 */
/*---------------------------------------------------------------------------*/
/* parameters     :      address                                             */
/*                       buffer: local memory buffer                         */
/*                       n_word: number bytes                                */
/*                       a_type: 39 = USER_A24  09 = USER_A32                */
/*                       d_type: 4 = D32  8 = D64                            */
/* return         :      0 -> bus error,   1 -> OK                           */
/*---------------------------------------------------------------------------*/
/* VME read block tranfer cycle                                              */
/*****************************************************************************/
short vme_read_blk(ulong address, ulong *localmem, ushort size, ushort a_type, ushort d_type) {
  
  int c;

  vme_adjust_window(address, a_type, d_type, MODE_DMA);

  lseek(vme_handle, address, SEEK_SET);
  c = read(vme_handle, localmem, size);
  
  if( c != size ) return(0);
  else            return(1);
}





/*****************************************************************************/
/*                              VME_WRITE_BLK                                */
/*---------------------------------------------------------------------------*/
/* parameters     :      address                                             */
/*                       localmem: local memory buffer                       */
/*                       size: number of bytes                               */
/*                       a_type: 39 = USER_A24  09 = USER_A32                */
/*                       d_type: 4 = D32  8 = D64                            */
/* return         :      0 -> bus error,   1 -> OK                           */
/*---------------------------------------------------------------------------*/
/* VME read block tranfer cycle                                              */
/*****************************************************************************/
short vme_write_blk(ulong address, ulong *localmem, ushort size, ushort a_type, ushort d_type)
{
// ANCORA NON PROVATA

  int c;
  
  vme_adjust_window(address, a_type, d_type, MODE_DMA);
  
  lseek(vme_handle, address, SEEK_SET);
  
  c = write(vme_handle, localmem, size);
  
  if( c != size ) return(0);
  else            return(1);
  
}

/*****************************************************************************/
/*                          VME_ENABLE_INT                                   */
/*---------------------------------------------------------------------------*/
/* parameters     :      vector                                              */
/*                       level                                               */
/* return         :      1 -> OK                                             */
/*                       0 -> can't link vector                              */
/*---------------------------------------------------------------------------*/
/* enable an interrupt and link the vector to the int_handler                */
/*****************************************************************************/
short vme_enable_int(short vector, short level)
{
  ivectors[level]=vector;
  en_int[level]=1;
  if( ioctl(vme_int_handle, VINT_IOCTL_ENA, (unsigned long)level) ) 
    return(0);
  else
    return(1);
}

/*****************************************************************************/
/*                          VME_DISABLE_INT                                  */
/*---------------------------------------------------------------------------*/
/* parameters     :      level                                               */
/*---------------------------------------------------------------------------*/
/* disable an interrupt                                                      */
/*****************************************************************************/
short vme_disable_int(short level)
{
  en_int[level]=0;
  if( ioctl(vme_int_handle, VINT_IOCTL_DIS, (unsigned long)level) ) 
    return(0);
  else
    return(1);
}

/*****************************************************************************/
/*                            VME_CHECK_INT                                  */
/*---------------------------------------------------------------------------*/
/* parameters     :      level                                               */
/* return         :      0 -> no interrupt acknowledged                      */
/*                       1 -> an interrupt acknowledged                      */
/*---------------------------------------------------------------------------*/
/* check if an interrupt has occurred                                        */
/*****************************************************************************/
short vme_check_int(short level)
{
  int vector;
  
  if(!en_int[level])
    return(0);
  
  vector = ioctl(vme_int_handle, VINT_IOCTL_CHK, (unsigned long)level);
  
  if( vector == -1 ) 
    return(0);
  
  if( vector != ivectors[level] )
    return(0);
  
  return(1);
}
