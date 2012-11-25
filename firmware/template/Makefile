# Target file name.
TARGET = template

# List C source files here.
CCSOURCES = Game.c

# Additional local static libs to link against
LIBS = gaming stm32f4xx fatfs sdcard graphics

# Folder for object files
OBJDIR = obj

# Folder for sourcecode
SRCDIR = src

# Additional defines
DEFINES := -DRGB8Pixels

# Root dir (enables use for different target/folder names)
TARGET_ROOT_DIR := $(SELF_DIR)

-include $(ROOT)/build/firmware.mak
