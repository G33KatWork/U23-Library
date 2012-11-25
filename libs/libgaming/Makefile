# Target file name.
TARGET = libgaming

# List C source files here.
CCSOURCES = LED.c \
			SNES.c \
			Main.c \
			Filesystem.c \
			DiskIO.c \
			VGA.c \
			Font.c \
			Accelerometer.c \
			Audio.c \
			Gamestate.c \
			UserInterface.c \
			PushButtons.c \
			Rumble.c \
			Synth.c

# List Assembler to be assembled with NASM here
ASOURCES = 

# Additional include paths to consider
INCLUDES =	$(ROOT)/libs/$(TARGET)/inc \
			$(ROOT)/libs/libstm32f4xx/inc \
			$(ROOT)/libs/cmsis/inc \
			$(ROOT)/libs/libfatfs/inc \
			$(ROOT)/libs/libsdcard/inc \
			$(ROOT)/libs/libgraphics/inc \
			$(ROOT)/libs/libsystem/inc

# Folder for object files
OBJDIR = obj

# Folder for sourcecode
SRCDIR = src

# Additional defines
DEFINES = -DRGB8Pixels

-include $(ROOT)/build/libs.mak
