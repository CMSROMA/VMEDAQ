#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "printbit.h"

/*-------------------------------------------------*/
void printbit_long(unsigned long c) { 
  int i;
  unsigned long result=0x80000000;
  printf("bit: ");
  for(i=0;i<32;++i)
    { 
      if( (result&c) != 0){printf("1",result,c, i);}
      else{printf("0",result,c, i);}
      result=result>>1;
    }
  printf("\n");
}
