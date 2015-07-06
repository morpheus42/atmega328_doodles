

# TAB[	]
OPTI:=0
AVRDUDE_PORT:=/dev/ttyUSB0
AVRDUDE_BAUD:=115200
AVRDUDE_PROGRAMMER:=stk500v2

export DEVROOT := $(abspath ../..)
export BUILDROOT := $(abspath .)/BuildDir
MYPATH:=$(abspath .)

include $(DEVROOT)/tools/make/Makefile.first

PROJECT := tst3
INCLUDEPATH:=$(MYPATH)
SOURCEPATH:=$(MYPATH)
SOURCEFILES := $(addprefix $(MYPATH)/, main.c config.c)
#sck.c pq.c pkt.c serpkt.c lbdpkt.c
CFLAGS:= -fdefault-inline

all:


INCLUDE:=INCLUDE
include $(DEVROOT)/board/lnx/Makefile.cfg
include $(DEVROOT)/de/lnx/Makefile.cfg

include $(DEVROOT)/drv/lnx/uartc.Makefile
include $(DEVROOT)/util/evts/Makefile
include $(DEVROOT)/util/tmrs/Makefile
include $(DEVROOT)/util/buf/fl.Makefile
include $(DEVROOT)/util/stime/Makefile
include $(DEVROOT)/svc/n1/Makefile
include $(DEVROOT)/svc/pktifs/lbd.Makefile
include $(DEVROOT)/svc/pktifs/ser.Makefile
include $(DEVROOT)/svc/pktifs/lnxudpb.Makefile

include $(DEVROOT)/tools/make/Makefile.inc

	
	
	
