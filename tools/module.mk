# Makefile generique pour les modules

# make options
MAKEFLAGS = --no-print-directory

# Toolchain main directory for binaries and includes
TOOLCHAIN_DIR=/opt/avr8-gnu-toolchain-linux_x86_64

ARCH = avr
DEBUG = -g
LIB_ROOTFS = $(TOOLCHAIN_DIR)/lib
BIN_ROOTFS = $(TOOLCHAIN_DIR)/include

CROSS_COMPILE = $(TOOLCHAIN_DIR)/bin/$(ARCH)-

INCLUDES = \
	-Iinc \
	-Iapi

# Le fichier qui contient les dependances dans chaque module
include include.mk

LIBS_PATH += $(patsubst %, -L$(PWD)/%/lib, $(SUBDIRS_ENV))
LIBS_PATH += $(patsubst %, -L$(PWD)/%/lib, $(SUBDIRS_MOD))
LIBS_PATH += -L$(LIB_ROOTFS)

LIBS += $(patsubst modules/%, -l%, $(SUBDIRS_MOD))
LIBS += $(patsubst env/%, -l%, $(SUBDIRS_ENV))
#LIBS += -lpthread #-lwiringPi -lwiringPiDev

INCLUDES += $(patsubst %, -I$(PWD)/%/api, $(SUBDIRS_MOD))
INCLUDES += $(patsubst %, -I$(PWD)/%/api, $(SUBDIRS_ENV))
INCLUDES += $(patsubst %, -I$(PWD)/%/api, $(SUBDIR_MAIN))
INCLUDES += -I$(PWD)/tools/api
INCLUDES += -I$(BIN_ROOTFS)

C_CPLUS_FLAGS = -Wextra -Wall -Wundef -Wfloat-equal -Wshadow -Wpointer-arith -Wcast-align -Wstrict-overflow=5
C_CPLUS_FLAGS += -Os -pedantic
C_CPLUS_FLAGS += -Wswitch-default -Wswitch-enum
C_CPLUS_FLAGS += -Wunreachable-code -Wconversion -Wcast-qual
C_CPLUS_FLAGS += -mmcu=atmega328p -DF_CPU=16000000UL
C_CPLUS_FLAGS += -DARDUINO=10819 -DARDUINO_AVR_UNO -DARDUINO_ARCH_AVR -MMD
C_CPLUS_FLAGS += $(INTEG_LOG_LEVEL)

C_FLAGS		+= $(C_CPLUS_FLAGS) -Wstrict-prototypes -std=c99
C_FLAGS		+= -D_POSIX_C_SOURCE=199506L
CPLUS_FLAGS	+= $(C_CPLUS_FLAGS) -std=c++11

AR = ar
CC = gcc
CXX = g++
STRIP = strip
RM = rm -rf
OBJCOPY = objcopy

LIB = lib$(notdir $(shell pwd)).a
OBJ_C = $(patsubst src/%.c, obj/%.o, $(wildcard src/*.c))
OBJ_CXX = $(patsubst src/%.cpp, obj/%.o, $(wildcard src/*.cpp))
PRE_C = $(patsubst src/%.c, pre/%.d, $(wildcard src/*.c))
PRE_CXX = $(patsubst src/%.cpp, pre/%.d, $(wildcard src/*.cpp))

BIN = arduino.bin
HEX = $(BIN:.bin=.hex)

LIB_DIR = lib
OBJ_DIR = obj
SRC_DIR = src
BIN_DIR = bin
PRE_DIR = pre

# On compile une librairie statique
all: lib

# Include all prerequisites
include $(PRE_C) $(PRE_CXX)

hex: $(BIN_DIR)/$(HEX)

bin: $(BIN_DIR)/$(BIN)

$(PRE_DIR):
	@echo "     MKDIR $@"
	@mkdir -p $@

$(OBJ_DIR):
	@echo "     MKDIR $@"
	@mkdir -p $@

$(LIB_DIR):
	@echo "     MKDIR $@"
	@mkdir -p $@

%_dir:
	@echo "     MKDIR $@"
	@mkdir -p $(BIN_DIR)

$(OBJ_C) $(OBJ_CXX): | $(OBJ_DIR)

$(PRE_C) $(PRE_CXX): | $(PRE_DIR)

#$(LIB_DIR)/$(LIB): | $(LIB_DIR)

$(BIN_DIR)/$(HEX): $(BIN_DIR)/$(BIN) | bin_dir
	@echo "   OBJCOPY $(notdir $@)"
	@$(CROSS_COMPILE)$(OBJCOPY) -O ihex -R .eeprom $< $@

$(BIN_DIR)/$(BIN): $(OBJ_C) $(OBJ_CXX) $(addprefix $(PWD)/, $(LIST_LIBENV) $(LIST_LIBMOD)) | bin_dir
	@echo "        LD $(notdir $@)"
	@$(CROSS_COMPILE)$(CXX) $(C_CPLUS_FLAGS) $^ $(LIBS_PATH) $(LIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "        CC $(notdir $@)"
	@$(CROSS_COMPILE)$(CC) $(INCLUDES) $(C_FLAGS) $(DEBUG) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo "       CXX $(notdir $@)"
	@$(CROSS_COMPILE)$(CXX) $(INCLUDES) $(CPLUS_FLAGS) $(DEBUG) -c $< -o $@

$(PRE_DIR)/%.d: $(SRC_DIR)/%.c
	@echo "       PRE $(notdir $@)"
	@$(RM) $@
	@$(CROSS_COMPILE)$(CC) $(INCLUDES) $(C_FLAGS) -MM $< > $@.$$$$; \
	sed 's#$(subst .c,.o,$(notdir $<))[ :]*#$(OBJ_DIR)/$(subst .c,.o,$(notdir $<)) $@: #g' < $@.$$$$ > $@; \
	$(RM) $@.$$$$

$(PRE_DIR)/%.d: $(SRC_DIR)/%.cpp
	@echo "       PRE $(notdir $@)"
	@$(CROSS_COMPILE)$(CXX) $(INCLUDES) $(CPLUS_FLAGS) -MM $< > $@

$(LIB_DIR)/$(LIB): $(OBJ_C) $(OBJ_CXX) | $(LIB_DIR)
	@echo "        AR $(notdir $@)"
	@$(CROSS_COMPILE)$(AR) rcs $@ $^

clean:
	@echo "     CLEAN $(OBJ_DIR)"
	@$(RM) $(OBJ_DIR)

prereq_clean:
	@echo "     CLEAN $(PRE_DIR)"
	@$(RM) $(PRE_DIR)

libclean: clean prereq_clean
	@echo "     CLEAN $(LIB_DIR)"
	@$(RM) $(LIB_DIR)

distclean: clean prereq_clean
	@echo "     CLEAN $(BIN_DIR)"
	@$(RM) $(BIN_DIR)

print-%:
	@echo $* = $($(*))

.PHONY: all lib bin obj_dir lib_dir bin_dir clean prereq_clean libclean distclean
