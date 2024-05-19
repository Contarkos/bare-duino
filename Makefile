# Raspberry pi specific makefile

##################################################
# Definition for the location of the modules
include ./tools/libs.mk

##################################################
# Fonctions

# For every dir provided, provide the complete path to the library file
_add_lib_suffix=$(foreach _dir,$(1),$(addsuffix /lib/lib$(notdir $(_dir)).a, $(_dir)))
# Get all objects file for one lib
_get_object_files=$(subst $(2),.o,$(subst /src/,/obj/,$(wildcard $(subst /lib/,/src,$(dir $(1)))/*$(2))))
# Get all prereq files for one directory
_get_prereq_files=$(subst $(2),.d,$(subst /src/,/pre/,$(wildcard $(1)/src/*$(2))))
# Get source from object file
_get_src_obj_file=$(subst /obj/,/src,$(dir $(1)))/$(subst .o,$(2),$(notdir $(1)))
# Get source from prerequisite file
_get_src_pre_file=$(subst /pre/,/src,$(dir $(1)))/$(subst .d,$(2),$(notdir $(1)))
# Get object file from source
_get_obj_src_file=$(subst /src/,/obj,$(dir $(1)))/$(subst $(2),.o,$(notdir $(1)))

##################################################
# Environment variables

PARALLEL = -j6
MAKEFLAGS = --no-print-directory

BIN = arduino.bin
INTEG_LOG_LEVEL = -DINTEGRATION_LOG_LEVEL=4

PWD = $(shell pwd)
SUBDIR_MAIN = env/MAIN
SUBDIR_BASE = env/BASE
SUBDIRS_ENV = $(filter-out $(SUBDIR_MAIN) $(SUBDIR_BASE), $(wildcard env/* ))
SUBDIRS_MOD = $(wildcard modules/*)
SUBDIR_DATA = DATA
SUBDIR_KERN = tools/modules

# List of the libraries needed to compile the binary
LIST_LIBENV = $(call _add_lib_suffix,$(SUBDIRS_ENV))
LIST_LIBMOD = $(call _add_lib_suffix,$(SUBDIRS_MOD))

# Create a list of rules to clean all the libs
LIST_CLEAN_ENV = $(addprefix clean_, $(notdir $(SUBDIRS_ENV)) $(notdir $(SUBDIR_MAIN)))
LIST_CLEAN_MOD = $(addprefix clean_, $(notdir $(SUBDIRS_MOD)))

DIST_CLEAN_ENV = $(addprefix distclean_, $(notdir $(SUBDIRS_ENV)) $(notdir $(SUBDIR_MAIN)))
DIST_CLEAN_MOD = $(addprefix distclean_, $(notdir $(SUBDIRS_MOD)))

# Get the list of all the objects file needed to compile the file
OBJ_FILES := $(foreach lib,$(LIST_LIBENV) $(LIST_LIBMOD),$(call _get_object_files,$(lib),.c))
OBJ_FILES += $(foreach lib,$(LIST_LIBENV) $(LIST_LIBMOD),$(call _get_object_files,$(lib),.cpp))

PREREQ_FILES := $(foreach _dir, env/MAIN $(SUBDIRS_ENV) $(SUBDIRS_MOD), $(call _get_prereq_files,$(_dir),.c))
PREREQ_FILES += $(foreach _dir, env/MAIN $(SUBDIRS_ENV) $(SUBDIRS_MOD), $(call _get_prereq_files,$(_dir),.cpp))

OBJ_FILES_MAIN = $(patsubst $(SUBDIR_MAIN)/src/%.cpp, $(SUBDIR_MAIN)/obj/%.o, $(wildcard $(SUBDIR_MAIN)/src/*.cpp))

PATH_BINARY = $(SUBDIR_MAIN)/bin/$(BIN)
PATH_STRIP  = $(PATH_BINARY).stripped
PATH_MAP    = $(PATH_BINARY:.bin=.map)
PATH_HEX    = $(PATH_BINARY:.bin=.hex)

OUTPUT_FILES = $(PATH_MAP) $(PATH_HEX)

##################################################
# Compilation variables

SCRIPT_FLASH=./tools/scripts/flash_arduino.sh
DEVICE_TTY_NAME=ACM0

TOOLCHAIN_DIR=/opt/avr-gcc-8.5.0-1-x64-linux

ARCH = avr
DEBUG = -g
LIB_ROOTFS = $(TOOLCHAIN_DIR)/lib
BIN_ROOTFS = $(TOOLCHAIN_DIR)/include

CROSS_COMPILE = $(TOOLCHAIN_DIR)/bin/$(ARCH)-

LIBS_PATH += $(patsubst %, -L$(PWD)/%/lib, $(SUBDIRS_ENV))
LIBS_PATH += $(patsubst %, -L$(PWD)/%/lib, $(SUBDIRS_MOD))
LIBS_PATH += -L$(LIB_ROOTFS)

LIBS += $(patsubst modules/%, -l%, $(SUBDIRS_MOD))
LIBS += $(patsubst env/%, -l%, $(SUBDIRS_ENV))
LIBS += -lpthread #-lwiringPi -lwiringPiDev

INCLUDES += $(patsubst %, -I$(PWD)/%/api, $(SUBDIR_BASE))
INCLUDES += $(patsubst %, -I$(PWD)/%/api, $(SUBDIRS_MOD))
INCLUDES += $(patsubst %, -I$(PWD)/%/api, $(SUBDIRS_ENV))
INCLUDES += $(patsubst %, -I$(PWD)/%/api, $(SUBDIR_MAIN))
INCLUDES += -I$(BIN_ROOTFS)

export PWD
export SUBDIR_MAIN
export SUBDIRS_ENV
export SUBDIRS_MOD
export SUBDIR_DATA

export LIST_LIBENV
export LIST_LIBMOD

##################################################
# Compilation rules

all: $(OUTPUT_FILES) | /tftpboot/
	@echo "        CP $(notdir $(PATH_HEX))"
	@cp    $(PATH_HEX) /tftpboot/
	@cp    $(PATH_HEX) /media/sf_partage_vm/
	@echo "$(shell date)"
	@echo "-----------------------------------"
	@echo " Done"

love:
	@echo "not war !"

# Include all the file defining the prerequisites for each source file
-include $(PREREQ_FILES)

.SECONDARY: $(addsuffix /lib/, $(SUBDIRS_ENV) $(SUBDIRS_MOD)) \
    $(addsuffix /obj/, $(SUBDIRS_ENV) $(SUBDIRS_MOD) $(SUBDIR_MAIN)) \
    $(SUBDIR_MAIN)/bin \
    $(addsuffix /pre/, $(SUBDIRS_ENV) $(SUBDIRS_MOD) $(SUBDIR_MAIN))

# Get the source file associated with the object file being compiled.
# One rule for C and another for C++
.SECONDEXPANSION:

$(PATH_HEX): $(PATH_BINARY)
	@$(MAKE) $(PARALLEL) -C $(SUBDIR_MAIN) -f module.mk hex

$(PATH_BINARY): $(LIST_LIBENV) $(LIST_LIBMOD) $(OBJ_FILES) $(OBJ_FILES_MAIN) | $(SUBDIR_MAIN)/bin
	@$(MAKE) $(PARALLEL) -C $(SUBDIR_MAIN) -f module.mk bin
	@#$(CROSS_COMPILE)$(CXX) $(OBJ_FILES_MAIN) $(LIBS_PATH) $(LIBS) -Xlinker -Map=$(PATH_MAP) -o $@

$(PATH_MAP): $(PATH_BINARY)
	@echo "       MAP $(notdir $@)"

# Get all the object files associated with the library being compiled
%.a: FORCE
	@$(MAKE) $(PARALLEL) -C $(subst /lib,,$(dir $@)) -f module.mk lib/$(notdir $@)
	@#$(CROSS_COMPILE)$(AR) rcs $@ $^

clean_%: modules/%
	@$(MAKE) -C $< -f module.mk clean

clean_%: env/%
	@$(MAKE) -C $< -f module.mk clean

distclean_%: modules/%
	@$(MAKE) -C $< -f module.mk libclean

distclean_%: env/% clean_%
	@$(MAKE) -C $< -f module.mk libclean

clean: $(LIST_CLEAN_MOD) $(LIST_CLEAN_ENV)

distclean: $(DIST_CLEAN_MOD) $(DIST_CLEAN_ENV)

install: $(PATH_HEX)
	@$(SCRIPT_FLASH) $(DEVICE_TTY_NAME) $<

# Tools
print-%:
	@echo $* = $($(*))

FORCE:

.PHONY: all main modules env clean distclean

