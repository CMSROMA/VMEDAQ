#include <fstream>
#include "modules_config.h"
#include <vector>

#ifndef _DEF_V262_
#define _DEF_V262_
using namespace std;

unsigned short init_V262(int32_t BHandle); 
unsigned short PulseCh_V262(int32_t BHandle,unsigned short channel);
unsigned short OutCh_V262(int32_t BHandle,unsigned short channel, unsigned short level);

#define V262_ADDRESS    V262_0_BA /* fissato dai rotary switches sulla scheda */
/* #define V262_AM         0x39    /\* address modifier -> D24  = cvA24_U_DATA *\/ */
/* #define V262_DS         0x2     /\* data size -> D16  = cvD16 *\/ */
/* #define V262_BUSY_VALUE    0x1  */
/* #define V262_CHAN_POLARITY 0x1 */
/* #define V262_CHAN_INPUT_MODE 0x1 */
/* #define DAQ_BUSY_ON      0x1 */
/* #define DAQ_BUSY_OFF     0x0 */
#define V262_debug            0

/* struct i_shift{ */
/*   int clear_inp; */
/*   int init_status; */
/*   int strobe_clear; */
/*   int reset; */
/*   int clear_int; */
/*   int int_mask; */
/*   int strobe_reg; */
/*   int input; */
/*   int output; */
/*   int level_int; */
/*   int vector_int; */
/* }; */

/* static const struct i_shift io_shift = */
/* {0x48,0x46,0x44,0x42,0x40,0x8,0x6,0x4,0x4,0x2,0x0 }; */

/* /\* DIRECTION -> 0 = Output ; 1 = Input*\/ */

/* static const int V262_CHAN_DIRECTION[V262_CHANNEL] = */
/* {0x0,0x0,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1}; */

/* static const int V262_CHAN_TRANSFER_MODE[V262_CHANNEL]= */
/* {0x0,0x0,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1}; */


#endif
