/*
 * ============================================================================
 * HBOOT — Home Boot Loader
 * Copyright (C) 2024-2026 Winora Company. All rights reserved.
 * File: include/hboot.h — Master header
 * ============================================================================
 */
#pragma once

#include "uefi.h"
#include "hfs.h"
#include "bootinfo.h"

/* =========================================================================
 * Version
 * ========================================================================= */
#define HBOOT_VERSION_MAJOR     2
#define HBOOT_VERSION_MINOR     1
#define HBOOT_VERSION_PATCH     0
#define HBOOT_VERSION_STR       L"2.1.0"
#define HBOOT_VERSION_STR8      "2.1.0"
#define HBOOT_BUILD_DATE        __DATE__
#define HBOOT_VENDOR            L"Winora Company"

/* =========================================================================
 * Paths
 * ========================================================================= */
#define HBOOT_CONFIG_PATH       L"\\EFI\\HBOOT\\HBOOT.CFG"
#define HDOS_KERNEL_DEFAULT     L"\\HDOS\\KERNEL\\HDOSKER.ELF"
#define HDOS_INITRD_DEFAULT     L"\\HDOS\\INITRD.IMG"
#define HDOS_SPLASH_PATH        L"\\EFI\\HBOOT\\SPLASH.BMP"
#define HDOS_LOGO_PATH          L"\\EFI\\HBOOT\\LOGO.BMP"

/* =========================================================================
 * GUI constants
 * ========================================================================= */
/* 32bpp pixel: 0x00RRGGBB */
#define COLOR_BG            0x0A0A1A    /* Deep navy background          */
#define COLOR_BG_DARK       0x05050F    /* Darker areas                  */
#define COLOR_ACCENT        0x1E90FF    /* Winora blue                   */
#define COLOR_ACCENT_DIM    0x0A4080    /* Dim accent                    */
#define COLOR_WHITE         0xFFFFFF
#define COLOR_GRAY          0xAAAAAA
#define COLOR_GRAY_DARK     0x555555
#define COLOR_GREEN         0x22CC44
#define COLOR_RED           0xEE3333
#define COLOR_YELLOW        0xFFCC00
#define COLOR_SELECTED_BG   0x1A2A4A    /* Selected item background      */
#define COLOR_SELECTED_FG   0x88CCFF    /* Selected item foreground      */
#define COLOR_BORDER        0x2244AA
#define COLOR_SHADOW        0x000000

/* GUI layout (relative to 1920x1080, scaled at runtime) */
#define GUI_HEADER_HEIGHT   80
#define GUI_FOOTER_HEIGHT   40
#define GUI_MENU_X_FRAC     3           /* 1/3 from left */
#define GUI_MENU_W_FRAC     3           /* 1/3 of total width */
#define GUI_INFO_X_FRAC     2           /* Right half */

/* Font: 8x16 bitmapped */
#define FONT_W              8
#define FONT_H              16

/* =========================================================================
 * Config structures
 * ========================================================================= */
#define MAX_BOOT_ENTRIES    12
#define MAX_PATH_LEN        256
#define MAX_LABEL_LEN       80
#define MAX_CMDLINE_LEN     1024

typedef struct {
    CHAR16   Label[MAX_LABEL_LEN];
    CHAR16   KernelPath[MAX_PATH_LEN];
    CHAR16   InitrdPath[MAX_PATH_LEN];
    CHAR16   CmdLine[MAX_CMDLINE_LEN];
    BOOLEAN  HasInitrd;
    BOOLEAN  IsDefault;
    BOOLEAN  IsRecovery;
} BOOT_ENTRY;

typedef struct {
    /* Menu */
    UINT32      NumEntries;
    UINT32      DefaultEntry;
    UINT32      TimeoutSec;
    BOOLEAN     ShowMenu;
    BOOLEAN     ForceMenu;          /* F8 pressed */

    /* Active entry (resolved from DefaultEntry) */
    CHAR16      KernelPath[MAX_PATH_LEN];
    CHAR16      InitrdPath[MAX_PATH_LEN];
    CHAR16      CmdLine[MAX_CMDLINE_LEN];
    BOOLEAN     HasInitrd;

    /* Display */
    BOOLEAN     UseGUI;             /* Graphical boot menu */
    UINT32      PreferredWidth;
    UINT32      PreferredHeight;

    /* Debug */
    BOOLEAN     Verbose;
    BOOLEAN     WaitBefore;         /* Wait keypress before kernel */
    BOOLEAN     ShowBuildInfo;

    BOOT_ENTRY  Entries[MAX_BOOT_ENTRIES];
} HBOOT_CONFIG;

/* =========================================================================
 * Globals
 * ========================================================================= */
extern EFI_HANDLE           gImageHandle;
extern EFI_SYSTEM_TABLE    *gST;
extern EFI_BOOT_SERVICES   *gBS;
extern EFI_RUNTIME_SERVICES *gRT;

extern HBOOT_CONFIG         gConfig;
extern HDOS_BOOT_INFO      *gBootInfo;      /* Allocated in EfiLoaderData  */
extern EFI_FILE_PROTOCOL   *gRootDir;

/* Graphics */
extern EFI_GRAPHICS_OUTPUT_PROTOCOL *gGOP;
extern UINT32 *gFB;                         /* Framebuffer pointer (32bpp) */
extern UINT32  gFBWidth;
extern UINT32  gFBHeight;
extern UINT32  gFBPitch;                    /* BYTES per scanline (stride) */

/* Kernel */
extern UINT64  gKernelEntry;

/* =========================================================================
 * Function prototypes — util.c
 * ========================================================================= */
VOID    UPrint(CONST CHAR16 *Str);
VOID    UPrintLn(CONST CHAR16 *Str);
VOID    UPrintHex(UINT64 Value);
VOID    UPrintDec(UINT64 Value);
VOID    UPrintSizeMB(UINT64 Bytes);
VOID    UError(CONST CHAR16 *Msg, EFI_STATUS Status);
VOID    UInfo(CONST CHAR16 *Msg);
VOID    UOK(CONST CHAR16 *Msg);
VOID    UWarn(CONST CHAR16 *Msg);
VOID    UClearScreen(VOID);
VOID    USetAttr(UINT32 Attr);
VOID    USetCursor(UINTN Col, UINTN Row);

VOID    MemSet(VOID *Dst, UINT8 Val, UINTN Sz);
VOID    MemCopy(VOID *Dst, CONST VOID *Src, UINTN Sz);
BOOLEAN MemEqual(CONST VOID *A, CONST VOID *B, UINTN Sz);

UINTN   StrLen16(CONST CHAR16 *S);
VOID    StrCopy16(CHAR16 *Dst, CONST CHAR16 *Src, UINTN Max);
INT32   StrCmp16(CONST CHAR16 *A, CONST CHAR16 *B);
INT32   StrICmp16(CONST CHAR16 *A, CONST CHAR16 *B);
CHAR16 *StrChr16(CONST CHAR16 *S, CHAR16 C);
VOID    StrTrimRight(CHAR16 *S);
CHAR16 *StrTrimLeft(CHAR16 *S);
BOOLEAN StrStartsWith(CONST CHAR16 *S, CONST CHAR16 *Prefix);

/* String Conversion & Utilities */
VOID    A2U(CONST CHAR8 *Src, CHAR16 *Dst, UINTN Max);
VOID    AsciiToUnicode(CONST CHAR8 *Src, CHAR16 *Dst, UINTN Max);
VOID    U2A(CONST CHAR16 *Src, CHAR8 *Dst, UINTN Max);
UINT64  StrToU64(CONST CHAR16 *S);

/* Math & System Helpers */
UINT32  MathClamp(UINT32 Value, UINT32 Min, UINT32 Max);
UINT32  MathScale(UINT32 Val, UINT32 Numerator, UINT32 Denominator);
UINT32  CRC32Compute(CONST VOID *Data, UINTN Len);
UINT64  ReadTSC(VOID);

VOID    WaitKey(VOID);
BOOLEAN PollKey(EFI_INPUT_KEY *Key);

/* =========================================================================
 * Function prototypes — memory.c
 * ========================================================================= */
EFI_STATUS MemInit(VOID);
EFI_STATUS MemGetMap(VOID);
UINTN      MemGetMapKey(VOID);
EFI_STATUS PageAlloc(UINTN Pages, EFI_PHYSICAL_ADDRESS *Addr);
EFI_STATUS PageAllocAt(EFI_PHYSICAL_ADDRESS Addr, UINTN Pages);
EFI_STATUS PoolAlloc(UINTN Size, VOID **Buf);
VOID       PoolFree(VOID *Buf);
VOID       MemBuildRegionMap(VOID);

/* =========================================================================
 * Function prototypes — gfx.c
 * ========================================================================= */
EFI_STATUS GfxInit(VOID);
VOID       GfxFillRect(UINT32 X, UINT32 Y, UINT32 W, UINT32 H, UINT32 Color);
VOID       GfxDrawRect(UINT32 X, UINT32 Y, UINT32 W, UINT32 H, UINT32 Color, UINT32 Thick);
VOID       GfxDrawLine(UINT32 X0, UINT32 Y0, UINT32 X1, UINT32 Y1, UINT32 Color);
VOID       GfxPutPixel(UINT32 X, UINT32 Y, UINT32 Color);
VOID       GfxDrawChar(UINT32 X, UINT32 Y, CHAR16 Ch, UINT32 FG, UINT32 BG);
VOID       GfxDrawString(UINT32 X, UINT32 Y, CONST CHAR16 *Str, UINT32 FG, UINT32 BG);
VOID       GfxDrawStringA(UINT32 X, UINT32 Y, CONST CHAR8 *Str, UINT32 FG, UINT32 BG);
UINT32     GfxStringWidth(CONST CHAR16 *Str);
VOID       GfxDrawGradientV(UINT32 X, UINT32 Y, UINT32 W, UINT32 H,
                            UINT32 ColorTop, UINT32 ColorBot);
VOID       GfxDrawRoundRect(UINT32 X, UINT32 Y, UINT32 W, UINT32 H,
                            UINT32 R, UINT32 Color);
VOID       GfxScrollUp(UINT32 Lines);
VOID       GfxClear(UINT32 Color);
UINT32     GfxBlend(UINT32 ColA, UINT32 ColB, UINT32 Alpha); /* Alpha 0-255 */
UINT32     GfxLerp(UINT32 A, UINT32 B, UINT32 T256);        /* T: 0-256 */

/* =========================================================================
 * Function prototypes — fs.c
 * ========================================================================= */
EFI_STATUS FsInit(VOID);
EFI_STATUS FsReadFile(CONST CHAR16 *Path, VOID **Buf, UINTN *Size);
BOOLEAN    FsFileExists(CONST CHAR16 *Path);

/* =========================================================================
 * Function prototypes — hfs_detect.c
 * ========================================================================= */
EFI_STATUS HfsDetect(VOID);

/* =========================================================================
 * Function prototypes — config.c
 * ========================================================================= */
EFI_STATUS ConfigLoad(VOID);
VOID       ConfigSetDefaults(VOID);

/* =========================================================================
 * Function prototypes — gui_menu.c
 * ========================================================================= */
EFI_STATUS GuiMenu(VOID);

/* =========================================================================
 * Function prototypes — txt_menu.c
 * ========================================================================= */
EFI_STATUS TxtMenu(VOID);

/* =========================================================================
 * Function prototypes — kernel.c
 * ========================================================================= */
EFI_STATUS KernelLoad(VOID);

/* =========================================================================
 * Function prototypes — sysinfo.c
 * ========================================================================= */
EFI_STATUS SysInfoCollect(VOID);
EFI_STATUS SysInfoCPUID(VOID);
EFI_STATUS SysInfoACPI(VOID);
EFI_STATUS SysInfoSMP(VOID);
EFI_STATUS SysInfoPCI(VOID);

/* =========================================================================
 * Function prototypes — boot.c
 * ========================================================================= */
EFI_STATUS BootHandoff(VOID);