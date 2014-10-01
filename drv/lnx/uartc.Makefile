ifndef DRV_LNX_UARTC_MAKEFILE
DRV_LNX_UARTC_MAKEFILE=1
MYDIR:=$(abspath $(dir $(lastword $(MAKEFILE_LIST))))


SOURCEFILES+=$(addprefix $(MYDIR)/, uartc.c)
INCLUDEPATH+=$(MYDIR)
SOURCEPATH+=$(MYDIR)
MODULES+=$(MYDIR)/uartc.Makefile

LDFLAGS+=-pthread

ifndef INCLUDED
# Included make!

else
# Normal make!

endif

endif


