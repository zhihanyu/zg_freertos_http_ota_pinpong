
SUBDIRS := kv 

all: $(SUBDIRS)
$(SUBDIRS): ECHO
	$(MAKE) -C $@
ECHO:
	$(QUIET)$(ECHO) "Enter $(SUBDIRS) to compile."
