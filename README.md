VMEDAQ V0.1
============

Applications to read and support DAQ using VME and CAEN V1718 bus controller

Dependencies:
a) CAEN V1718 device driver 
b) CAEN VMELIB

Usage:
Edit the modules base addresses in modules_config.h 

Compile:
make all

There are 2 main applications:

# Main DAQ application
./acquire -p <#events to average the rate> -n <number of total event> -f <output_raw_file> 
# Simple count application
./count -t <time_in_msec>

Modules debugged in this version are V1719, ADCV792, SCALERV560
