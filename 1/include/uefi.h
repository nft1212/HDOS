/*
 * ============================================================================
 *  HBOOT — Home Boot Loader
 *  Copyright (C) 2024-2026 Winora Company. All rights reserved.
 *  File: include/uefi.h — Complete UEFI 2.10 type definitions
 * ============================================================================
 */
#pragma once

/* =========================================================================
 * Fundamental types — no libc dependency
 * ========================================================================= */
typedef unsigned char       UINT8;
typedef unsigned short      UINT16;
typedef unsigned int        UINT32;
typedef unsigned long long  UINT64;
typedef signed char         INT8;
typedef signed short        INT16;
typedef signed int          INT32;
typedef signed long long    INT64;
typedef unsigned long long  UINTN;
typedef signed long long    INTN;
typedef unsigned char       BOOLEAN;
typedef unsigned short      CHAR16;
typedef char                CHAR8;
typedef void                VOID;
typedef UINT64              EFI_STATUS;
typedef void               *EFI_HANDLE;
typedef void               *EFI_EVENT;
typedef UINT64              EFI_PHYSICAL_ADDRESS;
typedef UINT64              EFI_VIRTUAL_ADDRESS;
typedef UINT64              EFI_LBA;
typedef UINT8               EFI_MAC_ADDRESS[32];

#define TRUE    ((BOOLEAN)1)
#define FALSE   ((BOOLEAN)0)
#define NULL    ((void*)0)
#define CONST   const
#define IN
#define OUT
#define OPTIONAL

/* UEFI calling convention = Microsoft x64 ABI */
#define EFIAPI  __attribute__((ms_abi))

/* =========================================================================
 * EFI Status codes
 * ========================================================================= */
#define EFI_SUCCESS                 0ULL
#define EFI_ERROR_BIT               (1ULL << 63)
#define EFI_LOAD_ERROR              (EFI_ERROR_BIT | 1)
#define EFI_INVALID_PARAMETER       (EFI_ERROR_BIT | 2)
#define EFI_UNSUPPORTED             (EFI_ERROR_BIT | 3)
#define EFI_BAD_BUFFER_SIZE         (EFI_ERROR_BIT | 4)
#define EFI_BUFFER_TOO_SMALL        (EFI_ERROR_BIT | 5)
#define EFI_NOT_READY               (EFI_ERROR_BIT | 6)
#define EFI_DEVICE_ERROR            (EFI_ERROR_BIT | 7)
#define EFI_WRITE_PROTECTED         (EFI_ERROR_BIT | 8)
#define EFI_OUT_OF_RESOURCES        (EFI_ERROR_BIT | 9)
#define EFI_VOLUME_CORRUPTED        (EFI_ERROR_BIT | 10)
#define EFI_VOLUME_FULL             (EFI_ERROR_BIT | 11)
#define EFI_NO_MEDIA                (EFI_ERROR_BIT | 12)
#define EFI_MEDIA_CHANGED           (EFI_ERROR_BIT | 13)
#define EFI_NOT_FOUND               (EFI_ERROR_BIT | 14)
#define EFI_ACCESS_DENIED           (EFI_ERROR_BIT | 15)
#define EFI_NO_RESPONSE             (EFI_ERROR_BIT | 16)
#define EFI_CRC_ERROR               (EFI_ERROR_BIT | 27)
#define EFI_SECURITY_VIOLATION      (EFI_ERROR_BIT | 26)
#define EFI_ERROR(s)                (((INT64)(s)) < 0)

/* =========================================================================
 * Console colors
 * ========================================================================= */
#define EFI_BLACK           0x00
#define EFI_BLUE            0x01
#define EFI_GREEN           0x02
#define EFI_CYAN            0x03
#define EFI_RED             0x04
#define EFI_MAGENTA         0x05
#define EFI_BROWN           0x06
#define EFI_LIGHTGRAY       0x07
#define EFI_DARKGRAY        0x08
#define EFI_LIGHTBLUE       0x09
#define EFI_LIGHTGREEN      0x0A
#define EFI_LIGHTCYAN       0x0B
#define EFI_LIGHTRED        0x0C
#define EFI_LIGHTMAGENTA    0x0D
#define EFI_YELLOW          0x0E
#define EFI_WHITE           0x0F
#define EFI_BACKGROUND_BLACK      0x00
#define EFI_BACKGROUND_BLUE       0x10
#define EFI_BACKGROUND_GREEN      0x20
#define EFI_BACKGROUND_CYAN       0x30
#define EFI_BACKGROUND_RED        0x40
#define EFI_BACKGROUND_MAGENTA    0x50
#define EFI_BACKGROUND_BROWN      0x60
#define EFI_BACKGROUND_LIGHTGRAY  0x70

/* =========================================================================
 * GUID
 * ========================================================================= */
typedef struct {
    UINT32 Data1;
    UINT16 Data2;
    UINT16 Data3;
    UINT8  Data4[8];
} EFI_GUID;

#define GUID(a,b,c,d0,d1,d2,d3,d4,d5,d6,d7) \
    { (a),(b),(c),{(d0),(d1),(d2),(d3),(d4),(d5),(d6),(d7)} }

/* =========================================================================
 * Input / Output protocols
 * ========================================================================= */
typedef struct _EFI_SIMPLE_TEXT_INPUT_PROTOCOL  EFI_SIMPLE_TEXT_INPUT_PROTOCOL;
typedef struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;

typedef struct {
    UINT16 ScanCode;
    CHAR16 UnicodeChar;
} EFI_INPUT_KEY;

/* Scan codes */
#define SCAN_NULL    0x00
#define SCAN_UP      0x01
#define SCAN_DOWN    0x02
#define SCAN_RIGHT   0x03
#define SCAN_LEFT    0x04
#define SCAN_HOME    0x05
#define SCAN_END     0x06
#define SCAN_INSERT  0x07
#define SCAN_DELETE  0x08
#define SCAN_PAGE_UP 0x09
#define SCAN_PAGE_DN 0x0A
#define SCAN_F1      0x0B
#define SCAN_F2      0x0C
#define SCAN_F5      0x0F
#define SCAN_F10     0x14
#define SCAN_ESC     0x17

#define CHAR_NULL    0x0000
#define CHAR_BS      0x0008
#define CHAR_TAB     0x0009
#define CHAR_LF      0x000A
#define CHAR_CR      0x000D
#define CHAR_ESC     0x001B
#define CHAR_SPACE   0x0020

typedef EFI_STATUS (EFIAPI *EFI_INPUT_RESET)(EFI_SIMPLE_TEXT_INPUT_PROTOCOL*, BOOLEAN);
typedef EFI_STATUS (EFIAPI *EFI_INPUT_READ_KEY)(EFI_SIMPLE_TEXT_INPUT_PROTOCOL*, EFI_INPUT_KEY*);

struct _EFI_SIMPLE_TEXT_INPUT_PROTOCOL {
    EFI_INPUT_RESET    Reset;
    EFI_INPUT_READ_KEY ReadKeyStroke;
    EFI_EVENT          WaitForKey;
};

typedef EFI_STATUS (EFIAPI *EFI_TEXT_RESET_OUT)(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL*, BOOLEAN);
typedef EFI_STATUS (EFIAPI *EFI_TEXT_STRING)(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL*, CHAR16*);
typedef EFI_STATUS (EFIAPI *EFI_TEXT_SET_ATTRIBUTE)(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL*, UINTN);
typedef EFI_STATUS (EFIAPI *EFI_TEXT_CLEAR_SCREEN)(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL*);
typedef EFI_STATUS (EFIAPI *EFI_TEXT_SET_CURSOR)(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL*, UINTN, UINTN);
typedef EFI_STATUS (EFIAPI *EFI_TEXT_ENABLE_CURSOR)(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL*, BOOLEAN);

typedef struct {
    INT32  MaxMode;
    INT32  Mode;
    INT32  Attribute;
    INT32  CursorColumn;
    INT32  CursorRow;
    BOOLEAN CursorVisible;
} SIMPLE_TEXT_OUTPUT_MODE;

struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL {
    EFI_TEXT_RESET_OUT  Reset;
    EFI_TEXT_STRING     OutputString;
    VOID               *TestString;
    VOID               *QueryMode;
    VOID               *SetMode;
    EFI_TEXT_SET_ATTRIBUTE SetAttribute;
    EFI_TEXT_CLEAR_SCREEN  ClearScreen;
    EFI_TEXT_SET_CURSOR    SetCursorPosition;
    EFI_TEXT_ENABLE_CURSOR EnableCursor;
    SIMPLE_TEXT_OUTPUT_MODE *Mode;
};

/* =========================================================================
 * Memory
 * ========================================================================= */
typedef enum {
    EfiReservedMemoryType = 0,
    EfiLoaderCode,
    EfiLoaderData,
    EfiBootServicesCode,
    EfiBootServicesData,
    EfiRuntimeServicesCode,
    EfiRuntimeServicesData,
    EfiConventionalMemory,
    EfiUnusableMemory,
    EfiACPIReclaimMemory,
    EfiACPIMemoryNVS,
    EfiMemoryMappedIO,
    EfiMemoryMappedIOPortSpace,
    EfiPalCode,
    EfiPersistentMemory,
    EfiMaxMemoryType
} EFI_MEMORY_TYPE;

typedef enum {
    AllocateAnyPages = 0,
    AllocateMaxAddress,
    AllocateAddress,
    MaxAllocateType
} EFI_ALLOCATE_TYPE;

typedef struct {
    UINT32               Type;
    UINT32               Pad;
    EFI_PHYSICAL_ADDRESS PhysicalStart;
    EFI_VIRTUAL_ADDRESS  VirtualStart;
    UINT64               NumberOfPages;
    UINT64               Attribute;
} EFI_MEMORY_DESCRIPTOR;

#define EFI_MEMORY_RUNTIME    0x8000000000000000ULL
#define EFI_PAGE_SIZE         4096ULL

/* =========================================================================
 * Boot Services
 * ========================================================================= */
typedef EFI_STATUS (EFIAPI *EFI_ALLOCATE_PAGES)(EFI_ALLOCATE_TYPE, EFI_MEMORY_TYPE, UINTN, EFI_PHYSICAL_ADDRESS*);
typedef EFI_STATUS (EFIAPI *EFI_FREE_PAGES)(EFI_PHYSICAL_ADDRESS, UINTN);
typedef EFI_STATUS (EFIAPI *EFI_GET_MEMORY_MAP)(UINTN*, EFI_MEMORY_DESCRIPTOR*, UINTN*, UINTN*, UINT32*);
typedef EFI_STATUS (EFIAPI *EFI_ALLOCATE_POOL)(EFI_MEMORY_TYPE, UINTN, VOID**);
typedef EFI_STATUS (EFIAPI *EFI_FREE_POOL)(VOID*);
typedef EFI_STATUS (EFIAPI *EFI_WAIT_FOR_EVENT)(UINTN, EFI_EVENT*, UINTN*);
typedef EFI_STATUS (EFIAPI *EFI_HANDLE_PROTOCOL)(EFI_HANDLE, EFI_GUID*, VOID**);
typedef EFI_STATUS (EFIAPI *EFI_LOCATE_HANDLE_BUFFER)(UINT32, EFI_GUID*, VOID*, UINTN*, EFI_HANDLE**);
typedef EFI_STATUS (EFIAPI *EFI_LOCATE_PROTOCOL)(EFI_GUID*, VOID*, VOID**);
typedef EFI_STATUS (EFIAPI *EFI_EXIT_BOOT_SERVICES)(EFI_HANDLE, UINTN);
typedef EFI_STATUS (EFIAPI *EFI_SET_WATCHDOG_TIMER)(UINTN, UINT64, UINTN, CHAR16*);
typedef EFI_STATUS (EFIAPI *EFI_STALL)(UINTN);
typedef EFI_STATUS (EFIAPI *EFI_OPEN_PROTOCOL)(EFI_HANDLE, EFI_GUID*, VOID**, EFI_HANDLE, EFI_HANDLE, UINT32);
typedef EFI_STATUS (EFIAPI *EFI_CLOSE_PROTOCOL)(EFI_HANDLE, EFI_GUID*, EFI_HANDLE, EFI_HANDLE);
typedef EFI_STATUS (EFIAPI *EFI_GET_NEXT_MONOTONIC_COUNT)(UINT64*);

#define EFI_BY_PROTOCOL 2
#define EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL  0x00000001
#define EFI_OPEN_PROTOCOL_GET_PROTOCOL        0x00000002

typedef struct {
    UINT64 Signature;
    UINT32 Revision;
    UINT32 HeaderSize;
    UINT32 CRC32;
    UINT32 Reserved;

    /* Task Priority */
    VOID *RaisePriority;
    VOID *RestorePriority;
    /* Memory */
    EFI_ALLOCATE_PAGES  AllocatePages;
    EFI_FREE_PAGES      FreePages;
    EFI_GET_MEMORY_MAP  GetMemoryMap;
    EFI_ALLOCATE_POOL   AllocatePool;
    EFI_FREE_POOL       FreePool;
    /* Events */
    VOID *CreateEvent;
    VOID *SetTimer;
    EFI_WAIT_FOR_EVENT WaitForEvent;
    VOID *SignalEvent;
    VOID *CloseEvent;
    VOID *CheckEvent;
    /* Protocol Handler */
    VOID *InstallProtocolInterface;
    VOID *ReinstallProtocolInterface;
    VOID *UninstallProtocolInterface;
    EFI_HANDLE_PROTOCOL HandleProtocol;
    VOID *Reserved2;
    VOID *RegisterProtocolNotify;
    VOID *LocateHandle;
    VOID *LocateDevicePath;
    VOID *InstallConfigurationTable;
    /* Image */
    VOID *LoadImage;
    VOID *StartImage;
    VOID *Exit;
    VOID *UnloadImage;
    EFI_EXIT_BOOT_SERVICES ExitBootServices;
    /* Misc */
    EFI_GET_NEXT_MONOTONIC_COUNT GetNextMonotonicCount;
    EFI_STALL Stall;
    EFI_SET_WATCHDOG_TIMER SetWatchdogTimer;
    /* Driver */
    VOID *ConnectController;
    VOID *DisconnectController;
    /* Open/Close */
    EFI_OPEN_PROTOCOL  OpenProtocol;
    EFI_CLOSE_PROTOCOL CloseProtocol;
    VOID *OpenProtocolInformation;
    /* Library */
    VOID *ProtocolsPerHandle;
    EFI_LOCATE_HANDLE_BUFFER LocateHandleBuffer;
    EFI_LOCATE_PROTOCOL      LocateProtocol;
    VOID *InstallMultipleProtocolInterfaces;
    VOID *UninstallMultipleProtocolInterfaces;
    VOID *CalculateCrc32;
    VOID *CopyMem;
    VOID *SetMem;
    VOID *CreateEventEx;
} EFI_BOOT_SERVICES;

/* =========================================================================
 * Runtime Services
 * ========================================================================= */
typedef enum {
    EfiResetCold = 0,
    EfiResetWarm,
    EfiResetShutdown,
    EfiResetPlatformSpecific
} EFI_RESET_TYPE;

typedef struct {
    UINT16 Year;
    UINT8  Month;
    UINT8  Day;
    UINT8  Hour;
    UINT8  Minute;
    UINT8  Second;
    UINT8  Pad1;
    UINT32 Nanosecond;
    INT16  TimeZone;
    UINT8  Daylight;
    UINT8  Pad2;
} EFI_TIME;

typedef EFI_STATUS (EFIAPI *EFI_GET_TIME)(EFI_TIME*, VOID*);
typedef EFI_STATUS (EFIAPI *EFI_SET_TIME)(EFI_TIME*);
typedef VOID       (EFIAPI *EFI_RESET_SYSTEM)(EFI_RESET_TYPE, EFI_STATUS, UINTN, VOID*);
typedef EFI_STATUS (EFIAPI *EFI_SET_VIRTUAL_ADDRESS_MAP)(UINTN, UINTN, UINT32, EFI_MEMORY_DESCRIPTOR*);
typedef EFI_STATUS (EFIAPI *EFI_GET_VARIABLE)(CHAR16*, EFI_GUID*, UINT32*, UINTN*, VOID*);
typedef EFI_STATUS (EFIAPI *EFI_SET_VARIABLE)(CHAR16*, EFI_GUID*, UINT32, UINTN, VOID*);

#define EFI_VARIABLE_NON_VOLATILE       0x00000001
#define EFI_VARIABLE_BOOTSERVICE_ACCESS 0x00000002
#define EFI_VARIABLE_RUNTIME_ACCESS     0x00000004

typedef struct {
    UINT64 Signature;
    UINT32 Revision;
    UINT32 HeaderSize;
    UINT32 CRC32;
    UINT32 Reserved;

    EFI_GET_TIME               GetTime;
    EFI_SET_TIME               SetTime;
    VOID                      *GetWakeupTime;
    VOID                      *SetWakeupTime;
    EFI_SET_VIRTUAL_ADDRESS_MAP SetVirtualAddressMap;
    VOID                      *ConvertPointer;
    EFI_GET_VARIABLE           GetVariable;
    VOID                      *GetNextVariableName;
    EFI_SET_VARIABLE           SetVariable;
    VOID                      *GetNextHighMonotonicCount;
    EFI_RESET_SYSTEM           ResetSystem;
    VOID                      *UpdateCapsule;
    VOID                      *QueryCapsuleCapabilities;
    VOID                      *QueryVariableInfo;
} EFI_RUNTIME_SERVICES;

/* =========================================================================
 * System Table
 * ========================================================================= */
typedef struct {
    EFI_GUID VendorGuid;
    VOID    *VendorTable;
} EFI_CONFIGURATION_TABLE;

typedef struct {
    UINT64                          Signature;
    UINT32                          Revision;
    UINT32                          HeaderSize;
    UINT32                          CRC32;
    UINT32                          Reserved;
    CHAR16                         *FirmwareVendor;
    UINT32                          FirmwareRevision;
    UINT32                          Pad;
    EFI_HANDLE                      ConsoleInHandle;
    EFI_SIMPLE_TEXT_INPUT_PROTOCOL *ConIn;
    EFI_HANDLE                      ConsoleOutHandle;
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *ConOut;
    EFI_HANDLE                      StandardErrorHandle;
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *StdErr;
    EFI_RUNTIME_SERVICES           *RuntimeServices;
    EFI_BOOT_SERVICES              *BootServices;
    UINTN                           NumberOfTableEntries;
    EFI_CONFIGURATION_TABLE        *ConfigurationTable;
} EFI_SYSTEM_TABLE;

/* =========================================================================
 * Graphics Output Protocol (GOP)
 * ========================================================================= */
#define EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID \
    GUID(0x9042a9de,0x23dc,0x4a38,0x96,0xfb,0x7a,0xde,0xd0,0x80,0x51,0x6a)

typedef enum {
    PixelRedGreenBlueReserved8BitPerColor = 0,
    PixelBlueGreenRedReserved8BitPerColor,
    PixelBitMask,
    PixelBltOnly,
    PixelFormatMax
} EFI_GRAPHICS_PIXEL_FORMAT;

typedef struct {
    UINT32 RedMask;
    UINT32 GreenMask;
    UINT32 BlueMask;
    UINT32 ReservedMask;
} EFI_PIXEL_BITMASK;

typedef struct {
    UINT32                    Version;
    UINT32                    HorizontalResolution;
    UINT32                    VerticalResolution;
    EFI_GRAPHICS_PIXEL_FORMAT PixelFormat;
    EFI_PIXEL_BITMASK         PixelInformation;
    UINT32                    PixelsPerScanLine;
} EFI_GRAPHICS_OUTPUT_MODE_INFORMATION;

typedef struct {
    UINT32                               MaxMode;
    UINT32                               Mode;
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *Info;
    UINTN                                SizeOfInfo;
    EFI_PHYSICAL_ADDRESS                 FrameBufferBase;
    UINTN                                FrameBufferSize;
} EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE;

typedef struct _EFI_GRAPHICS_OUTPUT_PROTOCOL EFI_GRAPHICS_OUTPUT_PROTOCOL;
typedef EFI_STATUS (EFIAPI *EFI_GOP_QUERY_MODE)(EFI_GRAPHICS_OUTPUT_PROTOCOL*, UINT32, UINTN*, EFI_GRAPHICS_OUTPUT_MODE_INFORMATION**);
typedef EFI_STATUS (EFIAPI *EFI_GOP_SET_MODE)(EFI_GRAPHICS_OUTPUT_PROTOCOL*, UINT32);
typedef EFI_STATUS (EFIAPI *EFI_GOP_BLT)(EFI_GRAPHICS_OUTPUT_PROTOCOL*, VOID*, UINT32, UINTN, UINTN, UINTN, UINTN, UINTN, UINTN, UINTN);

struct _EFI_GRAPHICS_OUTPUT_PROTOCOL {
    EFI_GOP_QUERY_MODE               QueryMode;
    EFI_GOP_SET_MODE                 SetMode;
    EFI_GOP_BLT                      Blt;
    EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE *Mode;
};

/* BLT operations */
typedef enum {
    EfiBltVideoFill = 0,
    EfiBltVideoToBltBuffer,
    EfiBltBufferToVideo,
    EfiBltVideoToVideo,
    EfiGraphicsOutputBltOperationMax
} EFI_GRAPHICS_OUTPUT_BLT_OPERATION;

typedef struct {
    UINT8 Blue;
    UINT8 Green;
    UINT8 Red;
    UINT8 Reserved;
} EFI_GRAPHICS_OUTPUT_BLT_PIXEL;

/* =========================================================================
 * File Protocol
 * ========================================================================= */
#define EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID \
    GUID(0x964e5b22,0x6459,0x11d2,0x8e,0x39,0x00,0xa0,0xc9,0x69,0x72,0x3b)
#define EFI_LOADED_IMAGE_PROTOCOL_GUID \
    GUID(0x5b1b31a1,0x9562,0x11d2,0x8e,0x3f,0x00,0xa0,0xc9,0x69,0x72,0x3b)
#define EFI_FILE_INFO_GUID \
    GUID(0x09576e92,0x6d3f,0x11d2,0x8e,0x39,0x00,0xa0,0xc9,0x69,0x72,0x3b)

#define EFI_FILE_MODE_READ    0x0000000000000001ULL
#define EFI_FILE_MODE_WRITE   0x0000000000000002ULL
#define EFI_FILE_MODE_CREATE  0x8000000000000000ULL
#define EFI_FILE_READ_ONLY    0x0000000000000001ULL
#define EFI_FILE_DIRECTORY    0x0000000000000010ULL

typedef struct _EFI_FILE_PROTOCOL EFI_FILE_PROTOCOL;
typedef EFI_STATUS (EFIAPI *EFI_FILE_OPEN)(EFI_FILE_PROTOCOL*, EFI_FILE_PROTOCOL**, CHAR16*, UINT64, UINT64);
typedef EFI_STATUS (EFIAPI *EFI_FILE_CLOSE)(EFI_FILE_PROTOCOL*);
typedef EFI_STATUS (EFIAPI *EFI_FILE_READ)(EFI_FILE_PROTOCOL*, UINTN*, VOID*);
typedef EFI_STATUS (EFIAPI *EFI_FILE_WRITE)(EFI_FILE_PROTOCOL*, UINTN*, VOID*);
typedef EFI_STATUS (EFIAPI *EFI_FILE_GET_INFO)(EFI_FILE_PROTOCOL*, EFI_GUID*, UINTN*, VOID*);
typedef EFI_STATUS (EFIAPI *EFI_FILE_SET_POSITION)(EFI_FILE_PROTOCOL*, UINT64);
typedef EFI_STATUS (EFIAPI *EFI_FILE_GET_POSITION)(EFI_FILE_PROTOCOL*, UINT64*);
typedef EFI_STATUS (EFIAPI *EFI_FILE_FLUSH)(EFI_FILE_PROTOCOL*);

struct _EFI_FILE_PROTOCOL {
    UINT64              Revision;
    EFI_FILE_OPEN       Open;
    EFI_FILE_CLOSE      Close;
    VOID               *Delete;
    EFI_FILE_READ       Read;
    EFI_FILE_WRITE      Write;
    EFI_FILE_GET_POSITION GetPosition;
    EFI_FILE_SET_POSITION SetPosition;
    EFI_FILE_GET_INFO   GetInfo;
    VOID               *SetInfo;
    EFI_FILE_FLUSH      Flush;
};

typedef struct {
    UINT64   Size;
    UINT64   FileSize;
    UINT64   PhysicalSize;
    EFI_TIME CreateTime;
    EFI_TIME LastAccessTime;
    EFI_TIME ModificationTime;
    UINT64   Attribute;
    CHAR16   FileName[256];
} EFI_FILE_INFO;

typedef struct _EFI_SIMPLE_FILE_SYSTEM_PROTOCOL EFI_SIMPLE_FILE_SYSTEM_PROTOCOL;
typedef EFI_STATUS (EFIAPI *EFI_VOLUME_OPEN)(EFI_SIMPLE_FILE_SYSTEM_PROTOCOL*, EFI_FILE_PROTOCOL**);
struct _EFI_SIMPLE_FILE_SYSTEM_PROTOCOL {
    UINT64          Revision;
    EFI_VOLUME_OPEN OpenVolume;
};

typedef struct {
    UINT32           Revision;
    EFI_HANDLE       ParentHandle;
    EFI_SYSTEM_TABLE *SystemTable;
    EFI_HANDLE       DeviceHandle;
    VOID            *FilePath;
    VOID            *Reserved;
    UINT32           LoadOptionsSize;
    VOID            *LoadOptions;
    VOID            *ImageBase;
    UINT64           ImageSize;
    EFI_MEMORY_TYPE  ImageCodeType;
    EFI_MEMORY_TYPE  ImageDataType;
    VOID            *Unload;
} EFI_LOADED_IMAGE_PROTOCOL;

/* =========================================================================
 * Block I/O Protocol — for HFS raw disk access
 * ========================================================================= */
#define EFI_BLOCK_IO_PROTOCOL_GUID \
    GUID(0x964e5b21,0x6459,0x11d2,0x8e,0x39,0x00,0xa0,0xc9,0x69,0x72,0x3b)

typedef struct {
    UINT32  MediaId;
    BOOLEAN RemovableMedia;
    BOOLEAN MediaPresent;
    BOOLEAN LogicalPartition;
    BOOLEAN ReadOnly;
    BOOLEAN WriteCaching;
    UINT32  BlockSize;
    UINT32  IoAlign;
    UINT64  LastBlock;
} EFI_BLOCK_IO_MEDIA;

typedef struct _EFI_BLOCK_IO_PROTOCOL EFI_BLOCK_IO_PROTOCOL;
typedef EFI_STATUS (EFIAPI *EFI_BLOCK_RESET)(EFI_BLOCK_IO_PROTOCOL*, BOOLEAN);
typedef EFI_STATUS (EFIAPI *EFI_BLOCK_READ)(EFI_BLOCK_IO_PROTOCOL*, UINT32, UINT64, UINTN, VOID*);
typedef EFI_STATUS (EFIAPI *EFI_BLOCK_WRITE)(EFI_BLOCK_IO_PROTOCOL*, UINT32, UINT64, UINTN, VOID*);
typedef EFI_STATUS (EFIAPI *EFI_BLOCK_FLUSH)(EFI_BLOCK_IO_PROTOCOL*);

struct _EFI_BLOCK_IO_PROTOCOL {
    UINT64              Revision;
    EFI_BLOCK_IO_MEDIA *Media;
    EFI_BLOCK_RESET     Reset;
    EFI_BLOCK_READ      ReadBlocks;
    EFI_BLOCK_WRITE     WriteBlocks;
    EFI_BLOCK_FLUSH     FlushBlocks;
};
