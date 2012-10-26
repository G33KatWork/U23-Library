ROOT := $(PWD)
include $(ROOT)/build/base.mak

FIRMWARE ?= gametest

SUBDIRS = libs firmware

STARTTIME := $(shell date +%s)
# Main targets
all:
	$(call cmd_msg,NOTICE,Build completed in $$(($$(date +%s)-$(STARTTIME))) seconds)

distclean: clean

upload: upload-$(FIRMWARE)
upload-gdb: upload-gdb-$(FIRMWARE)
debug-gdb: debug-gdb-$(FIRMWARE) 

include $(addsuffix /Makefile,$(SUBDIRS))

.PHONY: all upload clean distclean
