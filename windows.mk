ROOT_DIR := $(abspath .)

OFFSET=\x1b[41G
COLOR=\x1b[1;34m
RESET=\x1b[0m
CLEAR=\x1b[H\x1b[J

TOOLS := $(CC) $(COPY) $(DUMP) $(AR) $(MK) $(MV) $(CP) $(SED)

#copmiling dirs
CODE_DIR = $(ROOT_DIR)/code

OBJS_DIR  = $(ROOT_DIR)/out/GCC/Debug/Obj
BINS_DIR  = $(ROOT_DIR)/out/GCC/Debug/Exe
RULES_DIR = $(ROOT_DIR)/example/main/project/GCC/build_rules/

SUBDIRS  = $(CODE_DIR)

include $(RULES_DIR)/host_cmd_windows.mk
include $(RULES_DIR)/.config
include $(RULES_DIR)/make_flags.mk


all: COMPILE_BOJS LINK_OUTPUT
	$(QUIET)$(ECHO) "Call bat to build ota bin."
	$(QUIET)$(ROOT_DIR)/script/links/GCC/s9070_build.bat $(ROOT_DIR)/out/GCC $(ROOT_DIR)/out/GCC

COMPILE_BOJS :
	$(QUIET)$(ECHO) "$(SUBDIRS)"
	$(MAKE) -C $(SUBDIRS)

LINK_OUTPUT: 
	$(QUIET)$(ECHO)
	$(QUIET)$(ECHO) "*********************************************"
	$(QUIET)$(ECHO) "*              build elf...                 *"
	$(QUIET)$(ECHO) "*********************************************"
	$(QUIET)$(ECHO)
	$(QUIET)$(CP) $(RULES_DIR)/make_target.mk	$(OBJS_DIR)/Makefile
	$(QUIET)$(MAKE) -C $(OBJS_DIR) clean
	$(QUIET)$(MAKE) -C $(OBJS_DIR)

.PHONY :

clean :
	$(QUIET)$(RM) -f $(OBJS_DIR)/*.o
	$(QUIET)$(RM) -f $(OBJS_DIR)/*.d
	$(QUIET)$(ECHO) "Clean objects and depends completed."
	
help:
	@echo "*******************MAKEFILE HELP*************************"
	@echo "* the project include these compoments:                 *"
	@echo "* atcmd cmsis core net utils startup                    *"
	@echo "* input: make , start building project                  *"
	@echo "* input: make clean, can remove all objects             *"
	@echo "* input: make +compoment can clean the compoment        *"
	@echo "* for exmaple: make atcmd can clean the objects of atcmd*"
	@echo "*******************SCI-MAKEFILE**************************"
atcmd :
	cd $(CODE_DIR)/atcmd; make clean 
cmsis :
	cd $(CODE_DIR)/cmsis; make clean 
core  :
	cd $(CODE_DIR)/core; make clean 
net   :
	cd $(CODE_DIR)/net; make clean 
utils :
	cd $(CODE_DIR)/utils; make clean 
startup :
	cd $(CODE_DIR)/startup; make clean 


	
