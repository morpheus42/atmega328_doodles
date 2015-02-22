ifndef DRV_ATMEGA_TICP_MAKEFILE
DRV_ATMEGA_TICP_MAKEFILE=1
MYDIR:=$(abspath $(dir $(lastword $(MAKEFILE_LIST))))


SOURCEFILES+=$(addprefix $(MYDIR)/, ticp.c)
INCLUDEPATH+=$(MYDIR)
SOURCEPATH+=$(MYDIR)
MODULES+=$(MYDIR)/ticp.Makefile


#include $(DEVROOT)/util/buf/fl.Makefile


ifndef INCLUDED
# Included make!

else
# Normal make!

endif

endif


