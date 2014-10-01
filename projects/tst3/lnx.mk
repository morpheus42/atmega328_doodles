

# TAB[	]
OPTI:=s
AVRDUDE_PORT:=/dev/ttyUSB0
AVRDUDE_BAUD:=115200
AVRDUDE_PROGRAMMER:=stk500v2

export DEVROOT := $(abspath ../..)
export BUILDROOT := $(abspath .)/BuildDir
MYPATH:=$(abspath .)

PROJECT := tst1
INCLUDEPATH:=$(MYPATH)
SOURCEPATH:=$(MYPATH)
SOURCEFILES := $(addprefix $(MYPATH)/, main.c )


all:


INCLUDE:=INCLUDE
include $(DEVROOT)/board/lnx/Makefile.cfg
include $(DEVROOT)/de/lnx/Makefile.cfg

include $(DEVROOT)/drv/lnx/uartc.Makefile
include $(DEVROOT)/util/evts/Makefile
include $(DEVROOT)/util/cb/Makefile
#include $(DEVROOT)/svc/mudem/Makefile



include $(DEVROOT)/tools/make/Makefile.inc

	
	
	