# Target file name.
TARGET = exposer

# List C source files here.
CCSOURCES = Main.c serencode.c

# Additional local static libs to link against
LIBS = system stm32f4xx

# Folder for object files
OBJDIR = obj

# Folder for sourcecode
SRCDIR = src

# Additional defines
DEFINES := -DRGB8Pixels

TARGET_ROOT_DIR := $(SELF_DIR)

-include $(ROOT)/build/firmware.mak
