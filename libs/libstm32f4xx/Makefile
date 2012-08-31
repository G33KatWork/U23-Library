ROOT                 := ../..
include $(ROOT)/build/base.mak

# Target file name.
TARGET = libstm32f4xx

# List C source files here.
CCSOURCES = misc.c \
			stm32f4xx_cryp_aes.c \
			stm32f4xx_dac.c \
			stm32f4xx_exti.c \
			stm32f4xx_hash.c \
			stm32f4xx_iwdg.c \
			stm32f4xx_rtc.c \
			stm32f4xx_tim.c \
			stm32f4xx_adc.c \
			stm32f4xx_cryp.c \
			stm32f4xx_dbgmcu.c \
			stm32f4xx_flash.c \
			stm32f4xx_hash_md5.c \
			stm32f4xx_pwr.c \
			stm32f4xx_sdio.c \
			stm32f4xx_usart.c \
			stm32f4xx_can.c \
			stm32f4xx_cryp_des.c \
			stm32f4xx_dcmi.c \
			stm32f4xx_fsmc.c \
			stm32f4xx_hash_sha1.c \
			stm32f4xx_rcc.c \
			stm32f4xx_spi.c \
			stm32f4xx_wwdg.c \
			stm32f4xx_crc.c \
			stm32f4xx_cryp_tdes.c \
			stm32f4xx_dma.c \
			stm32f4xx_gpio.c \
			stm32f4xx_i2c.c \
			stm32f4xx_rng.c \
			stm32f4xx_syscfg.c

# List C++ source files here
CXXSOURCES = 

# List Assembler to be assembled with NASM here
ASOURCES = 

# Additional include paths to consider
INCLUDES =	inc $(ROOT)/libs/cmsis/inc

# Folder for object files
OBJDIR = obj

# Folder for sourcecode
SRCDIR = src

# Additional defines
DEFINES := 

# C compiler flags
CFLAGS := -std=c99 -ggdb -O2 -Werror -mthumb -mcpu=cortex-m4
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
