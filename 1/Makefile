# ==============================================================================
#  HBOOT Makefile — Home Boot Loader v2
#  Copyright (C) 2024-2026 Winora Company. All rights reserved.
#
#  Build requirements:
#    clang >= 14   (or gcc cross-compiler x86_64-w64-mingw32-gcc)
#    lld-link      (from llvm package)
#    mtools        (for disk image creation)
#    dosfstools    (mkfs.fat)
#    OVMF          (for QEMU testing: apt install ovmf)
# ==============================================================================

CC       = clang
LDLINK   = /usr/lib/llvm/21/bin/lld-link

TARGET   = BOOTX64.EFI
MAPFILE  = hboot.map

SRCDIR   = src
INCDIR   = include
OBJDIR   = obj
OUTDIR   = out

SRCS = \
    $(SRCDIR)/hboot.c     \
    $(SRCDIR)/util.c      \
    $(SRCDIR)/gfx.c       \
    $(SRCDIR)/gui_menu.c  \
    $(SRCDIR)/memory_fs.c \
    $(SRCDIR)/core.c

OBJS = $(SRCS:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

# ==============================================================================
#  Compiler flags — targeting UEFI (Windows PE32+ ABI for clang)
# ==============================================================================
CFLAGS = \
    -target x86_64-unknown-windows      \
    -ffreestanding                       \
    -fno-stack-protector                 \
    -fno-builtin                         \
    -fshort-wchar                        \
    -mno-red-zone                        \
    -mno-mmx                             \
    -mno-sse                             \
    -mno-sse2                            \
    -std=c11                             \
    -Wall                                \
    -Wextra                              \
    -Wno-unused-parameter                \
    -Wno-language-extension-token        \
    -O2                                  \
    -I$(INCDIR)                          \
    -DHBOOT_BUILD                        \
    -DHBOOT_COPYRIGHT="\"Copyright (C) 2024-2026 Winora Company\""

# ==============================================================================
#  Linker flags — produce PE32+ UEFI application
# ==============================================================================
LDFLAGS = \
    -flavor link                         \
    -subsystem:efi_application           \
    -entry:efi_main                      \
    -map:$(OUTDIR)/$(MAPFILE)            \
    -nodefaultlib                        \
    -dll

.PHONY: all clean image qemu qemu-nox info check

all: dirs $(OUTDIR)/$(TARGET)

dirs:
	@mkdir -p $(OBJDIR) $(OUTDIR)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@echo "  CC  $<"
	@$(CC) $(CFLAGS) -c $< -o $@

$(OUTDIR)/$(TARGET): $(OBJS)
	@echo "  LD  $@"
	@$(LDLINK) $(LDFLAGS) -out:$@ $(OBJS)
	@echo ""
	@echo "  ┌─────────────────────────────────────────────┐"
	@echo "  │  HBOOT v2.0.0 — Build complete              │"
	@echo "  │  Winora Company © 2024-2026                 │"
	@echo "  │  Output: $(OUTDIR)/$(TARGET)                │"
	@ls -lh $(OUTDIR)/$(TARGET) | awk '{print "  │  Size:   " $$5 "                             │"}'
	@echo "  └─────────────────────────────────────────────┘"

# Syntax check only (no link) — useful for CI
check:
	@echo "  Syntax checking..."
	@for f in $(SRCS); do \
		echo "    Checking $$f"; \
		$(CC) $(CFLAGS) -fsyntax-only $$f || exit 1; \
	done
	@echo "  All files OK"

# Create bootable FAT32 disk image for testing
image: $(OUTDIR)/$(TARGET)
	@echo "  Creating 128MB test disk image..."
	@dd if=/dev/zero of=$(OUTDIR)/hboot.img bs=1M count=128 status=none
	@mkfs.fat -F 32 -n "HDOS_BOOT" $(OUTDIR)/hboot.img
	@mmd -i $(OUTDIR)/hboot.img ::/EFI ::/EFI/BOOT ::/EFI/HBOOT ::/HDOS ::/HDOS/KERNEL
	@mcopy -i $(OUTDIR)/hboot.img $(OUTDIR)/$(TARGET) ::/EFI/BOOT/BOOTX64.EFI
	@mcopy -i $(OUTDIR)/hboot.img configs/HBOOT.CFG   ::/EFI/HBOOT/HBOOT.CFG
	@echo "  Image ready: $(OUTDIR)/hboot.img"

# Run in QEMU with OVMF
OVMF_CODE ?= /usr/share/OVMF/OVMF_CODE.fd
OVMF_VARS ?= /usr/share/OVMF/OVMF_VARS.fd

qemu: image
	qemu-system-x86_64 \
		-drive if=pflash,format=raw,unit=0,readonly=on,file=$(OVMF_CODE) \
		-drive if=pflash,format=raw,unit=1,file=$(OVMF_VARS) \
		-drive format=raw,file=$(OUTDIR)/hboot.img \
		-m 512M -smp 2 \
		-vga std \
		-serial stdio \
		-display gtk

qemu-nox: image
	qemu-system-x86_64 \
		-drive if=pflash,format=raw,unit=0,readonly=on,file=$(OVMF_CODE) \
		-drive if=pflash,format=raw,unit=1,file=$(OVMF_VARS) \
		-drive format=raw,file=$(OUTDIR)/hboot.img \
		-m 512M -smp 2 \
		-nographic

clean:
	@rm -rf $(OBJDIR) $(OUTDIR)
	@echo "  Clean complete"

info:
	@echo "HBOOT Build Configuration"
	@echo "  Compiler : $(CC)"
	@echo "  Linker   : $(LDLINK)"
	@echo "  Target   : $(OUTDIR)/$(TARGET)"
	@$(CC) --version 2>&1 | head -1

# Install to a mounted ESP
ESP ?= /mnt/esp
install: $(OUTDIR)/$(TARGET)
	@mkdir -p $(ESP)/EFI/BOOT $(ESP)/EFI/HBOOT
	@cp $(OUTDIR)/$(TARGET) $(ESP)/EFI/BOOT/BOOTX64.EFI
	@cp configs/HBOOT.CFG   $(ESP)/EFI/HBOOT/HBOOT.CFG
	@echo "  Installed to $(ESP)"
