include config.mk
include definitions.mk

ELF := armucos.elf
ASM := armucos.asm
SYM := armucos.sym
.PHONY: default all modules clean cleanall modules_clean modules_cleanall distclean

all : $(OUT)/$(ELF)

ALL_MODULES := 
#include $(call all-makefiles-under, $(TOP))
include $(TOP)/boot/local.mk
include $(TOP)/arch/local.mk
include $(TOP)/memory/local.mk
include $(TOP)/utils/local.mk
include $(TOP)/ucos/local.mk
ALL_STATIC_LIBS := $(foreach m, $(ALL_MODULES), $(ALL_MODULES.$(m).BUILT))
ALL_OBJECTS := $(foreach m, $(ALL_MODULES), $(ALL_MODULES.$(m).OBJECTS))
ARM_LIBGCC = $(shell arm-elf-gcc -msoft-float -print-libgcc-file-name)
ARM_LIBC = $(shell arm-elf-gcc -msoft-float -print-file-name=libc.a)
LDFLAGS := -Tldscript.lds -nostdlib 

$(OUT)/$(ELF): $(ALL_OBJECTS) $(ALL_STATIC_LIBS) $(ARM_LIBGCC) $(ARM_LIBC)
	@echo "Generate ELF file $@..."
	$(CC) $(LDFLAGS) -o $@ $^
	@echo "Generate Symbol file $(OUT)/$(SYM)..."
	$(OBJDUMP) -t $@ > $(OUT)/$(SYM)
	@echo "Generate Assembly file $(OUT)/$(ASM)..."
	$(OBJDUMP) -S $@ > $(OUT)/$(ASM)

modules : $(ALL_STATIC_LIBS)

modules_clean : $(foreach m, $(ALL_MODULES), $(m).clean)

modules_cleanall : $(foreach m, $(ALL_MODULES), $(m).cleanall)

clean: modules_clean
	$(RM) $(OUT)/$(ELF) -f

cleanall: modules_cleanall
	$(RM) $(OUT)/$(ELF) -f

distclean:
	@echo "Remove dir $(OUT)..."
	$(RM) $(OUT) -rf

