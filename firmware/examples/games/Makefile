SUBDIRS = spacecraft controller_test gamestate_test gametest gameoflife

SELF_DIR = $(dir $(lastword $(MAKEFILE_LIST)))
include $(addprefix $(SELF_DIR),$(addsuffix /Makefile,$(SUBDIRS)))
