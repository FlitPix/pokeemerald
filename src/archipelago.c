#include "global.h"
#include "main.h"
#include "archipelago.h"
#include "pokemon.h"
#include "item.h"
#include "event_data.h"
#include "event_scripts.h"
#include "script.h"
#include "overworld.h"
#include "util.h"
#include "constants/maps.h"

const struct ArchipelagoOptions gArchipelagoOptions = {
    .introSpecies = SPECIES_LOTAD,
    .pcItem = ITEM_POTION,
    .startingLocation = MAP_LITTLEROOT_TOWN,
    //.startingSpawn

    .expPercentMultiplier = 100,
    .optionsWindowFrameType = 0,
    .normalizeEncounterRates = FALSE,
    .optionsTextSpeed = OPTIONS_TEXT_SPEED_FAST,
    .optionsTurboButton = OPTIONS_TURBO_BUTTON_NONE,
    .optionsButtonMode = 1,
    .optionsBattleScene = TRUE,
    .optionsBattleStyle = OPTIONS_BATTLE_STYLE_SHIFT,
    .optionsSound = OPTIONS_SOUND_MONO,
    .optionsSkipFanfares = TRUE,
    .optionsBikeMusic = TRUE,
    .optionsSurfMusic = TRUE,
    .optionsLowHpBeep = TRUE,
    .optionsSkipNicknames = FALSE,
    .optionsReceivedItemMessageFilter = AP_CLASS_PROGRESSION,
    .optionsReceivedItemSound = TRUE,
    .optionsGuaranteedCatch = FALSE,
    .optionsGuaranteedRun = FALSE,
    .optionsDeathLink = FALSE,
    .optionsBlindTrainers = FALSE,
    .optionsAutoRun = FALSE,
    .reusableTms = TRUE,
    .purgeSpinners = FALSE,
    .matchTrainerLevels = FALSE,
    .optionsGuaranteedReelFish = FALSE,
    .unlockSeenDexInfo = FALSE,
    .matchTrainerLevelBonus = 0,
    .betterShopsEnabled = FALSE,

    .eliteFourNeedsGyms = FALSE,
    .eliteFourRequiredCount = 8,
    .normanNeedsGyms = FALSE,
    .normanRequiredCount = 4,

    .removeBadgeRequirement = 16,
    //.additionalDarkCaves
    .freeFlyHmLocation = 0,
    .freeFlyPokenavLocation = 0,
    .terraCaveLocationId = 0,
    .marineCaveLocationId = 0,

    .addRoute115Boulders = FALSE,
    .addBumpySlopes = FALSE,
    .modifyRoute118 = FALSE,
    //.removedBlockers

    .berryTreesRandomized = FALSE,
    .isTrainersanity = FALSE,
    .isDexsanity = FALSE,
    .isShopsanity = FALSE,
    .flyUnlocks = FALSE,
    .extraKeyItems = FALSE,
    .gymKeys = FALSE,
    .shuffleBag = FALSE,
    .shufflePokedex = 0,
    .shufflePokenav = FALSE,
    .shuffleRunningShoes = FALSE,

    .startingMoney = 3000,
    .wonderTradeAllowed = FALSE,
    .remoteItems = FALSE,
    .isChallengeMode = FALSE,
};

const struct ArchipelagoInfo gArchipelagoInfo = {
    .auth = {0}
};

extern const u8 LittlerootTown_ProfessorBirchsLab_EventScript_ReceivePokedex[];

EWRAM_DATA struct ArchipelagoReceivedItem gArchipelagoReceivedItem = {0};
EWRAM_DATA struct ArchipelagoReceivedItem gArchipelagoNextTrap = {0};
EWRAM_DATA struct ArchipelagoReward gArchipelagoReceiveItemQueue[REWARD_QUEUE_SIZE] = {0};

const u8 gArchipelagoPlayerNames[PLAYER_NAME_BUFFER_SIZE] = {0};
const u8 gArchipelagoItemNames[ITEM_NAME_BUFFER_SIZE] = {0};
const u8 gArchipelagoNameTable[NAME_TABLE_BUFFER_SIZE] = {0};

bool8 CheckQueuedRewards()
{
    if (gArchipelagoReceiveItemQueue[0].itemId != ITEM_NONE)
    {
        ScriptContext_SetupScript(ArchipelagoScript_ReceiveReward);
        return TRUE;
    }
    return FALSE;
}

bool8 Archipelago_CheckReceivedItem()
{
    if (gArchipelagoReceivedItem.isFilled == TRUE) {
        if (
            (gArchipelagoOptions.optionsReceivedItemMessageFilter == AP_CLASS_FILLER) ||
            (gArchipelagoOptions.optionsReceivedItemMessageFilter == AP_CLASS_USEFUL && gArchipelagoReceivedItem.itemClass > AP_CLASS_USEFUL) ||
            (gArchipelagoOptions.optionsReceivedItemMessageFilter == AP_CLASS_PROGRESSION && gArchipelagoReceivedItem.itemClass == AP_CLASS_PROGRESSION) ||
            (gArchipelagoReceivedItem.itemId >= ITEM_BADGE_1 && gArchipelagoReceivedItem.itemId <= ITEM_BADGE_8)
        )
            ScriptContext_SetupScript(ArchipelagoScript_ReceiveRemoteItem);
        else
            ScriptContext_SetupScript(ArchipelagoScript_ReceiveRemoteItemSilent);
        return TRUE;
    }
    return FALSE;
}

bool8 ArchipelagoSpecial_IsItemSpecial(void)
{
    return (gSpecialVar_ArchipelagoItemFound >= ITEM_BADGE_1 && gSpecialVar_ArchipelagoItemFound <= ITEM_BADGE_8) ||
           gSpecialVar_ArchipelagoItemFound == ITEM_POKEDEX ||
           gSpecialVar_ArchipelagoItemFound == ITEM_NATIONAL_POKEDEX ||
           gSpecialVar_ArchipelagoItemFound == ITEM_PROGRESSIVE_POKEDEX ||
           gSpecialVar_ArchipelagoItemFound == ITEM_POKENAV ||
           gSpecialVar_ArchipelagoItemFound == ITEM_BAG;
}

bool8 ArchipelagoSpecial_ShouldHandle(void)
{
    return gSpecialVar_ArchipelagoItemFound == ITEM_ARCHIPELAGO || ArchipelagoSpecial_IsItemSpecial();
}

u16 ArchipelagoSpecial_GetObjectScriptFlag(void)
{
    u8 i;

    for (i = 0; i < gMapHeader.events->objectEventCount; i++)
    {
        if (gSaveBlock1Ptr->objectEventTemplates[i].localId == gSpecialVar_LastTalked)
            return gSaveBlock1Ptr->objectEventTemplates[i].flagId;
    }

    return 0;
}

u16 ArchipelagoSpecial_PopReward(void)
{
    u16 reward = gArchipelagoReceiveItemQueue[0].itemId;
    u16 locationId = gArchipelagoReceiveItemQueue[0].locationId;
    u8 i;

    for (i = 0; i < REWARD_QUEUE_SIZE - 1; ++i)
    {
        gArchipelagoReceiveItemQueue[i].itemId = gArchipelagoReceiveItemQueue[i + 1].itemId;
        gArchipelagoReceiveItemQueue[i].locationId = gArchipelagoReceiveItemQueue[i + 1].locationId;
    }
    gArchipelagoReceiveItemQueue[REWARD_QUEUE_SIZE - 1].itemId = ITEM_NONE;
    gArchipelagoReceiveItemQueue[REWARD_QUEUE_SIZE - 1].locationId = 0;

    gSpecialVar_0x8003 = locationId;
    return reward;
}

u16 ArchipelagoSpecial_ConsumeReceivedItem(void)
{
    gSaveBlock1Ptr->archipelagoLastReceivedItemIndex = gArchipelagoReceivedItem.itemIndex;
    gArchipelagoReceivedItem.isFilled = FALSE;
    return gArchipelagoReceivedItem.itemId;
}

bool8 ArchipelagoSpecial_CanChallengeNorman(void)
{
    if (gArchipelagoOptions.normanNeedsGyms)
    {
        u8 gymCount = 0;
        gymCount += FlagGet(FLAG_DEFEATED_RUSTBORO_GYM) ? 1 : 0;
        gymCount += FlagGet(FLAG_DEFEATED_DEWFORD_GYM) ? 1 : 0;
        gymCount += FlagGet(FLAG_DEFEATED_MAUVILLE_GYM) ? 1 : 0;
        gymCount += FlagGet(FLAG_DEFEATED_LAVARIDGE_GYM) ? 1 : 0;
        gymCount += FlagGet(FLAG_DEFEATED_PETALBURG_GYM) ? 1 : 0;
        gymCount += FlagGet(FLAG_DEFEATED_FORTREE_GYM) ? 1 : 0;
        gymCount += FlagGet(FLAG_DEFEATED_MOSSDEEP_GYM) ? 1 : 0;
        gymCount += FlagGet(FLAG_DEFEATED_SOOTOPOLIS_GYM) ? 1 : 0;
        
        return gymCount >= gArchipelagoOptions.normanRequiredCount;
    }
    else
    {
        u8 badgeCount = 0;
        badgeCount += FlagGet(FLAG_BADGE01_GET) ? 1 : 0;
        badgeCount += FlagGet(FLAG_BADGE02_GET) ? 1 : 0;
        badgeCount += FlagGet(FLAG_BADGE03_GET) ? 1 : 0;
        badgeCount += FlagGet(FLAG_BADGE04_GET) ? 1 : 0;
        badgeCount += FlagGet(FLAG_BADGE05_GET) ? 1 : 0;
        badgeCount += FlagGet(FLAG_BADGE06_GET) ? 1 : 0;
        badgeCount += FlagGet(FLAG_BADGE07_GET) ? 1 : 0;
        badgeCount += FlagGet(FLAG_BADGE08_GET) ? 1 : 0;

        return badgeCount >= gArchipelagoOptions.normanRequiredCount;
    }
}

bool8 ArchipelagoSpecial_CanChallengeEliteFour(void)
{
    if (gArchipelagoOptions.eliteFourNeedsGyms)
    {
        u8 gymCount = 0;
        gymCount += FlagGet(FLAG_DEFEATED_RUSTBORO_GYM) ? 1 : 0;
        gymCount += FlagGet(FLAG_DEFEATED_DEWFORD_GYM) ? 1 : 0;
        gymCount += FlagGet(FLAG_DEFEATED_MAUVILLE_GYM) ? 1 : 0;
        gymCount += FlagGet(FLAG_DEFEATED_LAVARIDGE_GYM) ? 1 : 0;
        gymCount += FlagGet(FLAG_DEFEATED_PETALBURG_GYM) ? 1 : 0;
        gymCount += FlagGet(FLAG_DEFEATED_FORTREE_GYM) ? 1 : 0;
        gymCount += FlagGet(FLAG_DEFEATED_MOSSDEEP_GYM) ? 1 : 0;
        gymCount += FlagGet(FLAG_DEFEATED_SOOTOPOLIS_GYM) ? 1 : 0;
        
        return gymCount >= gArchipelagoOptions.eliteFourRequiredCount;
    }
    else
    {
        u8 badgeCount = 0;
        badgeCount += FlagGet(FLAG_BADGE01_GET) ? 1 : 0;
        badgeCount += FlagGet(FLAG_BADGE02_GET) ? 1 : 0;
        badgeCount += FlagGet(FLAG_BADGE03_GET) ? 1 : 0;
        badgeCount += FlagGet(FLAG_BADGE04_GET) ? 1 : 0;
        badgeCount += FlagGet(FLAG_BADGE05_GET) ? 1 : 0;
        badgeCount += FlagGet(FLAG_BADGE06_GET) ? 1 : 0;
        badgeCount += FlagGet(FLAG_BADGE07_GET) ? 1 : 0;
        badgeCount += FlagGet(FLAG_BADGE08_GET) ? 1 : 0;

        return badgeCount >= gArchipelagoOptions.eliteFourRequiredCount;
    }
}

u8 ArchipelagoSpecial_HandleProgressivePokedex(void)
{
    if (!FlagGet(FLAG_SYS_POKEDEX_GET))
        return 0;
    if (!IsNationalPokedexEnabled())
        return 1;
}

bool8 ArchipelagoSpecial_CheckReusableTms(void)
{
    return gArchipelagoOptions.reusableTms;
}

bool8 CanUseHmOutsideBattle(u8 fieldMove)
{
    u8 ObtainedBadgeMask = 0;
    if (FlagGet(FLAG_BADGE01_GET))
        ObtainedBadgeMask |= gBitTable[0];
    if (FlagGet(FLAG_BADGE02_GET))
        ObtainedBadgeMask |= gBitTable[1];
    if (FlagGet(FLAG_BADGE03_GET))
        ObtainedBadgeMask |= gBitTable[2];
    if (FlagGet(FLAG_BADGE04_GET))
        ObtainedBadgeMask |= gBitTable[3];
    if (FlagGet(FLAG_BADGE05_GET))
        ObtainedBadgeMask |= gBitTable[4];
    if (FlagGet(FLAG_BADGE06_GET))
        ObtainedBadgeMask |= gBitTable[5];
    if (FlagGet(FLAG_BADGE07_GET))
        ObtainedBadgeMask |= gBitTable[6];
    if (FlagGet(FLAG_BADGE08_GET))
        ObtainedBadgeMask |= gBitTable[7];

    return ((1 << fieldMove) & (ObtainedBadgeMask | gArchipelagoOptions.removeBadgeRequirement)) == (1 << fieldMove);
}

bool8 ArchipelagoSpecial_CanUseHmOutsideBattle(void)
{
    return CanUseHmOutsideBattle(gSpecialVar_0x8003);
}
