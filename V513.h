#include <fstream>
#include <vector>

#ifndef _DEF_V513_
#define _DEF_V513_
using namespace std;

unsigned short init_V513(int32_t BHandle); 
unsigned short trigger_V513(int32_t BHandle);
unsigned short reset_daq(int32_t BHandle);
unsigned short clear_strobe_V513(int32_t BHandle);
unsigned short read_V513_old(int32_t BHandle,unsigned short*);
vector <int> read_V513(int32_t BHandle,unsigned short*);
unsigned short busy_V513(int32_t BHandle,int);
unsigned short PulseCh1_V513(int32_t BHandle);

#define V513_CHANNEL       16
#define V513_ADDRESS    0x00020000 /* fissato dai rotary switches sulla scheda */
#define V513_AM         0x39    /* address modifier -> D24  = cvA24_U_DATA */
#define V513_DS         0x2     /* data size -> D16  = cvD16 */
#define V513_BUSY_VALUE    0x1 
#define V513_CHAN_POLARITY 0x1
#define V513_CHAN_INPUT_MODE 0x1
#define DAQ_BUSY_ON      0x1
#define DAQ_BUSY_OFF     0x0
#define V513_debug            0

struct i_shift{
  int clear_inp;
  int init_status;
  int strobe_clear;
  int reset;
  int clear_int;
  int int_mask;
  int strobe_reg;
  int input;
  int output;
  int level_int;
  int vector_int;
};

static const struct i_shift io_shift =
{0x48,0x46,0x44,0x42,0x40,0x8,0x6,0x4,0x4,0x2,0x0 };

/* DIRECTION -> 0 = Output ; 1 = Input*/

static const int V513_CHAN_DIRECTION[V513_CHANNEL] =
{0x0,0x0,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1};

static const int V513_CHAN_TRANSFER_MODE[V513_CHANNEL]=
{0x0,0x0,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1};


#endif
