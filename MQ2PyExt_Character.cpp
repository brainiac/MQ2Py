/* MQ2PyExt_Character.cpp
 * Copyright (c) 2009 Stephen Raub.
 *
 * Distribution is not allowed without the consent of the author. 
 *
 * This module implements the character info support
 */

#include "MQ2PyPCH.h"
#include "MQ2Py.h"
#include "MQ2PyExt.h"
#include "MQ2PyExt_Spawn.h"
#include "MQ2PyExt_Spell.h"
#include "MQ2PyExt_Character.h"

using namespace std;
using namespace boost;
using namespace boost::python;

//const int SIZE_OF_BUFFS = sizeof(_CHARINFO2::Buff) / sizeof(_SPELLBUFF);
//const int SIZE_OF_SHORTBUFFS = sizeof(_CHARINFO2::ShortBuff) / sizeof(_SPELLBUFF);

//----------------------------------------------------------------------------

#pragma region SpellBuffInfo
SpellBuffInfo::SpellBuffInfo() : spell()
{
	level = 0;
	duration = 0;
	modifier = 0;
	absorb = 0;
}

SpellBuffInfo::SpellBuffInfo(_SPELLBUFF const& info) : spell(info.SpellID)
{
	level = info.Level;
	duration = info.Duration;
	modifier = info.Modifier;
	absorb = info.DamageAbsorbRemaining;		
}

std::string SpellBuffInfo::__repr__()
{
	ostringstream oss;
	oss << "Buff: ";

	if (!IsValid())
		oss << "<Invalid>";
	else
		oss << spell.Name() << " (L" << level << ")";

	return oss.str();
}

PythonSpell SpellBuffInfo::Spell()
{
	return spell;
}

int SpellBuffInfo::Level()
{
	return level;
}

int SpellBuffInfo::Absorb()
{
	return absorb;
}

float SpellBuffInfo::Modifier()
{
	return (float)modifier / 10.0f;
}

int SpellBuffInfo::Duration()
{
	return (duration + 1) * 6;
}

bool SpellBuffInfo::IsValid()
{
	return spell.IsValid() && spell.Id() > 0;
}
#pragma endregion

//----------------------------------------------------------------------------
//
PythonCharacter::PythonCharacter()
{
	pChar = GetCharInfo();
	Valid = false;

	if (pChar != NULL) {
		SetSpawn(GetCharInfo()->pSpawn);
	}
}


bool PythonCharacter::IsValid()
{
	if (!Valid) {
		pChar = GetCharInfo();
		if (pChar != NULL) {
			SetSpawn(GetCharInfo()->pSpawn);
		} 
	}

	return Valid;
}

//----------------------------------------------------------------------------
// Experience

#pragma region Experience
int PythonCharacter::XP()
{
	AssertIsValid();
	return pChar->Exp;
}

float PythonCharacter::PctXP()
{
	AssertIsValid();
	return (float)pChar->Exp / 3.30f;
}

int PythonCharacter::AAXP()
{
	AssertIsValid();
	return pChar->AAExp;
}

float PythonCharacter::PctAAXP()
{
	AssertIsValid();
	return (float)pChar->AAExp / 3.30f;
}

int PythonCharacter::AAPoints()
{
	AssertIsValid();
	return GetCharInfo2()->AAPoints;
}

int PythonCharacter::AAPointsSpent()
{
	AssertIsValid();
	return GetCharInfo2()->AAPointsSpent;
}

int PythonCharacter::AAPointsTotal()
{
	AssertIsValid();
	return GetCharInfo2()->AAPoints + GetCharInfo2()->AAPointsSpent;
}

python::dict PythonCharacter::LeaderAbilities()
{
	AssertIsValid();

	python::dict d = python::dict();
	
	d["Mark NPC"] = pChar->ActiveAbilities.MarkNPC;
	d["NPC Health"] = pChar->ActiveAbilities.NPCHealth;
	d["Delegate Mark NPC"] = pChar->ActiveAbilities.DelegateMarkNPC;
	d["Delegate Main Assist"] = pChar->ActiveAbilities.DelegateMA;
	d["Inspect Buffs"] = pChar->ActiveAbilities.InspectBuffs;
	d["Spell Awareness"] = pChar->ActiveAbilities.SpellAwareness;
	d["Offense Enhancement"] = pChar->ActiveAbilities.OffenseEnhancement;
	d["Mana Enhancement"] = pChar->ActiveAbilities.ManaEnhancement;
	d["Health Enhancement"] = pChar->ActiveAbilities.HealthEnhancement;
	d["Health Regeneration"] = pChar->ActiveAbilities.HealthRegen;
	d["Find Path to PC"] = pChar->ActiveAbilities.FindPathPC;
	d["Health of Target's Target"] = pChar->ActiveAbilities.HoTT;

	return d;
}

float PythonCharacter::GroupLeaderXP()
{
	AssertIsValid();
	return (float)pChar->GroupLeadershipExp;
}

float PythonCharacter::PctGroupLeaderXP()
{
	AssertIsValid();
	return (float)pChar->GroupLeadershipExp / 10.0f;
}

int PythonCharacter::GroupLeaderPoints()
{
	AssertIsValid();
	return pChar->GroupLeadershipPoints;
}

float PythonCharacter::RaidLeaderXP()
{
	AssertIsValid();
	return (float)pChar->RaidLeadershipExp;
}

float PythonCharacter::PctRaidLeaderXP()
{
	AssertIsValid();
	return (float)pChar->RaidLeadershipExp / 10.0f;
}

int PythonCharacter::RaidLeaderPoints()
{
	AssertIsValid();
	return pChar->RaidLeadershipPoints;
}

#pragma endregion

//----------------------------------------------------------------------------
// Mana

int PythonCharacter::CurrentMana()
{
	AssertIsValid();
	return GetCharInfo2()->Mana;
}

float PythonCharacter::PctMana()
{
	AssertIsValid();
	if (int mana = GetMaxMana())
		return (float)GetCharInfo2()->Mana * 100.0f / (float)mana;
	
	return 0.0f;
}

int PythonCharacter::MaxMana()
{
	AssertIsValid();
	return GetMaxMana();
}

//----------------------------------------------------------------------------
// Endurance

int PythonCharacter::Endurance()
{
	AssertIsValid();
	return GetCharInfo2()->Endurance;
}

float PythonCharacter::PctEndurance()
{
	AssertIsValid();
	if (int end = GetMaxEndurance())
		return (float)GetCharInfo2()->Endurance * 100.0f / (float)end;

	return 0.0f;
}

int PythonCharacter::MaxEndurance()
{
	AssertIsValid();
	return GetMaxEndurance();
}

//----------------------------------------------------------------------------
// Skills

python::dict PythonCharacter::Skills()
{
	AssertIsValid();

	python::dict d = python::dict();

	for (int skill = 0; skill < NUM_SKILLS; skill++) {
		int skillLevel = GetCharInfo2()->Skill[skill];
		if (skillLevel > 0) {
			std::string name = szSkills[skill];
			d[name] = GetCharInfo2()->Skill[skill];		
		}
	}

	return d;
}

python::list PythonCharacter::Abilities()
{
	AssertIsValid();

	python::list d = python::list();

	return d;
}

//----------------------------------------------------------------------------
// Inventory

python::list PythonCharacter::Inventory()
{
	python::list items = python::list();

	PCHARINFO2 pChar2 = GetCharInfo2();
	for (unsigned int slot = 0; slot < NUM_INV_SLOTS; slot++) {
		PCONTENTS contents = pChar2->pInventoryArray->InventoryArray[slot];
		if (contents != NULL) {
			items.append(new PythonItem(contents));
		}
	}

	return items;
}


//----------------------------------------------------------------------------
// Statistics

#pragma region Stats
python::dict PythonCharacter::Resists()
{
	AssertIsValid();

	python::dict d = python::dict();

	d["Magic"] = pChar->SaveMagic;
	d["Fire"] = pChar->SaveFire;
	d["Cold"] = pChar->SaveCold;
	d["Poison"] = pChar->SavePoison;
	d["Disease"] = pChar->SaveDisease;
	d["Corruption"] = pChar->SaveCorruption;

	// Calculate 'Prismatic' Resist (Average)
	d["Prismatic"] = (pChar->SaveMagic + pChar->SaveFire
		+ pChar->SaveCold + pChar->SavePoison 
		+ pChar->SaveDisease) / 5;

	// Calculate 'Chromatic' Resist (lowest)
	unsigned int lowest = pChar->SaveMagic;
	if (lowest > pChar->SaveFire)
		lowest = pChar->SaveFire;
	if (lowest > pChar->SaveCold)
		lowest = pChar->SaveCold;
	if (lowest > pChar->SavePoison)
		lowest = pChar->SavePoison;
	if (lowest > pChar->SaveDisease)
		lowest = pChar->SaveDisease;
	d["Chromatic"] = lowest;

	return d;	
}

python::dict PythonCharacter::Stats()
{
	AssertIsValid();

	python::dict d = python::dict();

	d["STR"] = pChar->STR;
	d["STA"] = pChar->STA;
	d["AGI"] = pChar->AGI;
	d["DEX"] = pChar->DEX;
	d["WIS"] = pChar->WIS;
	d["INT"] = pChar->INT;
	d["CHA"] = pChar->CHA;

	return d;
}

python::dict PythonCharacter::StatBonuses()
{
	AssertIsValid();

	python::dict d = python::dict();

	d["HP"]				= pChar->HPBonus;
	d["Mana"]			= pChar->ManaBonus;
	d["Endurance"]		= pChar->EnduranceBonus;
	d["CombatEffects"]	= pChar->CombatEffectsBonus;
	d["Shielding"]		= pChar->ShieldingBonus;
	d["SpellShield"]	= pChar->SpellShieldBonus;
	d["Avoidance"]		= pChar->AvoidanceBonus;
	d["Accuracy"]		= pChar->AccuracyBonus;
	d["StunResist"]		= pChar->StunResistBonus;
	d["StrikeThrough"]	= pChar->StrikeThroughBonus;
	d["DoTShield"]		= pChar->DoTShieldBonus;
	d["Attack"]			= pChar->AttackBonus;
	d["HPRegen"]		= pChar->HPRegenBonus;
	d["ManaRegen"]		= pChar->ManaRegenBonus;
	d["DamageShield"]	= pChar->DamageShieldBonus;
	d["DamageShieldMitigation"] = pChar->DamageShieldMitigationBonus;
	d["Heroic STR"]		= pChar->HeroicSTRBonus;
	d["Heroic INT"]		= pChar->HeroicINTBonus;
	d["Heroic WIS"]		= pChar->HeroicWISBonus;
	d["Heroic AGI"]		= pChar->HeroicAGIBonus;
	d["Heroic DEX"]		= pChar->HeroicDEXBonus;
	d["Heroic STA"]		= pChar->HeroicSTABonus;
	d["Heroic CHA"]		= pChar->HeroicCHABonus;
	d["HealAmount"]		= pChar->HealAmountBonus;
	d["SpellDamage"]	= pChar->SpellDamageBonus;
	d["Clairvoyance"]	= pChar->ClairvoyanceBonus;
	d["EnduranceRegen"] = pChar->EnduranceRegenBonus;
	
	return d;
}

int PythonCharacter::AttackSpeed()
{
	AssertIsValid();
	return pChar->AttackSpeed;
}

int PythonCharacter::HPRegen()
{
	AssertIsValid();
	return HealthGained;
}

int PythonCharacter::ManaRegen()
{
	AssertIsValid();
	return ManaGained;
}

int PythonCharacter::EnduranceRegen()
{
	AssertIsValid();
	return EnduranceGained;
}

int PythonCharacter::DamageAbsorbRemaining()
{
	AssertIsValid();

	int absorb = 0;
	for (int i = 0; i < 25; i++) {
		if (PSPELL pSpell = GetSpellByID(GetCharInfo2()->Buff[i].SpellID))
			if (pSpell->SpellType != 0)
				absorb += GetCharInfo2()->Buff[i].DamageAbsorbRemaining;
	}

	return absorb;
}
#pragma endregion

//----------------------------------------------------------------------------
// Tribute

#pragma region Tribute
int PythonCharacter::CurrentFavor()
{
	AssertIsValid();
	return pChar->CurrFavor;
}

int PythonCharacter::CareerFavor()
{
	AssertIsValid();
	return pChar->CareerFavor;
}

bool PythonCharacter::TributeActive()
{
	AssertIsValid();
	return *pTributeActive;
}

int PythonCharacter::TributeTimer()
{
	AssertIsValid();
	return pChar->TributeTimer / 1000;
}

int PythonCharacter::ActiveFavorCost()
{
	AssertIsValid();
	
	if (*pTributeActive) {
		return pEQMisc->GetActiveFavorCost();
	}

	return 0;
}
#pragma endregion

//----------------------------------------------------------------------------
// Currency

#pragma region Currencies
int PythonCharacter::Platinum()
{
	AssertIsValid();
	return GetCharInfo2()->Plat;
}

int PythonCharacter::Gold()
{
	AssertIsValid();
	return GetCharInfo2()->Gold;
}

int PythonCharacter::Silver()
{
	AssertIsValid();
	return GetCharInfo2()->Silver;
}

int PythonCharacter::Copper()
{
	AssertIsValid();
	return GetCharInfo2()->Copper;
}

int PythonCharacter::PlatinumShared()
{
	AssertIsValid();
	return pChar->BankSharedPlat;
}

int PythonCharacter::PlatinumBank()
{
	AssertIsValid();
	return pChar->BankPlat;
}

int PythonCharacter::GoldBank()
{
	AssertIsValid();
	return pChar->BankGold;
}

int PythonCharacter::SilverBank()
{
	AssertIsValid();
	return pChar->BankSilver;
}

int PythonCharacter::CopperBank()
{
	AssertIsValid();
	return pChar->BankCopper;
}

int PythonCharacter::Cash()
{
	AssertIsValid();

	PCHARINFO2 pChar2 = GetCharInfo2();
	return pChar2->Plat  * 1000 
		+ pChar2->Gold   * 100
		+ pChar2->Silver * 10
		+ pChar2->Copper;
}

int PythonCharacter::CashBank()
{
	AssertIsValid();
	return pChar->BankPlat  * 1000
		+ pChar->BankGold   * 100
		+ pChar->BankSilver * 10 
		+ pChar->BankCopper;
}


int PythonCharacter::RadiantCrystals()
{
	AssertIsValid();
	return pChar->RadiantCrystals;
}

int PythonCharacter::EbonCrystals()
{
	AssertIsValid();
	return pChar->EbonCrystals;
}

python::dict PythonCharacter::AltCurrency()
{
	AssertIsValid();

	python::dict d = python::dict();

	d["Radiant Crystals"]	= pChar->RadiantCrystals;
	d["Ebon Crystals"]		= pChar->EbonCrystals;
	d["Doubloons"]			= pPlayerPointManager->GetAltCurrency(ALTCURRENCY_DOUBLOONS);;
	d["Orux"]				= pPlayerPointManager->GetAltCurrency(ALTCURRENCY_ORUX);
	d["Phosphenes"]			= pPlayerPointManager->GetAltCurrency(ALTCURRENCY_PHOSPHENES);
	d["Phosphites"]			= pPlayerPointManager->GetAltCurrency(ALTCURRENCY_PHOSPHITES);
	d["Faycites"]			= pPlayerPointManager->GetAltCurrency(ALTCURRENCY_FAYCITES);
	d["Chronobines"]		= pPlayerPointManager->GetAltCurrency(ALTCURRENCY_CHRONOBINES);

	return d;
}
#pragma endregion

//----------------------------------------------------------------------------
// Hunger / Thirst

#pragma region Hunger & Thirst
int PythonCharacter::Hunger()
{
	AssertIsValid();
	return GetCharInfo2()->hungerlevel;
}

int PythonCharacter::Thirst()
{
	AssertIsValid();
	return GetCharInfo2()->thirstlevel;
}

int PythonCharacter::Drunk()
{
	AssertIsValid();
	return GetCharInfo2()->Drunkenness;
}
#pragma endregion

//----------------------------------------------------------------------------
// Misc

#pragma region Miscellaneous Properties
std::string PythonCharacter::CombatState()
{
	AssertIsValid();

	switch (((PCPLAYERWND)pPlayerWnd)->CombatState)
	{
	case 0:
		if (((CXWnd*)pPlayerWnd)->GetChildItem("PW_CombatStateAnim"))
			return "Combat";
		break;

	case 1:	return "Debuffed";
	case 2: return "Cooldown";
	case 3: return "Active";
	case 4: return "Resting";
	}
	
	return "Unknown";
}

int PythonCharacter::Downtime()
{
	AssertIsValid();

	if (pChar->DowntimeStamp)
		return pChar->Downtime - (GetFastTime() - pChar->DowntimeStamp);

	return 0;
}

bool PythonCharacter::Moving()
{
	AssertIsValid();
	return (gbMoving && pSpawn->SpeedRun == 0.0f && pSpawn->Mount == NULL)
		|| fabs(FindSpeed(pSpawn)) > 0.0f;
}

bool PythonCharacter::Stunned()
{
	AssertIsValid();
	return pChar->Stunned == 1;
}

bool PythonCharacter::RangedReady()
{
	AssertIsValid();
	return gbRangedAttackReady;
}

bool PythonCharacter::Shrouded()
{
	AssertIsValid();
	return GetCharInfo2()->Shrouded;
}

bool PythonCharacter::Autofire()
{
	AssertIsValid();
	return gAutoFire;
}

int PythonCharacter::CurrentWeight()
{
	AssertIsValid();
	return pChar->CurrWeight;
}

bool PythonCharacter::Running()
{
	AssertIsValid();
	return *EQADDR_RUNWALKSTATE;
}

bool PythonCharacter::AltTimerReady()
{
	AssertIsValid();
	return gbAltTimerReady;
}

bool PythonCharacter::Combat()
{
	AssertIsValid();
	return *EQADDR_ATTACK;
}
#pragma endregion

//----------------------------------------------------------------------------
// Spell Gems

#pragma region Spell Gems
python::dict PythonCharacter::SpellGems()
{
	AssertIsValid();

	python::dict d = python::dict();

	for (int i = 0; i < GemSlots(); i++) {
		if (PSPELL spell = GetSpellByID(GetCharInfo2()->MemorizedSpells[i]))
			d[i+1] = PythonSpell(spell->ID);
		else
			d[i+1] = PythonSpell();
	}

	return d;
}

PythonSpell PythonCharacter::SpellGem_Int(int gem)
{
	AssertIsValid();

	if (gem > NUM_SPELL_GEMS || gem <= 0) {
		PyErr_SetString(PyExc_RuntimeError, "Invalid Gem Slot");
		throw_error_already_set();
	}

	return PythonSpell(GetCharInfo2()->MemorizedSpells[gem]);
}

int PythonCharacter::SpellGem_String(std::string name)
{
	AssertIsValid();

	for (int gem = 0; gem < NUM_SPELL_GEMS; gem++) {
		if (PSPELL pSpell = GetSpellByID(GetCharInfo2()->MemorizedSpells[gem]))
		{
			if (!stricmp(name.c_str(), pSpell->Name))
				return gem + 1;
		}
	}
	return 0;
}

int PythonCharacter::SpellGem_Spell(PythonSpell& spell)
{
	return SpellGem_String(spell.Name());
}

int PythonCharacter::GemSlots()
{
	AssertIsValid();

	int index = GetAAIndexByName("Mnemonic Retention");
	if (PlayerHasAAAbility(index)) {
		for (int i = 0; i < AA_CHAR_MAX_REAL; i++) {
			if (pPCData->GetAltAbilityIndex(i) == index) {
				int points = GetCharInfo2()->AAList[i].PointsSpent;
				if (points > 3)
					return 10;
				else if (points == 3)
					return 9;
			}
		}
	}
	return 8;
}

bool PythonCharacter::SpellReady_Int(int gem)
{
	AssertIsValid();

	if (gem < 1 || gem > GemSlots()) {
		PyErr_SetString(PyExc_RuntimeError, "Invalid Gem Slot");
		throw_error_already_set();
	}

	PEQCASTSPELLWINDOW pWnd = (PEQCASTSPELLWINDOW)pCastSpellWnd;

	if (!pWnd->SpellSlots[gem - 1])
		return false;

	return pWnd->SpellSlots[gem - 1]->spellstate != 1;
}

bool PythonCharacter::SpellReady_String(std::string name)
{
	AssertIsValid();

	PCHARINFO2 pChar2 = GetCharInfo2();
	PEQCASTSPELLWINDOW pWnd = (PEQCASTSPELLWINDOW)pCastSpellWnd;

	for (int gem = 0; gem < GemSlots(); gem++) {
		if (PSPELL pSpell = GetSpellByID(pChar2->MemorizedSpells[gem])) {
			if (!stricmp(name.c_str(), pSpell->Name)) {
				if (!pWnd->SpellSlots[gem])
					return false;
				return pWnd->SpellSlots[gem]->spellstate != 1;
			}
		}
	}

	return false;
}

bool PythonCharacter::SpellReady_Spell(PythonSpell& spell)
{
	return SpellReady_String(spell.Name());
}
#pragma endregion

//----------------------------------------------------------------------------
// Buffs

#pragma region Buff Properties
SpellBuffInfo PythonCharacter::Buff_Int(int num)
{
	AssertIsValid();

	PCHARINFO2 pChar = GetCharInfo2();
	if (pChar == NULL) {
		PyErr_SetString(PyExc_RuntimeError, "Character is Invalid");
		throw_error_already_set();
	}

	if (num > 24) {
		PyErr_SetString(PyExc_RuntimeError, "Invalid Buff Index");
		throw_error_already_set();
	}

	return SpellBuffInfo(pChar->Buff[num]);
}

SpellBuffInfo PythonCharacter::Buff_String(std::string name)
{
	AssertIsValid();

	PCHARINFO2 pChar = GetCharInfo2();
	if (pChar == NULL) {
		PyErr_SetString(PyExc_RuntimeError, "Character is Invalid");
		throw_error_already_set();
	}

	for (int buff = 0; buff < 25; buff++) {
		if (PSPELL pSpell = GetSpellByID(pChar->Buff[buff].SpellID)) {
			if (!stricmp(name.c_str(), pSpell->Name) 
					|| (strstr(pSpell->Name, "Rk. II") && !strnicmp(name.c_str(), pSpell->Name, name.length())))
				return SpellBuffInfo(pChar->Buff[buff]);
		}
	}

	return SpellBuffInfo();
}

SpellBuffInfo PythonCharacter::ShortBuff_Int(int num)
{
	AssertIsValid();

	PCHARINFO2 pChar = GetCharInfo2();
	if (pChar == NULL) {
		PyErr_SetString(PyExc_RuntimeError, "Character is Invalid");
		throw_error_already_set();
	}

	if (num > 34) {
		PyErr_SetString(PyExc_RuntimeError, "Invalid Buff Index");
		throw_error_already_set();
	}

	return SpellBuffInfo(pChar->ShortBuff[num]);
}

SpellBuffInfo PythonCharacter::ShortBuff_String(std::string name)
{
	AssertIsValid();

	PCHARINFO2 pChar = GetCharInfo2();
	if (pChar == NULL) {
		PyErr_SetString(PyExc_RuntimeError, "Character is Invalid");
		throw_error_already_set();
	}

	for (int buff = 0; buff < 35; buff++) {
		if (PSPELL pSpell = GetSpellByID(pChar->ShortBuff[buff].SpellID)) {
			if (!stricmp(name.c_str(), pSpell->Name) 
					|| (strstr(pSpell->Name, "Rk. II") && !strnicmp(name.c_str(), pSpell->Name, name.length())))
				return SpellBuffInfo(pChar->ShortBuff[buff]);
		}
	}

	return SpellBuffInfo();
}

python::list PythonCharacter::Buffs()
{
	AssertIsValid();

	PCHARINFO2 pChar = GetCharInfo2();
	if (pChar == NULL) {
		PyErr_SetString(PyExc_RuntimeError, "Character is Invalid");
		throw_error_already_set();
	}

	python::list buffs;

	// Iterate through valid buffs and add them to the list
	for (int i = 0; i < 25; i++) {
		int SpellID = pChar->Buff[i].SpellID;
		if (SpellID != 0xffffffff && SpellID != 0) {
			buffs.append(SpellBuffInfo(pChar->Buff[i]));
		}
	}

	return buffs;
}

python::list PythonCharacter::ShortBuffs()
{
	AssertIsValid();

	PCHARINFO2 pChar = GetCharInfo2();
	if (pChar == NULL) {
		PyErr_SetString(PyExc_RuntimeError, "Character is Invalid");
		throw_error_already_set();
	}

	python::list buffs;

	// Iterate through valid buffs and add them to the list
	for (int i = 0; i < 35; i++) {
		int SpellID = pChar->ShortBuff[i].SpellID;
		if (SpellID != 0xffffffff && SpellID != 0) {
			buffs.append(SpellBuffInfo(pChar->ShortBuff[i]));
		}
	}

	return buffs;
}

int PythonCharacter::BuffCount()
{
	AssertIsValid();

	PCHARINFO2 pChar = GetCharInfo2();
	if (pChar == NULL) {
		PyErr_SetString(PyExc_RuntimeError, "Character is Invalid");
		throw_error_already_set();
	}

	int count = 0;
	for (int buff = 0; buff < 25; buff++) {
		if (pChar->Buff[buff].SpellID > 0)
			count++;
	}

	return count;
}

int PythonCharacter::BuffSlots()
{
	AssertIsValid();

	int count = 15;

	int aa = GetAAIndexByName("Mystical Attuning");
	if (PlayerHasAAAbility(aa)) {
		for (int index = 0; index < AA_CHAR_MAX_REAL; index++) {
			if (pPCData->GetAltAbilityIndex(index) == aa) {
				count += GetCharInfo2()->AAList[index].PointsSpent / 5;
				break;
			}
		}
	}

	if (GetAAIndexByName("Embrace of the Dark Reign"))
		count++;
	if (GetAAIndexByName("Embrace of the Keepers"))
		count++;

	if (pSpawn->Level > 71)
		count++;
	if (pSpawn->Level > 74)
		count++;

	return count;
}
#pragma endregion


//----------------------------------------------------------------------------
// Spell Book

#pragma region Spell Book
python::list PythonCharacter::SpellBook()
{
	AssertIsValid();

	python::list spells = python::list();

	for (int i = 0; i < NUM_BOOK_SLOTS; i++) {
		int spellId = GetCharInfo2()->SpellBook[i];

		if (spellId > 0) {
			spells.append(PythonSpell(spellId));
		}
	}

	return spells;
}
#pragma endregion

void Init_Module_PyMQ2_Character()
{
	class_<PythonCharacter, bases<PythonSpawn> >("Character")

		.add_property("CombatState", &PythonCharacter::CombatState)
		.add_property("Downtime", &PythonCharacter::Downtime)
		.add_property("Moving", &PythonCharacter::Moving)
		.add_property("Stunned", &PythonCharacter::Stunned)
		.add_property("RangedReady", &PythonCharacter::RangedReady)
		.add_property("Shrouded", &PythonCharacter::Shrouded)
		.add_property("Autofire", &PythonCharacter::Autofire)
		.add_property("CurrentWeight", &PythonCharacter::CurrentWeight)
		.add_property("Running", &PythonCharacter::Running)
		.add_property("AltTimerReady", &PythonCharacter::AltTimerReady)
		.add_property("Combat", &PythonCharacter::Combat)

		// Buffs
		.add_property("Buffs", &PythonCharacter::Buffs)
		.def("Buff", &PythonCharacter::Buff_Int)
		.def("Buff", &PythonCharacter::Buff_String)
		.add_property("BuffCount", &PythonCharacter::BuffCount)
		.add_property("ShortBuffs", &PythonCharacter::ShortBuffs)
		.def("ShortBuff", &PythonCharacter::ShortBuff_Int)
		.def("ShortBuff", &PythonCharacter::ShortBuff_String)
		.add_property("BuffSlots", &PythonCharacter::BuffSlots)

		// Spell Gems
		.add_property("SpellGems", &PythonCharacter::SpellGems)
		.def("SpellGem", &PythonCharacter::SpellGem_Int)
		.def("SpellGem", &PythonCharacter::SpellGem_String)
		.def("SpellGem", &PythonCharacter::SpellGem_Spell)
		.add_property("GemSlots", &PythonCharacter::GemSlots)
		.def("SpellReady", &PythonCharacter::SpellReady_Int)
		.def("SpellReady", &PythonCharacter::SpellReady_String)
		.def("SpellReady", &PythonCharacter::SpellReady_Spell)

		// Spell Book
		.add_property("SpellBook", &PythonCharacter::SpellBook)

		// Experience
		.add_property("XP", &PythonCharacter::XP)
		.add_property("PctXP", &PythonCharacter::PctXP)
		.add_property("AAXP", &PythonCharacter::AAXP)
		.add_property("PctAAXP", &PythonCharacter::PctAAXP)
		.add_property("AAPoints", &PythonCharacter::AAPoints)
		.add_property("AAPointsTotal", &PythonCharacter::AAPointsTotal)
		.add_property("AAPointsSpent", &PythonCharacter::AAPointsSpent)
		.add_property("LeaderAbilities", &PythonCharacter::LeaderAbilities)
		.add_property("GroupLeaderXP", &PythonCharacter::GroupLeaderXP)
		.add_property("PctGroupLeaderXP", &PythonCharacter::PctGroupLeaderXP)
		.add_property("GroupLeaderPoints", &PythonCharacter::GroupLeaderPoints)
		.add_property("RaidLeaderXP", &PythonCharacter::RaidLeaderXP)
		.add_property("PctRaidLeaderXP", &PythonCharacter::PctRaidLeaderXP)
		.add_property("RaidLeaderPoints", &PythonCharacter::RaidLeaderPoints)

		// Mana
		.add_property("CurrentMana", &PythonCharacter::CurrentMana)
		.add_property("PctMana", &PythonCharacter::PctMana)
		.add_property("MaxMana", &PythonCharacter::MaxMana)

		// Endurance
		.add_property("CurrentEndurance", &PythonCharacter::Endurance)
		.add_property("PctEndurance", &PythonCharacter::PctEndurance)
		.add_property("MaxEndurance", &PythonCharacter::MaxEndurance)

		// Skills
		.add_property("Skills", &PythonCharacter::Skills)
		.add_property("Abilities", &PythonCharacter::Abilities)
		
		// Tribute
		.add_property("CurrentFavor", &PythonCharacter::CurrentFavor)
		.add_property("CareerFavor", &PythonCharacter::CareerFavor)
		.add_property("TributeActive", &PythonCharacter::TributeActive)
		.add_property("TributeTimer", &PythonCharacter::TributeTimer)
		.add_property("ActiveFavorCost", &PythonCharacter::ActiveFavorCost)
		
		// Hunger
		.add_property("Hunger", &PythonCharacter::Hunger)
		.add_property("Thirst", &PythonCharacter::Thirst)
		.add_property("Drunkenness", &PythonCharacter::Drunk)

		// Stats
		.add_property("Resists", &PythonCharacter::Resists)
		.add_property("Stats", &PythonCharacter::Stats)
		.add_property("StatBonuses", &PythonCharacter::StatBonuses)
		.add_property("AttackSpeed", &PythonCharacter::AttackSpeed)
		.add_property("HPRegen", &PythonCharacter::HPRegen)
		.add_property("ManaRegen", &PythonCharacter::ManaRegen)
		.add_property("EnduranceRegen", &PythonCharacter::EnduranceRegen)
		.add_property("DamageAbsorbRemaining", &PythonCharacter::DamageAbsorbRemaining)

		// Inventory
		.add_property("Inventory", &PythonCharacter::Inventory)

		// Currencies
		.add_property("Platinum", &PythonCharacter::Platinum)
		.add_property("Gold", &PythonCharacter::Gold)
		.add_property("Silver", &PythonCharacter::Silver)
		.add_property("Copper", &PythonCharacter::Copper)
		.add_property("PlatinumShared", &PythonCharacter::PlatinumShared)
		.add_property("PlatinumBank", &PythonCharacter::PlatinumBank)
		.add_property("GoldBank", &PythonCharacter::GoldBank)
		.add_property("SilverBank", &PythonCharacter::SilverBank)
		.add_property("CopperBank", &PythonCharacter::CopperBank)
		.add_property("RadiantCrystals", &PythonCharacter::RadiantCrystals)
		.add_property("EbonCrystals", &PythonCharacter::EbonCrystals)
		.add_property("AltCurrency", &PythonCharacter::AltCurrency)
		.add_property("Cash", &PythonCharacter::Cash)
		.add_property("CashBank", &PythonCharacter::CashBank)
	;

	class_<SpellBuffInfo>("Buff", no_init)
		.add_property("Level", &SpellBuffInfo::Level)
		.add_property("Modifier", &SpellBuffInfo::Modifier)
		.add_property("Spell", &SpellBuffInfo::Spell)
		.def("__repr__", &SpellBuffInfo::__repr__)
		.add_property("Duration", &SpellBuffInfo::Duration)
		.add_property("Absorb", &SpellBuffInfo::Absorb)
		.def("Exists", &SpellBuffInfo::IsValid)
	;
}