#ifndef _V792N_CAENCOMM_HEADER_
#define _V792N_CAENCOMM_HEADER_

#include <vector>
#include "modules_config.h"

#define NUMADBOARDS	        1
#define V792N_CAENCOMM_ADDRESS               0
#define V792N_CAENCOMM_ADDRESS2              0
#define V792N_CAENCOMM_ADDRESS3              0
#define V792N_CAENCOMM_CHANNEL               32

#define V792N_CAENCOMM_REG1_STATUS           0x100e
#define V792N_CAENCOMM_REG2_STATUS           0x1022
#define V792N_CAENCOMM_REG1_CONTROL          0x1010
#define V792N_CAENCOMM_BIT_SET1              0x1006
#define V792N_CAENCOMM_BIT_CLEAR1            0x1008
#define V792N_CAENCOMM_BIT_SET2              0x1032
#define V792N_CAENCOMM_BIT_CLEAR2            0x1034
#define V792N_CAENCOMM_OUTPUT_BUFFER         0x0000
#define V792N_CAENCOMM_MCST_CBLT_ADDRESS     0x1004
#define V792N_CAENCOMM_INTERRUPT_LEVEL       0x100a
#define V792N_CAENCOMM_INTERRUPT_VECTOR      0x100c
#define V792N_CAENCOMM_ADER_HIGH             0x1012
#define V792N_CAENCOMM_ADER_LOW              0x1014
#define V792N_CAENCOMM_MCST_CBLT_CTRL        0x101a
#define V792N_CAENCOMM_EVENT_TRIGGER_REG     0x1020
#define V792N_CAENCOMM_EVENT_COUNTER_L       0x1024
#define V792N_CAENCOMM_EVENT_COUNTER_H       0x1026
#define V792N_CAENCOMM_FCLR_WINDOW           0x102e
#define V792N_CAENCOMM_W_MEM_TEST_ADDRESS    0x1036
#define V792N_CAENCOMM_MEM_TEST_WORD_HIGH    0x1038
#define V792N_CAENCOMM_CRATE_SELECT          0x103c
#define V792N_CAENCOMM_IPED                  0x1060
#define V792N_CAENCOMM_R_TEST_ADDRESS        0x1064
#define V792N_CAENCOMM_SS_RESET_REG          0x1016

#define adc792_CAENCOMM_debug    1

void print_adc792_CAENCOMM_debug_word(uint32_t word);
void check_adc792_CAENCOMM_status_afterRead(int32_t BHandle,int idB);
int find_adc792_CAENCOMM_eventSize(std::vector<int>& events,unsigned int evtStart);
unsigned short init_adc792_CAENCOMM(int32_t BHandle);
unsigned short dataReset792_CAENCOMM(int32_t BHandle);
unsigned short softReset792_CAENCOMM(int32_t BHandle);
/* std::vector<int> read_adc792_CAENCOMM(int32_t BHandle, short int& status); */
/* std::vector<int> readFastadc792_CAENCOMM(int32_t BHandle, int idB, short int& status); */
std::vector<int> readFastNadc792_CAENCOMM(int32_t BHandle, short int& status, int nevts, std::vector<int> &outW);

struct a792_CAENCOMM_shift{
  int datareg;
  int statusreg1;
  int statusreg2;
} ;
static const struct a792_CAENCOMM_shift adc792_CAENCOMM_shift=
{0x0000,0x100e,0x1022};

struct a792_CAENCOMM_bitmask{
  unsigned long rdy;
  unsigned long busy;
  unsigned long full;
  unsigned long empty;
};
static const struct a792_CAENCOMM_bitmask adc792_CAENCOMM_bitmask=
{0x1,0x4,0x4,0x2};
#endif
