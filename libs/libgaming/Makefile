# Target file name.
TARGET = libgaming

# List C source files here.
CCSOURCES = LED.c \
			NewlibSyscalls.c \
			SNES.c \
			Startup.c \
			SystemInit.c \
			USART.c \
			Main.c \
			Debug.c \
			SysTick.c \
			Filesystem.c \
			DiskIO.c \
			VGA.c \
			Font.c \
			Random.c \
			Accelerometer.c \
			Audio.c \
			Gamestate.c

# List Assembler to be assembled with NASM here
ASOURCES = 

# Additional include paths to consider
INCLUDES =	$(ROOT)/libs/$(TARGET)/inc \
			$(ROOT)/libs/libstm32f4xx/inc \
			$(ROOT)/libs/cmsis/inc \
			$(ROOT)/libs/libfatfs/inc \
			$(ROOT)/libs/libsdcard/inc \
			$(ROOT)/libs/libgraphics/inc

# Folder for object files
OBJDIR = obj

# Folder for sourcecode
SRCDIR = src

# Additional defines
DEFINES = -DRGB8Pixels

-include $(ROOT)/build/libs.mak
