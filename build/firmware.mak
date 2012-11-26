# Variable mangling
OBJDIR-$(TARGET) := $(addprefix $(TARGET_ROOT_DIR),$(OBJDIR))
SRCDIR-$(TARGET) := $(addprefix $(TARGET_ROOT_DIR),$(SRCDIR))

# Linker flags
LDFLAGS-$(TARGET) := -mthumb -mcpu=cortex-m4 \
		  -mfloat-abi=hard -mfpu=fpv4-sp-d16 \
		  -nostartfiles -Wl,-T,$(ROOT)/build/firmware.ld,--gc-sections,-Map,$(TARGET_ROOT_DIR)linker.map,-\(,-lc,--whole-archive,-lm,$(call spaceListToCommaList,$(LIBS:%=-l%)),--no-whole-archive,-\)

# C compiler flags
CFLAGS-$(TARGET) := -std=gnu99 -ggdb -O2 -Werror -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16
CFLAGS-$(TARGET) += -fdata-sections -ffunction-sections
CFLAGS-$(TARGET) += $(DEFINES)

# Determinte objects to be created
OBJECTS-$(TARGET) := $(CCSOURCES:%.c=%.o)
OBJECTS-$(TARGET) += $(ASOURCES:%.S=%.o)

# Build include directories for libraries
INCLUDEDIRS-$(TARGET) := -I$(TARGET_ROOT_DIR)inc $(LIBS:%=-I$(ROOT)/libs/lib%/inc) -I$(ROOT)/libs/cmsis/inc

# Build lib search directories
LIBDIRS-$(TARGET) := $(LIBS:%=-L$(ROOT)/libs/lib%)

# Build dependency list of libraries
LIBDEPS-$(TARGET) := $(foreach LIB,$(LIBS),$(subst libname,lib$(LIB), $(ROOT)/libs/libname/libname.a))

# define output-directories
# sort for removing duplicates
OBJDIRS := $(sort $(addprefix $(OBJDIR-$(TARGET))/, $(dir $(OBJECTS-$(TARGET)))))

# Main targets
$(TARGET): createdirs $(TARGET_ROOT_DIR)$(TARGET).bin
all: $(TARGET)

# Create output directories
$(OBJDIRS):
	$(call cmd_msg,MKDIR,$(@))
	$(Q)$(MKDIR) -p $@

createdirs: $(OBJDIRS)

$(TARGET_ROOT_DIR)$(TARGET).bin: $(TARGET_ROOT_DIR)$(TARGET).elf
	$(call cmd_msg,OBJCOPY,$< -> $@)
	$(Q)$(OBJCOPY) -O binary $< $@

$(TARGET_ROOT_DIR)$(TARGET).elf: LIBDIRS := $(LIBDIRS-$(TARGET))
$(TARGET_ROOT_DIR)$(TARGET).elf: LDFLAGS := $(LDFLAGS-$(TARGET))
$(TARGET_ROOT_DIR)$(TARGET).elf: $(addprefix $(OBJDIR-$(TARGET))/,$(OBJECTS-$(TARGET))) $(LIBDEPS-$(TARGET))
	$(call cmd_msg,LINK,$(@))
	$(Q)$(LD) $(LIBDIRS) $(LDFLAGS) -o $@ $^

# Cleaning
clean: clean-$(TARGET)
clean-$(TARGET): clean-% :
	$(Q)$(RM) -f $(TARGET_ROOT_DIR)$*/$*.bin
	$(Q)$(RM) -f $(TARGET_ROOT_DIR)$*/$*.elf
	$(Q)$(RM) -f $(TARGET_ROOT_DIR)$*/linker.map
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

upload-$(TARGET): $(TARGET_ROOT_DIR)$(TARGET).bin
	$(call cmd_msg,OPENOCD,$<)
	$(Q)openocd -f interface/stlink-v2.cfg -f target/stm32f4x_stlink.cfg \
	-c init -c "reset halt" -c "stm32f2x mass_erase 0" \
	-c "flash write_bank 0 $^ 0" \
	-c "reset run" -c shutdown

upload-fast-$(TARGET): $(TARGET_ROOT_DIR)$(TARGET).bin
	$(call cmd_msg,STLINK,$<)
	$(Q)st-flash write $< 0x8000000

upload-gdb-$(TARGET): $(TARGET_ROOT_DIR)$(TARGET).elf
	$(call cmd_msg,GDB,$<)
	$(Q)st-util & $(GDB) -ex "tar ext :4242" -ex "load $<" < /dev/null

debug-gdb-$(TARGET): $(TARGET_ROOT_DIR)$(TARGET).elf
	st-util & $(GDB) -ex "tar ext :4242" $<

.PHONY: clean-$(TARGET) upload-gdb-$(TARGET) upload-$(TARGET) debug-gdb-$(TARGET) $(TARGET)

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),distclean)
ifneq ($(MAKECMDGOALS),clean-$(TARGET))
-include $(addprefix $(OBJDIR-$(TARGET))/, $(OBJECTS-$(TARGET):%.o=%.d))
endif
endif
endif
