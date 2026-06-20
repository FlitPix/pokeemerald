.include "data/text/archipelago.inc"

ArchipelagoScript_ReceiveReward::
	specialvar VAR_RESULT, ArchipelagoSpecial_PopReward
	giveitem VAR_RESULT, 1, VAR_0x8003
	releaseall
	end

ArchipelagoScript_ReceiveRemoteItem::
	lockall
	specialvar VAR_RESULT, ArchipelagoSpecial_ConsumeReceivedItem
	giveitem VAR_RESULT
	releaseall
	end

ArchipelagoScript_ReceiveRemoteItemSilent::
	specialvar VAR_RESULT, ArchipelagoSpecial_ConsumeReceivedItem
	additem VAR_RESULT, 1
	end

ArchipelagoScript_HandleTextBox::
	bufferapitemstrings STR_VAR_1, STR_VAR_2, VAR_0x8003
	goto_if_eq VAR_RESULT, 0, ArchipelagoScript_FoundArchipelagoItem
	goto_if_eq VAR_RESULT, 1, ArchipelagoScript_FoundPlayersItem
	return

ArchipelagoScript_FoundArchipelagoItem::
	call EventScript_PlayFanfareObtainedItem
	msgbox gText_PlayerFoundArchipelagoItem, MSGBOX_DEFAULT
	waitfanfare
	waitmessage
	return

ArchipelagoScript_FoundPlayersItem::
	call EventScript_PlayFanfareObtainedItem
	msgbox gText_PlayerFoundPlayersItem, MSGBOX_DEFAULT
	waitmessage
	waitfanfare
	return

ArchipelagoScript_ObtainItem::
	specialvar VAR_RESULT, ArchipelagoSpecial_IsItemSpecial
	goto_if_eq VAR_RESULT, TRUE, ArchipelagoScript_HandleSpecialItem
	call ArchipelagoScript_HandleTextBox
	setvar VAR_RESULT, TRUE
	return

ArchipelagoScript_FindItem::
	removeobject VAR_LAST_TALKED
	specialvar VAR_RESULT, ArchipelagoSpecial_IsItemSpecial
	goto_if_eq VAR_RESULT, TRUE, ArchipelagoScript_HandleSpecialItem
	call ArchipelagoScript_HandleTextBox
	release
	return

ArchipelagoScript_HandleSpecialItem::
	switch VAR_AP_ITEM
	case ITEM_BADGE_1, ArchipelagoScript_ReceiveStoneBadge
	case ITEM_BADGE_2, ArchipelagoScript_ReceiveKnuckleBadge
	case ITEM_BADGE_3, ArchipelagoScript_ReceiveDynamoBadge
	case ITEM_BADGE_4, ArchipelagoScript_ReceiveHeatBadge
	case ITEM_BADGE_5, ArchipelagoScript_ReceiveBalanceBadge
	case ITEM_BADGE_6, ArchipelagoScript_ReceiveFeatherBadge
	case ITEM_BADGE_7, ArchipelagoScript_ReceiveMindBadge
	case ITEM_BADGE_8, ArchipelagoScript_ReceiveRainBadge
	case ITEM_POKEDEX, ArchipelagoScript_ReceiveHoennPokedex
	case ITEM_NATIONAL_POKEDEX, ArchipelagoScript_ReceiveNationalPokedex
	case ITEM_PROGRESSIVE_POKEDEX, ArchipelagoScript_ReceiveProgressivePokedex
	case ITEM_POKENAV, ArchipelagoScript_ReceivePokenav
	case ITEM_BAG, ArchipelagoScript_ReceiveBag
	case ITEM_RUNNING_SHOES, ArchipelagoScript_ReceiveRunningShoes
	return

ArchipelagoScript_ReceiveProgressivePokedex::
	specialvar VAR_RESULT, ArchipelagoSpecial_HandleProgressivePokedex
	goto_if_eq VAR_RESULT, 0, ArchipelagoScript_ReceiveHoennPokedex
	goto_if_eq VAR_RESULT, 1, ArchipelagoScript_ReceiveNationalPokedex

ArchipelagoScript_ReceiveHoennPokedex::
	call EventScript_PlayFanfareObtainedItem
	msgbox LittlerootTown_ProfessorBirchsLab_Text_ReceivedPokedex, MSGBOX_DEFAULT
	waitfanfare
	setflag FLAG_SYS_POKEDEX_GET
	special SetUnlockedPokedexFlags
	release
	return

ArchipelagoScript_ReceiveNationalPokedex::
	call EventScript_PlayFanfareObtainedItem
	msgbox LittlerootTown_ProfessorBirchsLab_Text_PokedexUpgradedToNational, MSGBOX_DEFAULT
	waitfanfare
	setflag FLAG_SYS_NATIONAL_DEX
	special EnableNationalPokedex
	release
	return

ArchipelagoScript_ReceivePokenav::
	call EventScript_PlayFanfareObtainedItem
	msgbox RustboroCity_DevonCorp_3F_Text_ReceivedPokenav, MSGBOX_DEFAULT
	waitfanfare
	setflag FLAG_SYS_POKENAV_GET
	setflag FLAG_HAS_MATCH_CALL
	setflag FLAG_ADDED_MATCH_CALL_TO_POKENAV
	release
	return

ArchipelagoScript_ReceiveBag::
	call EventScript_PlayFanfareObtainedItem
	msgbox gText_PlayerReceivedBag, MSGBOX_DEFAULT
	waitfanfare
	setflag FLAG_SYS_BAG_GET
	release
	return

ArchipelagoScript_ReceiveRunningShoes::
	call EventScript_PlayFanfareObtainedItem
	msgbox gText_PlayerReceivedRunningShoes, MSGBOX_DEFAULT
	waitfanfare
	setflag FLAG_SYS_B_DASH
	msgbox LittlerootTown_Text_SwitchShoesWithRunningShoes, MSGBOX_DEFAULT
	closemessage
	release
	return

ArchipelagoScript_ReceiveStoneBadge::
	call Common_EventScript_PlayGymBadgeFanfare
	msgbox RustboroCity_Gym_Text_ReceivedStoneBadge, MSGBOX_DEFAULT
	waitfanfare
	setflag FLAG_BADGE01_GET
	release
	return

ArchipelagoScript_ReceiveKnuckleBadge::
	call Common_EventScript_PlayGymBadgeFanfare
	msgbox DewfordTown_Gym_Text_ReceivedKnuckleBadge, MSGBOX_DEFAULT
	waitfanfare
	setflag FLAG_BADGE02_GET
	release
	return

ArchipelagoScript_ReceiveDynamoBadge::
	call Common_EventScript_PlayGymBadgeFanfare
	msgbox MauvilleCity_Gym_Text_ReceivedDynamoBadge, MSGBOX_DEFAULT
	waitfanfare
	setflag FLAG_BADGE03_GET
	release
	return

ArchipelagoScript_ReceiveHeatBadge::
	call Common_EventScript_PlayGymBadgeFanfare
	msgbox LavaridgeTown_Gym_1F_Text_ReceivedHeatBadge, MSGBOX_DEFAULT
	waitfanfare
	setflag FLAG_BADGE04_GET
	release
	return

ArchipelagoScript_ReceiveBalanceBadge::
	call Common_EventScript_PlayGymBadgeFanfare
	msgbox PetalburgCity_Gym_Text_ReceivedBalanceBadge, MSGBOX_DEFAULT
	waitfanfare
	setflag FLAG_BADGE05_GET
	release
	return

ArchipelagoScript_ReceiveFeatherBadge::
	call Common_EventScript_PlayGymBadgeFanfare
	msgbox FortreeCity_Gym_Text_ReceivedFeatherBadge, MSGBOX_DEFAULT
	waitfanfare
	setflag FLAG_BADGE06_GET
	release
	return

ArchipelagoScript_ReceiveMindBadge::
	call Common_EventScript_PlayGymBadgeFanfare
	msgbox MossdeepCity_Gym_Text_ReceivedMindBadge, MSGBOX_DEFAULT
	waitfanfare
	setflag FLAG_BADGE07_GET
	release
	return

ArchipelagoScript_ReceiveRainBadge::
	call Common_EventScript_PlayGymBadgeFanfare
	msgbox SootopolisCity_Gym_1F_Text_ReceivedRainBadge, MSGBOX_DEFAULT
	waitfanfare
	setflag FLAG_BADGE08_GET
	release
	return

ArchipelagoScript_GoHome::
	checkplayergender
	compare VAR_RESULT, FEMALE
	goto_if_eq ArchipelagoScript_GoHomeFemale
	warpteleport MAP_LITTLEROOT_TOWN, 5, 9
	call EverGrandeCity_HallOfFame_EventScript_ResetEliteFour
	goto EventScript_MoveMrBrineyToHouse

ArchipelagoScript_GoHomeFemale::
	warpteleport MAP_LITTLEROOT_TOWN, 14, 9
	call EverGrandeCity_HallOfFame_EventScript_ResetEliteFour
	goto EventScript_MoveMrBrineyToHouse
