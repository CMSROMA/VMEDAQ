#------------------------------------------------------------------------------ 
# Title:        Makefile for the CT acquisition
# Date:         2009                                                    
# Author:	A.Sarti
# Platform:	Linux 2.4.x
# Language:	GCC 2.95 and 3.0
#------------------------------------------------------------------------------  
# daq for the CAEN bridge v1718


OBJS = adc265_lib.o adc792_lib.o tdc1190_lib.o tdcV488A_lib.o v1718_lib.o vme_bridge.o V513_lib.o V262_lib.o scaler560_lib.o V1742_lib.o V814_lib.o   X742CorrectionRoutines.o

OBJS_CAENCOMM = V814_lib_CAENComm.o V513_lib_CAENComm.o V262_lib_CAENComm.o adc792_lib_CAENComm.o X742CorrectionRoutines.o V1742_lib.o 

BINS = acquire count acquire_CAENComm

INCLUDE_DIR = .

COPTS = -g -Wall -DLINUX -fPIC -I$(INCLUDE_DIR)

all: $(BINS)

acquire: $(OBJS) main_acquisition.o 
	g++ -g -DLINUX -o acquire $(OBJS) -lncurses -lm -l CAENVME  -lCAENComm -lCAENDigitizer main_acquisition.o

count: $(OBJS) simple_count.o
	g++ -g -DLINUX -o count $(OBJS) -lncurses -lm -l CAENVME -lCAENComm -lCAENDigitizer simple_count.o

acquire_CAENComm: $(OBJS_CAENCOMM) testDigitizer.o
	g++ -g -DLINUX -o acquire_CAENComm $(OBJS_CAENCOMM) -lncurses -lm -l CAENVME -lCAENComm -lCAENDigitizer testDigitizer.o

main_acquisition.o: main_acquisition.c main_acquisition.h
	g++ $(COPTS) -c main_acquisition.c

# main_acquisition_digitizer.o: main_acquisition_digitizer.c main_acquisition.h
# 	g++ $(COPTS) -c main_acquisition_digitizer.c

testDigitizer.o: testDigitizer.c
	g++ $(COPTS) -c testDigitizer.c

simple_count.o: simple_count.c
	g++ $(COPTS) -c simple_count.c

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

V513_lib_CAENComm.o: V513_lib_CAENComm.c V513_CAENComm.h 
	g++ $(COPTS) -c V513_lib_CAENComm.c

V262_lib.o: V262_lib.cc V262.h 
	g++ $(COPTS) -c V262_lib.cc

V1742_lib.o: V1742_lib.c V1742_lib.h
	g++ $(COPTS) -c  V1742_lib.c

V814_lib.o: V814_lib.c V814_lib.h 
	g++ $(COPTS) -c V814_lib.c

V814_lib_CAENComm.o: V814_lib_CAENComm.c V814_lib_CAENComm.h 
	g++ $(COPTS) -c V814_lib_CAENComm.c

V262_lib_CAENComm.o: V262_lib_CAENComm.c V262_CAENComm.h 
	g++ $(COPTS) -c V262_lib_CAENComm.c

adc792_lib_CAENComm.o: adc792_lib_CAENComm.c adc792_lib_CAENComm.h 
	g++ $(COPTS) -c adc792_lib_CAENComm.c

vme_bridge.o: vme_bridge.c $(INCLUDE_DIR)/vme_bridge.h 	
	g++ $(COPTS) -c vme_bridge.c	

X742CorrectionRoutines.o:  X742CorrectionRoutines.h  X742CorrectionRoutines.c
	g++ $(COPTS) -c  X742CorrectionRoutines.c		
clean:
	-rm -f #*#
	-rm -f *~
	-rm -f core
	-rm -f $(BINS)
	-rm -f $(OBJS)
	-rm -f $(OBJS_CAENCOMM)
