ROOT                 := ../..
include $(ROOT)/build/base.mak

# Target file name.
TARGET = libusbcore

# List C source files here.
CCSOURCES_CLIENT = usb_dcd.c usb_dcd_int.c
CCSOURCES_HOST = usb_hcd.c usb_hcd_int.c
CCSOURCES_OTG = usb_otg.c

CCSOURCES = usb_core.c usb_bsp.c $(CCSOURCES_CLIENT)

# List Assembler to be assembled with NASM here
ASOURCES = 

# Additional include paths to consider
INCLUDES =	inc $(ROOT)/libs/libstm32f4xx/inc $(ROOT)/libs/cmsis/inc

# Folder for object files
OBJDIR = obj

# Folder for sourcecode
SRCDIR = src

# Additional defines
DEFINES := 

-include $(ROOT)/build/libs.mak

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),distclean)
-include $(addprefix $(OBJDIR)/, $(OBJECTS:%.o=%.d))
endif
endif
