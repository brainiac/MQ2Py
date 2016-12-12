/* MQ2PyExt_Enum.cpp
 * Copyright (c) 2009 Stephen Raub.
 *
 * Distribution is not allowed without the consent of the author. 
 *
 * This module implements the enumerations for use in python
 */

#include "MQ2PyPCH.h"
#include "MQ2Py.h"
#include "MQ2PyExt.h"

using namespace boost;
using namespace boost::python;

namespace enumerations {

enum GameState
{
	CharSelect = GAMESTATE_CHARSELECT,
	CharCreate = GAMESTATE_CHARCREATE,
	InGame = GAMESTATE_INGAME,
	PreCharSelect = GAMESTATE_PRECHARSELECT,
	LoggingIn = GAMESTATE_LOGGINGIN,
	Unloading = GAMESTATE_UNLOADING
};

enum Deity
{
	Bertoxxulous = DEITY_Bertoxxulous,
	BrellSerilis = DEITY_BrellSerilis,
	CazicThule = DEITY_CazicThule,
	ErollisiMarr = DEITY_ErollisiMarr,
	Bristlebane = DEITY_Bristlebane,
	Innoruuk = DEITY_Innoruuk,
	Karana = DEITY_Karana,
	MithanielMarr = DEITY_MithanielMarr,
	Prexus = DEITY_Prexus,
	Quellious = DEITY_Quellious,
	RallosZek = DEITY_RallosZek,
	RodcetNife = DEITY_RodcetNife,
	SolusekRo = DEITY_SolusekRo,
	TheTribunal = DEITY_TheTribunal,
	Tunare = DEITY_Tunare,
	Veeshan = DEITY_Veeshan
};

enum UserColor
{
	Say = USERCOLOR_SAY,
	Tell = USERCOLOR_TELL,
	Group = USERCOLOR_GROUP,
	Guild = USERCOLOR_GUILD,
	OOC = USERCOLOR_OOC,
	Auction = USERCOLOR_AUCTION,
	Shout = USERCOLOR_SHOUT,
	Emote = USERCOLOR_EMOTE,
	Spells = USERCOLOR_SPELLS,
	YouHitOther = USERCOLOR_YOU_HIT_OTHER,
	OtherHitYou = USERCOLOR_OTHER_HIT_YOU,
	YouMissOther = USERCOLOR_YOU_MISS_OTHER,
	OtherMissYou = USERCOLOR_OTHER_MISS_YOU,
	Duels = USERCOLOR_DUELS,
	Skills = USERCOLOR_SKILLS,
	Disciplines = USERCOLOR_DISCIPLINES,
	//UNUSED001 = USERCOLOR_UNUSED001,
	Default = USERCOLOR_DEFAULT,
	//UNUSED002 = USERCOLOR_UNUSED002,
	MerchantOffer = USERCOLOR_MERCHANT_OFFER,
	MerchantExchange = USERCOLOR_MERCHANT_EXCHANGE,
	YourDeath = USERCOLOR_YOUR_DEATH,
	OtherDeath = USERCOLOR_OTHER_DEATH,
	OtherHitOther = USERCOLOR_OTHER_HIT_OTHER,
	OtherMissOther = USERCOLOR_OTHER_MISS_OTHER,
	Who = USERCOLOR_WHO,
	Yell = USERCOLOR_YELL,
	NonMelee = USERCOLOR_NON_MELEE,
	SpellWornOff = USERCOLOR_SPELL_WORN_OFF,
	MoneySplit = USERCOLOR_MONEY_SPLIT,
	Loot = USERCOLOR_LOOT,
	Random = USERCOLOR_RANDOM,
	OthersSpells = USERCOLOR_OTHERS_SPELLS,
	SpellFailure = USERCOLOR_SPELL_FAILURE,
	ChatChannel = USERCOLOR_CHAT_CHANNEL,
	Chat1 = USERCOLOR_CHAT_1,
	Chat2 = USERCOLOR_CHAT_2,
	Chat3 = USERCOLOR_CHAT_3,
	Chat4 = USERCOLOR_CHAT_4,
	Chat5 = USERCOLOR_CHAT_5,
	Chat6 = USERCOLOR_CHAT_6,
	Chat7 = USERCOLOR_CHAT_7,
	Chat8 = USERCOLOR_CHAT_8,
	Chat9 = USERCOLOR_CHAT_9,
	Chat10 = USERCOLOR_CHAT_10,
	MeleeCrit = USERCOLOR_MELEE_CRIT,
	SpellCrit = USERCOLOR_SPELL_CRIT,
	TooFarAway = USERCOLOR_TOO_FAR_AWAY,
	NPCRampage = USERCOLOR_NPC_RAMPAGE,
	NPCFlurry = USERCOLOR_NPC_FLURRY,
	NPCEnrage = USERCOLOR_NPC_ENRAGE,
	EchoSay = USERCOLOR_ECHO_SAY,
	EchoTell = USERCOLOR_ECHO_TELL,
	EchoGroup = USERCOLOR_ECHO_GROUP,
	EchoGuild = USERCOLOR_ECHO_GUILD,
	EchoOOC = USERCOLOR_ECHO_OOC,
	EchoAuction= USERCOLOR_ECHO_AUCTION,
	EchoShout = USERCOLOR_ECHO_SHOUT,
	EchoChatEmote = USERCOLOR_ECHO_EMOTE,
	EchoChat1 = USERCOLOR_ECHO_CHAT_1,
	EchoChat2 = USERCOLOR_ECHO_CHAT_2,
	EchoChat3 = USERCOLOR_ECHO_CHAT_3,
	EchoChat4 = USERCOLOR_ECHO_CHAT_4,
	EchoChat5 = USERCOLOR_ECHO_CHAT_5,
	EchoChat6 = USERCOLOR_ECHO_CHAT_6,
	EchoChat7 = USERCOLOR_ECHO_CHAT_7,
	EchoChat8 = USERCOLOR_ECHO_CHAT_8,
	EchoChat9 = USERCOLOR_ECHO_CHAT_9,
	EchoChat10 = USERCOLOR_ECHO_CHAT_10,
	Reserved = USERCOLOR_RESERVED,
	Link = USERCOLOR_LINK,
	Raid = USERCOLOR_RAID,
	Pet = USERCOLOR_PET,
	DamageShield = USERCOLOR_DAMAGESHIELD,
	Leader = USERCOLOR_LEADER,
	PetRampFlurry = USERCOLOR_PETRAMPFLURRY,
	PetCrits = USERCOLOR_PETCRITS,
	Focus = USERCOLOR_FOCUS,
	XP = USERCOLOR_XP,
	System = USERCOLOR_SYSTEM,
	PetSpells = USERCOLOR_PET_SPELLS,
	PetResponses = USERCOLOR_PET_RESPONSES,
	ItemSpeech = USERCOLOR_ITEM_SPEECH,
	StrikeThrough = USERCOLOR_STRIKETHROUGH,
	Stun = USERCOLOR_STUN,
	Reserved2 = USERCOLOR_RESERVED2,
	Fellowship = USERCOLOR_FELLOWSHIP,
	NPCSpeech = USERCOLOR_NPC_SPEECH,
	NPCSpeechToYou = USERCOLOR_NPC_SPEECH_TO_YOU,
	GuildMsg = USERCOLOR_GUILD_MSG
};

enum StandState
{
	Stand = STANDSTATE_STAND,
	Casting = STANDSTATE_CASTING,
	BindWound = STANDSTATE_BIND,
	Sit = STANDSTATE_SIT,
	Duck = STANDSTATE_DUCK,
	FeignDeath = STANDSTATE_FEIGN,
	Dead = STANDSTATE_DEAD
};

void Initialize_Enumerations()
{
	enum_<GameState>("GameState", "Enumeration of possible gamestates returned from GameState or passed to SetGameState")
		.value("CharSelect", CharSelect)
		.value("CharCreate", CharCreate)
		.value("InGame", InGame)
		.value("PreCharSelect", PreCharSelect)
		.value("LoggingIn", LoggingIn)
		.value("Unloading", Unloading);

	enum_<UserColor>("UserColor")
		.value("Say", Say)
		.value("Tell", Tell)
		.value("Group", Group)
		.value("Guild", Guild)
		.value("OOC", OOC)
		.value("Auction", Auction)
		.value("Shout", Shout)
		.value("Emote", Emote)
		.value("Spells", Spells)
		.value("YouHitOther", YouHitOther)
		.value("OtherHitYou", OtherHitYou)
		.value("YouMissOther", YouMissOther)
		.value("OtherMissYou", OtherMissYou)
		.value("Duels", Duels)
		.value("Skills", Skills)
		.value("Disciplines", Disciplines)
		.value("Default", Default)
		.value("MerchantOffer", MerchantOffer)
		.value("MerchantExchange", MerchantExchange)
		.value("YourDeath", YourDeath)
		.value("OtherDeath", OtherDeath)
		.value("OtherHitOther", OtherHitOther)
		.value("OtherMissOther", OtherMissOther)
		.value("Who", Who)
		.value("Yell", Yell)
		.value("NonMelee", NonMelee)
		.value("SpellWornOff", SpellWornOff)
		.value("MoneySplit", MoneySplit)
		.value("Loot", Loot)
		.value("Random", Random)
		.value("OthersSpells", OthersSpells)
		.value("SpellFailure", SpellFailure)
		.value("ChatChannel", ChatChannel)
		.value("Chat1", Chat1)
		.value("Chat2", Chat2)
		.value("Chat3", Chat3)
		.value("Chat4", Chat4)
		.value("Chat5", Chat5)
		.value("Chat6", Chat6)
		.value("Chat7", Chat7)
		.value("Chat8", Chat8)
		.value("Chat9", Chat9)
		.value("Chat10", Chat10)
		.value("MeleeCrit", MeleeCrit)
		.value("SpellCrit", SpellCrit)
		.value("TooFarAway", TooFarAway)
		.value("NPCRampage", NPCRampage)
		.value("NPCFlurry", NPCFlurry)
		.value("NPCEnrage", NPCEnrage)
		.value("EchoSay", EchoSay)
		.value("EchoTell", EchoTell)
		.value("EchoGroup", EchoGroup)
		.value("EchoGuild", EchoGuild)
		.value("EchoOOC", EchoOOC)
		.value("EchoAuction", EchoAuction)
		.value("EchoShout", EchoShout)
		.value("EchoChatEmote", EchoChatEmote)
		.value("EchoChat1", EchoChat1)
		.value("EchoChat2", EchoChat2)
		.value("EchoChat3", EchoChat3)
		.value("EchoChat4", EchoChat4)
		.value("EchoChat5", EchoChat5)
		.value("EchoChat6", EchoChat6)
		.value("EchoChat7", EchoChat7)
		.value("EchoChat8", EchoChat8)
		.value("EchoChat9", EchoChat9)
		.value("EchoChat10", EchoChat10)
		.value("Reserved", Reserved)
		.value("Link", Link)
		.value("Raid", Raid)
		.value("Pet", Pet)
		.value("DamageShield", DamageShield)
		.value("Leader", Leader)
		.value("PetRampFlurry", PetRampFlurry)
		.value("PetCrits", PetCrits)
		.value("Focus", Focus)
		.value("XP", XP)
		.value("System", System)
		.value("PetSpells", PetSpells)
		.value("PetResponses", PetResponses)
		.value("ItemSpeech", ItemSpeech)
		.value("StrikeThrough", StrikeThrough)
		.value("Stun", Stun)
		.value("Reserved2", Reserved2)
		.value("Fellowship", Fellowship)
		.value("NPCSpeech", NPCSpeech)
		.value("NPCSpeechToYou", NPCSpeechToYou)
		.value("GuildMsg", GuildMsg);

	enum_<Deity>("Deity")
		.value("Bertoxxulous", Bertoxxulous)
		.value("BrellSerilis", BrellSerilis)
		.value("CazicThule", CazicThule)
		.value("ErollisiMarr", ErollisiMarr)
		.value("Bristlebane", Bristlebane)
		.value("Innoruuk", Innoruuk)
		.value("Karana", Karana)
		.value("MithanielMarr", MithanielMarr)
		.value("Prexus", Prexus)
		.value("Quellious", Quellious)
		.value("RallosZek", RallosZek)
		.value("RodcetNife", RodcetNife)
		.value("SolusekRo", SolusekRo)
		.value("TheTribunal", TheTribunal)
		.value("Tunare", Tunare)
		.value("Veeshan", Veeshan);

	enum_<PlayerClass>("PlayerClass", "Enumeration of values used to identify player classes")
		.value("Unknown", Unknown)
		.value("Warrior", Warrior)
		.value("Cleric", Cleric)
		.value("Paladin", Paladin)
		.value("Ranger", Ranger)
		.value("Shadowknight", Shadowknight)
		.value("Druid", Druid)
		.value("Monk", Monk)
		.value("Bard", Bard)
		.value("Rogue", Rogue)
		.value("Shaman", Shaman)
		.value("Necromancer", Necromancer)
		.value("Wizard", Wizard)
		.value("Mage", Mage)
		.value("Enchanter", Enchanter)
		.value("Beastlord", Beastlord)
		.value("Berserker", Berserker);

	enum_<StandState>("StandState")
		.value("Stand", Stand)
		.value("Casting", Casting)
		.value("BindWound", BindWound)
		.value("Sit", Sit)
		.value("Duck", Duck)
		.value("FeignDeath", FeignDeath)
		.value("Dead", Dead);

	enum_<eSpawnType>("SpawnType")
		.value("None", NONE)
		.value("PC", PC)
		.value("Mount", MOUNT)
		.value("Pet", PET)
		.value("NPC", NPC)
		.value("Corpse", CORPSE)
		.value("Trigger", TRIGGER)
		.value("Trap", TRAP)
		.value("Timer", TIMER)
		.value("Untargetable", UNTARGETABLE)
		.value("Chest", CHEST)
		.value("Item", ITEM)
		.value("Aura", AURA)
		.value("Object", OBJECT)
		.value("Banner", BANNER)
		.value("Mercenary", MERCENARY)
		.value("Flyer", FLYER);
}

}

void Init_Module_PyMQ2_Enum()
{
	enumerations::Initialize_Enumerations();
}