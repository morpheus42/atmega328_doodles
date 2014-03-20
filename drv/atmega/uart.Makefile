ifndef DRV_ATMEGA_UART_MAKEFILE
DRV_ATMEGA_UART_MAKEFILE=1
MYDIR:=$(abspath $(dir $(lastword $(MAKEFILE_LIST))))


SOURCEFILES+=$(addprefix $(MYDIR)/, uart.c)
INCLUDEPATH+=$(MYDIR)
SOURCEPATH+=$(MYDIR)
MODULES+=$(MYDIR)/uart.Makefile

ifndef INCLUDED
# Included make!

else
# Normal make!

endif

endif


