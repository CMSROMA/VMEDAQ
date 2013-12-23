//------------------------------------------------------------------------------  
// Title:        VME I/O procedures for Linux
// Version:      Linux 1.0
// Date:         January 2002                                                                
// Author:       Stefano Coluccini - CAEN SpA - Computing Division
// Platform:     Linux 2.4.x
// Language:     GCC 2.95 and 3.0
//------------------------------------------------------------------------------  
//  Purpose:   
//  Docs:                                  
//                                         
//------------------------------------------------------------------------------  

/****************************************************************************/
/* This file contains the following procedures and functions:               */
/* ------------------------------------------------------------------------ */
/* init_vme               initialize the VME                                */
/* deinit_vme             deinitialize the vme                              */
/* vme_read_dt            read a single data from the VME                   */
/* vme_write_dt           write a single data into the VME                  */
/* vme_read_blk           read a block from the VME                         */
/* vme_write_blk          write a block into the VME                        */
/* vme_enable_int         enable an interrupt                               */
/* vme_disable_int        disable an interrupt                              */
/* vme_check_int          check if an interrupt has occurred                */
/****************************************************************************/



#ifndef __VMEIO_H
#define __VMEIO_H


#ifndef ulong
#define ulong unsigned long 
#endif
#ifndef uint
#define uint unsigned int 
#endif
#ifndef ushort
#define ushort unsigned short
#endif
#ifndef uchar
#define uchar unsigned char
#endif

/*****************************************************************************/
/*                        VME PARAMETERS AND CONSTANTS                       */
/*****************************************************************************/
#define AD16          0x29
#define AD24          0x39
#define AD32          0x09

#define D8           1
#define D16          2
#define D32          4
#define D64          8

#define WRITE        0
#define READ         1

/* Error Codes */
#define BUS_ERROR       0
#define OK              1
#define IRQ_OFF         0
#define IRQ_ON          1

#define MODE_PROGRAMMED 0x01
#define MODE_DMA				0x02

/*****************************************************************************/
/*                                 INIT_VME                                  */
/*---------------------------------------------------------------------------*/
/* return         :      0 -> OK,   -1 -> init_error                         */
/*---------------------------------------------------------------------------*/
/* Initialize the VME IO library                                             */
/*****************************************************************************/
short init_vme(void);


/*****************************************************************************/
/*                                DEINIT_VME                                 */
/*---------------------------------------------------------------------------*/
/* return         :      0 -> OK,   -1 -> deinit_error                       */
/*---------------------------------------------------------------------------*/
/* Deinitialize the VME IO library                                           */
/*****************************************************************************/
short deinit_vme(void);


/*****************************************************************************/
/*                             VME_READ_DT                                   */
/*---------------------------------------------------------------------------*/
/* parameters     :      address                                             */
/*                       data                                                */
/*                       a_type: 0x39 = USER_A24  0x09 = USER_A32            */
/*                       d_type: 1 = D8, 2 = D16, 4 = D32                    */
/* return         :      0 -> bus error,   1 -> OK                           */
/*---------------------------------------------------------------------------*/
/* VME read cycle for V718                                                   */
/*****************************************************************************/
short vme_read_dt(ulong, ulong *, ushort, ushort);


/*****************************************************************************/
/*                             VME_WRITE_DT                                  */
/*---------------------------------------------------------------------------*/
/* parameters     :      address                                             */
/*                       data                                                */
/*                       a_type: 39 = USER_A24  09 = USER_A32                */
/*                       d_type: 1 = D8, 2 = D16, 4 = D32                    */
/* return         :      0 -> bus error,   1 -> OK                           */
/*---------------------------------------------------------------------------*/
/* VME write cycle for V718                                                  */
/*****************************************************************************/
short vme_write_dt(ulong, ulong *, ushort, ushort);


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
/* VME read block tranfer cycle for V718                                     */
/*****************************************************************************/
short vme_read_blk(ulong, ulong *, ushort, ushort, ushort);


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
/* VME read block tranfer cycle for V718                                     */
/*****************************************************************************/
short vme_write_blk(ulong, ulong *, ushort, ushort, ushort);


/*****************************************************************************/
/*                          VME_ENABLE_INT                                   */
/*---------------------------------------------------------------------------*/
/* parameters     :      vector                                              */
/*                       level                                               */
/* return         :      1 -> OK                                             */
/*                       0 -> can't link vector                              */
/*---------------------------------------------------------------------------*/
/* enables an interrupt and links the vector to the int_handler              */
/*****************************************************************************/
short vme_enable_int(short, short);


/*****************************************************************************/
/*                          VME_DISABLE_INT                                  */
/*---------------------------------------------------------------------------*/
/* parameters     :      level                                               */
/*---------------------------------------------------------------------------*/
/* disables an interrupt                                                     */
/*****************************************************************************/
short vme_disable_int(short);


/*****************************************************************************/
/*                            VME_CHECK_INT                                  */
/*---------------------------------------------------------------------------*/
/* parameters     :      level                                               */
/* return         :      0 -> no interrupt acknowledged                      */
/*                       1 -> an interrupt acknowledged                      */
/*---------------------------------------------------------------------------*/
/* check if an interrupt has occurred                                        */
/*****************************************************************************/
short vme_check_int(short);


#endif
