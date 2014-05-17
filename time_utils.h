#include <sys/time.h> 
#include <time.h> 

//Reference time 2014-05-01 00:00:00
static tm ref_date;
static time_t ref_time;


long gettimestamp(struct timeval *time)
{
  long time_msec;
  time_msec=(time->tv_sec-ref_time)*1000;
  time_msec+=time->tv_usec/1000;
  return time_msec;
}

long timevaldiff(struct timeval *starttime, struct timeval *finishtime)
{
  long usec;
  usec=(finishtime->tv_sec-starttime->tv_sec)*1000000;
  usec+=(finishtime->tv_usec-starttime->tv_usec);
  return usec;
}

