ifndef UTIL_LTENUM_MAKEFILE
UTIL_LTENUM_MAKEFILE=1
MYDIR:=$(abspath $(dir $(lastword $(MAKEFILE_LIST))))


#SOURCEFILES+=$(addprefix $(MYDIR)/, circularBuffer.c)
INCLUDEPATH+=$(MYDIR)
#SOURCEPATH+=$(MYDIR)
MODULES+=$(MYDIR)/Makefile

ifneq (X$(AVRMCU),X)
  LDSCRIPTS+=$(MYDIR)/ltenum.lds
endif

ifndef INCLUDED
# Included make!

else
# Normal make!

endif

endif


