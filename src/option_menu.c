#include "global.h"
#include "archipelago.h"
#include "option_menu.h"
#include "bg.h"
#include "gpu_regs.h"
#include "international_string_util.h"
#include "main.h"
#include "menu.h"
#include "palette.h"
#include "scanline_effect.h"
#include "sprite.h"
#include "strings.h"
#include "string_util.h"
#include "task.h"
#include "text.h"
#include "text_window.h"
#include "util.h"
#include "window.h"
#include "gba/m4a_internal.h"
#include "constants/rgb.h"

#define tMenuSelection data[0]
#define tOptions1 data[1]
#define tOptions2 data[2]

// page 1
enum
{
    MENUITEM_TEXTSPEED,
    MENUITEM_BATTLESCENE,
    MENUITEM_BATTLESTYLE,
    MENUITEM_SOUND,
    MENUITEM_BUTTONMODE,
    MENUITEM_FRAMETYPE,
    MENUITEM_CANCEL,
    MENUITEM_COUNT,
};
// page 2
enum
{
    MENUITEM_TURBOBUTTON,
    MENUITEM_SKIPNICKNAME,
    MENUITEM_AUTORUN,
    MENUITEM_BIKEMUSIC,
    MENUITEM_SURFMUSIC,
    MENUITEM_LOWHPBEEP,
    MENUITEM_CANCEL_PG2,
    MENUITEM_COUNT_PG2,
};
// page 3
enum
{
    MENUITEM_DEATHLINK,
    MENUITEM_BLINDTRAINERS,
    MENUITEM_ALWAYSCATCH,
    MENUITEM_ALWAYSESCAPE,
    MENUITEM_ALWAYSREEL,
    MENUITEM_APMESSAGES,
    MENUITEM_CANCEL_PG3,
    MENUITEM_COUNT_PG3,
};

enum
{
    WIN_HEADER,
    WIN_OPTIONS
};

struct MenuOptions
{
/*tOptions1*/ u8 buttonMode:2;
              u8 textSpeed:2;
              u8 sound:1;
              u8 battleStyle:1;
              u8 battleSceneOff:1;
              u8 skipFanfares:1;
              u8 windowFrameType:5;
              u8 turboButton:2;
              u8 bikeMusic:1;
/*tOptions2*/ u8 surfMusic:1;
              u8 lowHpBeep:1;
              u8 skipNicknames:1;
              u8 receivedItemMessageFilter:2;
              u8 deathLink:1;
              u8 blindTrainers:1;
              u8 autoRun:1;
              u8 guaranteedReelFish:1;
              u8 guaranteedCatch:1;
              u8 guaranteedRun:1;
};

// page 1
#define MENUITEMDATA_TEXTSPEED   ((gTasks[taskId].tOptions1 & 12) >> 2)
#define MENUITEMDATA_BATTLESCENE ((gTasks[taskId].tOptions1 & gBitTable[6]) >> 6)
#define MENUITEMDATA_BATTLESTYLE ((gTasks[taskId].tOptions1 & gBitTable[5]) >> 5)
#define MENUITEMDATA_SOUND       ((gTasks[taskId].tOptions1 & gBitTable[4]) >> 4)
#define MENUITEMDATA_BUTTONMODE  (gTasks[taskId].tOptions1 & 3)
#define MENUITEMDATA_FRAMETYPE   ((gTasks[taskId].tOptions1 & 7936) >> 8)
// page 2
#define MENUITEMDATA_TURBOBUTTON  ((gTasks[taskId].tOptions1 & 24576) >> 13)
#define MENUITEMDATA_SKIPNICKNAME ((gTasks[taskId].tOptions2 & gBitTable[1]) >> 1)
#define MENUITEMDATA_AUTORUN      ((gTasks[taskId].tOptions2 & gBitTable[6]) >> 6)
#define MENUITEMDATA_BIKEMUSIC    ((gTasks[taskId].tOptions1 & gBitTable[15]) >> 15)
#define MENUITEMDATA_SURFMUSIC    ((gTasks[taskId].tOptions2 & gBitTable[10]) >> 10)
#define MENUITEMDATA_LOWHPBEEP    (gTasks[taskId].tOptions2 & gBitTable[0])
// page 3
#define MENUITEMDATA_DEATHLINK     ((gTasks[taskId].tOptions2 & gBitTable[4]) >> 4)
#define MENUITEMDATA_BLINDTRAINERS ((gTasks[taskId].tOptions2 & gBitTable[5]) >> 5)
#define MENUITEMDATA_ALWAYSCATCH   ((gTasks[taskId].tOptions2 & gBitTable[8]) >> 8)
#define MENUITEMDATA_ALWAYSESCAPE  ((gTasks[taskId].tOptions2 & gBitTable[9]) >> 9)
#define MENUITEMDATA_ALWAYSREEL    ((gTasks[taskId].tOptions2 & gBitTable[7]) >> 7)
#define MENUITEMDATA_APMESSAGES    ((gTasks[taskId].tOptions2 & 12) >> 2)

// page 1
#define YPOS_TEXTSPEED     (MENUITEM_TEXTSPEED * 16)
#define YPOS_BATTLESCENE   (MENUITEM_BATTLESCENE * 16)
#define YPOS_BATTLESTYLE   (MENUITEM_BATTLESTYLE * 16)
#define YPOS_SOUND         (MENUITEM_SOUND * 16)
#define YPOS_BUTTONMODE    (MENUITEM_BUTTONMODE * 16)
#define YPOS_FRAMETYPE     (MENUITEM_FRAMETYPE * 16)
// page 2
#define YPOS_TURBOBUTTON   (MENUITEM_TURBOBUTTON * 16)
#define YPOS_SKIPNICKNAME  (MENUITEM_SKIPNICKNAME * 16)
#define YPOS_AUTORUN       (MENUITEM_AUTORUN * 16)
#define YPOS_BIKEMUSIC     (MENUITEM_BIKEMUSIC * 16)
#define YPOS_SURFMUSIC     (MENUITEM_SURFMUSIC * 16)
#define YPOS_LOWHPBEEP     (MENUITEM_LOWHPBEEP * 16)
// page 3
#define YPOS_DEATHLINK     (MENUITEM_DEATHLINK * 16)
#define YPOS_BLINDTRAINERS (MENUITEM_BLINDTRAINERS * 16)
#define YPOS_ALWAYSCATCH   (MENUITEM_ALWAYSCATCH * 16)
#define YPOS_ALWAYSESCAPE  (MENUITEM_ALWAYSESCAPE * 16)
#define YPOS_ALWAYSREEL    (MENUITEM_ALWAYSREEL * 16)
#define YPOS_APMESSAGES    (MENUITEM_APMESSAGES * 16)

#define PAGE_COUNT 3

static void Task_OptionMenuFadeIn(u8 taskId);
static void Task_OptionMenuProcessInput(u8 taskId);
static void Task_OptionMenuFadeIn_Pg2(u8 taskId);
static void Task_OptionMenuProcessInput_Pg2(u8 taskId);
static void Task_OptionMenuFadeIn_Pg3(u8 taskId);
static void Task_OptionMenuProcessInput_Pg3(u8 taskId);
static void Task_OptionMenuSave(u8 taskId);
static void Task_OptionMenuFadeOut(u8 taskId);
static void HighlightOptionMenuItem(u8 selection);
static u8 XOptions_ProcessInput(u8 optionCount, u8 selection);
static u8 XOptionsChallengeMode_ProcessInput(u8 optionCount, u8 selection);
static u8 TwoOptions_ProcessInput(u8 selection);
static u8 TwoOptionsChallengeMode_ProcessInput(u8 selection);
static u8 ThreeOptions_ProcessInput(u8 selection);
static u8 FourOptions_ProcessInput(u8 selection);
static void OnOff_DrawChoices(u8 yPos, u8 selection);
static void OnOffChallengeMode_DrawChoices(u8 yPos, u8 selection);
static u8 TextSpeed_ProcessInput(u8 selection);
static void TextSpeed_DrawChoices(u8 selection);
static u8 BattleScene_ProcessInput(u8 selection);
static void BattleScene_DrawChoices(u8 selection);
static u8 BattleStyle_ProcessInput(u8 selection);
static void BattleStyle_DrawChoices(u8 selection);
static u8 Sound_ProcessInput(u8 selection);
static void Sound_DrawChoices(u8 selection);
static u8 FrameType_ProcessInput(u8 selection);
static void FrameType_DrawChoices(u8 selection);
static u8 ButtonMode_ProcessInput(u8 selection);
static void ButtonMode_DrawChoices(u8 selection);
static u8 TurboButton_ProcessInput(u8 selection);
static void TurboButton_DrawChoices(u8 selection);
static u8 SkipNickname_ProcessInput(u8 selection);
static void SkipNickname_DrawChoices(u8 selection);
static u8 AutoRun_ProcessInput(u8 selection);
static void AutoRun_DrawChoices(u8 selection);
static u8 BikeMusic_ProcessInput(u8 selection);
static void BikeMusic_DrawChoices(u8 selection);
static u8 SurfMusic_ProcessInput(u8 selection);
static void SurfMusic_DrawChoices(u8 selection);
static u8 LowHpBeep_ProcessInput(u8 selection);
static void LowHpBeep_DrawChoices(u8 selection);
static u8 DeathLink_ProcessInput(u8 selection);
static void DeathLink_DrawChoices(u8 selection);
static u8 BlindTrainers_ProcessInput(u8 selection);
static void BlindTrainers_DrawChoices(u8 selection);
static u8 AlwaysCatch_ProcessInput(u8 selection);
static void AlwaysCatch_DrawChoices(u8 selection);
static u8 AlwaysEscape_ProcessInput(u8 selection);
static void AlwaysEscape_DrawChoices(u8 selection);
static u8 AlwaysReel_ProcessInput(u8 selection);
static void AlwaysReel_DrawChoices(u8 selection);
static u8 ApMessages_ProcessInput(u8 selection);
static void ApMessages_DrawChoices(u8 selection);
static u16 packMenuOptions1(void);
static u16 packMenuOptions2(void);
static void DrawHeaderText(void);
static void DrawOptionMenuTexts(void);
static void DrawBgWindowFrames(void);

EWRAM_DATA static bool8 sArrowPressed = FALSE;
EWRAM_DATA static u8 sCurrPage = 0;

static const u16 sOptionMenuText_Pal[] = INCBIN_U16("graphics/interface/option_menu_text.gbapal");
// note: this is only used in the Japanese release
static const u8 sEqualSignGfx[] = INCBIN_U8("graphics/interface/option_menu_equals_sign.4bpp");

static const u8 *const sOptionMenuItemsNames[MENUITEM_COUNT] =
{
    [MENUITEM_TEXTSPEED]   = gText_TextSpeed,
    [MENUITEM_BATTLESCENE] = gText_BattleScene,
    [MENUITEM_BATTLESTYLE] = gText_BattleStyle,
    [MENUITEM_SOUND]       = gText_Sound,
    [MENUITEM_BUTTONMODE]  = gText_ButtonMode,
    [MENUITEM_FRAMETYPE]   = gText_Frame,
    [MENUITEM_CANCEL]      = gText_OptionMenuCancel,
};

static const u8 *const sOptionMenuItemsNames_Pg2[MENUITEM_COUNT_PG2] =
{
    [MENUITEM_TURBOBUTTON] =  gText_TurboButton,
    [MENUITEM_SKIPNICKNAME] = gText_SkipNickname,
    [MENUITEM_AUTORUN] =      gText_AutoRun,
    [MENUITEM_BIKEMUSIC] =    gText_BikeMusic,
    [MENUITEM_SURFMUSIC] =    gText_SurfMusic,
    [MENUITEM_LOWHPBEEP] =    gText_LowHpBeep,
    [MENUITEM_CANCEL_PG2] =   gText_OptionMenuCancel,
};

static const u8 *const sOptionMenuItemsNames_Pg3[MENUITEM_COUNT_PG3] =
{
    [MENUITEM_DEATHLINK] =     gText_DeathLink,
    [MENUITEM_BLINDTRAINERS] = gText_BlindTrainers,
    [MENUITEM_ALWAYSCATCH] =   gText_AlwaysCatch,
    [MENUITEM_ALWAYSESCAPE] =  gText_AlwaysEscape,
    [MENUITEM_ALWAYSREEL] =    gText_AlwaysReel,
    [MENUITEM_APMESSAGES] =    gText_ApMessages,
    [MENUITEM_CANCEL_PG3] =    gText_OptionMenuCancel,
};

static const struct WindowTemplate sOptionMenuWinTemplates[] =
{
    [WIN_HEADER] = {
        .bg = 1,
        .tilemapLeft = 2,
        .tilemapTop = 1,
        .width = 26,
        .height = 2,
        .paletteNum = 1,
        .baseBlock = 2
    },
    [WIN_OPTIONS] = {
        .bg = 0,
        .tilemapLeft = 2,
        .tilemapTop = 5,
        .width = 26,
        .height = 14,
        .paletteNum = 1,
        .baseBlock = 0x36
    },
    DUMMY_WIN_TEMPLATE
};

static const struct BgTemplate sOptionMenuBgTemplates[] =
{
    {
        .bg = 1,
        .charBaseIndex = 1,
        .mapBaseIndex = 30,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 0,
        .baseTile = 0
    },
    {
        .bg = 0,
        .charBaseIndex = 1,
        .mapBaseIndex = 31,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 1,
        .baseTile = 0
    }
};

static const u16 sOptionMenuBg_Pal[] = {RGB(17, 18, 31)};

static void MainCB2(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void VBlankCB(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static void ReadCurrentOptions(u8 taskId)
{
    gTasks[taskId].tMenuSelection = 0;
    if (!gTasks[taskId].tOptions1)
        gTasks[taskId].tOptions1 = packMenuOptions1();
    if (!gTasks[taskId].tOptions2)
        gTasks[taskId].tOptions2 = packMenuOptions2();
}

static void DrawOptionsPg1(u8 taskId)
{
    ReadCurrentOptions(taskId);
    TextSpeed_DrawChoices(MENUITEMDATA_TEXTSPEED);
    BattleScene_DrawChoices(MENUITEMDATA_BATTLESCENE);
    BattleStyle_DrawChoices(MENUITEMDATA_BATTLESTYLE);
    Sound_DrawChoices(MENUITEMDATA_SOUND);
    ButtonMode_DrawChoices(MENUITEMDATA_BUTTONMODE);
    FrameType_DrawChoices(MENUITEMDATA_FRAMETYPE);
    HighlightOptionMenuItem(gTasks[taskId].tMenuSelection);
    CopyWindowToVram(WIN_OPTIONS, COPYWIN_FULL);
}

static void DrawOptionsPg2(u8 taskId)
{
    ReadCurrentOptions(taskId);
    TurboButton_DrawChoices(MENUITEMDATA_TURBOBUTTON);
    OnOff_DrawChoices(YPOS_SKIPNICKNAME, MENUITEMDATA_SKIPNICKNAME);
    OnOff_DrawChoices(YPOS_AUTORUN, MENUITEMDATA_AUTORUN);
    OnOff_DrawChoices(YPOS_BIKEMUSIC, MENUITEMDATA_BIKEMUSIC);
    OnOff_DrawChoices(YPOS_SURFMUSIC, MENUITEMDATA_SURFMUSIC);
    OnOff_DrawChoices(YPOS_LOWHPBEEP, MENUITEMDATA_LOWHPBEEP);
    HighlightOptionMenuItem(gTasks[taskId].tMenuSelection);
    CopyWindowToVram(WIN_OPTIONS, COPYWIN_FULL);
}

static void DrawOptionsPg3(u8 taskId)
{
    ReadCurrentOptions(taskId);
    OnOff_DrawChoices(YPOS_DEATHLINK, MENUITEMDATA_DEATHLINK);
    OnOffChallengeMode_DrawChoices(YPOS_BLINDTRAINERS, MENUITEMDATA_BLINDTRAINERS);
    OnOffChallengeMode_DrawChoices(YPOS_ALWAYSCATCH, MENUITEMDATA_ALWAYSCATCH);
    OnOffChallengeMode_DrawChoices(YPOS_ALWAYSESCAPE, MENUITEMDATA_ALWAYSESCAPE);
    OnOffChallengeMode_DrawChoices(YPOS_ALWAYSREEL, MENUITEMDATA_ALWAYSREEL);
    ApMessages_DrawChoices(MENUITEMDATA_APMESSAGES);
    HighlightOptionMenuItem(gTasks[taskId].tMenuSelection);
    CopyWindowToVram(WIN_OPTIONS, COPYWIN_FULL);
}

void CB2_InitOptionMenu(void)
{
    u8 taskId;
    switch (gMain.state)
    {
    default:
    case 0:
        SetVBlankCallback(NULL);
        gMain.state++;
        break;
    case 1:
        DmaClearLarge16(3, (void *)(VRAM), VRAM_SIZE, 0x1000);
        DmaClear32(3, OAM, OAM_SIZE);
        DmaClear16(3, PLTT, PLTT_SIZE);
        SetGpuReg(REG_OFFSET_DISPCNT, 0);
        ResetBgsAndClearDma3BusyFlags(0);
        InitBgsFromTemplates(0, sOptionMenuBgTemplates, ARRAY_COUNT(sOptionMenuBgTemplates));
        ChangeBgX(0, 0, BG_COORD_SET);
        ChangeBgY(0, 0, BG_COORD_SET);
        ChangeBgX(1, 0, BG_COORD_SET);
        ChangeBgY(1, 0, BG_COORD_SET);
        ChangeBgX(2, 0, BG_COORD_SET);
        ChangeBgY(2, 0, BG_COORD_SET);
        ChangeBgX(3, 0, BG_COORD_SET);
        ChangeBgY(3, 0, BG_COORD_SET);
        InitWindows(sOptionMenuWinTemplates);
        DeactivateAllTextPrinters();
        SetGpuReg(REG_OFFSET_WIN0H, 0);
        SetGpuReg(REG_OFFSET_WIN0V, 0);
        SetGpuReg(REG_OFFSET_WININ, WININ_WIN0_BG0);
        SetGpuReg(REG_OFFSET_WINOUT, WINOUT_WIN01_BG0 | WINOUT_WIN01_BG1 | WINOUT_WIN01_CLR);
        SetGpuReg(REG_OFFSET_BLDCNT, BLDCNT_TGT1_BG0 | BLDCNT_EFFECT_DARKEN);
        SetGpuReg(REG_OFFSET_BLDALPHA, 0);
        SetGpuReg(REG_OFFSET_BLDY, 4);
        SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_WIN0_ON | DISPCNT_OBJ_ON | DISPCNT_OBJ_1D_MAP);
        ShowBg(0);
        ShowBg(1);
        gMain.state++;
        break;
    case 2:
        ResetPaletteFade();
        ScanlineEffect_Stop();
        ResetTasks();
        ResetSpriteData();
        gMain.state++;
        break;
    case 3:
        LoadBgTiles(1, GetWindowFrameTilesPal(gSaveBlock2Ptr->optionsWindowFrameType)->tiles, 0x120, 0x1A2);
        gMain.state++;
        break;
    case 4:
        LoadPalette(sOptionMenuBg_Pal, BG_PLTT_ID(0), sizeof(sOptionMenuBg_Pal));
        LoadPalette(GetWindowFrameTilesPal(gSaveBlock2Ptr->optionsWindowFrameType)->pal, BG_PLTT_ID(7), PLTT_SIZE_4BPP);
        gMain.state++;
        break;
    case 5:
        LoadPalette(sOptionMenuText_Pal, BG_PLTT_ID(1), sizeof(sOptionMenuText_Pal));
        gMain.state++;
        break;
    case 6:
        PutWindowTilemap(WIN_HEADER);
        DrawHeaderText();
        gMain.state++;
        break;
    case 7:
        gMain.state++;
        break;
    case 8:
        PutWindowTilemap(WIN_OPTIONS);
        DrawOptionMenuTexts();
        gMain.state++;
    case 9:
        DrawBgWindowFrames();
        gMain.state++;
        break;
    case 10:
    {
        switch (sCurrPage)
        {
            case 0:
                taskId = CreateTask(Task_OptionMenuFadeIn, 0);
                DrawOptionsPg1(taskId);
                break;
            case 1:
                taskId = CreateTask(Task_OptionMenuFadeIn_Pg2, 0);
                DrawOptionsPg2(taskId);
                break;
            case 2:
                taskId = CreateTask(Task_OptionMenuFadeIn_Pg3, 0);
                DrawOptionsPg3(taskId);
                break;
        }
        gMain.state++;
        break;
    }
    case 11:
        BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
        SetVBlankCallback(VBlankCB);
        SetMainCallback2(MainCB2);
        return;
    }
}

static u8 Process_ChangePage(u8 currentPage)
{
    if (JOY_NEW(R_BUTTON))
    {
        if (currentPage < PAGE_COUNT - 1)
            currentPage++;
        else
            currentPage = 0;
    }
    if (JOY_NEW(L_BUTTON))
    {
        if (currentPage != 0)
            currentPage--;
        else
            currentPage = PAGE_COUNT - 1;
    }
    return currentPage;
}

static void Task_ChangePage(u8 taskId)
{
    DrawHeaderText();
    PutWindowTilemap(WIN_OPTIONS);
    DrawOptionMenuTexts();
    switch (sCurrPage)
    {
        case 0:
            DrawOptionsPg1(taskId);
            gTasks[taskId].func = Task_OptionMenuFadeIn;
            break;
        case 1:
            DrawOptionsPg2(taskId);
            gTasks[taskId].func = Task_OptionMenuFadeIn_Pg2;
            break;
        case 2:
            DrawOptionsPg3(taskId);
            gTasks[taskId].func = Task_OptionMenuFadeIn_Pg3;
            break;
    }
}

static void Task_OptionMenuFadeIn(u8 taskId)
{
    if (!gPaletteFade.active)
        gTasks[taskId].func = Task_OptionMenuProcessInput;
}

static void Task_OptionMenuProcessInput(u8 taskId)
{
    if (JOY_NEW(R_BUTTON) || JOY_NEW(L_BUTTON))
    {
        FillWindowPixelBuffer(WIN_OPTIONS, PIXEL_FILL(1));
        ClearStdWindowAndFrame(WIN_OPTIONS, FALSE);
        sCurrPage = Process_ChangePage(sCurrPage);
        gTasks[taskId].func = Task_ChangePage;
    }
    else if (JOY_NEW(A_BUTTON))
    {
        if (gTasks[taskId].tMenuSelection == MENUITEM_CANCEL)
            gTasks[taskId].func = Task_OptionMenuSave;
    }
    else if (JOY_NEW(B_BUTTON))
    {
        gTasks[taskId].func = Task_OptionMenuSave;
    }
    else if (JOY_NEW(DPAD_UP))
    {
        if (gTasks[taskId].tMenuSelection > 0)
            gTasks[taskId].tMenuSelection--;
        else
            gTasks[taskId].tMenuSelection = MENUITEM_CANCEL;
        HighlightOptionMenuItem(gTasks[taskId].tMenuSelection);
    }
    else if (JOY_NEW(DPAD_DOWN))
    {
        if (gTasks[taskId].tMenuSelection < MENUITEM_CANCEL)
            gTasks[taskId].tMenuSelection++;
        else
            gTasks[taskId].tMenuSelection = 0;
        HighlightOptionMenuItem(gTasks[taskId].tMenuSelection);
    }
    else
    {
        u8 previousOption, newOption;

        switch (gTasks[taskId].tMenuSelection)
        {
        case MENUITEM_TEXTSPEED:
            previousOption = MENUITEMDATA_TEXTSPEED;
            newOption = ThreeOptions_ProcessInput(MENUITEMDATA_TEXTSPEED);
            
            if (newOption != previousOption)
            {
                gTasks[taskId].tOptions1 &= ~(gBitTable[2] | gBitTable[3]);
                gTasks[taskId].tOptions1 |= (newOption << 2);
                TextSpeed_DrawChoices(MENUITEMDATA_TEXTSPEED);
            }
            break;
        case MENUITEM_BATTLESCENE:
            previousOption = MENUITEMDATA_BATTLESCENE;
            newOption = TwoOptions_ProcessInput(MENUITEMDATA_BATTLESCENE);

            if (newOption != previousOption)
            {
                gTasks[taskId].tOptions1 &= ~(gBitTable[6]);
                gTasks[taskId].tOptions1 |= (newOption << 6);
                BattleScene_DrawChoices(MENUITEMDATA_BATTLESCENE);
            }
            break;
        case MENUITEM_BATTLESTYLE:
            previousOption = MENUITEMDATA_BATTLESTYLE;
            newOption = TwoOptions_ProcessInput(MENUITEMDATA_BATTLESTYLE);

            if (newOption != previousOption)
            {
                gTasks[taskId].tOptions1 &= ~(gBitTable[5]);
                gTasks[taskId].tOptions1 |= (newOption << 5);
                BattleStyle_DrawChoices(MENUITEMDATA_BATTLESTYLE);
            }
            break;
        case MENUITEM_SOUND:
            previousOption = MENUITEMDATA_SOUND;
            newOption = TwoOptions_ProcessInput(MENUITEMDATA_SOUND);

            if (newOption != previousOption)
            {
                gTasks[taskId].tOptions1 &= ~(gBitTable[4]);
                gTasks[taskId].tOptions1 |= (newOption << 4);
                Sound_DrawChoices(MENUITEMDATA_SOUND);
            }
            break;
        case MENUITEM_BUTTONMODE:
            previousOption = MENUITEMDATA_BUTTONMODE;
            newOption = ThreeOptions_ProcessInput(MENUITEMDATA_BUTTONMODE);

            if (newOption != previousOption)
            {
                gTasks[taskId].tOptions1 &= ~(gBitTable[0] | gBitTable[1]);
                gTasks[taskId].tOptions1 |= newOption;
                ButtonMode_DrawChoices(MENUITEMDATA_BUTTONMODE);
            }
            break;
        case MENUITEM_FRAMETYPE:
            previousOption = MENUITEMDATA_FRAMETYPE;
            newOption = FrameType_ProcessInput(MENUITEMDATA_FRAMETYPE);

            if (newOption != previousOption)
            {
                gTasks[taskId].tOptions1 &= ~(gBitTable[8] | gBitTable[9] | gBitTable[10] | gBitTable[11] | gBitTable[12]);
                gTasks[taskId].tOptions1 |= (newOption << 8);
                FrameType_DrawChoices(MENUITEMDATA_FRAMETYPE);
            }
            break;
        default:
            return;
        }

        if (sArrowPressed)
        {
            sArrowPressed = FALSE;
            CopyWindowToVram(WIN_OPTIONS, COPYWIN_GFX);
        }
    }
}

static void Task_OptionMenuFadeIn_Pg2(u8 taskId)
{
    if (!gPaletteFade.active)
        gTasks[taskId].func = Task_OptionMenuProcessInput_Pg2;
}

static void Task_OptionMenuProcessInput_Pg2(u8 taskId)
{
    if (JOY_NEW(R_BUTTON) || JOY_NEW(L_BUTTON))
    {
        FillWindowPixelBuffer(WIN_OPTIONS, PIXEL_FILL(1));
        ClearStdWindowAndFrame(WIN_OPTIONS, FALSE);
        sCurrPage = Process_ChangePage(sCurrPage);
        gTasks[taskId].func = Task_ChangePage;
    }
    else if (JOY_NEW(A_BUTTON))
    {
        if (gTasks[taskId].tMenuSelection == MENUITEM_CANCEL_PG2)
            gTasks[taskId].func = Task_OptionMenuSave;
    }
    else if (JOY_NEW(B_BUTTON))
    {
        gTasks[taskId].func = Task_OptionMenuSave;
    }
    else if (JOY_NEW(DPAD_UP))
    {
        if (gTasks[taskId].tMenuSelection > 0)
            gTasks[taskId].tMenuSelection--;
        else
            gTasks[taskId].tMenuSelection = MENUITEM_CANCEL_PG2;
        HighlightOptionMenuItem(gTasks[taskId].tMenuSelection);
    }
    else if (JOY_NEW(DPAD_DOWN))
    {
        if (gTasks[taskId].tMenuSelection < MENUITEM_CANCEL_PG2)
            gTasks[taskId].tMenuSelection++;
        else
            gTasks[taskId].tMenuSelection = 0;
        HighlightOptionMenuItem(gTasks[taskId].tMenuSelection);
    }
    else
    {
        u8 previousOption, newOption;

        switch (gTasks[taskId].tMenuSelection)
        {
        case MENUITEM_TURBOBUTTON:
            previousOption = MENUITEMDATA_TURBOBUTTON;
            newOption = ThreeOptions_ProcessInput(MENUITEMDATA_TURBOBUTTON);
            
            if (newOption != previousOption)
            {
                gTasks[taskId].tOptions1 &= ~(gBitTable[13] | gBitTable[14]);
                gTasks[taskId].tOptions1 |= (newOption << 13);
                TurboButton_DrawChoices(MENUITEMDATA_TURBOBUTTON);
            }
            break;
        case MENUITEM_SKIPNICKNAME:
            previousOption = MENUITEMDATA_SKIPNICKNAME;
            newOption = TwoOptions_ProcessInput(MENUITEMDATA_SKIPNICKNAME);

            if (newOption != previousOption)
            {
                gTasks[taskId].tOptions2 &= ~(gBitTable[1]);
                gTasks[taskId].tOptions2 |= (newOption << 1);
                OnOff_DrawChoices(YPOS_SKIPNICKNAME, MENUITEMDATA_SKIPNICKNAME);
            }
            break;
        case MENUITEM_AUTORUN:
            previousOption = MENUITEMDATA_AUTORUN;
            newOption = TwoOptions_ProcessInput(MENUITEMDATA_AUTORUN);

            if (newOption != previousOption)
            {
                gTasks[taskId].tOptions2 &= ~(gBitTable[6]);
                gTasks[taskId].tOptions2 |= (newOption << 6);
                OnOff_DrawChoices(YPOS_AUTORUN, MENUITEMDATA_AUTORUN);
            }
            break;
        case MENUITEM_BIKEMUSIC:
            previousOption = MENUITEMDATA_BIKEMUSIC;
            newOption = TwoOptions_ProcessInput(MENUITEMDATA_BIKEMUSIC);

            if (newOption != previousOption)
            {
                gTasks[taskId].tOptions1 &= ~(gBitTable[15]);
                gTasks[taskId].tOptions1 |= (newOption << 15);
                OnOff_DrawChoices(YPOS_BIKEMUSIC, MENUITEMDATA_BIKEMUSIC);
            }
            break;
        case MENUITEM_SURFMUSIC:
            previousOption = MENUITEMDATA_SURFMUSIC;
            newOption = TwoOptions_ProcessInput(MENUITEMDATA_SURFMUSIC);

            if (newOption != previousOption)
            {
                gTasks[taskId].tOptions2 &= ~(gBitTable[10]);
                gTasks[taskId].tOptions2 |= (newOption << 10);
                OnOff_DrawChoices(YPOS_SURFMUSIC, MENUITEMDATA_SURFMUSIC);
            }
            break;
        case MENUITEM_LOWHPBEEP:
            previousOption = MENUITEMDATA_LOWHPBEEP;
            newOption = TwoOptions_ProcessInput(MENUITEMDATA_LOWHPBEEP);

            if (newOption != previousOption)
            {
                gTasks[taskId].tOptions2 &= ~(gBitTable[0]);
                gTasks[taskId].tOptions2 |= newOption;
                OnOff_DrawChoices(YPOS_LOWHPBEEP, MENUITEMDATA_LOWHPBEEP);
            }
            break;
        default:
            return;
        }

        if (sArrowPressed)
        {
            sArrowPressed = FALSE;
            CopyWindowToVram(WIN_OPTIONS, COPYWIN_GFX);
        }
    }
}

static void Task_OptionMenuFadeIn_Pg3(u8 taskId)
{
    if (!gPaletteFade.active)
        gTasks[taskId].func = Task_OptionMenuProcessInput_Pg3;
}

static void Task_OptionMenuProcessInput_Pg3(u8 taskId)
{
    if (JOY_NEW(R_BUTTON) || JOY_NEW(L_BUTTON))
    {
        FillWindowPixelBuffer(WIN_OPTIONS, PIXEL_FILL(1));
        ClearStdWindowAndFrame(WIN_OPTIONS, FALSE);
        sCurrPage = Process_ChangePage(sCurrPage);
        gTasks[taskId].func = Task_ChangePage;
    }
    else if (JOY_NEW(A_BUTTON))
    {
        if (gTasks[taskId].tMenuSelection == MENUITEM_CANCEL_PG3)
            gTasks[taskId].func = Task_OptionMenuSave;
    }
    else if (JOY_NEW(B_BUTTON))
    {
        gTasks[taskId].func = Task_OptionMenuSave;
    }
    else if (JOY_NEW(DPAD_UP))
    {
        if (gTasks[taskId].tMenuSelection > 0)
            gTasks[taskId].tMenuSelection--;
        else
            gTasks[taskId].tMenuSelection = MENUITEM_CANCEL_PG3;
        HighlightOptionMenuItem(gTasks[taskId].tMenuSelection);
    }
    else if (JOY_NEW(DPAD_DOWN))
    {
        if (gTasks[taskId].tMenuSelection < MENUITEM_CANCEL_PG3)
            gTasks[taskId].tMenuSelection++;
        else
            gTasks[taskId].tMenuSelection = 0;
        HighlightOptionMenuItem(gTasks[taskId].tMenuSelection);
    }
    else
    {
        u8 previousOption, newOption;

        switch (gTasks[taskId].tMenuSelection)
        {
        case MENUITEM_DEATHLINK:
            previousOption = MENUITEMDATA_DEATHLINK;
            newOption = TwoOptions_ProcessInput(MENUITEMDATA_DEATHLINK);
            
            if (newOption != previousOption)
            {
                gTasks[taskId].tOptions2 &= ~(gBitTable[4]);
                gTasks[taskId].tOptions2 |= (newOption << 4);
                OnOff_DrawChoices(YPOS_DEATHLINK, MENUITEMDATA_DEATHLINK);
            }
            break;
        case MENUITEM_BLINDTRAINERS:
            previousOption = MENUITEMDATA_BLINDTRAINERS;
            newOption = TwoOptionsChallengeMode_ProcessInput(MENUITEMDATA_BLINDTRAINERS);

            if (newOption != previousOption)
            {
                gTasks[taskId].tOptions2 &= ~(gBitTable[5]);
                gTasks[taskId].tOptions2 |= (newOption << 5);
                OnOffChallengeMode_DrawChoices(YPOS_BLINDTRAINERS, MENUITEMDATA_BLINDTRAINERS);
            }
            break;
        case MENUITEM_ALWAYSCATCH:
            previousOption = MENUITEMDATA_ALWAYSCATCH;
            newOption = TwoOptionsChallengeMode_ProcessInput(MENUITEMDATA_ALWAYSCATCH);

            if (newOption != previousOption)
            {
                gTasks[taskId].tOptions2 &= ~(gBitTable[8]);
                gTasks[taskId].tOptions2 |= (newOption << 8);
                OnOffChallengeMode_DrawChoices(YPOS_ALWAYSCATCH, MENUITEMDATA_ALWAYSCATCH);
            }
            break;
        case MENUITEM_ALWAYSESCAPE:
            previousOption = MENUITEMDATA_ALWAYSESCAPE;
            newOption = TwoOptionsChallengeMode_ProcessInput(MENUITEMDATA_ALWAYSESCAPE);

            if (newOption != previousOption)
            {
                gTasks[taskId].tOptions2 &= ~(gBitTable[9]);
                gTasks[taskId].tOptions2 |= (newOption << 9);
                OnOffChallengeMode_DrawChoices(YPOS_ALWAYSESCAPE, MENUITEMDATA_ALWAYSESCAPE);
            }
            break;
        case MENUITEM_ALWAYSREEL:
            previousOption = MENUITEMDATA_ALWAYSREEL;
            newOption = TwoOptionsChallengeMode_ProcessInput(MENUITEMDATA_ALWAYSREEL);

            if (newOption != previousOption)
            {
                gTasks[taskId].tOptions2 &= ~(gBitTable[7]);
                gTasks[taskId].tOptions2 |= (newOption << 7);
                OnOffChallengeMode_DrawChoices(YPOS_ALWAYSREEL, MENUITEMDATA_ALWAYSREEL);
            }
            break;
        case MENUITEM_APMESSAGES:
            previousOption = MENUITEMDATA_APMESSAGES;
            newOption = XOptions_ProcessInput(3, MENUITEMDATA_APMESSAGES);

            if (newOption != previousOption)
            {
                gTasks[taskId].tOptions2 &= ~(gBitTable[2] | gBitTable[3]);
                gTasks[taskId].tOptions2 |= (newOption << 2);
                ApMessages_DrawChoices(MENUITEMDATA_APMESSAGES);
            }
            break;
        default:
            return;
        }

        if (sArrowPressed)
        {
            sArrowPressed = FALSE;
            CopyWindowToVram(WIN_OPTIONS, COPYWIN_GFX);
        }
    }
}

static void Task_OptionMenuSave(u8 taskId)
{
    gSaveBlock2Ptr->optionsTextSpeed = MENUITEMDATA_TEXTSPEED;
    gSaveBlock2Ptr->optionsBattleSceneOff = MENUITEMDATA_BATTLESCENE;
    gSaveBlock2Ptr->optionsSound = MENUITEMDATA_SOUND;
    gSaveBlock2Ptr->optionsButtonMode = MENUITEMDATA_BUTTONMODE;
    gSaveBlock2Ptr->optionsWindowFrameType = MENUITEMDATA_FRAMETYPE;
    gSaveBlock2Ptr->optionsTurboButton = MENUITEMDATA_TURBOBUTTON;
    gSaveBlock2Ptr->optionsSkipNicknames = MENUITEMDATA_SKIPNICKNAME;
    gSaveBlock2Ptr->optionsAutoRun = MENUITEMDATA_AUTORUN;
    gSaveBlock2Ptr->optionsBikeMusic = MENUITEMDATA_BIKEMUSIC;
    gSaveBlock2Ptr->optionsSurfMusic = MENUITEMDATA_SURFMUSIC;
    gSaveBlock2Ptr->optionsLowHpBeep = MENUITEMDATA_LOWHPBEEP;
    gSaveBlock2Ptr->optionsDeathLink = MENUITEMDATA_DEATHLINK;
    gSaveBlock2Ptr->optionsReceivedItemMessageFilter = MENUITEMDATA_APMESSAGES;
    if (!gArchipelagoOptions.isChallengeMode)
    {
        gSaveBlock2Ptr->optionsBattleStyle = MENUITEMDATA_BATTLESTYLE;
        gSaveBlock2Ptr->optionsBlindTrainers = MENUITEMDATA_BLINDTRAINERS;
        gSaveBlock2Ptr->optionsGuaranteedCatch = MENUITEMDATA_ALWAYSCATCH;
        gSaveBlock2Ptr->optionsGuaranteedRun = MENUITEMDATA_ALWAYSESCAPE;
        gSaveBlock2Ptr->optionsGuaranteedReelFish = MENUITEMDATA_ALWAYSREEL;
    }

    BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
    gTasks[taskId].func = Task_OptionMenuFadeOut;
}

static void Task_OptionMenuFadeOut(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        DestroyTask(taskId);
        FreeAllWindowBuffers();
        SetMainCallback2(gMain.savedCallback);
    }
}

static void HighlightOptionMenuItem(u8 index)
{
    SetGpuReg(REG_OFFSET_WIN0H, WIN_RANGE(16, DISPLAY_WIDTH - 16));
    SetGpuReg(REG_OFFSET_WIN0V, WIN_RANGE(index * 16 + 40, index * 16 + 56));
}

static void DrawOptionMenuChoice(const u8 *text, u8 x, u8 y, u8 style)
{
    u8 dst[16];
    u16 i;

    for (i = 0; *text != EOS && i < ARRAY_COUNT(dst) - 1; i++)
        dst[i] = *(text++);

    if (style != 0)
    {
        dst[2] = TEXT_COLOR_RED;
        dst[5] = TEXT_COLOR_LIGHT_RED;
    }

    dst[i] = EOS;
    AddTextPrinterParameterized(WIN_OPTIONS, FONT_NORMAL, dst, x, y + 1, TEXT_SKIP_DRAW, NULL);
}

static u8 XOptions_ProcessInput(u8 optionCount, u8 selection)
{
    if (JOY_NEW(DPAD_RIGHT))
    {
        if (++selection > (optionCount - 1))
            selection = 0;
        sArrowPressed = TRUE;
    }
    if (JOY_NEW(DPAD_LEFT))
    {
        if (selection != 0)
            selection--;
        else
            selection = optionCount - 1;
        sArrowPressed = TRUE;
    }

    return selection;
}

static u8 XOptionsChallengeMode_ProcessInput(u8 optionCount, u8 selection)
{
    if (!gArchipelagoOptions.isChallengeMode)
    {
        if (JOY_NEW(DPAD_RIGHT))
        {
            if (++selection > (optionCount - 1))
                selection = 0;
            sArrowPressed = TRUE;
        }
        if (JOY_NEW(DPAD_LEFT))
        {
            if (selection != 0)
                selection--;
            else
                selection = optionCount - 1;
            sArrowPressed = TRUE;
        }
    }

    return selection;
}

static u8 TwoOptions_ProcessInput(u8 selection)
{
    if (JOY_NEW(DPAD_RIGHT) | JOY_NEW(DPAD_LEFT))
    {
        selection ^= 1;
        sArrowPressed = TRUE;
    }

    return selection;
}

static u8 TwoOptionsChallengeMode_ProcessInput(u8 selection)
{
    if (!gArchipelagoOptions.isChallengeMode)
    {
        if (JOY_NEW(DPAD_RIGHT) | JOY_NEW(DPAD_LEFT))
        {
            selection ^= 1;
            sArrowPressed = TRUE;
        }
    }

    return selection;
}

static u8 ThreeOptions_ProcessInput(u8 selection)
{
    return XOptions_ProcessInput(3, selection);
}

static u8 FourOptions_ProcessInput(u8 selection)
{
    return XOptions_ProcessInput(4, selection);
}

static u8 TextSpeed_ProcessInput(u8 selection)
{
    if (JOY_NEW(DPAD_RIGHT))
    {
        if (selection <= 1)
            selection++;
        else
            selection = 0;

        sArrowPressed = TRUE;
    }
    if (JOY_NEW(DPAD_LEFT))
    {
        if (selection != 0)
            selection--;
        else
            selection = 2;

        sArrowPressed = TRUE;
    }
    return selection;
}

static void OnOff_DrawChoices(u8 yPos, u8 selection)
{
    u8 styles[2];

    styles[0] = 0;
    styles[1] = 0;
    styles[selection] = 1;

    DrawOptionMenuChoice(gText_BattleSceneOff, 104, yPos, styles[0]);
    DrawOptionMenuChoice(gText_BattleSceneOn, GetStringRightAlignXOffset(FONT_NORMAL, gText_BattleSceneOn, 198), yPos, styles[1]);
}

static void OnOffChallengeMode_DrawChoices(u8 yPos, u8 selection)
{
    if (gArchipelagoOptions.isChallengeMode)
    {
        u8 styles[1];

        styles[0] = 0;

        DrawOptionMenuChoice(gText_BattleSceneOff, ((94 - GetStringWidth(FONT_NORMAL, gText_BattleSceneOff, 0)) / 2 + 104), yPos, styles[0]);
    }
    else
    {
        u8 styles[2];

        styles[0] = 0;
        styles[1] = 0;
        styles[selection] = 1;

        DrawOptionMenuChoice(gText_BattleSceneOff, 104, yPos, styles[0]);
        DrawOptionMenuChoice(gText_BattleSceneOn, GetStringRightAlignXOffset(FONT_NORMAL, gText_BattleSceneOn, 198), yPos, styles[1]);
    }
}

static void TextSpeed_DrawChoices(u8 selection)
{
    u8 styles[3];
    s32 widthMid, widthFast, widthInstant, xFast;

    styles[0] = 0;
    styles[1] = 0;
    styles[2] = 0;
    styles[selection] = 1;

    DrawOptionMenuChoice(gText_TextSpeedMid, 104, YPOS_TEXTSPEED, styles[0]);

    widthMid = GetStringWidth(FONT_NORMAL, gText_TextSpeedMid, 0);
    widthFast = GetStringWidth(FONT_NORMAL, gText_TextSpeedFast, 0);
    widthInstant = GetStringWidth(FONT_NORMAL, gText_TextSpeedInstant, 0);

    widthFast -= 94;
    xFast = (widthMid - widthFast - widthInstant) / 2 + 104;
    DrawOptionMenuChoice(gText_TextSpeedFast, xFast, YPOS_TEXTSPEED, styles[1]);

    DrawOptionMenuChoice(gText_TextSpeedInstant, GetStringRightAlignXOffset(FONT_NORMAL, gText_TextSpeedInstant, 198), YPOS_TEXTSPEED, styles[2]);
}

static void BattleScene_DrawChoices(u8 selection)
{
    u8 styles[2];

    styles[0] = 0;
    styles[1] = 0;
    styles[selection] = 1;

    DrawOptionMenuChoice(gText_BattleSceneOn, 104, YPOS_BATTLESCENE, styles[0]);
    DrawOptionMenuChoice(gText_BattleSceneOff, GetStringRightAlignXOffset(FONT_NORMAL, gText_BattleSceneOff, 198), YPOS_BATTLESCENE, styles[1]);
}

static u8 BattleStyle_ProcessInput(u8 selection)
{
    if (gArchipelagoOptions.isChallengeMode)
        return selection;
    else
    {
        if (JOY_NEW(DPAD_LEFT | DPAD_RIGHT))
        {
            selection ^= 1;
            sArrowPressed = TRUE;
        }
    }

    return selection;
}

static void BattleStyle_DrawChoices(u8 selection)
{
    if (gArchipelagoOptions.isChallengeMode)
    {
        u8 styles[1];

        styles[0] = 0;

        DrawOptionMenuChoice(gText_BattleStyleSet, ((94 - GetStringWidth(FONT_NORMAL, gText_BattleSceneOff, 0)) / 2 + 104), YPOS_BATTLESTYLE, styles[0]);
    }
    else
    {
        u8 styles[2];

        styles[0] = 0;
        styles[1] = 0;
        styles[selection] = 1;

        DrawOptionMenuChoice(gText_BattleStyleShift, 104, YPOS_BATTLESTYLE, styles[0]);
        DrawOptionMenuChoice(gText_BattleStyleSet, GetStringRightAlignXOffset(FONT_NORMAL, gText_BattleStyleSet, 198), YPOS_BATTLESTYLE, styles[1]);
    }
}

static u8 Sound_ProcessInput(u8 selection)
{
    if (JOY_NEW(DPAD_LEFT | DPAD_RIGHT))
    {
        selection ^= 1;
        SetPokemonCryStereo(selection);
        sArrowPressed = TRUE;
    }

    return selection;
}

static void Sound_DrawChoices(u8 selection)
{
    u8 styles[2];

    styles[0] = 0;
    styles[1] = 0;
    styles[selection] = 1;

    DrawOptionMenuChoice(gText_SoundMono, 104, YPOS_SOUND, styles[0]);
    DrawOptionMenuChoice(gText_SoundStereo, GetStringRightAlignXOffset(FONT_NORMAL, gText_SoundStereo, 198), YPOS_SOUND, styles[1]);
}

static u8 FrameType_ProcessInput(u8 selection)
{
    if (JOY_NEW(DPAD_RIGHT))
    {
        if (selection < WINDOW_FRAMES_COUNT - 1)
            selection++;
        else
            selection = 0;

        LoadBgTiles(1, GetWindowFrameTilesPal(selection)->tiles, 0x120, 0x1A2);
        LoadPalette(GetWindowFrameTilesPal(selection)->pal, BG_PLTT_ID(7), PLTT_SIZE_4BPP);
        sArrowPressed = TRUE;
    }
    if (JOY_NEW(DPAD_LEFT))
    {
        if (selection != 0)
            selection--;
        else
            selection = WINDOW_FRAMES_COUNT - 1;

        LoadBgTiles(1, GetWindowFrameTilesPal(selection)->tiles, 0x120, 0x1A2);
        LoadPalette(GetWindowFrameTilesPal(selection)->pal, BG_PLTT_ID(7), PLTT_SIZE_4BPP);
        sArrowPressed = TRUE;
    }
    return selection;
}

static void FrameType_DrawChoices(u8 selection)
{
    u8 text[16];
    u8 n = selection + 1;
    u16 i;

    for (i = 0; gText_FrameTypeNumber[i] != EOS && i <= 5; i++)
        text[i] = gText_FrameTypeNumber[i];

    // Convert a number to decimal string
    if (n / 10 != 0)
    {
        text[i] = n / 10 + CHAR_0;
        i++;
        text[i] = n % 10 + CHAR_0;
        i++;
    }
    else
    {
        text[i] = n % 10 + CHAR_0;
        i++;
        text[i] = CHAR_SPACER;
        i++;
    }

    text[i] = EOS;

    DrawOptionMenuChoice(gText_FrameType, 104, YPOS_FRAMETYPE, 0);
    DrawOptionMenuChoice(text, 128, YPOS_FRAMETYPE, 1);
}

static void ButtonMode_DrawChoices(u8 selection)
{
    s32 widthNormal, widthLR, widthLA, xLR;
    u8 styles[3];

    styles[0] = 0;
    styles[1] = 0;
    styles[2] = 0;
    styles[selection] = 1;

    DrawOptionMenuChoice(gText_ButtonTypeNormal, 104, YPOS_BUTTONMODE, styles[0]);

    widthNormal = GetStringWidth(FONT_NORMAL, gText_ButtonTypeNormal, 0);
    widthLR = GetStringWidth(FONT_NORMAL, gText_ButtonTypeLR, 0);
    widthLA = GetStringWidth(FONT_NORMAL, gText_ButtonTypeLEqualsA, 0);

    widthLR -= 94;
    xLR = (widthNormal - widthLR - widthLA) / 2 + 104;
    DrawOptionMenuChoice(gText_ButtonTypeLR, xLR, YPOS_BUTTONMODE, styles[1]);

    DrawOptionMenuChoice(gText_ButtonTypeLEqualsA, GetStringRightAlignXOffset(FONT_NORMAL, gText_ButtonTypeLEqualsA, 198), YPOS_BUTTONMODE, styles[2]);
}

static void TurboButton_DrawChoices(u8 selection)
{
    s32 widthOff, widthA, widthB, xA;
    u8 styles[3];

    styles[0] = 0;
    styles[1] = 0;
    styles[2] = 0;
    styles[selection] = 1;

    DrawOptionMenuChoice(gText_BattleSceneOff, 104, YPOS_TURBOBUTTON, styles[0]);

    widthOff = GetStringWidth(FONT_NORMAL, gText_BattleSceneOff, 0);
    widthA = GetStringWidth(FONT_NORMAL, gText_TurboButtonA, 0);
    widthB = GetStringWidth(FONT_NORMAL, gText_TurboButtonB, 0);

    widthA -= 94;
    xA = (widthOff - widthA - widthB) / 2 + 104;
    DrawOptionMenuChoice(gText_TurboButtonA, xA, YPOS_TURBOBUTTON, styles[1]);

    DrawOptionMenuChoice(gText_TurboButtonB, GetStringRightAlignXOffset(FONT_NORMAL, gText_TurboButtonB, 198), YPOS_TURBOBUTTON, styles[2]);
}

static void ApMessages_DrawChoices(u8 selection)
{
    s32 widthFiller, widthUseful, widthProgression, xUseful;
    u8 styles[3];

    styles[0] = 0;
    styles[1] = 0;
    styles[2] = 0;
    styles[selection] = 1;

    DrawOptionMenuChoice(gText_ApMessagesFiller, 104, YPOS_APMESSAGES, styles[0]);

    widthFiller = GetStringWidth(FONT_NORMAL, gText_ApMessagesFiller, 0);
    widthUseful = GetStringWidth(FONT_NORMAL, gText_ApMessagesUseful, 0);
    widthProgression = GetStringWidth(FONT_NORMAL, gText_ApMessagesProgression, 0);

    widthUseful -= 94;
    xUseful = (widthFiller - widthUseful - widthProgression) / 2 + 104;
    DrawOptionMenuChoice(gText_ApMessagesUseful, xUseful, YPOS_APMESSAGES, styles[1]);

    DrawOptionMenuChoice(gText_ApMessagesProgression, GetStringRightAlignXOffset(FONT_NORMAL, gText_ApMessagesProgression, 198), YPOS_APMESSAGES, styles[2]);
}

static u16 packMenuOptions1(void)
{
    struct MenuOptions menuOptions = {
        .buttonMode = gSaveBlock2Ptr->optionsButtonMode,
        .textSpeed = gSaveBlock2Ptr->optionsTextSpeed,
        .sound = gSaveBlock2Ptr->optionsSound,
        .battleStyle = gSaveBlock2Ptr->optionsBattleStyle,
        .battleSceneOff = gSaveBlock2Ptr->optionsBattleSceneOff,
        .skipFanfares = gSaveBlock2Ptr->optionsSkipFanfares,
        .windowFrameType = gSaveBlock2Ptr->optionsWindowFrameType,
        .turboButton = gSaveBlock2Ptr->optionsTurboButton,
        .bikeMusic = gSaveBlock2Ptr->optionsBikeMusic
    };

    return menuOptions.buttonMode
           | (menuOptions.textSpeed << 2)
           | (menuOptions.sound << 4)
           | (menuOptions.battleStyle << 5)
           | (menuOptions.battleSceneOff << 6)
           | (menuOptions.skipFanfares << 7)
           | (menuOptions.windowFrameType << 8)
           | (menuOptions.turboButton << 13)
           | (menuOptions.bikeMusic << 15);
}

static u16 packMenuOptions2(void)
{
    struct MenuOptions menuOptions = {
        .lowHpBeep = gSaveBlock2Ptr->optionsLowHpBeep,
        .skipNicknames = gSaveBlock2Ptr->optionsSkipNicknames,
        .receivedItemMessageFilter = gSaveBlock2Ptr->optionsReceivedItemMessageFilter,
        .deathLink = gSaveBlock2Ptr->optionsDeathLink,
        .blindTrainers = gSaveBlock2Ptr->optionsBlindTrainers,
        .autoRun = gSaveBlock2Ptr->optionsAutoRun,
        .guaranteedReelFish = gSaveBlock2Ptr->optionsGuaranteedReelFish,
        .guaranteedCatch = gSaveBlock2Ptr->optionsGuaranteedCatch,
        .guaranteedRun = gSaveBlock2Ptr->optionsGuaranteedRun,
        .surfMusic = gSaveBlock2Ptr->optionsSurfMusic,
    };

    return menuOptions.lowHpBeep
           | (menuOptions.skipNicknames << 1)
           | (menuOptions.receivedItemMessageFilter << 2)
           | (menuOptions.deathLink << 4)
           | (menuOptions.blindTrainers << 5)
           | (menuOptions.autoRun << 6)
           | (menuOptions.guaranteedReelFish << 7)
           | (menuOptions.guaranteedCatch << 8)
           | (menuOptions.guaranteedRun << 9)
           | (menuOptions.surfMusic << 10);
}

static void DrawHeaderText(void)
{
    u32 i, widthOptions, xMid;
    u8 pageDots[9] = _("");
    widthOptions = GetStringWidth(FONT_NORMAL, gText_Option, 0);

    for (i = 0; i < PAGE_COUNT; i++)
    {
        if (i == sCurrPage)
            StringAppend(pageDots, gText_LargeDot);
        else
            StringAppend(pageDots, gText_SmallDot);
    }
    xMid = (8 + widthOptions + 5);
    FillWindowPixelBuffer(WIN_HEADER, PIXEL_FILL(1));
    AddTextPrinterParameterized(WIN_HEADER, FONT_NORMAL, gText_Option, 8, 1, TEXT_SKIP_DRAW, NULL);
    AddTextPrinterParameterized(WIN_HEADER, FONT_NORMAL, pageDots, xMid, 1, TEXT_SKIP_DRAW, NULL);
    AddTextPrinterParameterized(WIN_HEADER, FONT_NORMAL, gText_PageNav, GetStringRightAlignXOffset(FONT_NORMAL, gText_PageNav, 198), 1, TEXT_SKIP_DRAW, NULL);
    CopyWindowToVram(WIN_HEADER, COPYWIN_FULL);
}

static void DrawOptionMenuTexts(void)
{
    u8 i, items;
    const u8* const* menu = NULL;

    switch (sCurrPage)
    {
        case 0:
            items = MENUITEM_COUNT;
            menu = sOptionMenuItemsNames;
            break;
        case 1:
            items = MENUITEM_COUNT_PG2;
            menu = sOptionMenuItemsNames_Pg2;
            break;
        case 2:
            items = MENUITEM_COUNT_PG3;
            menu = sOptionMenuItemsNames_Pg3;
    }

    FillWindowPixelBuffer(WIN_OPTIONS, PIXEL_FILL(1));
    for (i = 0; i < items; i++)
        AddTextPrinterParameterized(WIN_OPTIONS, FONT_NORMAL, menu[i], 8, (i * 16) + 1, TEXT_SKIP_DRAW, NULL);
    CopyWindowToVram(WIN_OPTIONS, COPYWIN_FULL);
}

#define TILE_TOP_CORNER_L 0x1A2
#define TILE_TOP_EDGE     0x1A3
#define TILE_TOP_CORNER_R 0x1A4
#define TILE_LEFT_EDGE    0x1A5
#define TILE_RIGHT_EDGE   0x1A7
#define TILE_BOT_CORNER_L 0x1A8
#define TILE_BOT_EDGE     0x1A9
#define TILE_BOT_CORNER_R 0x1AA

static void DrawBgWindowFrames(void)
{
    //                     bg, tile,              x, y, width, height, palNum
    // Draw title window frame
    FillBgTilemapBufferRect(1, TILE_TOP_CORNER_L,  1,  0,  1,  1,  7);
    FillBgTilemapBufferRect(1, TILE_TOP_EDGE,      2,  0, 27,  1,  7);
    FillBgTilemapBufferRect(1, TILE_TOP_CORNER_R, 28,  0,  1,  1,  7);
    FillBgTilemapBufferRect(1, TILE_LEFT_EDGE,     1,  1,  1,  2,  7);
    FillBgTilemapBufferRect(1, TILE_RIGHT_EDGE,   28,  1,  1,  2,  7);
    FillBgTilemapBufferRect(1, TILE_BOT_CORNER_L,  1,  3,  1,  1,  7);
    FillBgTilemapBufferRect(1, TILE_BOT_EDGE,      2,  3, 27,  1,  7);
    FillBgTilemapBufferRect(1, TILE_BOT_CORNER_R, 28,  3,  1,  1,  7);

    // Draw options list window frame
    FillBgTilemapBufferRect(1, TILE_TOP_CORNER_L,  1,  4,  1,  1,  7);
    FillBgTilemapBufferRect(1, TILE_TOP_EDGE,      2,  4, 26,  1,  7);
    FillBgTilemapBufferRect(1, TILE_TOP_CORNER_R, 28,  4,  1,  1,  7);
    FillBgTilemapBufferRect(1, TILE_LEFT_EDGE,     1,  5,  1, 18,  7);
    FillBgTilemapBufferRect(1, TILE_RIGHT_EDGE,   28,  5,  1, 18,  7);
    FillBgTilemapBufferRect(1, TILE_BOT_CORNER_L,  1, 19,  1,  1,  7);
    FillBgTilemapBufferRect(1, TILE_BOT_EDGE,      2, 19, 26,  1,  7);
    FillBgTilemapBufferRect(1, TILE_BOT_CORNER_R, 28, 19,  1,  1,  7);

    CopyBgTilemapBufferToVram(1);
}
