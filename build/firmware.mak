# Variable mangling
OBJDIR-$(TARGET) := $(addprefix $(ROOT)/firmware/$(TARGET)/,$(OBJDIR))
SRCDIR-$(TARGET) := $(addprefix $(ROOT)/firmware/$(TARGET)/,$(SRCDIR))

# Linker flags
LDFLAGS-$(TARGET) := -mthumb -mcpu=cortex-m4 \
		  -mfloat-abi=hard -mfpu=fpv4-sp-d16 \
		  -nostartfiles -Wl,-T,$(ROOT)/build/firmware.ld,--gc-sections,-Map,$(ROOT)/firmware/$(TARGET)/linker.map,-\(,-lc,--whole-archive,$(call spaceListToCommaList,$(LIBS:%=-l%)),--no-whole-archive,-\)

# C compiler flags
CFLAGS-$(TARGET) := -std=gnu99 -ggdb -O2 -Werror -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16
CFLAGS-$(TARGET) += -fdata-sections -ffunction-sections
CFLAGS-$(TARGET) += $(DEFINES)

# Determinte objects to be created
OBJECTS-$(TARGET) := $(CCSOURCES:%.c=%.o)
OBJECTS-$(TARGET) += $(ASOURCES:%.S=%.o)

# Build include directories for libraries
INCLUDEDIRS-$(TARGET) := -I$(ROOT)/firmware/$(TARGET)/inc $(LIBS:%=-I$(ROOT)/libs/lib%/inc) -I$(ROOT)/libs/cmsis/inc

# Build lib search directories
LIBDIRS-$(TARGET) := $(LIBS:%=-L$(ROOT)/libs/lib%)

# Build dependency list of libraries
LIBDEPS-$(TARGET) := $(foreach LIB,$(LIBS),$(subst libname,lib$(LIB), $(ROOT)/libs/libname/libname.a))

# define output-directories
# sort for removing duplicates
OBJDIRS := $(sort $(addprefix $(OBJDIR-$(TARGET))/, $(dir $(OBJECTS-$(TARGET)))))

# Main targets
$(TARGET): createdirs $(ROOT)/firmware/$(TARGET)/$(TARGET).bin
all: $(TARGET)

# Create output directories
$(OBJDIRS):
	$(call cmd_msg,MKDIR,$(@))
	$(Q)$(MKDIR) -p $@

createdirs: $(OBJDIRS)

$(ROOT)/firmware/$(TARGET)/$(TARGET).bin: $(ROOT)/firmware/$(TARGET)/$(TARGET).elf
	$(call cmd_msg,OBJCOPY,$< -> $@)
	$(Q)$(OBJCOPY) -O binary $< $@

$(ROOT)/firmware/$(TARGET)/$(TARGET).elf: LIBDIRS := $(LIBDIRS-$(TARGET))
$(ROOT)/firmware/$(TARGET)/$(TARGET).elf: LDFLAGS := $(LDFLAGS-$(TARGET))
$(ROOT)/firmware/$(TARGET)/$(TARGET).elf: $(addprefix $(OBJDIR-$(TARGET))/,$(OBJECTS-$(TARGET))) $(LIBDEPS-$(TARGET))
	$(call cmd_msg,LINK,$(@))
	$(Q)$(LD) $(LIBDIRS) $(LDFLAGS) -o $@ $^

# Cleaning
clean: clean-$(TARGET)
clean-$(TARGET): clean-% :
	$(Q)$(RM) -f $(ROOT)/firmware/$*/$*.bin
	$(Q)$(RM) -f $(ROOT)/firmware/$*/$*.elf
	$(Q)$(RM) -f $(ROOT)/firmware/$*/linker.map
	$(Q)$(RM) -rf $(OBJDIR-$*)

# Header dependency generation
$(OBJDIR-$(TARGET))/%.d: CFLAGS := $(CFLAGS-$(TARGET))
$(OBJDIR-$(TARGET))/%.d: INCLUDEDIRS := $(INCLUDEDIRS-$(TARGET))
$(OBJDIR-$(TARGET))/%.d: $(SRCDIR-$(TARGET))/%.c
	$(call cmd_msg,DEPENDS,$@)
	$(Q)$(MKDIR) -p $(dir $@)
	$(Q)$(CC) $(CFLAGS) $(INCLUDEDIRS) -MM -MG -MP -MT '$(@:%.d=%.o)' $< > $@

# Compile c files
$(OBJDIR-$(TARGET))/%.o: CFLAGS := $(CFLAGS-$(TARGET))
$(OBJDIR-$(TARGET))/%.o: INCLUDEDIRS := $(INCLUDEDIRS-$(TARGET))
$(OBJDIR-$(TARGET))/%.o: $(SRCDIR-$(TARGET))/%.c
	$(call cmd_msg,CC,$<)
	$(Q)$(CC) $(CFLAGS) $(INCLUDEDIRS) -c $< -o $@

upload-$(TARGET): $(ROOT)/firmware/$(TARGET)/$(TARGET).bin
	$(call cmd_msg,OPENOCD,$<)
	$(Q)openocd -f interface/stlink-v2.cfg -f target/stm32f4x_stlink.cfg \
	-c init -c "reset halt" -c "stm32f2x mass_erase 0" \
	-c "flash write_bank 0 $^ 0" \
	-c "reset run" -c shutdown

upload-gdb-$(TARGET): $(ROOT)/firmware/$(TARGET)/$(TARGET).elf
	$(call cmd_msg,GDB,$<)
	$(Q)st-util & arm-none-eabi-gdb -ex "tar ext :4242" -ex "load $<" < /dev/null

debug-gdb-$(TARGET): $(ROOT)/firmware/$(TARGET)/$(TARGET).elf
	st-util & gdb -ex "tar ext :4242" $<

.PHONY: clean-$(TARGET) upload-gdb-$(TARGET) upload-$(TARGET) debug-gdb-$(TARGET) $(TARGET)

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),distclean)
ifneq ($(MAKECMDGOALS),clean-$(TARGET))
-include $(addprefix $(OBJDIR-$(TARGET))/, $(OBJECTS-$(TARGET):%.o=%.d))
endif
endif
endif
