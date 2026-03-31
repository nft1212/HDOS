/*
 * ============================================================================
 *  HBOOT — Home Boot Loader
 *  Copyright (C) 2024-2026 Winora Company. All rights reserved.
 *  File: src/gui_menu.c — Graphical Boot Menu
 *
 *  Renders a full graphical boot selection interface on the GOP framebuffer.
 *  Layout:
 *   [Header: Winora / HBOOT logo + version]
 *   [Left panel: Boot entries list]
 *   [Right panel: Entry details + system info]
 *   [Progress bar: countdown timer]
 *   [Footer: keyboard shortcuts]
 * ============================================================================
 */
#include "../include/hboot.h"

/* =========================================================================
 * Layout computed at runtime based on actual screen size
 * ========================================================================= */
static UINT32 sHdrH;   /* Header height in pixels */
static UINT32 sFtrH;   /* Footer height in pixels */
static UINT32 sMenuX;  /* Menu list X start */
static UINT32 sMenuY;  /* Menu list Y start */
static UINT32 sMenuW;  /* Menu list width */
static UINT32 sMenuH;  /* Menu list height (total) */
static UINT32 sInfoX;  /* Info panel X start */
static UINT32 sInfoY;
static UINT32 sInfoW;
static UINT32 sItemH;  /* Height of one menu item */

/* =========================================================================
 * Internal helpers
 * ========================================================================= */
static VOID GuiDrawHeader(VOID) {
    /* Background gradient: dark blue top -> slightly lighter */
    GfxDrawGradientV(0, 0, gFBWidth, sHdrH, COLOR_BG_DARK, COLOR_BG);

    /* Bottom border */
    GfxFillRect(0, sHdrH-2, gFBWidth, 2, COLOR_ACCENT);

    /* Logo text */
    UINT32 LogoX = 32;
    UINT32 LogoY = (sHdrH - FONT_H*2) / 2;

    /* Logo text "HBOOT" */
    CONST CHAR16 *Logo = L"HBOOT";
    UINT32 LX = LogoX;
    /* Draw logo at normal size (font is 8x16) */
    GfxDrawString(LX, LogoY, Logo, COLOR_ACCENT, 0xFFFFFFFF);

    /* Company name */
    CONST CHAR16 *Company = L"  Home Boot Loader - Winora Company";
    GfxDrawString(LogoX + FONT_W*7, LogoY + FONT_H/2, Company,
                  COLOR_GRAY, 0xFFFFFFFF);

    /* Version */
    CONST CHAR16 *Ver = L"v" HBOOT_VERSION_STR;
    UINT32 VW = GfxStringWidth(Ver);
    GfxDrawString(gFBWidth - VW - 32, sHdrH/2 - FONT_H/2,
                  Ver, COLOR_ACCENT_DIM, 0xFFFFFFFF);

    /* Copyright */
    CONST CHAR16 *Copy = L"(C) 2024-2026 Winora Company";
    UINT32 CW = GfxStringWidth(Copy);
    GfxDrawString(gFBWidth - CW - 32, sHdrH/2 + FONT_H/2,
                  Copy, COLOR_GRAY_DARK, 0xFFFFFFFF);
}

static VOID GuiDrawFooter(UINT32 Timeout, UINT32 Total) {
    UINT32 FY = gFBHeight - sFtrH;

    GfxFillRect(0, FY, gFBWidth, 2, COLOR_ACCENT);
    GfxFillRect(0, FY+2, gFBWidth, sFtrH-2, COLOR_BG_DARK);

    /* Key hints */
    UINT32 TX = 32;
    UINT32 TY = FY + (sFtrH - FONT_H) / 2;

    struct { CONST CHAR16 *Key; CONST CHAR16 *Label; } Hints[] = {
        { L"[↑↓]",  L" Select  " },
        { L"[Enter]",L" Boot    " },
        { L"[E]",   L" Edit    " },
        { L"[C]",   L" Console " },
        { L"[F5]",  L" Reload  " },
        { L"[Esc]", L" Default " },
        { NULL, NULL }
    };

    for (INT32 i = 0; Hints[i].Key; i++) {
        GfxDrawString(TX, TY, Hints[i].Key,   COLOR_ACCENT, COLOR_BG_DARK);
        TX += GfxStringWidth(Hints[i].Key) + 2;
        GfxDrawString(TX, TY, Hints[i].Label, COLOR_GRAY,   COLOR_BG_DARK);
        TX += GfxStringWidth(Hints[i].Label) + 8;
        if (TX > gFBWidth - 300) break;
    }

    /* Timer + progress bar on the right */
    if (Total > 0 && Timeout <= Total) {
        UINT32 BarW = 200;
        UINT32 BarH = 8;
        UINT32 BX   = gFBWidth - BarW - 32;
        UINT32 BY   = FY + 8;

        GfxDrawRect(BX-1, BY-1, BarW+2, BarH+2, COLOR_BORDER, 1);
        UINT32 Filled = BarW * Timeout / Total;
        GfxFillRect(BX, BY, Filled, BarH, COLOR_ACCENT);
        GfxFillRect(BX+Filled, BY, BarW-Filled, BarH, COLOR_BG);

        /* Countdown text */
        CHAR16 TBuf[32];
        UINT32 Pos = 0;
        TBuf[Pos++] = L'A'; TBuf[Pos++] = L'u'; TBuf[Pos++] = L't';
        TBuf[Pos++] = L'o'; TBuf[Pos++] = L' ';
        UINT64 V = (UINT64)Timeout;
        if (V == 0) { TBuf[Pos++] = L'0'; }
        else {
            CHAR16 Num[10]; INT32 Ni = 8; Num[9]=L'\0';
            while (V > 0) { Num[Ni--] = L'0' + (CHAR16)(V%10); V/=10; }
            CHAR16 *NP = &Num[Ni+1];
            while (*NP) TBuf[Pos++] = *NP++;
        }
        TBuf[Pos++] = L's'; TBuf[Pos] = L'\0';
        UINT32 TW2 = GfxStringWidth(TBuf);
        GfxDrawString(gFBWidth - TW2 - 32,
                      BY + BarH + 4, TBuf, COLOR_GRAY, 0xFFFFFFFF);
    }
}

static VOID GuiDrawEntry(UINT32 Idx, BOOLEAN Selected, BOOLEAN Hovered) {
    UINT32 ItemY = sMenuY + Idx * sItemH;
    UINT32 BgColor = COLOR_BG;

    if (Selected) {
        BgColor = COLOR_SELECTED_BG;
        /* Glow border */
        GfxDrawRect(sMenuX+1, ItemY+1, sMenuW-2, sItemH-2,
                    COLOR_ACCENT, 1);
    } else if (Hovered) {
        BgColor = GfxBlend(COLOR_BG, COLOR_SELECTED_BG, 100);
    }

    GfxFillRect(sMenuX+2, ItemY+2, sMenuW-4, sItemH-4, BgColor);

    /* Selection indicator */
    if (Selected) {
        GfxFillRect(sMenuX+2, ItemY + sItemH/2 - 4,
                    4, 8, COLOR_ACCENT);
    }

    /* Entry label */
    BOOT_ENTRY *E = &gConfig.Entries[Idx];
    UINT32 TY = ItemY + (sItemH - FONT_H*2 - 4) / 2;
    UINT32 FG = Selected ? COLOR_WHITE : COLOR_GRAY;

    GfxDrawString(sMenuX + 16, TY, E->Label, FG, 0xFFFFFFFF);

    /* Subtitle: kernel path (smaller, dimmer) */
    CHAR16 Short[48]; UINTN KL = StrLen16(E->KernelPath);
    CONST CHAR16 *KP = E->KernelPath;
    if (KL > 40) KP = KP + KL - 40;
    StrCopy16(Short, KP, 48);

    GfxDrawString(sMenuX + 16, TY + FONT_H + 4,
                  Short, COLOR_GRAY_DARK, 0xFFFFFFFF);

    /* Recovery badge */
    if (E->IsRecovery) {
        GfxFillRect(sMenuX + sMenuW - 72, ItemY + 6, 64, FONT_H+4, COLOR_RED);
        GfxDrawString(sMenuX + sMenuW - 68, ItemY + 8,
                      L"RCVRY", COLOR_WHITE, COLOR_RED);
    }

    /* Default badge */
    if (E->IsDefault && !E->IsRecovery) {
        GfxFillRect(sMenuX + sMenuW - 72, ItemY + 6, 64, FONT_H+4, COLOR_GREEN);
        GfxDrawString(sMenuX + sMenuW - 68, ItemY + 8,
                      L"DFLT ", COLOR_WHITE, COLOR_GREEN);
    }
}

static VOID GuiDrawInfoPanel(UINT32 Selected) {
    GfxFillRect(sInfoX, sInfoY, sInfoW, sMenuH, COLOR_BG);
    GfxDrawRect(sInfoX, sInfoY, sInfoW, sMenuH, COLOR_BORDER, 1);

    BOOT_ENTRY *E = &gConfig.Entries[Selected];
    UINT32 TX = sInfoX + 12;
    UINT32 TY = sInfoY + 12;

    /* Title */
    GfxDrawString(TX, TY, L"Boot Entry Details", COLOR_ACCENT, 0xFFFFFFFF);
    TY += FONT_H + 8;
    GfxFillRect(sInfoX+4, TY, sInfoW-8, 1, COLOR_BORDER);
    TY += 8;

    /* Fields */
    struct { CONST CHAR16 *Key; CONST CHAR16 *Val; } Fields[] = {
        { L"Label:  ", E->Label },
        { L"Kernel: ", E->KernelPath },
        { E->HasInitrd ? L"InitRD: " : NULL, E->InitrdPath },
        { L"CmdLine:", E->CmdLine },
        { NULL, NULL }
    };

    for (INT32 i = 0; Fields[i].Key; i++) {
        if (!Fields[i].Key) continue;
        GfxDrawString(TX, TY, Fields[i].Key, COLOR_ACCENT_DIM, 0xFFFFFFFF);
        /* Truncate long paths to fit panel */
        CHAR16 Trunc[40]; StrCopy16(Trunc, Fields[i].Val, 40);
        GfxDrawString(TX + FONT_W*9, TY, Trunc, COLOR_WHITE, 0xFFFFFFFF);
        TY += FONT_H + 6;
    }

    TY += 16;
    GfxFillRect(sInfoX+4, TY, sInfoW-8, 1, COLOR_BORDER);
    TY += 8;

    /* System info */
    GfxDrawString(TX, TY, L"System Info", COLOR_ACCENT, 0xFFFFFFFF);
    TY += FONT_H + 8;

    if (gBootInfo && gBootInfo->TotalRAMBytes) {
        GfxDrawString(TX, TY, L"RAM: ", COLOR_ACCENT_DIM, 0xFFFFFFFF);
        CHAR16 Buf[24]; UINT32 BP=0;
        UINT64 MB = gBootInfo->TotalRAMBytes / (1024*1024);
        UINT64 V = MB;
        CHAR16 Num[10]; INT32 Ni=8; Num[9]=L'\0';
        if (V==0) Num[Ni]=L'0';
        else while(V>0){Num[Ni--]=L'0'+(CHAR16)(V%10);V/=10;}
        CHAR16 *NP = &Num[Ni+1];
        while(*NP) Buf[BP++] = *NP++;
        Buf[BP++]=L' ';Buf[BP++]=L'M';Buf[BP++]=L'B';Buf[BP]=L'\0';
        GfxDrawString(TX + FONT_W*6, TY, Buf, COLOR_WHITE, 0xFFFFFFFF);
        TY += FONT_H + 4;
    }

    if (gBootInfo && gBootInfo->Framebuffer.Valid) {
        GfxDrawString(TX, TY, L"GOP: ", COLOR_ACCENT_DIM, 0xFFFFFFFF);
        CHAR16 ResStr[24];
        UINT32 Pos = 0;
        UINT64 W = gFBWidth, H = gFBHeight;
        CHAR16 TmpN[10]; INT32 Ti;
        /* Width */
        Ti=8;TmpN[9]=L'\0';while(W>0){TmpN[Ti--]=L'0'+(CHAR16)(W%10);W/=10;}
        CHAR16*TP=&TmpN[Ti+1]; while(*TP)ResStr[Pos++]=*TP++;
        ResStr[Pos++]=L'x';
        /* Height */
        Ti=8;TmpN[9]=L'\0';while(H>0){TmpN[Ti--]=L'0'+(CHAR16)(H%10);H/=10;}
        TP=&TmpN[Ti+1];while(*TP)ResStr[Pos++]=*TP++;
        ResStr[Pos]=L'\0';
        GfxDrawString(TX + FONT_W*6, TY, ResStr, COLOR_WHITE, 0xFFFFFFFF);
        TY += FONT_H + 4;
    }

    if (gBootInfo && (gBootInfo->Flags & BOOT_FLAG_HFS)) {
        GfxDrawString(TX, TY, L"FS:  ", COLOR_ACCENT_DIM, 0xFFFFFFFF);
        GfxDrawString(TX + FONT_W*6, TY, L"HFS v2.0", COLOR_GREEN, 0xFFFFFFFF);
        TY += FONT_H + 4;
    }

    if (gBootInfo && (gBootInfo->Flags & BOOT_FLAG_ACPI)) {
        GfxDrawString(TX, TY, L"ACPI:", COLOR_ACCENT_DIM, 0xFFFFFFFF);
        GfxDrawString(TX + FONT_W*6, TY, L"OK", COLOR_GREEN, 0xFFFFFFFF);
        TY += FONT_H + 4;
    }

    /* CPU brand */
    if (gBootInfo && gBootInfo->CPU.BrandString[0]) {
        CHAR16 CpuW[49];
        A2U(gBootInfo->CPU.BrandString, CpuW, 49);
        GfxDrawString(TX, TY, L"CPU: ", COLOR_ACCENT_DIM, 0xFFFFFFFF);
        /* Trim to fit */
        CHAR16 CpuShort[30]; StrCopy16(CpuShort, CpuW, 30);
        GfxDrawString(TX + FONT_W*6, TY, CpuShort, COLOR_WHITE, 0xFFFFFFFF);
    }
}

static VOID GuiDrawFrame(UINT32 Selected, UINT32 Timeout, UINT32 Total) {
    /* Background */
    GfxClear(COLOR_BG);

    GuiDrawHeader();
    GuiDrawFooter(Timeout, Total);

    /* Menu panel background */
    GfxFillRect(sMenuX, sMenuY, sMenuW, sMenuH, COLOR_BG);
    GfxDrawRect(sMenuX, sMenuY, sMenuW, sMenuH, COLOR_BORDER, 1);

    /* Menu title */
    CONST CHAR16 *MT = L"Select OS to Boot";
    UINT32 MTW = GfxStringWidth(MT);
    GfxDrawString(sMenuX + (sMenuW - MTW)/2, sMenuY - FONT_H - 8,
                  MT, COLOR_GRAY, 0xFFFFFFFF);

    for (UINT32 i = 0; i < gConfig.NumEntries; i++) {
        GuiDrawEntry(i, i == Selected, FALSE);
    }

    GuiDrawInfoPanel(Selected);
}

/*
 * Note: sFontData_Get is defined locally here as a stub.
 * The actual font rendering is done via GfxDrawChar (which references
 * sFontData in gfx.c). The 2x logo in the header uses standard GfxDrawChar.
 */

/* =========================================================================
 * GuiMenu — main graphical menu entry point
 * ========================================================================= */
EFI_STATUS GuiMenu(VOID) {
    /* Compute layout */
    sHdrH   = 80;
    sFtrH   = 50;
    sMenuX  = 40;
    sMenuY  = sHdrH + 40;
    sMenuW  = (gFBWidth * 55) / 100;   /* 55% of screen */
    sItemH  = 56;
    sMenuH  = (UINT32)gConfig.NumEntries * sItemH + 8;
    sInfoX  = sMenuX + sMenuW + 20;
    sInfoY  = sMenuY;
    sInfoW  = gFBWidth - sInfoX - 40;

    UINT32 Selected = gConfig.DefaultEntry;
    if (Selected >= gConfig.NumEntries) Selected = 0;

    UINT32 Timeout = gConfig.TimeoutSec;
    UINT32 Total   = gConfig.TimeoutSec;
    BOOLEAN TimerActive = (Total > 0);

    GuiDrawFrame(Selected, Timeout, Total);

    /* Event loop */
    while (TRUE) {
        EFI_INPUT_KEY Key;

        if (TimerActive) {
            /* Poll for 1 second in 100ms increments */
            BOOLEAN KeyHit = FALSE;
            for (INT32 t = 0; t < 10; t++) {
                gBS->Stall(100000); /* 100ms */
                if (PollKey(&Key)) { KeyHit = TRUE; break; }
            }
            if (KeyHit) {
                TimerActive = FALSE;
                goto ProcessKey;
            }
            if (Timeout > 0) Timeout--;
            GuiDrawFooter(Timeout, Total);
            if (Timeout == 0) break; /* Auto boot */
            continue;
        }

        /* Blocking wait */
        {
            UINTN Idx;
            gBS->WaitForEvent(1, &gST->ConIn->WaitForKey, &Idx);
            gST->ConIn->ReadKeyStroke(gST->ConIn, &Key);
        }

ProcessKey:;
        UINT32 OldSel = Selected;

        if (Key.ScanCode == SCAN_UP) {
            if (Selected > 0) Selected--;
            else Selected = gConfig.NumEntries - 1;
        } else if (Key.ScanCode == SCAN_DOWN) {
            Selected = (Selected + 1) % gConfig.NumEntries;
        } else if (Key.ScanCode == SCAN_F5) {
            GuiDrawFrame(Selected, Timeout, Total);
            continue;
        } else if (Key.UnicodeChar == CHAR_CR) {
            break;  /* Boot selected */
        } else if (Key.ScanCode == SCAN_ESC || Key.UnicodeChar == CHAR_ESC) {
            Selected = gConfig.DefaultEntry;
            break;
        } else if (Key.UnicodeChar == L'c' || Key.UnicodeChar == L'C') {
            /* Drop to text console */
            gConfig.UseGUI = FALSE;
            GfxClear(COLOR_BG_DARK);
            break;
        }

        if (Selected != OldSel) {
            GuiDrawEntry(OldSel, FALSE, FALSE);
            GuiDrawEntry(Selected, TRUE, FALSE);
            GuiDrawInfoPanel(Selected);
        }
    }

    /* Apply selection */
    BOOT_ENTRY *E = &gConfig.Entries[Selected];
    StrCopy16(gConfig.KernelPath, E->KernelPath, MAX_PATH_LEN);
    StrCopy16(gConfig.InitrdPath, E->InitrdPath, MAX_PATH_LEN);
    StrCopy16(gConfig.CmdLine,    E->CmdLine,    MAX_CMDLINE_LEN);
    gConfig.HasInitrd    = E->HasInitrd;
    gConfig.DefaultEntry = Selected;

    /* Boot screen */
    GfxClear(COLOR_BG_DARK);
    CONST CHAR16 *BMsg = L"Booting HDOS...";
    UINT32 BMW = GfxStringWidth(BMsg);
    GfxDrawString((gFBWidth-BMW)/2, gFBHeight/2 - FONT_H,
                  BMsg, COLOR_WHITE, 0xFFFFFFFF);
    GfxDrawString((gFBWidth - GfxStringWidth(E->Label))/2,
                  gFBHeight/2 + FONT_H,
                  E->Label, COLOR_ACCENT, 0xFFFFFFFF);

    /* Brief loading bar animation */
    UINT32 LBW = gFBWidth / 2;
    UINT32 LBX = (gFBWidth - LBW) / 2;
    UINT32 LBY = gFBHeight/2 + FONT_H*3;
    GfxDrawRect(LBX-1, LBY-1, LBW+2, 8+2, COLOR_BORDER, 1);
    for (UINT32 p = 0; p <= LBW; p += LBW/20) {
        GfxFillRect(LBX, LBY, p, 8, COLOR_ACCENT);
        gBS->Stall(30000); /* 30ms per step = ~600ms total */
    }
    GfxFillRect(LBX, LBY, LBW, 8, COLOR_ACCENT);

    return EFI_SUCCESS;
}
