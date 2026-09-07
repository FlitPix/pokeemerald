#include "global.h"
#include "util.h"
#include "archipelago.h"
#include "new_game.h"
#include "random.h"
#include "pokemon.h"
#include "roamer.h"
#include "pokemon_size_record.h"
#include "script.h"
#include "lottery_corner.h"
#include "play_time.h"
#include "mauville_old_man.h"
#include "match_call.h"
#include "lilycove_lady.h"
#include "load_save.h"
#include "pokeblock.h"
#include "dewford_trend.h"
#include "berry.h"
#include "rtc.h"
#include "easy_chat.h"
#include "event_data.h"
#include "money.h"
#include "trainer_hill.h"
#include "tv.h"
#include "coins.h"
#include "text.h"
#include "overworld.h"
#include "mail.h"
#include "battle_records.h"
#include "item.h"
#include "pokedex.h"
#include "apprentice.h"
#include "frontier_util.h"
#include "pokedex.h"
#include "save.h"
#include "link_rfu.h"
#include "main.h"
#include "contest.h"
#include "item_menu.h"
#include "pokemon_storage_system.h"
#include "pokemon_jump.h"
#include "decoration_inventory.h"
#include "secret_base.h"
#include "player_pc.h"
#include "field_specials.h"
#include "berry_powder.h"
#include "mystery_gift.h"
#include "union_room_chat.h"
#include "constants/items.h"

extern const u8 EventScript_ResetAllMapFlags[];

static void ClearFrontierRecord(void);
static void WarpToStartLocation(void);
static void ResetMiniGamesRecords(void);
static void SetNewGameFlagsVars(void);

EWRAM_DATA bool8 gDifferentSaveFile = FALSE;
EWRAM_DATA bool8 gEnableContestDebugging = FALSE;

static const struct ContestWinner sContestWinnerPicDummy =
{
    .monName = _(""),
    .trainerName = _("")
};

void SetTrainerId(u32 trainerId, u8 *dst)
{
    dst[0] = trainerId;
    dst[1] = trainerId >> 8;
    dst[2] = trainerId >> 16;
    dst[3] = trainerId >> 24;
}

u32 GetTrainerId(u8 *trainerId)
{
    return (trainerId[3] << 24) | (trainerId[2] << 16) | (trainerId[1] << 8) | (trainerId[0]);
}

void CopyTrainerId(u8 *dst, u8 *src)
{
    s32 i;
    for (i = 0; i < TRAINER_ID_LENGTH; i++)
        dst[i] = src[i];
}

static void InitPlayerTrainerId(void)
{
    u32 trainerId = (Random() << 16) | GetGeneratedTrainerIdLower();
    SetTrainerId(trainerId, gSaveBlock2Ptr->playerTrainerId);
}

static void SetDefaultOptions(void)
{
    gSaveBlock2Ptr->optionsButtonMode = gArchipelagoOptions.optionsButtonMode;
    gSaveBlock2Ptr->optionsTextSpeed = gArchipelagoOptions.optionsTextSpeed;
    gSaveBlock2Ptr->optionsWindowFrameType = gArchipelagoOptions.optionsWindowFrameType;
    gSaveBlock2Ptr->optionsSound = gArchipelagoOptions.optionsSound;
    gSaveBlock2Ptr->optionsBattleSceneOff = !gArchipelagoOptions.optionsBattleScene;
    gSaveBlock2Ptr->optionsTurboButton = gArchipelagoOptions.optionsTurboButton;
    gSaveBlock2Ptr->optionsSkipFanfares = gArchipelagoOptions.optionsSkipFanfares;
    gSaveBlock2Ptr->optionsBikeMusic = gArchipelagoOptions.optionsBikeMusic;
    gSaveBlock2Ptr->optionsSurfMusic = gArchipelagoOptions.optionsSurfMusic;
    gSaveBlock2Ptr->optionsLowHpBeep = gArchipelagoOptions.optionsLowHpBeep;
    gSaveBlock2Ptr->optionsSkipNicknames = gArchipelagoOptions.optionsSkipNicknames;
    gSaveBlock2Ptr->optionsReceivedItemMessageFilter = gArchipelagoOptions.optionsReceivedItemMessageFilter;
    gSaveBlock2Ptr->optionsDeathLink = gArchipelagoOptions.optionsDeathLink;
    gSaveBlock2Ptr->optionsAutoRun = gArchipelagoOptions.optionsAutoRun;
    if (gArchipelagoOptions.isChallengeMode)
    {
        gSaveBlock2Ptr->optionsBattleStyle = OPTIONS_BATTLE_STYLE_SET;
        gSaveBlock2Ptr->optionsBlindTrainers = FALSE;
        gSaveBlock2Ptr->optionsGuaranteedRun = FALSE;
        gSaveBlock2Ptr->optionsGuaranteedReelFish = FALSE;
        gSaveBlock2Ptr->optionsGuaranteedCatch = FALSE;
    }
    else
    {
        gSaveBlock2Ptr->optionsBattleStyle = gArchipelagoOptions.optionsBattleStyle;
        gSaveBlock2Ptr->optionsBlindTrainers = gArchipelagoOptions.optionsBlindTrainers;
        gSaveBlock2Ptr->optionsGuaranteedRun = gArchipelagoOptions.optionsGuaranteedRun;
        gSaveBlock2Ptr->optionsGuaranteedReelFish = gArchipelagoOptions.optionsGuaranteedReelFish;
        gSaveBlock2Ptr->optionsGuaranteedCatch = gArchipelagoOptions.optionsGuaranteedCatch;
    }
    gSaveBlock2Ptr->regionMapZoom = FALSE;
}

static void ClearPokedexFlags(void)
{
    gUnusedPokedexU8 = 0;
    memset(&gSaveBlock2Ptr->pokedex.owned, 0, sizeof(gSaveBlock2Ptr->pokedex.owned));
    memset(&gSaveBlock2Ptr->pokedex.seen, 0, sizeof(gSaveBlock2Ptr->pokedex.seen));
}

void ClearAllContestWinnerPics(void)
{
    s32 i;

    ClearContestWinnerPicsInContestHall();

    // Clear Museum paintings
    for (i = MUSEUM_CONTEST_WINNERS_START; i < NUM_CONTEST_WINNERS; i++)
        gSaveBlock1Ptr->contestWinners[i] = sContestWinnerPicDummy;
}

static void ClearFrontierRecord(void)
{
    CpuFill32(0, &gSaveBlock2Ptr->frontier, sizeof(gSaveBlock2Ptr->frontier));

    gSaveBlock2Ptr->frontier.opponentNames[0][0] = EOS;
    gSaveBlock2Ptr->frontier.opponentNames[1][0] = EOS;
}

static void WarpToStartLocation(void)
{
    if (gSaveBlock2Ptr->playerGender == MALE)
        SetWarpDestination(MAP_GROUP(MAP_LITTLEROOT_TOWN_BRENDANS_HOUSE_2F), MAP_NUM(MAP_LITTLEROOT_TOWN_BRENDANS_HOUSE_2F), WARP_ID_NONE, -1, -1);
    else
        SetWarpDestination(MAP_GROUP(MAP_LITTLEROOT_TOWN_MAYS_HOUSE_2F), MAP_NUM(MAP_LITTLEROOT_TOWN_MAYS_HOUSE_2F), WARP_ID_NONE, -1, -1);
    WarpIntoMap();
}

void Sav2_ClearSetDefault(void)
{
    ClearSav2();
    SetDefaultOptions();
}

void ResetMenuAndMonGlobals(void)
{
    gDifferentSaveFile = FALSE;
    ResetPokedexScrollPositions();
    ZeroPlayerPartyMons();
    ZeroEnemyPartyMons();
    ResetBagScrollPositions();
    ResetPokeblockScrollPositions();
}

void NewGameInitData(void)
{
    if (gSaveFileStatus == SAVE_STATUS_EMPTY || gSaveFileStatus == SAVE_STATUS_CORRUPT)
        RtcReset();

    gDifferentSaveFile = TRUE;
    gSaveBlock2Ptr->encryptionKey = 0;
    ZeroPlayerPartyMons();
    ZeroEnemyPartyMons();
    ResetPokedex();
    ClearFrontierRecord();
    ClearSav1();
    ClearAllMail();
    gSaveBlock2Ptr->specialSaveWarpFlags = 0;
    gSaveBlock2Ptr->gcnLinkFlags = 0;
    InitPlayerTrainerId();
    PlayTimeCounter_Reset();
    ClearPokedexFlags();
    InitEventData();
    ClearTVShowData();
    ResetGabbyAndTy();
    ClearSecretBases();
    ClearBerryTrees();
    SetMoney(&gSaveBlock1Ptr->money, gArchipelagoOptions.startingMoney);
    SetCoins(0);
    ResetLinkContestBoolean();
    ResetGameStats();
    ClearAllContestWinnerPics();
    ClearPlayerLinkBattleRecords();
    InitSeedotSizeRecord();
    InitLotadSizeRecord();
    gPlayerPartyCount = 0;
    ZeroPlayerPartyMons();
    ResetPokemonStorageSystem();
    ClearRoamerData();
    ClearRoamerLocationData();
    gSaveBlock1Ptr->registeredItem = ITEM_NONE;
    ClearBag();
    NewGameInitPCItems();
    ClearPokeblocks();
    ClearDecorationInventories();
    InitEasyChatPhrases();
    SetMauvilleOldMan();
    InitDewfordTrend();
    ResetFanClub();
    ResetLotteryCorner();
    SetNewGameFlagsVars();
    WarpToStartLocation();
    RunScriptImmediately(EventScript_ResetAllMapFlags);
    ResetMiniGamesRecords();
    InitUnionRoomChatRegisteredTexts();
    InitLilycoveLady();
    ResetAllApprenticeData();
    ClearRankingHallRecords();
    InitMatchCallCounters();
    ClearMysteryGift();
    WipeTrainerNameRecords();
    ResetTrainerHillResults();
    ResetContestLinkResults();
}

static void SetNewGameFlagsVars(void)
{
    // see also new_game.inc
    if (!gArchipelagoOptions.shuffleBag) FlagSet(FLAG_SYS_BAG_GET);
    if (gArchipelagoOptions.shufflePokedex == 0) FlagSet(FLAG_SYS_POKEDEX_GET);
    if (gArchipelagoOptions.shufflePokedex == 0) EnableNationalPokedex();
    if (!gArchipelagoOptions.shufflePokenav) FlagSet(FLAG_SYS_POKENAV_GET);
    if (!gArchipelagoOptions.shuffleRunningShoes) FlagSet(FLAG_SYS_B_DASH);
    if (gArchipelagoOptions.startingBadges & gBitTable[0]) FlagSet(FLAG_BADGE01_GET);
    if (gArchipelagoOptions.startingBadges & gBitTable[1]) FlagSet(FLAG_BADGE02_GET);
    if (gArchipelagoOptions.startingBadges & gBitTable[2]) FlagSet(FLAG_BADGE03_GET);
    if (gArchipelagoOptions.startingBadges & gBitTable[3]) FlagSet(FLAG_BADGE04_GET);
    if (gArchipelagoOptions.startingBadges & gBitTable[4]) FlagSet(FLAG_BADGE05_GET);
    if (gArchipelagoOptions.startingBadges & gBitTable[5]) FlagSet(FLAG_BADGE06_GET);
    if (gArchipelagoOptions.startingBadges & gBitTable[6]) FlagSet(FLAG_BADGE07_GET);
    if (gArchipelagoOptions.startingBadges & gBitTable[7]) FlagSet(FLAG_BADGE08_GET);
    // TODO: instead remove events and flags
    VarSet(VAR_LITTLEROOT_TOWN_STATE, 4);
    VarSet(VAR_LITTLEROOT_INTRO_STATE, 7);
    VarSet(VAR_LITTLEROOT_RIVAL_STATE, 3);
    VarSet(VAR_LITTLEROOT_HOUSES_STATE_BRENDAN, 2);
    VarSet(VAR_LITTLEROOT_HOUSES_STATE_MAY, 2);
    FlagSet(FLAG_HIDE_LITTLEROOT_TOWN_BRENDANS_HOUSE_RIVAL_BEDROOM);
    FlagSet(FLAG_HIDE_LITTLEROOT_TOWN_MAYS_HOUSE_RIVAL_BEDROOM);
    VarSet(VAR_BIRCH_LAB_STATE, 5);
    VarSet(VAR_ROUTE101_STATE, 3);
    FlagClear(FLAG_HIDE_LITTLEROOT_TOWN_BIRCHS_LAB_BIRCH);
    FlagClear(FLAG_HIDE_LITTLEROOT_TOWN_BIRCHS_LAB_UNKNOWN_0x380);
    VarSet(VAR_PETALBURG_CITY_STATE, 3);
    VarSet(VAR_PETALBURG_GYM_STATE, 2);
    VarSet(VAR_BRINEY_HOUSE_STATE, 1);
    FlagSet(FLAG_HAS_MATCH_CALL);
    FlagSet(FLAG_ADDED_MATCH_CALL_TO_POKENAV);
    FlagSet(FLAG_ENABLE_MOM_MATCH_CALL);
    FlagSet(FLAG_ENABLE_PROF_BIRCH_MATCH_CALL);
    FlagSet(FLAG_ENABLE_NORMAN_MATCH_CALL);
    VarSet(VAR_CABLE_CLUB_TUTORIAL_STATE, 2);
    if (gSaveBlock2Ptr->playerGender == MALE)
    {
        FlagSet(FLAG_HIDE_LITTLEROOT_TOWN_MAYS_HOUSE_MOM);
        FlagSet(FLAG_HIDE_LITTLEROOT_TOWN_BRENDANS_HOUSE_RIVAL_MOM);
        FlagSet(FLAG_HIDE_LITTLEROOT_TOWN_BRENDANS_HOUSE_RIVAL_SIBLING);
    }
    else
    {
        FlagSet(FLAG_HIDE_LITTLEROOT_TOWN_BRENDANS_HOUSE_MOM);
        FlagSet(FLAG_HIDE_LITTLEROOT_TOWN_MAYS_HOUSE_RIVAL_MOM);
        FlagSet(FLAG_HIDE_LITTLEROOT_TOWN_MAYS_HOUSE_RIVAL_SIBLING);
    }
}

static void ResetMiniGamesRecords(void)
{
    CpuFill16(0, &gSaveBlock2Ptr->berryCrush, sizeof(struct BerryCrush));
    SetBerryPowder(&gSaveBlock2Ptr->berryCrush.berryPowderAmount, 0);
    ResetPokemonJumpRecords();
    CpuFill16(0, &gSaveBlock2Ptr->berryPick, sizeof(struct BerryPickingResults));
}
