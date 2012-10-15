SUBDIRS = libstm32f4xx libusbcore libusbdevice libsystem libgraphics libsdcard libgaming libfatfs

SELF_DIR := $(dir $(lastword $(MAKEFILE_LIST)))
include $(addprefix $(SELF_DIR),$(addsuffix /Makefile,$(SUBDIRS)))
