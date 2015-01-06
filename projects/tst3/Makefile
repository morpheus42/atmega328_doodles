

# TAB[	]
OPTI:=3
AVRDUDE_PORT:=/dev/ttyUSB0
AVRDUDE_BAUD:=115200
AVRDUDE_PROGRAMMER:=stk500v2

export DEVROOT := $(abspath ../..)
export BUILDROOT := $(abspath .)/BuildDir
MYPATH:=$(abspath .)

PROJECT := tst1
INCLUDEPATH:=$(MYPATH)
SOURCEPATH:=$(MYPATH)
SOURCEFILES := $(addprefix $(MYPATH)/, main.c config.c)
CFLAGS += -DNOPRINTF




all:


INCLUDE:=INCLUDE
include $(DEVROOT)/board/arduino/nano/Makefile.cfg
include $(DEVROOT)/de/avr/Makefile.cfg

include $(DEVROOT)/drv/atmega/uart.Makefile
include $(DEVROOT)/util/evts/Makefile
include $(DEVROOT)/util/buf/fl.Makefile
include $(DEVROOT)/svc/n1/Makefile
include $(DEVROOT)/svc/pktifs/lbd.Makefile
include $(DEVROOT)/svc/pktifs/ser.Makefile

include $(DEVROOT)/tools/make/Makefile.inc

	
	
	
