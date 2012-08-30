ROOT                 := .
include $(ROOT)/build/base.mak

UPLOADNAME ?= helloworld

SUBDIRS = libs firmware applications

STARTTIME := $(shell date +%s)
# Main targets
all: $(SUBDIRS:%=%.subdir)
	$(call cmd_msg,NOTICE,Build completed in $$(($$(date +%s)-$(STARTTIME))) seconds)

clean: $(SUBDIRS:%=%.subclean)

distclean: clean $(SUBDIRS:%=%.subdistclean)

upload: all
	$(call cmd_msg,UPLOAD,$(UPLOADNAME))
	$(call call_submake,firmware/$(UPLOADNAME),upload)

#subdir rules
%.subdir:
	$(call cmd_msg,SUBDIR,$*)
	$(call call_submake,$*,all)

%.subclean:
	$(call call_submake,$*,clean)

%.subdistclean:
	$(call call_submake,$*,distclean)

.PHONY: all upload clean distclean
