////////////////////////////////////////////////////////////////////////////////////////////////
/*! \file    cvt_V814.h
*   \brief   V814 VME board definitions
*   \author  NDA
*   \version 1.0
*   \date    03/2006
*            
*            Provides methods, properties and defines to handle V814 VME boards
*/
////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __CVT_V814_DEF_H
#define __CVT_V814_DEF_H

#include "modules_config.h"

////////////////////////////////////////////////////////////////////////////////////////////////
// File includes
////////////////////////////////////////////////////////////////////////////////////////////////
/* #include "cvt_common_defs.h" */
/* #include "cvt_board_commons.h" */
////////////////////////////////////////////////////////////////////////////////////////////////
// Global defines
////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////
/*! \enum    CVT_DISCR_TYPES
*   \brief   The discriminator board type
*            
*            Provides an entry for each board type
*/
////////////////////////////////////////////////////////////////////////////////////////////////
/* typedef enum */
/* { */
/* 	CVT_V814= 0,						/\*!< \brief The board is V814 *\/ */
/* 	CVT_V814,							/\*!< \brief The board is V814 *\/ */
/* 	CVT_V895,							/\*!< \brief The board is V895 *\/ */
/* } CVT_DISCR_TYPES; */

////////////////////////////////////////////////////////////////////////////////////////////////
/*! \struct  cvt_V814_data
*   \brief   V814 board data structure
*            
*            This structure stores any board data, common and specific ones. 
*            It must be passed to each V814's API 
*/
////////////////////////////////////////////////////////////////////////////////////////////////
/* typedef struct */
/* { */
/* 	cvt_board_data	m_common_data;				/\*!< Boards common data structure: always let at offset 0 *\/ */
/* 	// */
/* 	// Board specific data */
/* 	CVT_DISCR_TYPES m_type;						/\*!< The board type (either V814, V814, V895) *\/ */

/* } cvt_V814_data; */

////////////////////////////////////////////////////////////////////////////////////////////////
// Registers address
////////////////////////////////////////////////////////////////////////////////////////////////
#define CVT_V814_THRESHOLD_0_ADD				0x0000			/*!< \brief Threshold 0 register relative address */
#define CVT_V814_THRESHOLD_1_ADD				0x0002			/*!< \brief Threshold 1 register relative address */
#define CVT_V814_THRESHOLD_2_ADD				0x0004			/*!< \brief Threshold 2 register relative address */
#define CVT_V814_THRESHOLD_3_ADD				0x0006			/*!< \brief Threshold 3 register relative address */
#define CVT_V814_THRESHOLD_4_ADD				0x0008			/*!< \brief Threshold 4 register relative address */
#define CVT_V814_THRESHOLD_5_ADD				0x000A			/*!< \brief Threshold 5 register relative address */
#define CVT_V814_THRESHOLD_6_ADD				0x000C			/*!< \brief Threshold 6 register relative address */
#define CVT_V814_THRESHOLD_7_ADD				0x000E			/*!< \brief Threshold 7 register relative address */
#define CVT_V814_THRESHOLD_8_ADD				0x0010			/*!< \brief Threshold 8 register relative address */
#define CVT_V814_THRESHOLD_9_ADD				0x0012			/*!< \brief Threshold 9 register relative address */
#define CVT_V814_THRESHOLD_10_ADD				0x0014			/*!< \brief Threshold 10 register relative address */
#define CVT_V814_THRESHOLD_11_ADD				0x0016			/*!< \brief Threshold 11 register relative address */
#define CVT_V814_THRESHOLD_12_ADD				0x0018			/*!< \brief Threshold 12 register relative address */
#define CVT_V814_THRESHOLD_13_ADD				0x001A			/*!< \brief Threshold 13 register relative address */
#define CVT_V814_THRESHOLD_14_ADD				0x001C			/*!< \brief Threshold 14 register relative address */
#define CVT_V814_THRESHOLD_15_ADD				0x001E			/*!< \brief Threshold 15 register relative address */
#define CVT_V814_OUT_WIDTH_0_7_ADD				0x0040			/*!< \brief Output width register Ch 0-7 relative address */
#define CVT_V814_OUT_WIDTH_8_15_ADD				0x0042			/*!< \brief Output width register Ch 8-15 relative address */
#define CVT_V814_MAJORITY_ADD					0x0048			/*!< \brief CVT_V814_MAJORITY threshold register relative address */
#define CVT_V814_PATTERN_INHIBIT_ADD			0x004A			/*!< \brief Pattern inhibit register relative address */
#define CVT_V814_TEST_PULSE_ADD					0x004C			/*!< \brief Test pulse register relative address */
#define CVT_V814_FIXED_CODE_ADD					0x00FA			/*!< \brief Fixed code register relative address */
#define CVT_V814_MANUFACTURER_ADD				0x00FC			/*!< \brief Manufacturer and Module type register relative address */
#define CVT_V814_VERSION_ADD					0x00FD			/*!< \brief Version and serial number register relative address */

/* #define CVT_V814_THRESHOLD_ADD_STEP				2				/\*!< \brief Threshold registers address step for V814 *\/ */

////////////////////////////////////////////////////////////////////////////////////////////////
// Registers data size
////////////////////////////////////////////////////////////////////////////////////////////////
/* #define CVT_V814_THRESHOLD_DATA_SIZE			cvD16			/\*!< \brief First threshold register data size *\/ */
/* #define CVT_V814_OUT_WIDTH_0_7_DATA_SIZE		cvD16			/\*!< \brief Output width register Ch 0-7 data size *\/ */
/* #define CVT_V814_OUT_WIDTH_8_15_DATA_SIZE		cvD16			/\*!< \brief Output width register Ch 8-15 data size *\/ */
/* #define CVT_V814_MAJORITY_DATA_SIZE				cvD16			/\*!< \brief CVT_V814_MAJORITY threshold register data size *\/ */
/* #define CVT_V814_PATTERN_INHIBIT_DATA_SIZE		cvD16			/\*!< \brief Pattern inhibit register data size *\/ */
/* #define CVT_V814_TEST_PULSE_DATA_SIZE			cvD16			/\*!< \brief Test pulse register relative data size *\/ */
/* #define CVT_V814_FIXED_CODE_DATA_SIZE			cvD16			/\*!< \brief Fixed code register relative data size *\/ */
/* #define CVT_V814_MANUFACTURER_DATA_SIZE			cvD16			/\*!< \brief Manufacturer and Module type register relative data size *\/ */
/* #define CVT_V814_VERSION_DATA_SIZE				cvD16			/\*!< \brief Version and serial number register relative data size *\/ */

////////////////////////////////////////////////////////////////////////////////////////////////
// Registers address modifiers
////////////////////////////////////////////////////////////////////////////////////////////////
/* #define CVT_V814_THRESHOLD_AM					cvA32_S_DATA	/\*!< \brief First threshold register address modifier *\/ */
/* #define CVT_V814_OUT_WIDTH_0_7_AM				cvA32_S_DATA	/\*!< \brief Output width register Ch 0-7 address modifier *\/ */
/* #define CVT_V814_OUT_WIDTH_8_15_AM				cvA32_S_DATA	/\*!< \brief Output width register Ch 8-15 address modifier *\/ */
/* #define CVT_V814_MAJORITY_AM					cvA32_S_DATA	/\*!< \brief CVT_V814_MAJORITY threshold register address modifier *\/ */
/* #define CVT_V814_PATTERN_INHIBIT_AM				cvA32_S_DATA	/\*!< \brief Pattern inhibit register address modifier *\/ */
/* #define CVT_V814_TEST_PULSE_AM					cvA32_S_DATA	/\*!< \brief Test pulse register relative address modifier *\/ */
/* #define CVT_V814_FIXED_CODE_AM					cvA32_S_DATA	/\*!< \brief Fixed code register relative address modifier *\/ */
/* #define CVT_V814_MANUFACTURER_AM				cvA32_S_DATA	/\*!< \brief Manufacturer and Module type register relative address modifier *\/ */
/* #define CVT_V814_VERSION_AM						cvA32_S_DATA	/\*!< \brief Version and serial number register relative address modifier *\/ */

////////////////////////////////////////////////////////////////////////////////////////////////
/*! \enum    CVT_V814_REG_INDEX
*   \brief   The registers indexes
*            
*            Provides an entry for each register: This is the index into the CVT_V814_REG_TABLE board table
*/
////////////////////////////////////////////////////////////////////////////////////////////////
/* typedef enum */
/* { */
/* 	CVT_V814_THRESHOLD_0_INDEX,				/\*!< \brief Threshold 0 register index *\/ */
/* 	CVT_V814_THRESHOLD_1_INDEX,				/\*!< \brief Threshold 1 register index *\/ */
/* 	CVT_V814_THRESHOLD_2_INDEX,				/\*!< \brief Threshold 2 register index *\/ */
/* 	CVT_V814_THRESHOLD_3_INDEX,				/\*!< \brief Threshold 3 register index *\/ */
/* 	CVT_V814_THRESHOLD_4_INDEX,				/\*!< \brief Threshold 4 register index *\/ */
/* 	CVT_V814_THRESHOLD_5_INDEX,				/\*!< \brief Threshold 5 register index *\/ */
/* 	CVT_V814_THRESHOLD_6_INDEX,				/\*!< \brief Threshold 6 register index *\/ */
/* 	CVT_V814_THRESHOLD_7_INDEX,				/\*!< \brief Threshold 7 register index *\/ */
/* 	CVT_V814_THRESHOLD_8_INDEX,				/\*!< \brief Threshold 8 register index *\/ */
/* 	CVT_V814_THRESHOLD_9_INDEX,				/\*!< \brief Threshold 9 register index *\/ */
/* 	CVT_V814_THRESHOLD_10_INDEX,			/\*!< \brief Threshold 10 register index *\/ */
/* 	CVT_V814_THRESHOLD_11_INDEX,			/\*!< \brief Threshold 11 register index *\/ */
/* 	CVT_V814_THRESHOLD_12_INDEX,			/\*!< \brief Threshold 12 register index *\/ */
/* 	CVT_V814_THRESHOLD_13_INDEX,			/\*!< \brief Threshold 13 register index *\/ */
/* 	CVT_V814_THRESHOLD_14_INDEX,			/\*!< \brief Threshold 14 register index *\/ */
/* 	CVT_V814_THRESHOLD_15_INDEX,			/\*!< \brief Threshold 15 register index *\/ */
/* 	CVT_V814_OUT_WIDTH_0_7_INDEX,			/\*!< \brief Output width register Ch 0-7 index *\/ */
/* 	CVT_V814_OUT_WIDTH_8_15_INDEX,			/\*!< \brief Output width register Ch 8-15 index *\/ */
/* 	CVT_V814_MAJORITY_INDEX,				/\*!< \brief CVT_V814_MAJORITY threshold register index *\/ */
/* 	CVT_V814_PATTERN_INHIBIT_INDEX,			/\*!< \brief Pattern inhibit register index *\/ */
/* 	CVT_V814_TEST_PULSE_INDEX,				/\*!< \brief Test pulse register relative index *\/ */
/* 	CVT_V814_FIXED_CODE_INDEX,				/\*!< \brief Fixed code register relative index *\/ */
/* 	CVT_V814_MANUFACTURER_INDEX,			/\*!< \brief Manufacturer and Module type register relative index *\/ */
/* 	CVT_V814_VERSION_INDEX,					/\*!< \brief Version and serial number register relative index *\/ */
/* } CVT_V814_REG_INDEX; */


/* #define CVT_V814_FIXED_CODE_VALUE				0xFAF5			/\*!< \brief The fixed code register value *\/ */
/* #define CVT_V814_MANUFACTURER_NUMBER_VALUE		0x02			/\*!< \brief The manufacturer number register value *\/ */
/* #define CVT_V814_MODULE_TYPE_VALUE				0x0051			/\*!< \brief The type of module register value *\/ */

////////////////////////////////////////////////////////////////////////////////////////////////
/*! \enum    cvt_V814_threshold_id 
*   \brief   V814 discriminator threshold IDs
*/
////////////////////////////////////////////////////////////////////////////////////////////////
typedef enum 
{
	CVT_V814_THRESHOLD_ALL= -1,						/*!< Catch all the thresholds */
	CVT_V814_THRESHOLD_0= 0,						/*!< Threshold 0 register */
	CVT_V814_THRESHOLD_1,							/*!< Threshold 1 register */
	CVT_V814_THRESHOLD_2,							/*!< Threshold 2 register */
	CVT_V814_THRESHOLD_3,							/*!< Threshold 3 register */
	CVT_V814_THRESHOLD_4,							/*!< Threshold 4 register */
	CVT_V814_THRESHOLD_5,							/*!< Threshold 5 register */
	CVT_V814_THRESHOLD_6,							/*!< Threshold 6 register */
	CVT_V814_THRESHOLD_7,							/*!< Threshold 7 register */
	CVT_V814_THRESHOLD_8,							/*!< Threshold 8 register */
	CVT_V814_THRESHOLD_9,							/*!< Threshold 9 register */
	CVT_V814_THRESHOLD_10,							/*!< Threshold 10 register */
	CVT_V814_THRESHOLD_11,							/*!< Threshold 11 register */
	CVT_V814_THRESHOLD_12,							/*!< Threshold 12 register */
	CVT_V814_THRESHOLD_13,							/*!< Threshold 13 register */
	CVT_V814_THRESHOLD_14,							/*!< Threshold 14 register */
	CVT_V814_THRESHOLD_15,							/*!< Threshold 15 register */
	CVT_V814_MAX_THRESHOLD							/*!< Dummy last command: define as last to know the number of thresholds */
} cvt_V814_threshold_id ;

/* #define CVT_V814_THRESHOLD_ADD_STEP					2					/\*!< \brief Threshold registers address step for V814 *\/ */

////////////////////////////////////////////////////////////////////////////////////////////////
// Global variables declaration
////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////
// Global methods declaration
////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////
//
//     B O A R D S   H A N D L I N G
//
////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
/*! \fn      BOOL cvt_V814_open( cvt_V814_data* p_data, UINT16 base_address, long vme_handle, CVT_DISCR_TYPES type);
*   \brief   V814 VME boards data initialization
*            
*            Provides specific handling for V814 boards opening.
*   \param   p_data Pointer to board data
*   \param   base_address The board base address (MSW)
*   \param   vme_handle The VME handle
*   \param   type The board type
*   \return  TRUE: board successfully opened
*   \note    Must be called before any other board specific API.
*/
////////////////////////////////////////////////////////////////////////////////////////////////
unsigned short cvt_V814_init( int32_t BHandle);

/* //////////////////////////////////////////////////////////////////////////////////////////////// */
/* /\*! \fn      BOOL cvt_V814_close( cvt_V814_data* p_data); */
/* *   \brief   V814 VME boards closing and resource free */
/* *             */
/* *            Provides specific handling for V814 boards closing. */
/* *   \param   p_data Pointer to board data */
/* *   \return  TRUE: board successfully closed */
/* *   \note    Must be called when done with any other board specific API. */
/* *\/ */
/* //////////////////////////////////////////////////////////////////////////////////////////////// */
/* CVT_DLL_API BOOL cvt_V814_close( cvt_V814_data* p_data); */

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
/*! \fn      BOOL cvt_V814_set_threshold( cvt_V814_data* p_data, cvt_V814_threshold_id threshold_id, UINT16 value);
*   \brief   Set the specified discriminator threshold register.
*            
*            Writes the specified value to the specified discriminator threshold register. If threshold_id is CVT_V814_THRESHOLD_ALL
*            every threshold will be setted to the same value.
*   \param   p_data Pointer to board data
*   \param   threshold_id The threshold to set. It must be a \ref cvt_V814_threshold_id valid id.
*   \param   value The value to set
*   \return  TRUE: Procedure successfully executed
*   \sa      cvt_V814_threshold_id 
*/
////////////////////////////////////////////////////////////////////////////////////////////////
unsigned short cvt_V814_set_threshold( int32_t BHandle, cvt_V814_threshold_id threshold_id, unsigned int value);

////////////////////////////////////////////////////////////////////////////////////////////////
/*! \fn      BOOL cvt_V814_set_output_width( cvt_V814_data* p_data, UINT8 value);
*   \brief   Set the output width register.
*            
*            Writes the specified value to the output width register for channel 0-7 and 8-15.
*   \param   p_data Pointer to board data
*   \param   value The value to set
*   \return  TRUE: Procedure successfully executed
*/
////////////////////////////////////////////////////////////////////////////////////////////////
unsigned short cvt_V814_set_output_width( int32_t BHandle, unsigned int value);

////////////////////////////////////////////////////////////////////////////////////////////////
/*! \fn      BOOL cvt_V814_set_majority_threshold( cvt_V814_data* p_data, UINT8 value);
*   \brief   Set the CVT_V814_MAJORITY threshold register.
*            
*            Writes the specified value to the CVT_V814_MAJORITY threshold register.
*   \param   p_data Pointer to board data
*   \param   value The value to set
*   \return  TRUE: Procedure successfully executed
*/
////////////////////////////////////////////////////////////////////////////////////////////////
unsigned short cvt_V814_set_majority_threshold( int32_t BHandle, unsigned int value);
	
////////////////////////////////////////////////////////////////////////////////////////////////
/*! \fn      BOOL cvt_V814_set_pattern_inhibit( cvt_V814_data* p_data, UINT16 value);
*   \brief   Set the pattern of inhibit register.
*            
*            Writes the specified value to the pattern of inhibit register.
*   \param   p_data Pointer to board data
*   \param   value The value to set
*   \return  TRUE: Procedure successfully executed
*/
////////////////////////////////////////////////////////////////////////////////////////////////
unsigned short cvt_V814_set_pattern_inhibit( int32_t BHandle, unsigned int value);

////////////////////////////////////////////////////////////////////////////////////////////////
/*! \fn      BOOL cvt_V814_set_test_pulse( cvt_V814_data* p_data);
*   \brief   Set the test pulse register.
*            
*            Writes a dummy value to the test pulse register.
*   \param   p_data Pointer to board data
*   \return  TRUE: Procedure successfully executed
*/
////////////////////////////////////////////////////////////////////////////////////////////////
/* CVT_DLL_API BOOL cvt_V814_set_test_pulse( cvt_V814_data* p_data); */
	
////////////////////////////////////////////////////////////////////////////////////////////////
/*! \fn      BOOL cvt_V814_get_fixed_code( cvt_V814_data* p_data, UINT16 *p_value);
*   \brief   Gets the board fixed code.
*            
*            Reads the fixed code register. It must return \ref CVT_V814_FIXED_CODE_VALUE
*   \param   p_data Pointer to board data
*   \param   p_value The value read
*   \return  TRUE: Procedure successfully executed
*   \sa      CVT_V814_FIXED_CODE_VALUE
*/
////////////////////////////////////////////////////////////////////////////////////////////////
/* CVT_DLL_API BOOL cvt_V814_get_fixed_code( cvt_V814_data* p_data, UINT16 *p_value); */

////////////////////////////////////////////////////////////////////////////////////////////////
/*! \fn      BOOL cvt_V814_get_manufacturer_number( cvt_V814_data* p_data, UINT8 *p_value);
*   \brief   Gets the manufacturer number.
*            
*            Reads the manufacturer number register. It must return \ref CVT_V814_MANUFACTURER_NUMBER_VALUE
*   \param   p_data Pointer to board data
*   \param   p_value The value read
*   \return  TRUE: Procedure successfully executed
*   \sa      CVT_V814_MANUFACTURER_NUMBER_VALUE
*/
////////////////////////////////////////////////////////////////////////////////////////////////
/* CVT_DLL_API BOOL cvt_V814_get_manufacturer_number( cvt_V814_data* p_data, UINT8 *p_value); */

////////////////////////////////////////////////////////////////////////////////////////////////
/*! \fn      BOOL cvt_V814_get_module_type( cvt_V814_data* p_data, UINT16 *p_value);
*   \brief   Gets the module type.
*            
*            Reads the module type register. It must return \ref CVT_V814_MODULE_TYPE_VALUE
*   \param   p_data Pointer to board data
*   \param   p_value The value read
*   \return  TRUE: Procedure successfully executed
*   \sa      CVT_V814_MODULE_TYPE_VALUE
*/
////////////////////////////////////////////////////////////////////////////////////////////////
/* CVT_DLL_API BOOL cvt_V814_get_module_type( cvt_V814_data* p_data, UINT16 *p_value); */

////////////////////////////////////////////////////////////////////////////////////////////////
/*! \fn      BOOL cvt_V814_get_version( cvt_V814_data* p_data, UINT8 *p_value);
*   \brief   Gets the module version.
*            
*            Reads the module version register.
*   \param   p_data Pointer to board data
*   \param   p_value The value read
*   \return  TRUE: Procedure successfully executed
*/
////////////////////////////////////////////////////////////////////////////////////////////////
/* CVT_DLL_API BOOL cvt_V814_get_version( cvt_V814_data* p_data, UINT8 *p_value); */

////////////////////////////////////////////////////////////////////////////////////////////////
/*! \fn      BOOL cvt_V814_get_serial_number( cvt_V814_data* p_data, UINT16 *p_value);
*   \brief   Gets the serial number.
*            
*            Reads the serial number register.
*   \param   p_data Pointer to board data
*   \param   p_value The value read
*   \return  TRUE: Procedure successfully executed
*   \sa      CVT_V814_MODULE_TYPE_VALUE
*/
////////////////////////////////////////////////////////////////////////////////////////////////
/* CVT_DLL_API BOOL cvt_V814_get_serial_number( cvt_V814_data* p_data, UINT16 *p_value); */

////////////////////////////////////////////////////////////////////////////////////////////////
//
//     L E V E L   2   A P I s
//
////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////
//
//     M I S C E L L A N E O U S   A P I s
//
////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////
/*! \fn      const char* cvt_V814_SW_rev( void);
*   \brief   Returns the SDK software release
*            
*   \return  const char*: The SDK revision string
*/
////////////////////////////////////////////////////////////////////////////////////////////////
/* CVT_DLL_API const char* cvt_V814_SW_rev( void); */

#endif 
