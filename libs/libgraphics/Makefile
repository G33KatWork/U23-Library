ROOT                 := ../..
include $(ROOT)/build/base.mak

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
			RLEBitmap.c

# List C++ source files here
CXXSOURCES = 

# List Assembler to be assembled with NASM here
ASOURCES = 

# Additional include paths to consider
INCLUDES =	inc

# Folder for object files
OBJDIR = obj

# Folder for sourcecode
SRCDIR = src

# Additional defines
DEFINES := -DRGB8Pixels

# C compiler flags
CFLAGS := -std=c99 -ggdb -O2 -Werror -mthumb -mcpu=cortex-m4
CFLAGS += -fdata-sections -ffunction-sections
CFLAGS += $(addprefix -I,$(INCLUDES))
CFLAGS += $(DEFINES)

# C++ compiler flags
CXXFLAGS := $(addprefix -I,$(INCLUDES))

# Determinte objects to be created
OBJECTS += $(CXXSOURCES:%.cpp=%.o)
OBJECTS += $(CCSOURCES:%.c=%.o)

# define output-directories
# sort for removing duplicates
OBJDIRS = $(sort $(addprefix $(OBJDIR)/, $(dir $(OBJECTS))))

# Main targets
all: createdirs $(TARGET).a
	@:	#Shut up!

# Create output directories
$(OBJDIRS):
	$(call cmd_msg,MKDIR,$(@))
	$(Q)$(MKDIR) -p $@

createdirs: $(OBJDIRS)

$(TARGET).a: $(addprefix $(OBJDIR)/, $(OBJECTS))
	$(call cmd_msg,AR,$(@))
	$(Q)$(AR) rcs $@ $^

# Cleaning
clean:
	$(Q)$(RM) -f $(TARGET).a
	$(Q)$(RM) -rf obj

distclean: clean

# Header dependency generation
$(OBJDIR)/%.d: $(SRCDIR)/%.cpp
	$(call cmd_msg,DEPENDS,$@)
	$(Q)$(MKDIR) -p $(dir $@)
	$(Q)$(CXX) $(CXXFLAGS) -MM -MG -MP -MT '$(@:%.d=%.o)' $< > $@

$(OBJDIR)/%.d: $(SRCDIR)/%.c
	$(call cmd_msg,DEPENDS,$@)
	$(Q)$(MKDIR) -p $(dir $@)
	$(Q)$(CC) $(CFLAGS) -MM -MG -MP -MT '$(@:%.d=%.o)' $< > $@

# Compile cpp files
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(call cmd_msg,CXX,$<)
	$(Q)$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile c files
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(call cmd_msg,CC,$<)
	$(Q)$(CC) $(CFLAGS) -c $< -o $@

# Assembler S files
$(OBJDIR)/%.o: $(SRCDIR)/%.S
	$(call cmd_msg,NASM,$<)
	$(Q)$(NASM) $(ASFLAGS) -o $@ $<

.PHONY: createdirs clean

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),distclean)
-include $(addprefix $(OBJDIR)/, $(OBJECTS:%.o=%.d))
endif
endif
