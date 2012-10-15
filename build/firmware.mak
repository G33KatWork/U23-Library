# Linker flags
LDFLAGS := -mthumb -mcpu=cortex-m4 \
		  -mfloat-abi=hard -mfpu=fpv4-sp-d16 \
		  -nostartfiles -Wl,-T,Linker.ld,--gc-sections,-Map,linker.map,-\(,-lc,--whole-archive,$(call spaceListToCommaList,$(LIBS:%=-l%)),--no-whole-archive,-\)

# C compiler flags
CFLAGS := -std=gnu99 -ggdb -O2 -Werror -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16
CFLAGS += -fdata-sections -ffunction-sections
CFLAGS += $(DEFINES)

# Determinte objects to be created
OBJECTS += $(CCSOURCES:%.c=%.o)

# Build include directories for libraries
INCLUDEDIRS := -Iinc $(LIBS:%=-I$(ROOT)/libs/lib%/inc) -I$(ROOT)/libs/cmsis/inc

# Build lib search directories
LIBDIRS := $(LIBS:%=-L$(ROOT)/libs/lib%)

# Build dependency list of libraries
LIBDEPS := $(foreach LIB,$(LIBS),$(subst libname,lib$(LIB), $(ROOT)/libs/libname/libname.a))

# define output-directories
# sort for removing duplicates
OBJDIRS := $(sort $(addprefix $(OBJDIR)/, $(dir $(OBJECTS))))

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

$(TARGET).elf: $(addprefix $(OBJDIR)/, $(OBJECTS)) $(LIBDEPS)
	$(call cmd_msg,LINK,$(@))
	$(Q)$(LD) $(LIBDIRS) $(LDFLAGS) -o $@ $(addprefix $(OBJDIR)/, $(OBJECTS))

# Cleaning
clean:
	$(Q)$(RM) -f $(TARGET).bin
	$(Q)$(RM) -f $(TARGET).elf
	$(Q)$(RM) -f linker.map
	$(Q)$(RM) -rf obj

distclean: clean

# Header dependency generation
$(OBJDIR)/%.d: $(SRCDIR)/%.c
	$(call cmd_msg,DEPENDS,$@)
	$(Q)$(MKDIR) -p $(dir $@)
	$(Q)$(CC) $(CFLAGS) $(INCLUDEDIRS) -MM -MG -MP -MT '$(@:%.d=%.o)' $< > $@

# Compile c files
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(call cmd_msg,CC,$<)
	$(Q)$(CC) $(CFLAGS) $(INCLUDEDIRS) -c $< -o $@

upload: $(TARGET).bin
	$(call cmd_msg,OPENOCD,$<)
	$(Q)openocd -f interface/stlink-v1.cfg -f target/stm32f4x_stlink.cfg \
	-c init -c "reset halt" -c "stm32f2x mass_erase 0" \
	-c "flash write_bank 0 $^ 0" \
	-c "reset run" -c shutdown

.PHONY: createdirs clean upload
