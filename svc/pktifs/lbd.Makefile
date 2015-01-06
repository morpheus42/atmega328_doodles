ifndef SVC_PKTIFS_LBDMAKEFILE
SVC_PKTIFS_LBDMAKEFILE=1
MYDIR:=$(abspath $(dir $(lastword $(MAKEFILE_LIST))))


SOURCEFILES+=$(addprefix $(MYDIR)/, lbdpkt.c)
INCLUDEPATH+=$(MYDIR)
SOURCEPATH+=$(MYDIR)
MODULES+=$(MYDIR)/Makefile


include $(DEVROOT)/svc/pktifs/Makefile


ifndef INCLUDED
# Included make!

else
# Normal make!

endif

endif


