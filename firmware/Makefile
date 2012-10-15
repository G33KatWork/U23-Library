SUBDIRS = gametest spacecraft controller_test

SELF_DIR := $(dir $(lastword $(MAKEFILE_LIST)))
include $(addprefix $(SELF_DIR),$(addsuffix /Makefile,$(SUBDIRS)))
