# Variable mangling
OBJDIR-$(TARGET) := $(addprefix $(ROOT)/libs/$(TARGET)/,$(OBJDIR))
SRCDIR-$(TARGET) := $(addprefix $(ROOT)/libs/$(TARGET)/,$(SRCDIR))

# C compiler flags
CFLAGS-$(TARGET) := -std=gnu99 -ggdb -O2 -Werror -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16
CFLAGS-$(TARGET) += -fdata-sections -ffunction-sections
CFLAGS-$(TARGET) += $(addprefix -I,$(INCLUDES))
CFLAGS-$(TARGET) += $(DEFINES)

# Determinte objects to be created
OBJECTS-$(TARGET) := $(CCSOURCES:%.c=%.o)
OBJECTS-$(TARGET) += $(ASOURCES:%.S=%.o)

# define output-directories
# sort for removing duplicates
OBJDIRS := $(sort $(addprefix $(OBJDIR-$(TARGET))/, $(dir $(OBJECTS-$(TARGET)))))

# Main targets
$(TARGET): createdirs $(ROOT)/libs/$(TARGET)/$(TARGET).a
all: $(TARGET)

# Create output directories
$(OBJDIRS):
	$(call cmd_msg,MKDIR,$(@))
	$(Q)$(MKDIR) -p $@

createdirs: $(OBJDIRS)

$(ROOT)/libs/$(TARGET)/$(TARGET).a: $(addprefix $(OBJDIR-$(TARGET))/, $(OBJECTS-$(TARGET)))
	$(call cmd_msg,AR,$(@))
	$(Q)$(AR) rcs $@ $^

# Cleaning
clean: clean-$(TARGET)
clean-$(TARGET): clean-% :
	$(Q)$(RM) -f $(ROOT)/libs/$*/$*.a
	$(Q)$(RM) -rf $(OBJDIR-$*)

# Header dependency generation
$(OBJDIR-$(TARGET))/%.d: CFLAGS := $(CFLAGS-$(TARGET))
$(OBJDIR-$(TARGET))/%.d: $(SRCDIR-$(TARGET))/%.c
	$(call cmd_msg,DEPENDS,$@)
	$(Q)$(MKDIR) -p $(dir $@)
	$(Q)$(CC) $(CFLAGS) -MM -MG -MP -MT '$(@:%.d=%.o)' $< > $@

# Compile c files
$(OBJDIR-$(TARGET))/%.o: CFLAGS := $(CFLAGS-$(TARGET))
$(OBJDIR-$(TARGET))/%.o: $(SRCDIR-$(TARGET))/%.c
	$(call cmd_msg,CC,$<)
	$(Q)$(CC) $(CFLAGS) -c $< -o $@

# Assembler S files
$(OBJDIR-$(TARGET))/%.o: $(SRCDIR-$(TARGET))/%.S
	$(call cmd_msg,NASM,$<)
	$(Q)$(NASM) $(ASFLAGS) -o $@ $<

.PHONY: clean-$(TARGET) $(TARGET)

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),distclean)
ifneq ($(MAKECMDGOALS),clean-$(TARGET))
-include $(addprefix $(OBJDIR-$(TARGET))/, $(OBJECTS-$(TARGET):%.o=%.d))
endif
endif
endif
