#ifndef GUARD_ARCHIPELAGO_H
#define GUARD_ARCHIPELAGO_H

#define PLAYER_NAME_BUFFER_SIZE 17 * 250
#define ITEM_NAME_BUFFER_SIZE 36 * 500
#define NAME_TABLE_BUFFER_SIZE (2 + 2 + 1) * 1500 // 2 bytes for location ID, 2 bytes for item name offset, 1 byte for player name ID

// Archipelago item classifications
enum {
    AP_CLASS_FILLER,
    AP_CLASS_USEFUL,
    AP_CLASS_PROGRESSION,
    AP_CLASS_TRAP,
};

struct ArchipelagoOptions
{
    /* 0x00 */ u16 introSpecies;
    /* 0x02 */ u16 pcItem;
    /* 0x04 */ u8 startingLocation;
    /* 0x05 */ u8 startingSpawn;

    /* 0x06 */ u16 expPercentMultiplier;
    /* 0x08 */ u8 optionsWindowFrameType:5;
               u8 normalizeEncounterRates:1;
               u8 optionsTextSpeed:2;
    /* 0x09 */ u8 optionsTurboButton:2; // 0=off, 1=A, 2=B, 3=A/B
               u8 optionsButtonMode:2;
               u8 optionsBattleScene:1;
               u8 optionsBattleStyle:1;
               u8 optionsSound:1; // mono/stereo
               u8 optionsSkipFanfares:1;
    /* 0x0A */ u8 optionsBikeMusic:1;
               u8 optionsSurfMusic:1;
               u8 optionsLowHpBeep:1;
               u8 optionsSkipNicknames:1;
               u8 optionsReceivedItemMessageFilter:2;
               u8 optionsReceivedItemSound:1;
               u8 optionsGuaranteedCatch:1;
    /* 0x0B */ u8 optionsGuaranteedRun:1;
               u8 optionsDeathLink:1;
               u8 optionsBlindTrainers:1;
               u8 optionsAutoRun:1; // automatic running shoes
               u8 reusableTms:1;
               u8 purgeSpinners:1;
               u8 matchTrainerLevels:1;
               u8 optionsGuaranteedReelFish:1;
    /* 0x0C */ u8 unlockSeenDexInfo;
    /* 0x0D */ s8 matchTrainerLevelBonus;
    /* 0x0E */ bool8 betterShopsEnabled;

    /* 0x0F */ bool8 eliteFourNeedsGyms;
    /* 0x10 */ u8 eliteFourRequiredCount;
    /* 0x11 */ bool8 normanNeedsGyms;
    /* 0x12 */ u8 normanRequiredCount;

    /* 0x13 */ u8 removeBadgeRequirement;
    /* 0x14 */ u8 additionalDarkCaves;
    /* 0x15 */ u8 freeFlyHmLocation;
    /* 0x16 */ u8 freeFlyPokenavLocation;
    /* 0x17 */ u8 terraCaveLocationId:4;
    /* 0x18 */ u8 marineCaveLocationId:4;

    /* 0x19 */ bool8 addRoute115Boulders;
    /* 0x1A */ bool8 addBumpySlopes;
    /* 0x1B */ bool8 modifyRoute118;
    /* 0x1C */ u16 removedBlockers;
    
    /* 0x1E */ bool8 berryTreesRandomized;
    /* 0x1F */ bool8 isTrainersanity;
    /* 0x20 */ bool8 isDexsanity;
    /* 0x21 */ bool8 isShopsanity;
    /* 0x22 */ bool8 flyUnlocks;
    /* 0x23 */ bool8 extraKeyItems;
    /* 0x24 */ bool8 gymKeys;
    /* 0x25 */ bool8 shuffleBag;
    /* 0x26 */ u8 shufflePokedex; // 0=no, 1=yes, 2=progressive
    /* 0x27 */ bool8 shufflePokenav;
    /* 0x28 */ bool8 shuffleRunningShoes;

    /* 0x?? */ u32 startingMoney;
    /* 0x30 */ u8 startingBadges;
    /* 0x31 */ bool8 wonderTradeAllowed;
    /* 0x32 */ bool8 remoteItems;
    /* 0x33 */ bool8 isChallengeMode;
};  // offsets may be incorrect from unlockSeenDexInfo to startingMoney. i'll worry about it as i implement those features...

struct ArchipelagoReceivedItem // for items received from the multiworld
{
    u16 itemId; // in-game item ID
    u16 itemIndex; // AP item ID
    bool8 isFilled; // whether an item is waiting to be claimed
    u8 itemClass:2; // AP item classification, for message filtering purposes
};

struct ArchipelagoReward // for items that belong to this slot
{
    u16 itemId; // in-game item ID to be received
    u16 locationId; // the flag ID that gave this item
};

struct ArchipelagoInfo
{
    u8 auth[16];
};

#define REWARD_QUEUE_SIZE 10
extern struct ArchipelagoReceivedItem gArchipelagoReceivedItem;
extern struct ArchipelagoReward gArchipelagoReceiveItemQueue[REWARD_QUEUE_SIZE];

extern const u8 gArchipelagoItemNames[];
extern const u8 gArchipelagoPlayerNames[];
extern const u8 gArchipelagoNameTable[];

extern const struct ArchipelagoOptions gArchipelagoOptions;

bool8 CheckQueuedRewards();
bool8 Archipelago_CheckReceivedItem();
bool8 ArchipelagoSpecial_IsItemSpecial(void);
bool8 CanUseHmOutsideBattle(u8 fieldMove);

#endif //GUARD_ARCHIPELAGO_H
