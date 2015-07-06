

ifeq (X$(MAKEFILEFIRST),X)
  $(error 'Makefile.first' has not yet been included!!!)
endif

ifeq (X$(DEVROOT),X) 
  export DEVROOT = $(CURDIR)
  export DEVCUR = $(DEVROOT)
endif
ifeq (X$(BUILDROOT),X)
  export BUILDROOT = $(CURDIR)/BuildDir
  export BUILDCUR = $(BUILDROOT)
endif

INCLUDED=INCLUDED


#vpath %.c $(SOURCEPATH) /
#vpath %.S $(SOURCEPATH) /
VPATH = $(SOURCEPATH) /

#.PHONY: all $(MODULES)

CFLAGS+=$(addprefix -I,$(INCLUDEPATH))


oname = ${patsubst %.c,%.c.o,${patsubst %.S,%.S.o,$(1)}}
BOBJS += ${addprefix $(BUILDROOT)/,${call oname, $(notdir $(SOURCEFILES))}}


dname = ${patsubst %.c,%.d,${patsubst %.S,,$(1)}}
BDEPS += ${addprefix $(BUILDROOT)/,${call dname, $(notdir $(SOURCEFILES))}}


$(BUILDROOT)/%.d : %.c
	@echo "Rule c->d at tools/make"
	mkdir -p $(dir $@)
	$(CC) -M $(CFLAGS) -MF $@ $< -MT "$(BUILDROOT)/$*.c.o $@"
  


include $(BDEPS)

$(BUILDROOT)/$(PROJECT).elf: $(BOBJS) $(LDSCRIPTS)
	@echo LDSCRIPTS $(LDSCRIPTS)
	$(LD) $(LDSCRIPTS) $(LDFLAGS) -o $@  $(BOBJS)



.DEFAULT:
	@echo ".DEFAULT: $@ ....."

#.DEFAULT_GOAL:all


all:$(PROJECTTARGETS)
	@echo "$@: $^"
	@echo Modules: $(MODULES)
	@echo SourceFiles: $(SOURCEFILES)
	@echo IncPath: $(INCLUDEPATH)
	@echo SrcPath: $(SOURCEPATH)
	@echo LdScripts: $(LDSCRIPTS)



