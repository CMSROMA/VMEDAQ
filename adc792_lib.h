#ifndef _V792N_HEADER_
#define _V792N_HEADER_

#include <vector>
#include "modules_config.h"

#define NUMADBOARDS	        1
#define V792N_ADDRESS               V792_0_BA
#define V792N_ADDRESS2              0x00040000
#define V792N_ADDRESS3              0x00090000
#define V792N_CHANNEL               32

#define V792N_REG1_STATUS           0x100e
#define V792N_REG2_STATUS           0x1022
#define V792N_REG1_CONTROL          0x1010
#define V792N_BIT_SET1              0x1006
#define V792N_BIT_CLEAR1            0x1008
#define V792N_BIT_SET2              0x1032
#define V792N_BIT_CLEAR2            0x1034
#define V792N_OUTPUT_BUFFER         0x0000
#define V792N_MCST_CBLT_ADDRESS     0x1004
#define V792N_INTERRUPT_LEVEL       0x100a
#define V792N_INTERRUPT_VECTOR      0x100c
#define V792N_ADER_HIGH             0x1012
#define V792N_ADER_LOW              0x1014
#define V792N_MCST_CBLT_CTRL        0x101a
#define V792N_EVENT_TRIGGER_REG     0x1020
#define V792N_EVENT_COUNTER_L       0x1024
#define V792N_EVENT_COUNTER_H       0x1026
#define V792N_FCLR_WINDOW           0x102e
#define V792N_W_MEM_TEST_ADDRESS    0x1036
#define V792N_MEM_TEST_WORD_HIGH    0x1038
#define V792N_CRATE_SELECT          0x103c
#define V792N_IPED                  0x1060
#define V792N_R_TEST_ADDRESS        0x1064
#define V792N_SS_RESET_REG          0x1016

#define adc792_debug    0

void print_adc792_debug_word(uint32_t word);
void check_adc792_status_afterRead(int32_t BHandle,int idB);
int find_adc792_eventSize(std::vector<int>& events,unsigned int evtStart);
unsigned short init_adc792(int32_t BHandle,int idB);
unsigned short dataReset792(int32_t BHandle, int32_t address);
unsigned short softReset792(int32_t BHandle, int32_t address);
std::vector<int> read_adc792(int32_t BHandle, short int& status);
std::vector<int> readFastadc792(int32_t BHandle, int idB, short int& status);
std::vector<int> readFastNadc792(int32_t BHandle, int idB, short int& status, int nevts, std::vector<int> &outW);

struct a792_shift{
  int datareg;
  int statusreg1;
  int statusreg2;
} ;
static const struct a792_shift adc792_shift=
{0x0000,0x100e,0x1022};

struct a792_bitmask{
  unsigned long rdy;
  unsigned long busy;
  unsigned long full;
  unsigned long empty;
};
static const struct a792_bitmask adc792_bitmask=
{0x1,0x4,0x4,0x2};
#endif
