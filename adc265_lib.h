#include <vector>

#ifndef _DEF_ADC265_
#define _DEF_ADC265_
/*   ADC v265 header file   */

#define ADC265_ADDRESS   0x100000
#define ADC265_CHANNEL   8
#define ADC265_MAX_BUFFER_EVENT_SIZE 16
#define ADC265_AM        0x39    /* ADC265 address modifier -> A24 */
#define ADC265_DS        0x2     /* ADC265 data size -> D16 */

#define ADC265_DEBUG    0

unsigned short init_adc265(int32_t BHandle);
std::vector<int> read_adc265(int32_t BHandle,int status);
std::vector<int> read_Nadc265(int32_t BHandle, int nevents,int status);
unsigned short read_adc265_simple(int32_t BHandle, int *pAdcData);

struct a265_shift{
  int version;
  int module;
  int datareg;
  int gategen;
  int dacreg;
  int clear ;
  int statusreg;
} ;
static const struct a265_shift adc265_shift=
  {0xfe,0xfc,0x08,0x06,0x04,0x02,0x00};

struct a265_bitmask{
  unsigned long rdy;
  unsigned long full;
  unsigned long irlevel;
  unsigned long range;
  unsigned long channel;
  unsigned long convdata;
};
static const struct a265_bitmask adc265_bitmask=
  {0x8000,0x4000,0x700,0x1000,0xe000,0xfff };

#endif



