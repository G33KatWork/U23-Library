# Target file name.
TARGET = libsystem

# List C source files here.
CCSOURCES = Startup.c \
			SystemInit.c \
			NewlibSyscalls.c \
			SysTick.c \
			Debug.c \
			USART.c \
			Random.c

# List Assembler to be assembled with NASM here
ASOURCES = 

# Additional include paths to consider
INCLUDES =	$(ROOT)/libs/$(TARGET)/inc \
			$(ROOT)/libs/libstm32f4xx/inc \
			$(ROOT)/libs/cmsis/inc \
			
# Folder for object files
OBJDIR = obj

# Folder for sourcecode
SRCDIR = src

# Additional defines
DEFINES = -DRGB8Pixels

-include $(ROOT)/build/libs.mak
