# Target file name.
TARGET = libsystem

# List C source files here.
CCSOURCES = Startup.c System.c LED.c Button.c Accelerometer.c Audio.c VGA.c Random.c

# List Assembler to be assembled with NASM here
ASOURCES = 

# Additional include paths to consider
INCLUDES =	$(ROOT)/libs/$(TARGET)/inc

# Folder for object files
OBJDIR = obj

# Folder for sourcecode
SRCDIR = src

# Additional defines
DEFINES := -DARM_MATH_CM4

-include $(ROOT)/build/libs.mak
