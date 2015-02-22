ifndef SVC_DHT11_MAKEFILE
SVC_DHT11_MAKEFILE=1
MYDIR:=$(abspath $(dir $(lastword $(MAKEFILE_LIST))))


SOURCEFILES+=$(addprefix $(MYDIR)/, dht11.c)
INCLUDEPATH+=$(MYDIR)
SOURCEPATH+=$(MYDIR)
MODULES+=$(MYDIR)/Makefile


include $(DEVROOT)/util/cb/Makefile
include $(DEVROOT)/drv/atmega/ticp.Makefile



ifndef INCLUDED
# Included make!

else
# Normal make!

endif

endif


