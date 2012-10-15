ROOT                 := ../..
include $(ROOT)/build/base.mak

# Target file name.
TARGET = libsdcard

# List C source files here.
CCSOURCES = sdcard.c \
			sdcard_lowlevel.c

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
