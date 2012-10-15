ROOT                 := ../..
include $(ROOT)/build/base.mak

# Target file name.
TARGET = libfatfs

# List C source files here.
CCSOURCES = ff.c

# List Assembler to be assembled with NASM here
ASOURCES = 

# Additional include paths to consider
INCLUDES =	inc

# Folder for object files
OBJDIR = obj

# Folder for sourcecode
SRCDIR = src

# Additional defines
DEFINES := -DARM_MATH_CM4

-include $(ROOT)/build/libs.mak

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),distclean)
-include $(addprefix $(OBJDIR)/, $(OBJECTS:%.o=%.d))
endif
endif
