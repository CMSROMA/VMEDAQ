#include <vector>
#include <fstream>

/* GENERAL SETTING  */
#define V1718 1
#define IO513 0
#define IO262 1
#define TDC488A 0
#define TDC1190 1
#define TDC1190_2 0
#define ADC265 1
#define ADC792 1
#define ADC792_2 0
#define ADC792_3 0
#define DIG1742 1
#define SCALER560 1
#define DISCR814 1
#define debug 1

/* 1 modulo attivo*/

#define HEADER_SIZE 7
#define EVT_SIZE HEADER_SIZE
short TOTAL_value[EVT_SIZE];

unsigned short writeFastEvent(std::vector<int> wriD, std::ofstream *Fouf);
