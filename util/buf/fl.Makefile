ifndef UTIL_BUF_FLMAKEFILE
UTIL_BUF_FLMAKEFILE=1
MYDIR:=$(abspath $(dir $(lastword $(MAKEFILE_LIST))))


SOURCEFILES+=$(addprefix $(MYDIR)/, buffl.c)
INCLUDEPATH+=$(MYDIR)
SOURCEPATH+=$(MYDIR)
MODULES+=$(MYDIR)/Makefile


# include $(DEVROOT)/util/cb/Makefile


ifndef INCLUDED
# Included make!

else
# Normal make!

endif

endif


