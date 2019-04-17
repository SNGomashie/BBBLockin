ifdef PRU
	PRUN := $(PRU)
	PRU_ADDR := $(PRUN)+1
else
 	PRUN := 0
	PRU_ADDR := 1
endif

# System paths for compiler and support package
PRU_CGT:= /usr/share/ti/cgt-pru
PRU_SUPPORT:= /usr/lib/ti/pru-software-support-package

# Define current directory
MKFILE_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
CURRENT_DIR := $(notdir $(patsubst %/,%,$(dir $(MKFILE_PATH))))
PROJ_NAME=$(CURRENT_DIR)
FILE_NAME= Main
# Path to temporary folder
GEN_DIR:=gen

# Path to linker command file
LINKER_COMMAND_FILE=./AM335x_PRU.cmd

# Paths to libraries
LIBS=--library=$(PRU_SUPPORT)/lib/rpmsg_lib.lib
INCLUDE=--include_path=$(PRU_CGT)/include --include_path=$(PRU_SUPPORT)/include/ --include_path=$(PRU_SUPPORT)/include/am335x/ --include_path=../include/

# Stack & Heap size
STACK_SIZE=0x100
HEAP_SIZE=0x100

# Common compiler and linker flags (Defined in 'PRU Optimizing C/C++ Compiler User's Guide)
CFLAGS=-v3 -O2 --c99 --float_operations_allowed=none --display_error_number --endian=little --hardware_mac=on --printf_support=minimal --obj_directory=$(GEN_DIR) --pp_directory=$(GEN_DIR) -ppd -ppa

# Linker flags (Defined in 'PRU Optimizing C/C++ Compiler User's Guide)
LFLAGS=--reread_libs --warn_sections --stack_size=$(STACK_SIZE) --heap_size=$(HEAP_SIZE)

# Target file
TARGET=$(GEN_DIR)/$(PROJ_NAME).out

# Map file
MAP=$(GEN_DIR)/$(PROJ_NAME).map

# Source files
SOURCES=$(wildcard *.c)

#Using .object instead of .obj in order to not conflict with the CCS build process
OBJECTS=$(patsubst %,$(GEN_DIR)/%,$(SOURCES:.c=.object))

# Lookup PRU by address
ifeq ($(PRUN),0)
PRU_ADDR=remoteproc1
endif
ifeq ($(PRUN),1)
PRU_ADDR=remoteproc2
endif

# PRU sysfs interface directory
PRU_DIR=$(wildcard /sys/class/remoteproc/$(PRU_ADDR).*)

all: stop clean git install start

stop:
	@echo "-	Stopping PRU $(PRUN)"

	@echo 'stop' | sudo tee -a $(PRU_DIR)/state || echo Cannot stop $(PRUN)

git:
	@echo '-	Updating reposetory'
	@sudo git pull

start:
	@echo "-	Starting PRU $(PRUN)"

	@echo 'start' | sudo tee -a $(PRU_DIR)/state || echo Cannot start $(PRUN)

install: $(GEN_DIR)/$(PROJ_NAME).out
	@echo '-	Copying firmware file $(GEN_DIR)/$(PROJ_NAME).out to /lib/firmware/am335x-pru$(PRUN)-fw'
	@sudo cp $(GEN_DIR)/$(PROJ_NAME).out /lib/firmware/am335x-pru$(PRUN)-fw

$(GEN_DIR)/$(PROJ_NAME).out: $(GEN_DIR)/$(PROJ_NAME).obj
	@echo 'Invoking: PRU Linker: $^'
	@lnkpru -i$(PRU_CGT)/lib -i$(PRU_CGT)/include $(LFLAGS) -o $@ $^ $(LINKER_COMMAND_FILE) --library=libc.a $(LIBS) $^

$(GEN_DIR)/$(PROJ_NAME).obj: $(FILE_NAME).c
	@mkdir -p $(GEN_DIR)
	@echo 'Invoking: PRU Compiler: $<'
	@clpru --include_path=$(PRU_CGT)/include $(INCLUDE) $(CFLAGS) -D=PRUN=PRU$(PRUN) -fe $@ $<

clean:
	@echo '-	Clean'
	@rm -rf $(GEN_DIR)

# Includes the dependencies that the compiler creates (-ppd and -ppa flags)
-include $(OBJECTS:%.object=%.pp)
