ifndef SVC_N1_MAKEFILE
SVC_N1_MAKEFILE=1
MYDIR:=$(abspath $(dir $(lastword $(MAKEFILE_LIST))))


SOURCEFILES+=$(addprefix $(MYDIR)/, pkt.c pq.c sck.c)
INCLUDEPATH+=$(MYDIR)
SOURCEPATH+=$(MYDIR)
MODULES+=$(MYDIR)/Makefile


include $(DEVROOT)/util/buf/fl.Makefile
include $(DEVROOT)/svc/pktifs/Makefile


ifndef INCLUDED
# Included make!

else
# Normal make!

endif

endif


