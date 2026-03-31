/*
 * ============================================================================
 *  HBOOT — Home Boot Loader
 *  Copyright (C) 2024-2026 Winora Company. All rights reserved.
 *  File: include/bootinfo.h — HDOS Boot Information Structure
 *
 *  HDOS_BOOT_INFO is passed to the kernel via RDI register (SysV AMD64 ABI)
 *  at the moment HBOOT calls kernel_main(HDOS_BOOT_INFO *info).
 *
 *  The kernel MUST verify Magic == HDOS_BOOT_MAGIC before using any fields.
 * ============================================================================
 */
#pragma once
#include "uefi.h"
#include "hfs.h"

/* =========================================================================
 * Magic & Version
 * ========================================================================= */
#define HDOS_BOOT_MAGIC         0x534F4448544F4F42ULL  /* 'BOOTHDS' */
#define HDOS_BOOT_VERSION       3                      /* Struct version */

/* =========================================================================
 * CPU Information (collected via CPUID before ExitBootServices)
 * ========================================================================= */
typedef struct {
    CHAR8   VendorString[13];   /* "GenuineIntel" or "AuthenticAMD" */
    CHAR8   BrandString[49];    /* Full CPU brand string */
    UINT32  Family;
    UINT32  Model;
    UINT32  Stepping;
    UINT32  MaxLogicalCores;
    UINT32  PhysicalCores;
    UINT64  Features;           /* CPUID feature flags (EDX of leaf 1) */
    UINT64  Features2;          /* CPUID feature flags (ECX of leaf 1) */
    UINT64  ExtFeatures;        /* CPUID leaf 7 EBX */
    UINT64  BaseFreqHz;         /* Base frequency in Hz (if available) */
    UINT8   HasSSE;
    UINT8   HasSSE2;
    UINT8   HasSSE3;
    UINT8   HasSSE4_1;
    UINT8   HasSSE4_2;
    UINT8   HasAVX;
    UINT8   HasAVX2;
    UINT8   HasAVX512;
    UINT8   HasAES;
    UINT8   HasSHA;
    UINT8   HasRDRAND;
    UINT8   Has1GBPages;
    UINT8   HasNXBit;
    UINT8   Pad[3];
} __attribute__((packed)) HDOS_CPU_INFO;

/* =========================================================================
 * Memory Region Descriptor (simplified from EFI_MEMORY_DESCRIPTOR)
 * The kernel uses this to set up its PMM.
 * ========================================================================= */
#define HDOS_MEM_USABLE         0   /* Free RAM, usable by kernel */
#define HDOS_MEM_RESERVED       1   /* Firmware reserved, do not touch */
#define HDOS_MEM_ACPI_RECLAIM   2   /* ACPI tables, can be freed later */
#define HDOS_MEM_ACPI_NVS       3   /* ACPI NVS, never touch */
#define HDOS_MEM_MMIO           4   /* Memory-mapped I/O */
#define HDOS_MEM_BOOTLOADER     5   /* Used by HBOOT, kernel can reclaim */
#define HDOS_MEM_KERNEL         6   /* Kernel image */
#define HDOS_MEM_INITRD         7   /* InitRD */
#define HDOS_MEM_FRAMEBUFFER    8   /* Framebuffer memory */
#define HDOS_MEM_PERSISTENT     9   /* Persistent / NVDIMM */
#define HDOS_MEM_PAGETABLES     10  /* HBOOT page tables (reclaim after paging) */

typedef struct {
    UINT64  Base;           /* Physical base address */
    UINT64  Length;         /* Length in bytes */
    UINT32  Type;           /* HDOS_MEM_* */
    UINT32  Flags;          /* EFI_MEMORY_RUNTIME etc */
} __attribute__((packed)) HDOS_MEM_REGION;

#define HDOS_MAX_MEM_REGIONS    512

/* =========================================================================
 * Framebuffer / Display info
 * ========================================================================= */
#define HDOS_PIXEL_BGR  0   /* B8G8R8X8 */
#define HDOS_PIXEL_RGB  1   /* R8G8B8X8 */
#define HDOS_PIXEL_MASK 2   /* Custom bitmask */

typedef struct {
    UINT64  PhysBase;           /* Physical framebuffer address */
    UINT64  Size;               /* Total framebuffer size in bytes */
    UINT32  Width;              /* Horizontal resolution */
    UINT32  Height;             /* Vertical resolution */
    UINT32  Pitch;              /* Bytes per scanline (stride) */
    UINT32  PixelFormat;        /* HDOS_PIXEL_* */
    UINT32  RedMask;            /* Only for HDOS_PIXEL_MASK */
    UINT32  GreenMask;
    UINT32  BlueMask;
    UINT32  ReservedMask;
    UINT8   BitsPerPixel;       /* Usually 32 */
    UINT8   Valid;              /* 1 = framebuffer is usable */
    UINT8   Pad[2];
} __attribute__((packed)) HDOS_FRAMEBUFFER_INFO;

/* =========================================================================
 * ACPI info
 * ========================================================================= */
typedef struct {
    UINT64  RSDPAddress;        /* Physical address of RSDP */
    UINT64  RSDTAddress;        /* Physical address of RSDT */
    UINT64  XSDTAddress;        /* Physical address of XSDT (ACPI 2.0+) */
    UINT8   Version;            /* 1 = ACPI 1.0, 2 = ACPI 2.0+ */
    UINT8   Valid;
    UINT8   Pad[6];
} __attribute__((packed)) HDOS_ACPI_INFO;

/* =========================================================================
 * UEFI Runtime Services info
 * ========================================================================= */
typedef struct {
    UINT64  RuntimeServicesVA;  /* Virtual address (after SetVirtualAddressMap) */
    UINT64  RuntimeServicesPA;  /* Physical address */
    UINT8   Available;
    UINT8   Pad[7];
} HDOS_UEFI_RUNTIME_INFO;

/* =========================================================================
 * SMP / APIC info
 * ========================================================================= */
#define HDOS_MAX_CPUS   256

typedef struct {
    UINT32  APIC_ID;
    UINT32  Flags;          /* 1 = enabled */
} HDOS_CPU_ENTRY;

typedef struct {
    UINT32       APIC_Base;         /* Local APIC base address */
    UINT32       IOAPICBase;        /* I/O APIC base address */
    UINT32       IOAPICGSIBase;     /* GSI base of I/O APIC */
    UINT32       CPUCount;
    HDOS_CPU_ENTRY CPUs[HDOS_MAX_CPUS];
    UINT8        Valid;
    UINT8        Pad[3];
} HDOS_SMP_INFO;

/* =========================================================================
 * PCI info — up to 64 PCI devices detected by HBOOT
 * ========================================================================= */
#define HDOS_MAX_PCI_DEVICES 128

typedef struct {
    UINT16  VendorID;
    UINT16  DeviceID;
    UINT8   ClassCode;
    UINT8   SubClass;
    UINT8   ProgIF;
    UINT8   Bus;
    UINT8   Device;
    UINT8   Function;
    UINT8   Pad[2];
    UINT64  BAR[6];         /* Base Address Registers */
    UINT8   IRQ;
    UINT8   Pad2[3];
} __attribute__((packed)) HDOS_PCI_DEVICE;

typedef struct {
    UINT32          Count;
    HDOS_PCI_DEVICE Devices[HDOS_MAX_PCI_DEVICES];
    UINT8           Valid;
    UINT8           Pad[3];
} HDOS_PCI_INFO;

/* =========================================================================
 * Boot time measurement
 * ========================================================================= */
typedef struct {
    UINT64  FirmwareStart;      /* TSC at firmware hand-off */
    UINT64  HbootStart;         /* TSC at efi_main entry */
    UINT64  KernelHandoff;      /* TSC at kernel jump */
} HDOS_TIMING_INFO;

/* =========================================================================
 * HDOS Boot Information — Master structure passed to kernel
 * Total size: ~16 KB (allocated in EfiLoaderData)
 * ========================================================================= */
typedef struct {
    /* ---- Identification ---- */
    UINT64                  Magic;          /* HDOS_BOOT_MAGIC              */
    UINT32                  Version;        /* HDOS_BOOT_VERSION            */
    UINT32                  StructSize;     /* sizeof(HDOS_BOOT_INFO)       */
    UINT32                  Flags;          /* Global flags                 */
    UINT32                  Pad0;

    /* ---- Memory Map ---- */
    UINT64                  EfiMemMapAddr;  /* Original EFI Memory Map ptr  */
    UINT64                  EfiMemMapSize;
    UINT64                  EfiMemDescSize;
    UINT32                  EfiMemDescVer;
    UINT32                  Pad1;
    UINT64                  TotalRAMBytes;  /* Total usable RAM             */
    UINT64                  TotalRAMPages;  /* Total usable pages           */

    /* ---- Simplified Memory Map ---- */
    UINT32                  MemRegionCount;
    UINT32                  Pad2;
    HDOS_MEM_REGION         MemRegions[HDOS_MAX_MEM_REGIONS];

    /* ---- Kernel ---- */
    UINT64                  KernelPhysBase; /* Physical load address        */
    UINT64                  KernelVirtBase; /* Preferred virtual base       */
    UINT64                  KernelSize;
    UINT64                  KernelEntry;    /* Physical entry point         */

    /* ---- InitRD ---- */
    UINT64                  InitrdPhysBase;
    UINT64                  InitrdSize;

    /* ---- Framebuffer ---- */
    HDOS_FRAMEBUFFER_INFO   Framebuffer;

    /* ---- ACPI ---- */
    HDOS_ACPI_INFO          ACPI;

    /* ---- CPU ---- */
    HDOS_CPU_INFO           CPU;

    /* ---- SMP / APIC ---- */
    HDOS_SMP_INFO           SMP;

    /* ---- PCI ---- */
    HDOS_PCI_INFO           PCI;

    /* ---- HFS Volume ---- */
    HFS_BOOT_INFO           HFS;

    /* ---- UEFI Runtime ---- */
    HDOS_UEFI_RUNTIME_INFO  UEFIRuntime;

    /* ---- Time ---- */
    EFI_TIME                BootTime;

    /* ---- Timing ---- */
    HDOS_TIMING_INFO        Timing;

    /* ---- Boot Parameters ---- */
    CHAR16                  KernelPath[256];
    CHAR16                  CmdLine[1024];  /* Kernel command line          */
    CHAR16                  BootDev[128];   /* Boot device path             */

    /* ---- HBOOT Version ---- */
    CHAR8                   HBootVersion[32];
    CHAR8                   HBootBuildDate[32];

    /* ---- Checksum ---- */
    UINT32                  CRC32;          /* CRC32 of struct (excl. this) */
    UINT32                  Pad3;

} __attribute__((packed)) HDOS_BOOT_INFO;

/* Global flags in HDOS_BOOT_INFO.Flags */
#define BOOT_FLAG_GOP           (1 << 0)  /* Framebuffer available         */
#define BOOT_FLAG_INITRD        (1 << 1)  /* InitRD loaded                 */
#define BOOT_FLAG_ACPI          (1 << 2)  /* ACPI tables found             */
#define BOOT_FLAG_SMP           (1 << 3)  /* SMP info collected            */
#define BOOT_FLAG_PCI           (1 << 4)  /* PCI devices enumerated        */
#define BOOT_FLAG_HFS           (1 << 5)  /* HFS volume detected           */
#define BOOT_FLAG_UEFI_RT       (1 << 6)  /* UEFI Runtime available        */
#define BOOT_FLAG_CPU_INFO      (1 << 7)  /* CPU info collected            */
#define BOOT_FLAG_TIMING        (1 << 8)  /* TSC timing available          */
