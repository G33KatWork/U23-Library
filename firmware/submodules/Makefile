SUBDIRS = samplemodule minGame pewpewpew spiditorius fauli

SELF_DIR = $(dir $(lastword $(MAKEFILE_LIST)))
include $(addprefix $(SELF_DIR),$(addsuffix /Makefile,$(SUBDIRS)))
