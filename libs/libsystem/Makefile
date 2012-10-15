ROOT                 := ../..
include $(ROOT)/build/base.mak

# Target file name.
TARGET = libsystem

# List C source files here.
CCSOURCES = Startup.c System.c LED.c Button.c Accelerometer.c Audio.c VGA.c Random.c

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
