ROOT                 := ../..
include $(ROOT)/build/base.mak

# Target file name.
TARGET = audiotest

# List C source files here.
CCSOURCES = Main.c BitBin.c TestSong.c

# List C++ source files here
CXXSOURCES = 

# Linker flags
LDFLAGS = -mthumb -mcpu=cortex-m4 -nostartfiles -Wl,-T,Linker.ld,--gc-sections,-Map,linker.map -specs Terrible.specs

# Additional include paths to consider
INCLUDES =	inc $(ROOT)/libs/libsystem/inc

# Additional static libs to link agains
STATICLIBS = $(ROOT)/libs/libsystem/libsystem.a

# Folder for object files
OBJDIR = obj

# Folder for sourcecode
SRCDIR = src

# Additional defines
DEFINES := 

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
all: createdirs $(TARGET).bin
	@:	#Shut up!

# Create output directories
$(OBJDIRS):
	$(call cmd_msg,MKDIR,$(@))
	$(Q)$(MKDIR) -p $@

createdirs: $(OBJDIRS)

$(TARGET).bin: $(TARGET).elf
	$(call cmd_msg,OBJCOPY,$< -> $@)
	$(Q)$(OBJCOPY) -O binary $< $@

$(TARGET).elf: $(addprefix $(OBJDIR)/, $(OBJECTS)) $(STATICLIBS)
	$(call cmd_msg,LINK,$(@))
	$(Q)$(LD) $(LDFLAGS) -o $@ $^ $(STATICLIBS)

# Cleaning
clean:
	$(Q)$(RM) -f $(TARGET).bin
	$(Q)$(RM) -f $(TARGET).elf
	$(Q)$(RM) -f linker.map
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

upload: $(TARGET).bin
	$(call cmd_msg,OPENOCD,$<)
	$(Q)openocd -f interface/stlink-v2.cfg -f target/stm32f4x_stlink.cfg \
	-c init -c "reset halt" -c "stm32f2x mass_erase 0" \
	-c "flash write_bank 0 $^ 0" \
	-c "reset run" -c shutdown

.PHONY: createdirs clean upload

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),distclean)
-include $(addprefix $(OBJDIR)/, $(OBJECTS:%.o=%.d))
endif
endif
