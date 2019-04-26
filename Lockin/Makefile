# System paths for compiler and support package
PRU_CGT:= /usr/share/ti/cgt-pru
PRU_SUPPORT:= /usr/lib/ti/pru-software-support-package
PRU_BBB := /home/debian/BBBLockin

# Define current directory
MKFILE_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
CURRENT_DIR := $(notdir $(patsubst %/,%,$(dir $(MKFILE_PATH))))
PROJ_DIR=$(CURRENT_DIR)

# Path to temporary folder
GEN_DIR:=gen

# PRU Folders
PRU_1_DIR:=$(CURRENT_DIR)/PRU1/
PRU_2_DIR:=$(CURRENT_DIR)/PRU2/

LINKER_COMMAND_FILE=./AM335x_PRU.cmd

# Paths to libraries
LIBS=--library=$(PRU_SUPPORT)/lib/rpmsg_lib.lib
INCLUDE=--include_path=$(PRU_CGT)/include --include_path=$(PRU_SUPPORT)/include/ --include_path=$(PRU_SUPPORT)/include/am335x/

# Stack & Heap size
STACK_SIZE=0x100
HEAP_SIZE=0x100

# Common compiler and linker flags (Defined in 'PRU Optimizing C/C++ Compiler User's Guide)
CFLAGS=-v3 -O2 --c99 --float_operations_allowed=none --display_error_number --endian=little --hardware_mac=on --printf_support=minimal --obj_directory=$(GEN_DIR) --pp_directory=$(GEN_DIR) -ppd -ppa

# Linker flags (Defined in 'PRU Optimizing C/C++ Compiler User's Guide)
LFLAGS=--reread_libs --warn_sections --stack_size=$(STACK_SIZE) --heap_size=$(HEAP_SIZE)

# Targets
PRU_0_TARGET=$(GEN_DIR)/PRU0/$(PROJ_DIR)_0.out
PRU_1_TARGET=$(GEN_DIR)/PRU1/$(PROJ_DIR)_1.out

# Map file
PRU_0_MAP=$(GEN_DIR)/PRU0/$(PROJ_DIR)_0.out
PRU_1_MAP=$(GEN_DIR)/PRU1/$(PROJ_DIR)_1.out

# Source files
PRU_0_SOURCES=$(wildcard /PRU0/*.c)
PRU_1_SOURCES=$(wildcard /PRU1/*.c)

#Using .object instead of .obj in order to not conflict with the CCS build process
PRU_0_OBJECTS=$(patsubst %,$(GEN_DIR)%,$(PRU_0_SOURCES:.c=.object))
PRU_1_OBJECTS=$(patsubst %,$(GEN_DIR)%,$(PRU_1_SOURCES:.c=.object))

PRU_0_SYSFS=$(wildcard /sys/class/remoteproc/remoteproc1)
PRU_1_SYSFS=$(wildcard /sys/class/remoteproc/remoteproc2)

all: stop clean git install upload start

stop:
	@echo "-	Stopping PRU-ICSS"
	@echo 'stop' | sudo tee -a $(PRU_0_SYSFS)/state > /dev/null || echo '-	Cannot stop PRU 0'
	@echo 'stop' | sudo tee -a $(PRU_1_SYSFS)/state > /dev/null || echo '-	Cannot stop PRU 1'

git:
	@echo '-	Updating repository'
	@sudo git pull

start:
	@echo "-	Starting PRU-ICSS"
	@echo 'start' | sudo tee -a $(PRU_0_SYSFS)/state > /dev/null || echo '-	Cannot start PRU 0'
	@echo 'start' | sudo tee -a $(PRU_1_SYSFS)/state > /dev/null || echo '-	Cannot start PRU 1'

install: $(PRU_0_TARGET) $(PRU_1_TARGET)
	@echo '-	Copying firmware files $(GEN_DIR)/$(PROJ_DIR).out & $(GEN_DIR)/$(PROJ_DIR).out to /lib/firmware/am335x-pru0-fw & /lib/firmware/am335x-pru1-fw'
	@sudo cp $(GEN_DIR)/$(PROJ_DIR)_PRU0.out /lib/firmware/am335x-pru0-fw
	@sudo cp $(GEN_DIR)/$(PROJ_DIR)_PRU1.out /lib/firmware/am335x-pru1-fw

	$(PRU_0_TARGET): $(PRU_0_OBJECTS) $(LINKER_COMMAND_FILE)
		@echo '-	Invoking: PRU Linker for PRU0: $^'
		@$(PRU_CGT)/bin/clpru $(CFLAGS) -z -i$(PRU_BBB)/include -i$(PRU_CGT)/lib -i$(PRU_CGT)/include $(LFLAGS) -o $(TARGET0) $(PRU_0_OBJECTS) -m$(PRU_0_MAP) $(LINKER_COMMAND_FILE) --library=libc.a $(LIBS)

	$(PRU_1_TARGET): $(PRU_1_OBJECTS) $(LINKER_COMMAND_FILE)
		@echo '-	Invoking: PRU Linker for PRU1: $^'
		@$(PRU_CGT)/bin/clpru $(CFLAGS) -z -i$(PRU_BBB)/include -i$(PRU_CGT)/lib -i$(PRU_CGT)/include $(LFLAGS) -o $(TARGET1) $(PRU_1_OBJECTS) -m$(PRU_1_MAP) $(LINKER_COMMAND_FILE) --library=libc.a $(LIBS)

	$(GEN_DIR)/PRU0/%.object: /PRU0/%.c
		@mkdir -p $(GEN_DIR)
		@echo '-	Invoking: PRU Compiler for PRU0: $<'
		@$(PRU_CGT)/bin/clpru $(INCLUDE) $(CFLAGS) -fe $@ $< -D=PRUN=PRU0

	$(GEN_DIR)/PRU1/%.object: /PRU1/%.c
		@mkdir -p $(GEN_DIR)
		@echo '-	Invoking: PRU Compiler for PRU1: $<'
		@$(PRU_CGT)/bin/clpru $(INCLUDE) $(CFLAGS) -fe $@ $< -D=PRUN=PRU1

upload:


clean:
	@echo '-	Cleaning output folders'
	@rm -rf $(GEN_DIR)
