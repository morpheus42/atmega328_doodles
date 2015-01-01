ifndef UTIL_EVTS_MAKEFILE
UTIL_EVTS_MAKEFILE=1
MYDIR:=$(abspath $(dir $(lastword $(MAKEFILE_LIST))))


SOURCEFILES+=$(addprefix $(MYDIR)/, evts.c)
INCLUDEPATH+=$(MYDIR)
SOURCEPATH+=$(MYDIR)
MODULES+=$(MYDIR)/Makefile


include $(DEVROOT)/util/cb/Makefile


ifndef INCLUDED
# Included make!

else
# Normal make!

endif

endif


