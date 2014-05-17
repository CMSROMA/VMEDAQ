////////////////////////////////////////////////////////////////////////////////////////////////
/*! \file    cvt_V814.c
*   \brief   V814 VME board implementation
*   \author  NDA
*   \version 1.0
*   \date    03/2006
*            
*            Provides methods, properties and defines to handle V814 VME boards
*/
////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////
// File includes
////////////////////////////////////////////////////////////////////////////////////////////////
#include <memory.h>
#include <string.h>
#include <iostream>

#include "CAENVMElib.h"
#include "CAENVMEtypes.h" 
#include "CAENVMEoslib.h"

#include "V814_lib.h"

////////////////////////////////////////////////////////////////////////////////////////////////
// File local defines
////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Static variables declaration
////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////
/*! \var     static const cvt_reg_table CVT_V814_REG_TABLE[]
*   \brief   The board registers table
*            
*            Provides an entry for each declared register: keep synched with \ref CVT_V814_REG_INDEX
*/
////////////////////////////////////////////////////////////////////////////////////////////////
/* static const cvt_reg_table CVT_V814_REG_TABLE[]= */
/* { */
/* 	{ CVT_V814_THRESHOLD_0_ADD,		CVT_V814_THRESHOLD_AM,		CVT_V814_THRESHOLD_DATA_SIZE},				/\*!< \brief Threshold 0 register *\/ */
/* 	{ CVT_V814_THRESHOLD_1_ADD,		CVT_V814_THRESHOLD_AM,		CVT_V814_THRESHOLD_DATA_SIZE},				/\*!< \brief Threshold 1 register *\/ */
/* 	{ CVT_V814_THRESHOLD_2_ADD,		CVT_V814_THRESHOLD_AM,		CVT_V814_THRESHOLD_DATA_SIZE},				/\*!< \brief Threshold 2 register *\/ */
/* 	{ CVT_V814_THRESHOLD_3_ADD,		CVT_V814_THRESHOLD_AM,		CVT_V814_THRESHOLD_DATA_SIZE},				/\*!< \brief Threshold 3 register *\/ */
/* 	{ CVT_V814_THRESHOLD_4_ADD,		CVT_V814_THRESHOLD_AM,		CVT_V814_THRESHOLD_DATA_SIZE},				/\*!< \brief Threshold 4 register *\/ */
/* 	{ CVT_V814_THRESHOLD_5_ADD,		CVT_V814_THRESHOLD_AM,		CVT_V814_THRESHOLD_DATA_SIZE},				/\*!< \brief Threshold 5 register *\/ */
/* 	{ CVT_V814_THRESHOLD_6_ADD,		CVT_V814_THRESHOLD_AM,		CVT_V814_THRESHOLD_DATA_SIZE},				/\*!< \brief Threshold 6 register *\/ */
/* 	{ CVT_V814_THRESHOLD_7_ADD,		CVT_V814_THRESHOLD_AM,		CVT_V814_THRESHOLD_DATA_SIZE},				/\*!< \brief Threshold 7 register *\/ */
/* 	{ CVT_V814_THRESHOLD_8_ADD,		CVT_V814_THRESHOLD_AM,		CVT_V814_THRESHOLD_DATA_SIZE},				/\*!< \brief Threshold 8 register *\/ */
/* 	{ CVT_V814_THRESHOLD_9_ADD,		CVT_V814_THRESHOLD_AM,		CVT_V814_THRESHOLD_DATA_SIZE},				/\*!< \brief Threshold 9 register *\/ */
/* 	{ CVT_V814_THRESHOLD_10_ADD,	CVT_V814_THRESHOLD_AM,		CVT_V814_THRESHOLD_DATA_SIZE},				/\*!< \brief Threshold 10 register *\/ */
/* 	{ CVT_V814_THRESHOLD_11_ADD,	CVT_V814_THRESHOLD_AM,		CVT_V814_THRESHOLD_DATA_SIZE},				/\*!< \brief Threshold 11 register *\/ */
/* 	{ CVT_V814_THRESHOLD_12_ADD,	CVT_V814_THRESHOLD_AM,		CVT_V814_THRESHOLD_DATA_SIZE},				/\*!< \brief Threshold 12 register *\/ */
/* 	{ CVT_V814_THRESHOLD_13_ADD,	CVT_V814_THRESHOLD_AM,		CVT_V814_THRESHOLD_DATA_SIZE},				/\*!< \brief Threshold 13 register *\/ */
/* 	{ CVT_V814_THRESHOLD_14_ADD,	CVT_V814_THRESHOLD_AM,		CVT_V814_THRESHOLD_DATA_SIZE},				/\*!< \brief Threshold 14 register *\/ */
/* 	{ CVT_V814_THRESHOLD_15_ADD,	CVT_V814_THRESHOLD_AM,		CVT_V814_THRESHOLD_DATA_SIZE},				/\*!< \brief Threshold 15 register *\/ */
/* 	{ CVT_V814_OUT_WIDTH_0_7_ADD,	CVT_V814_OUT_WIDTH_0_7_AM,	CVT_V814_OUT_WIDTH_0_7_DATA_SIZE},			/\*!< \brief Output width register Ch 0-7 *\/ */
/* 	{ CVT_V814_OUT_WIDTH_8_15_ADD,	CVT_V814_OUT_WIDTH_8_15_AM,	CVT_V814_OUT_WIDTH_8_15_DATA_SIZE},			/\*!< \brief Output width register Ch 8-15 *\/ */
/* 	{ CVT_V814_DEAD_TIME_0_7_ADD,	CVT_V814_DEAD_TIME_0_7_AM,	CVT_V814_DEAD_TIME_0_7_DATA_SIZE},			/\*!< \brief Dead time register Ch 0-7 *\/ */
/* 	{ CVT_V814_DEAD_TIME_8_15_ADD,	CVT_V814_DEAD_TIME_8_15_AM,	CVT_V814_DEAD_TIME_8_15_DATA_SIZE},			/\*!< \brief Dead time register Ch 8-15 *\/ */
/* 	{ CVT_V814_MAJORITY_ADD,		CVT_V814_MAJORITY_AM,		CVT_V814_MAJORITY_DATA_SIZE},				/\*!< \brief CVT_V814_MAJORITY threshold register *\/ */
/* 	{ CVT_V814_PATTERN_INHIBIT_ADD,	CVT_V814_PATTERN_INHIBIT_AM,CVT_V814_PATTERN_INHIBIT_DATA_SIZE},		/\*!< \brief Pattern inhibit register *\/ */
/* 	{ CVT_V814_TEST_PULSE_ADD,		CVT_V814_TEST_PULSE_AM,		CVT_V814_TEST_PULSE_DATA_SIZE},				/\*!< \brief Test pulse register relative *\/ */
/* 	{ CVT_V814_FIXED_CODE_ADD,		CVT_V814_FIXED_CODE_AM,		CVT_V814_FIXED_CODE_DATA_SIZE},				/\*!< \brief Fixed code register relative *\/ */
/* 	{ CVT_V814_MANUFACTURER_ADD,	CVT_V814_MANUFACTURER_AM,	CVT_V814_MANUFACTURER_DATA_SIZE},			/\*!< \brief Manufacturer and Module type register relative *\/ */
/* 	{ CVT_V814_VERSION_ADD,			CVT_V814_VERSION_AM,		CVT_V814_VERSION_DATA_SIZE},				/\*!< \brief Version and serial number register relative *\/ */
/* }; */
////////////////////////////////////////////////////////////////////////////////////////////////
// Static methods declaration
////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////
// Global visible variables declaration
////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////
//
//     B O A R D S   H A N D L I N G
//
////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////////////

#define V814_DEBUG 1
unsigned short cvt_V814_init( int32_t BHandle )
{
  // basic data initialization
  /* address=V814_0_BA; */
  /* if( !cvt_board_open( &p_data->m_common_data, base_address, vme_handle, CVT_V814_REG_TABLE)) */
  /*   return FALSE; */
  /* // board specific data initialization */
  /* p_data->m_type= type; */

  unsigned short status(1);
 
  status*=cvt_V814_set_pattern_inhibit(BHandle,0x000F);
  if (status!=1)
    {
      std::cout << "Error initializing V814 " << status << std::endl;
      return -1;
    }
  if (V814_DEBUG)
    std::cout <<  "Setting pattern 0x000F " << status << std::endl;

  status*=cvt_V814_set_threshold(BHandle,CVT_V814_THRESHOLD_0,0x40);

  if (status!=1)
    {
      std::cout << "Error initializing V814 " << status << std::endl;
      return -1;
    }
  status*=cvt_V814_set_threshold(BHandle,CVT_V814_THRESHOLD_1,0x14);
  if (status!=1)
    {
      std::cout << "Error initializing V814" << std::endl;
      return -1;
    }
  status*=cvt_V814_set_threshold(BHandle,CVT_V814_THRESHOLD_2,0x2D);
  if (status!=1)
    {
      std::cout << "Error initializing V814" << std::endl;
      return -1;
    }
  status*=cvt_V814_set_threshold(BHandle,CVT_V814_THRESHOLD_3,0x2D);
  if (status!=1)
    {
      std::cout << "Error initializing V814" << std::endl;
      return -1;
    }
  if (V814_DEBUG)
    std::cout <<  "Set thresholds " << status << std::endl;

  status*=cvt_V814_set_majority_threshold(BHandle,0x6); // single 0x6 double 0x13 triple 0x1F
  if (status!=1)
    {
      std::cout << "Error initializing V814" << std::endl;
      return -1;
    }
  if (V814_DEBUG)
    std::cout <<  "Set majority " << status << std::endl;
  status*=cvt_V814_set_output_width(BHandle,0xC3);
  if (status!=1)
    {
      std::cout << "Error initializing V814" << std::endl;
      return -1;
    }
  if (V814_DEBUG)
    std::cout <<  "Set output width " << status << std::endl;
  return status;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////////////
/* unsigned short cvt_V814_close( int32_t BHandle) */
/* { */
/* 	if( !cvt_board_close( &p_data->m_common_data)) */
/* 		return FALSE; */
/* 	return TRUE; */
/* } */

////////////////////////////////////////////////////////////////////////////////////////////////
//
//     L E V E L   0   A P I s
//
////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////
//
//     L E V E L   1   A P I s
//
////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////////////
unsigned short cvt_V814_set_threshold( int32_t BHandle, cvt_V814_threshold_id threshold_id, unsigned int value)
{
  unsigned short status(1),caenst;
  unsigned int reg_value= value;
  
  // Parameter checking
  switch( threshold_id)
    {
    case CVT_V814_THRESHOLD_ALL:						// Catch all the thresholds
      {
	// write every threshold
	int i;
	for( i= CVT_V814_THRESHOLD_0; i< CVT_V814_MAX_THRESHOLD; i++)
	  {
	    //  status = vme_write_dt(address, poi2data, V513_AM, V513_DS);
	    caenst = CAENVME_WriteCycle(BHandle, V814_0_BA + ( i - CVT_V814_THRESHOLD_0) * 0x2 ,&reg_value,cvA24_U_DATA,cvD16);
	    status *= (1-caenst); 
	  /*   if( !cvt_write_reg( &p_data->m_common_data, , &reg_value)) */
	  /*     { */
	  /* 	printf( "V814 THRESHOLD %i write failed !\n", i- CVT_V814_THRESHOLD_0); */
	  /* 	return FALSE; */
	  /*     } */
	  /* } */
	  }
      }
      break;
    case CVT_V814_THRESHOLD_0:							// Threshold 0 register
    case CVT_V814_THRESHOLD_1:							// Threshold 1 register
    case CVT_V814_THRESHOLD_2:							// Threshold 2 register
    case CVT_V814_THRESHOLD_3:							// Threshold 3 register
    case CVT_V814_THRESHOLD_4:							// Threshold 4 register
    case CVT_V814_THRESHOLD_5:							// Threshold 5 register
    case CVT_V814_THRESHOLD_6:							// Threshold 6 register
    case CVT_V814_THRESHOLD_7:							// Threshold 7 register
    case CVT_V814_THRESHOLD_8:							// Threshold 8 register
    case CVT_V814_THRESHOLD_9:							// Threshold 9 register
    case CVT_V814_THRESHOLD_10:							// Threshold 10 register
    case CVT_V814_THRESHOLD_11:							// Threshold 11 register
    case CVT_V814_THRESHOLD_12:							// Threshold 12 register
    case CVT_V814_THRESHOLD_13:							// Threshold 13 register
    case CVT_V814_THRESHOLD_14:							// Threshold 14 register
    case CVT_V814_THRESHOLD_15:							// Threshold 15 register
		// write specific threshold
      caenst = CAENVME_WriteCycle(BHandle,V814_0_BA + ( threshold_id - CVT_V814_THRESHOLD_0)*0x2 ,&reg_value,cvA24_U_DATA,cvD16);
      status *= (1-caenst); 
      break;
    default:
      printf( "V814 Unhandled threshold id %i !\n", threshold_id);
      return -1;
    }
  return status;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////////////
unsigned short cvt_V814_set_output_width( int32_t BHandle, unsigned int value)
{
  unsigned short status(1),caenst;
  unsigned int reg_value= value;
  // write registers
  caenst = CAENVME_WriteCycle(BHandle,V814_0_BA + CVT_V814_OUT_WIDTH_0_7_ADD ,&reg_value,cvA24_U_DATA,cvD16);
  status *= (1-caenst); 


  caenst = CAENVME_WriteCycle(BHandle,V814_0_BA + CVT_V814_OUT_WIDTH_8_15_ADD ,&reg_value,cvA24_U_DATA,cvD16);
  status *= (1-caenst); 

  return status;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////////////
unsigned short cvt_V814_set_majority_threshold( int32_t BHandle, unsigned int value)
{
  unsigned short status(1),caenst;
  unsigned int reg_value= value;
  unsigned int address=V814_0_BA + CVT_V814_MAJORITY_ADD; 
  caenst = CAENVME_WriteCycle(BHandle,address,&reg_value,cvA24_U_DATA,cvD16);
  status *= (1-caenst); 

  return status;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////////////
unsigned short cvt_V814_set_pattern_inhibit( int32_t BHandle, unsigned int value)
{
  unsigned short status(1),caenst;
  unsigned int reg_value = value;
  unsigned int address=V814_0_BA + CVT_V814_PATTERN_INHIBIT_ADD; 
  caenst = CAENVME_WriteCycle(BHandle,address,&reg_value,cvA24_U_DATA,cvD16);
  status *= (1-caenst); 
  // write register
  return status;
}

/* //////////////////////////////////////////////////////////////////////////////////////////////// */
/* //  */
/* //////////////////////////////////////////////////////////////////////////////////////////////// */
/* unsigned short cvt_V814_set_test_pulse( int32_t BHandle) */
/* { */
/* 	unsigned int reg_value= 0; */
/* 	// write register */
/* 	if( !cvt_write_reg( &p_data->m_common_data, CVT_V814_TEST_PULSE_INDEX, &reg_value)) */
/* 	{ */
/* 		printf( "V814 CVT_V814_TEST_PULSE write failed !\n"); */
/* 		return FALSE; */
/* 	} */
/* 	return TRUE; */
/* } */

/* //////////////////////////////////////////////////////////////////////////////////////////////// */
/* //  */
/* //////////////////////////////////////////////////////////////////////////////////////////////// */
/* unsigned short cvt_V814_get_fixed_code( int32_t BHandle, unsigned int *p_value) */
/* { */
/* 	unsigned int reg_value= 0; */
/* 	// read register */
/* 	if( !cvt_read_reg( &p_data->m_common_data, CVT_V814_FIXED_CODE_INDEX, &reg_value)) */
/* 	{ */
/* 		printf( "V814 CVT_V814_FIXED_CODE read failed !\n"); */
/* 		return FALSE; */
/* 	} */
/* 	*p_value= reg_value; */
/* 	return TRUE; */
/* } */

/* //////////////////////////////////////////////////////////////////////////////////////////////// */
/* //  */
/* //////////////////////////////////////////////////////////////////////////////////////////////// */
/* unsigned short cvt_V814_get_manufacturer_number( int32_t BHandle, unsigned int *p_value) */
/* { */
/* 	unsigned int reg_value= 0; */
/* 	// read register */
/* 	if( !cvt_read_reg( &p_data->m_common_data, CVT_V814_MANUFACTURER_INDEX, &reg_value)) */
/* 	{ */
/* 		printf( "V814 CVT_V814_MANUFACTURER_NUMBER read failed !\n"); */
/* 		return FALSE; */
/* 	} */
/* 	*p_value= (unsigned int)( ((unsigned int)(reg_value>> 10))& 0x3f); */
/* 	return TRUE; */
/* } */

/* //////////////////////////////////////////////////////////////////////////////////////////////// */
/* //  */
/* //////////////////////////////////////////////////////////////////////////////////////////////// */
/* unsigned short cvt_V814_get_module_type( int32_t BHandle, unsigned int *p_value) */
/* { */
/* 	unsigned int reg_value= 0; */
/* 	// read register */
/* 	if( !cvt_read_reg( &p_data->m_common_data, CVT_V814_MANUFACTURER_INDEX, &reg_value)) */
/* 	{ */
/* 		printf( "V814 CVT_V814_MODULE_TYPE read failed !\n"); */
/* 		return FALSE; */
/* 	} */
/* 	*p_value= (unsigned int)( reg_value& 0x03ff); */
/* 	return TRUE; */
/* } */

/* //////////////////////////////////////////////////////////////////////////////////////////////// */
/* //  */
/* //////////////////////////////////////////////////////////////////////////////////////////////// */
/* unsigned short cvt_V814_get_version( int32_t BHandle, unsigned int *p_value) */
/* { */
/* 	unsigned int reg_value= 0; */
/* 	// read register */
/* 	if( !cvt_read_reg( &p_data->m_common_data, CVT_V814_VERSION_INDEX, &reg_value)) */
/* 	{ */
/* 		printf( "V814 CVT_V814_VERSION read failed !\n"); */
/* 		return FALSE; */
/* 	} */
/* 	*p_value= (unsigned int)( ((unsigned int)(reg_value>> 12))& 0x0f); */
/* 	return TRUE; */
/* } */

/* //////////////////////////////////////////////////////////////////////////////////////////////// */
/* //  */
/* //////////////////////////////////////////////////////////////////////////////////////////////// */
/* unsigned short cvt_V814_get_serial_number( int32_t BHandle, unsigned int *p_value) */
/* { */
/* 	unsigned int reg_value= 0; */
/* 	// read register */
/* 	if( !cvt_read_reg( &p_data->m_common_data, CVT_V814_VERSION_INDEX, &reg_value)) */
/* 	{ */
/* 		printf( "V814 CVT_V814_SERIAL_NUMBER read failed !\n"); */
/* 		return FALSE; */
/* 	} */
/* 	*p_value= (unsigned int)( reg_value& 0x0fff); */
/* 	return TRUE; */
/* } */


/* //////////////////////////////////////////////////////////////////////////////////////////////// */
/* // */
/* //     L E V E L   2   A P I s */
/* // */
/* //////////////////////////////////////////////////////////////////////////////////////////////// */


/* //////////////////////////////////////////////////////////////////////////////////////////////// */
/* // */
/* //     M I S C E L L A N E O U S   A P I s */
/* // */
/* //////////////////////////////////////////////////////////////////////////////////////////////// */
/* //////////////////////////////////////////////////////////////////////////////////////////////// */
/* // */
/* //////////////////////////////////////////////////////////////////////////////////////////////// */
/* const char* cvt_V814_SW_rev( void) */
/* { */
/* 	return "CAENdiscrSDK Rev. 1.1"; */
/* } */
