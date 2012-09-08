ROOT                 := ..
include $(ROOT)/build/base.mak

SUBDIRS = helloworld accelerometer audiotest spritetest usbtest usarttest sdcardtest fputest snestest gametest

# Main targets
all: $(SUBDIRS:%=%.subdir)
clean: $(SUBDIRS:%=%.subclean)
distclean: clean $(SUBDIRS:%=%.subdistclean)

#subdir rules
%.subdir:
	$(call cmd_msg,SUBDIR,$*)
	$(call call_submake,$*,all)

%.subclean:
	$(call call_submake,$*,clean)

%.subdistclean:
	$(call call_submake,$*,distclean)

.PHONY: all clean distclean
