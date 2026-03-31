/*
 * ============================================================================
 *  HBOOT — Home Boot Loader
 *  Copyright (C) 2024-2026 Winora Company. All rights reserved.
 *  File: include/hfs.h — HFS (Home File System) on-disk structures
 *
 *  HFS — Home File System
 *  A purpose-built filesystem for HDOS with:
 *   - Unique inode index system (HFS Index Table, HIT)
 *   - Extent-based block allocation
 *   - 64-bit addressing (up to 16 EB volumes)
 *   - CRC32 checksums on metadata
 *   - Journal for crash recovery
 *   - Inline xattr support
 * ============================================================================
 */
#pragma once
#include "uefi.h"

/* =========================================================================
 * HFS Magic numbers and version
 * ========================================================================= */
#define HFS_SUPER_MAGIC         0x534648444E4F4957ULL  /* 'WINODHFS' */
#define HFS_SUPER_MAGIC2        0x32305346484F4D48ULL  /* 'HMOHFS20' */
#define HFS_VERSION_MAJOR       2
#define HFS_VERSION_MINOR       0
#define HFS_VERSION             ((HFS_VERSION_MAJOR << 16) | HFS_VERSION_MINOR)

/* Block size options */
#define HFS_BLOCK_SIZE_MIN      512
#define HFS_BLOCK_SIZE_DEFAULT  4096
#define HFS_BLOCK_SIZE_MAX      65536

/* HFS Superblock LBA */
#define HFS_SUPERBLOCK_LBA      0
#define HFS_SUPERBLOCK_BACKUP   8

/* =========================================================================
 * HFS Superblock (512 bytes, at LBA 0)
 * ========================================================================= */
typedef struct {
    /* Signature */
    UINT64  Magic1;             /* HFS_SUPER_MAGIC                    +0  */
    UINT64  Magic2;             /* HFS_SUPER_MAGIC2                   +8  */
    UINT32  Version;            /* HFS_VERSION                        +16 */
    UINT32  Flags;              /* Volume flags                        +20 */

    /* Geometry */
    UINT32  BlockSize;          /* Bytes per block (default 4096)     +24 */
    UINT32  Pad0;
    UINT64  TotalBlocks;        /* Total blocks on volume             +32 */
    UINT64  FreeBlocks;         /* Free blocks count                  +40 */
    UINT64  UsedBlocks;         /* Used blocks count                  +48 */

    /* HIT — HFS Index Table */
    UINT64  HitLBA;             /* Start LBA of HIT                  +56 */
    UINT64  HitSize;            /* HIT size in blocks                 +64 */
    UINT64  HitEntries;         /* Total inode slots                  +72 */
    UINT64  HitFreeEntries;     /* Free inode slots                   +80 */
    UINT64  NextInodeID;        /* Next inode ID to allocate          +88 */

    /* Journal */
    UINT64  JournalLBA;         /* Journal start LBA                  +96 */
    UINT64  JournalSize;        /* Journal size in blocks             +104 */
    UINT64  JournalSeq;         /* Journal sequence number            +112 */

    /* Block Allocation Bitmap */
    UINT64  BitmapLBA;          /* Bitmap start LBA                   +120 */
    UINT64  BitmapSize;         /* Bitmap size in blocks              +128 */

    /* Root directory */
    UINT64  RootInodeID;        /* Inode ID of root directory         +136 */

    /* Volume metadata */
    CHAR16  VolumeName[128];    /* Volume label (UTF-16 LE)           +144 */
    UINT64  CreateTime;         /* Unix timestamp (ns)                +400 */
    UINT64  MountTime;          /* Last mount timestamp (ns)          +408 */
    UINT64  WriteTime;          /* Last write timestamp (ns)          +416 */
    UINT32  MountCount;         /* Number of mounts                   +424 */
    UINT32  MaxMountCount;      /* Max mounts before fsck             +428 */

    /* UUID */
    UINT8   VolumeUUID[16];     /* Unique volume identifier           +432 */

    /* Extended info */
    UINT64  ExtInfoLBA;         /* Extended metadata block LBA        +448 */
    UINT32  FeatureFlags;       /* Optional features bitmask          +456 */
    UINT32  ROCompatFlags;      /* Read-only compat features          +460 */
    UINT32  IncompatFlags;      /* Incompatible features              +464 */

    /* Checksum */
    UINT32  CRC32;              /* CRC32 of superblock (excl. this)  +468 */

    UINT8   Reserved[36];       /* Reserved for future use            +472 */
                                /* Total: 508 bytes                        */
    UINT32  BackupCRC32;        /* Backup CRC                         +508 */
} __attribute__((packed)) HFS_SUPERBLOCK;

/* Volume flags */
#define HFS_FLAG_CLEAN          0x00000001  /* Cleanly unmounted */
#define HFS_FLAG_ERRORS         0x00000002  /* Errors detected */
#define HFS_FLAG_READONLY       0x00000004  /* Mounted read-only */
#define HFS_FLAG_JOURNAL        0x00000008  /* Journal active */
#define HFS_FLAG_ENCRYPTED      0x00000010  /* Volume encrypted */
#define HFS_FLAG_COMPRESSED     0x00000020  /* Transparent compression */
#define HFS_FLAG_CASE_SENSITIVE 0x00000040  /* Case-sensitive filenames */

/* Feature flags */
#define HFS_FEAT_EXTENTS        0x00000001  /* Extent-based allocation */
#define HFS_FEAT_XATTR          0x00000002  /* Extended attributes */
#define HFS_FEAT_COMPRESS       0x00000004  /* File compression */
#define HFS_FEAT_ENCRYPT        0x00000008  /* File encryption */
#define HFS_FEAT_SPARSE         0x00000010  /* Sparse files */
#define HFS_FEAT_SYMLINKS       0x00000020  /* Symbolic links */
#define HFS_FEAT_HARDLINKS      0x00000040  /* Hard links */

/* =========================================================================
 * HFS Inode Types
 * ========================================================================= */
#define HFS_INODE_FREE          0x0000  /* Free slot */
#define HFS_INODE_FILE          0x0001  /* Regular file */
#define HFS_INODE_DIR           0x0002  /* Directory */
#define HFS_INODE_SYMLINK       0x0003  /* Symbolic link */
#define HFS_INODE_DEVICE        0x0004  /* Device node */
#define HFS_INODE_PIPE          0x0005  /* Named pipe */
#define HFS_INODE_SOCKET        0x0006  /* Socket */
#define HFS_INODE_JOURNAL       0x00FE  /* Journal file (internal) */
#define HFS_INODE_BITMAP        0x00FF  /* Bitmap file (internal) */

/* =========================================================================
 * HFS Extent — describes a contiguous run of blocks
 * ========================================================================= */
typedef struct {
    UINT64  StartBlock;     /* First block of extent */
    UINT64  BlockCount;     /* Number of blocks */
} __attribute__((packed)) HFS_EXTENT;

#define HFS_INLINE_EXTENTS  6   /* Extents stored directly in inode */

/* =========================================================================
 * HFS Inode — 256 bytes
 * The core of the HIT (HFS Index Table)
 * ========================================================================= */
typedef struct {
    /* Identity */
    UINT64  InodeID;            /* Unique inode ID                    +0  */
    UINT16  Type;               /* HFS_INODE_* type                   +8  */
    UINT16  Flags;              /* Inode flags                        +10 */
    UINT32  LinkCount;          /* Hard link count                    +12 */
    UINT32  UID;                /* Owner user ID                      +16 */
    UINT32  GID;                /* Owner group ID                     +20 */
    UINT32  Permissions;        /* Unix-style rwxrwxrwx               +24 */
    UINT32  Pad0;

    /* Size */
    UINT64  FileSize;           /* File size in bytes                 +32 */
    UINT64  AllocatedBlocks;    /* Number of allocated blocks         +40 */

    /* Timestamps (nanoseconds since Unix epoch) */
    UINT64  CreateTime;         /* Creation time                      +48 */
    UINT64  ModifyTime;         /* Last data modification             +56 */
    UINT64  ChangeTime;         /* Last metadata change               +64 */
    UINT64  AccessTime;         /* Last access time                   +72 */

    /* Parent */
    UINT64  ParentInodeID;      /* Parent directory inode ID          +80 */

    /* Extents (inline) */
    HFS_EXTENT Extents[HFS_INLINE_EXTENTS]; /* 6 * 16 = 96 bytes   +88 */

    /* Overflow */
    UINT64  ExtentOverflowLBA;  /* LBA of overflow extent table (+184) */
    UINT64  ExtentOverflowSize; /* Size of overflow table in bytes    +192 */

    /* Extended attributes */
    UINT64  XattrLBA;           /* LBA of xattr block                +200 */
    UINT32  XattrSize;          /* Size of xattr data in bytes        +208 */

    /* Inline data (for small symlinks / tiny files) */
    UINT16  InlineDataSize;     /* Bytes of inline data               +212 */

    /* Checksum */
    UINT32  CRC32;              /* CRC32 of this inode                +214 */

    UINT8   Reserved[34];       /* Reserved                           +218 */
                                /* Total: 252 bytes                        */
    UINT32  InodeCRC;           /* Final checksum                     +252 */
} __attribute__((packed)) HFS_INODE;  /* 256 bytes */

/* Inode flags */
#define HFS_IFLAG_IMMUTABLE     0x0001
#define HFS_IFLAG_APPEND_ONLY   0x0002
#define HFS_IFLAG_COMPRESSED    0x0004
#define HFS_IFLAG_ENCRYPTED     0x0008
#define HFS_IFLAG_INLINE_DATA   0x0010  /* Small files stored in inode */
#define HFS_IFLAG_HIT_OVERFLOW  0x0020  /* Inode uses extent overflow */
#define HFS_IFLAG_DELETED       0x8000  /* Soft-deleted */

/* =========================================================================
 * HFS Index Table (HIT) — Inode lookup structure
 *
 * Layout on disk:
 *   [HIT Header (1 block)]
 *   [Level-1 Index Blocks]
 *   [Level-2 Index Blocks]
 *   [Inode Data Blocks (HFS_INODE records, 256 bytes each)]
 *
 * Unique feature: The HIT uses a two-level B+-tree-like index
 * with 64-bit inode IDs. Each L1 block maps 512 inode ID ranges
 * to L2 blocks. Each L2 block maps 512 inode IDs to inode slots.
 * This gives O(1) inode lookup with 2 disk reads.
 * ========================================================================= */

#define HIT_HEADER_MAGIC    0x544948534648ULL   /* 'HFSHITx' */
#define HIT_INODES_PER_BLOCK  (4096 / 256)      /* 16 inodes per 4K block */
#define HIT_L1_ENTRIES      512                 /* Entries per L1 block */
#define HIT_L2_ENTRIES      512                 /* Entries per L2 block */
#define HIT_MAX_INODES      (HIT_L1_ENTRIES * HIT_L2_ENTRIES * HIT_INODES_PER_BLOCK)
                                                /* Max ~4M inodes per volume */

typedef struct {
    UINT64  Magic;              /* HIT_HEADER_MAGIC */
    UINT64  TotalInodes;        /* Total inode slots */
    UINT64  FreeInodes;         /* Free slots */
    UINT64  L1BlockLBA;         /* Start LBA of L1 index */
    UINT64  L1Blocks;           /* Number of L1 blocks */
    UINT64  InodeDataLBA;       /* Start LBA of inode data blocks */
    UINT64  InodeDataBlocks;    /* Number of inode data blocks */
    UINT64  AllocSerial;        /* Monotonic allocation counter */
    UINT32  CRC32;              /* Header checksum */
    UINT8   Reserved[4];
} __attribute__((packed)) HFS_HIT_HEADER;

/* L1 index entry */
typedef struct {
    UINT64  InodeIDBase;        /* First inode ID in this range */
    UINT64  L2BlockLBA;         /* LBA of L2 block for this range */
    UINT32  Count;              /* Used entries in L2 block */
    UINT32  CRC32;
} __attribute__((packed)) HFS_HIT_L1_ENTRY;

/* L2 index entry */
typedef struct {
    UINT64  InodeID;            /* Inode ID */
    UINT64  InodeBlockLBA;      /* LBA of the inode data block */
    UINT16  InodeSlot;          /* Slot within that block (0-15) */
    UINT16  Flags;              /* Quick flags (type, deleted...) */
    UINT32  CRC32;
} __attribute__((packed)) HFS_HIT_L2_ENTRY;

/* =========================================================================
 * HFS Directory Entry
 * ========================================================================= */
#define HFS_DIRENTRY_MAGIC  0x44455246485344ULL  /* 'DSHFRED' */
#define HFS_MAX_FILENAME    255

typedef struct {
    UINT64  InodeID;            /* Inode ID of the target */
    UINT16  Type;               /* HFS_INODE_* */
    UINT16  NameLen;            /* Length of name in CHAR16 units */
    UINT32  CRC32;              /* CRC32 of this entry */
    CHAR16  Name[HFS_MAX_FILENAME + 1]; /* Null-terminated UTF-16 name */
} __attribute__((packed)) HFS_DIR_ENTRY;

/* =========================================================================
 * HFS Journal
 * ========================================================================= */
#define HFS_JOURNAL_MAGIC   0x4C4E524A534648ULL  /* 'HFSJRNL' */

typedef struct {
    UINT64  Magic;
    UINT64  Sequence;           /* Transaction sequence number */
    UINT64  HeadLBA;            /* Write head LBA */
    UINT64  TailLBA;            /* Read tail LBA */
    UINT64  JournalStartLBA;
    UINT64  JournalEndLBA;
    UINT32  CRC32;
    UINT32  Flags;
} __attribute__((packed)) HFS_JOURNAL_HEADER;

/* =========================================================================
 * Boot sector info passed to HDOS kernel
 * Describes the HFS volume where the OS is installed
 * ========================================================================= */
typedef struct {
    UINT64  SuperblockLBA;      /* LBA of HFS superblock */
    UINT64  HitLBA;             /* LBA of HFS Index Table */
    UINT64  RootInodeID;        /* Root directory inode ID */
    UINT64  KernelInodeID;      /* Inode ID of kernel file */
    UINT32  BlockSize;          /* Volume block size */
    UINT32  MediaId;            /* UEFI Block IO media ID */
    UINT8   VolumeUUID[16];     /* Volume UUID */
    UINT64  TotalBlocks;
    UINT64  FreeBlocks;
    UINT8   Valid;              /* 1 if HFS volume found */
    UINT8   Reserved[7];
} HFS_BOOT_INFO;
