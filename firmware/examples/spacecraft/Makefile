# Target file name.
TARGET = spacecraft

# List C source files here.
CCSOURCES = Spacecraft.c Sprites.c Menu.c Highscore.c

# Additional local static libs to link against
LIBS = gaming system stm32f4xx fatfs sdcard graphics

# Folder for object files
OBJDIR = obj

# Folder for sourcecode
SRCDIR = src

# Additional defines
DEFINES := -DRGB8Pixels

# adds examples dir ...
TARGET_ROOT_DIR := $(SELF_DIR)

-include $(ROOT)/build/firmware.mak
