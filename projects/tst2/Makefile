

# TAB[	]


OPTI:=3
AVRDUDE_PORT:=/dev/ttyUSB0
AVRDUDE_BAUD:=115200
AVRDUDE_PROGRAMMER:=stk500v2

export DEVROOT := $(abspath ../..)
export BUILDROOT := $(abspath .)/BuildDir
MYPATH:=$(abspath .)

include $(DEVROOT)/tools/make/Makefile.first


PROJECT := tst2
INCLUDEPATH:=$(MYPATH)
SOURCEPATH:=$(MYPATH)
SOURCEFILES := $(addprefix $(MYPATH)/, main.c config.c)
CFLAGS += -DNOPRINTF




all:


INCLUDE:=INCLUDE
#include $(DEVROOT)/tools/make/Makefile.first
include $(DEVROOT)/board/arduino/nano/Makefile.cfg
include $(DEVROOT)/de/avr/Makefile.cfg

include $(DEVROOT)/drv/atmega/uart.Makefile
include $(DEVROOT)/drv/atmega/ticp.Makefile
include $(DEVROOT)/util/evts/Makefile
include $(DEVROOT)/util/buf/fl.Makefile
include $(DEVROOT)/util/tmrs/Makefile
include $(DEVROOT)/svc/n1/Makefile
include $(DEVROOT)/svc/pktifs/lbd.Makefile
include $(DEVROOT)/svc/pktifs/ser.Makefile
include $(DEVROOT)/svc/dht11/Makefile

include $(DEVROOT)/tools/make/Makefile.inc

	
	
	
