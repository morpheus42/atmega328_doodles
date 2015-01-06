ifndef SVC_PKTIFS_SERMAKEFILE
SVC_PKTIFS_SERMAKEFILE=1
MYDIR:=$(abspath $(dir $(lastword $(MAKEFILE_LIST))))


SOURCEFILES+=$(addprefix $(MYDIR)/, serpkt.c)
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


