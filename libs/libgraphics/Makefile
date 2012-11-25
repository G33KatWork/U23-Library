# Target file name.
TARGET = libgraphics

# List C source files here.
CCSOURCES = Bitmap.c \
			DrawingBitmaps.c \
			DrawingCircles.c \
			DrawingLines.c \
			DrawingPixels.c \
			DrawingRLEBitmaps.c \
			DrawingRectangles.c \
			DrawingStraightLines.c \
			RLEBitmap.c \
			SpriteEngine.c \
			TiledMap.c \
			Collision.c \
			list.c 

# List Assembler to be assembled with NASM here
ASOURCES = 

# Additional include paths to consider
INCLUDES =	$(ROOT)/libs/$(TARGET)/inc

# Folder for object files
OBJDIR = obj

# Folder for sourcecode
SRCDIR = src

# Additional defines
DEFINES := -DRGB8Pixels

-include $(ROOT)/build/libs.mak
