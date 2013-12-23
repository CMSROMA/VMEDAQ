#------------------------------------------------------------------------------ 
# Title:        Makefile for the CT acquisition
# Date:         2009                                                    
# Author:	A.Sarti
# Platform:	Linux 2.4.x
# Language:	GCC 2.95 and 3.0
#------------------------------------------------------------------------------  
# daq for the CAEN bridge v1718


OBJS = main_acquisition.o adc265_lib.o adc792_lib.o tdc1190_lib.o tdcV488A_lib.o v1718_lib.o vme_bridge.o V513_lib.o scaler560_lib.o
HEAD = main_acquisition.h adc265_lib.h adc792_lib.h tdc1190_lib.h tdcV488A_lib.h v1718_lib.h vme_bridge.h V513.h scaler560_lib.h

INCLUDE_DIR = .

COPTS = -g -Wall -DLINUX -fPIC -I$(INCLUDE_DIR)

acquire: $(OBJS)
	g++ -g -DLINUX -o acquire $(OBJS) -lncurses -lm -l CAENVME


main_acquisition.o: main_acquisition.c main_acquisition.h tdc1190_lib.h tdcV488A_lib.h adc265_lib.h adc792_lib.h v1718_lib.h vme_bridge.h V513.h scaler560_lib.h
	g++ $(COPTS) -c main_acquisition.c

adc265_lib.o: adc265_lib.c adc265_lib.h
	g++ $(COPTS) -c adc265_lib.c

adc792_lib.o: adc792_lib.c adc792_lib.h 
	g++ $(COPTS) -c adc792_lib.c

scaler560_lib.o: scaler560_lib.c scaler560_lib.h 
	g++ $(COPTS) -c scaler560_lib.c

tdc1190_lib.o: tdc1190_lib.c tdc1190_lib.h 
	g++ $(COPTS) -c tdc1190_lib.c

tdcV488A_lib.o: tdcV488A_lib.c tdcV488A_lib.h 
	g++ $(COPTS) -c tdcV488A_lib.c

v1718_lib.o: v1718_lib.c v1718_lib.h 
	g++ $(COPTS) -c v1718_lib.c

V513_lib.o: V513_lib.cc V513.h 
	g++ $(COPTS) -c V513_lib.cc

vme_bridge.o: vme_bridge.c $(INCLUDE_DIR)/vme_bridge.h 	
	g++ $(COPTS) -c vme_bridge.c	

clean:
	-rm -f acquire
	-rm -f #*#
	-rm -f *~
	-rm -f core
	-rm -f $(OBJS)

