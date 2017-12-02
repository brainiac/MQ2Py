/* MQ2PyExt_Character.h
 * Copyright (c) 2009 Stephen Raub.
 *
 * Distribution is not allowed without the consent of the author. 
 *
 * Definitions and such for MQ2Py Extension character component
 */

#pragma once

#include "MQ2PyExt_Item.h"

class SpellBuffInfo
{
	int level;
	float modifier;
	int absorb;
	int duration;

	PythonSpell spell;
	
public:
	SpellBuffInfo();
	SpellBuffInfo(_SPELLBUFF const& info);
	std::string __repr__();

	PythonSpell Spell();
	int Level();
	int Absorb();	
	float Modifier();
	int Duration();
	bool IsValid();
};


class PythonCharacter : public PythonSpawn
{
	PCHARINFO pChar;

public:
	PythonCharacter();
	virtual bool IsValid();

	// Face the character a particular direction. Adapted from /face
	void Face(PythonSpawn& spawn, bool fast = false, bool away = false);
	void Face(float y, float x, bool fast = false, bool away = false);
	void Face(int angle, bool fast = false, bool away = false);

	// Look changes the player's camera angle up or down
	void Look(float angle);

	// Cast a spell by name or spell id
	void CastSpell(std::string spellname);
	void CastSpell(int spellId);

	// Cast an item by name
	void CastItem(std::string itemname);

	// Assign a new target
	void Target(PythonSpawn& spawn);
	void Target(int spawnId);
	void Target(std::string name);

	//------------------------
	// Properties

	// Misc Info
	std::string CombatState();
	int Downtime();
	bool Moving();
	bool Stunned();
	bool RangedReady();
	bool Shrouded();
	bool Autofire();
	int CurrentWeight();
	bool Running();
	bool Combat();

	// Spell Gems
	boost::python::dict SpellGems();
	PythonSpell SpellGem_Int(int gem);
	int SpellGem_String(std::string spell);
	int SpellGem_Spell(PythonSpell& spell);
	int GemSlots();
	bool SpellReady_Int(int gem);
	bool SpellReady_String(std::string spell);
	bool SpellReady_Spell(PythonSpell& spell);

	// Spell Book!
	boost::python::list SpellBook();

	// Get information on buffs
	SpellBuffInfo Buff_Int(int num);
	SpellBuffInfo Buff_String(std::string name);
	SpellBuffInfo ShortBuff_Int(int num);
	SpellBuffInfo ShortBuff_String(std::string name);
	boost::python::list Buffs();
	boost::python::list ShortBuffs();
	int BuffCount();
	int BuffSlots();

	// Abilities & Skills
	boost::python::dict Skills();
	boost::python::list Abilities();		// NYI

	// Experience
	int64_t XP();
	float PctXP();
	int AAXP();
	float PctAAXP();
	int AAPoints();
	int AAPointsTotal();
	int AAPointsSpent();

	// Mana
	int CurrentMana();
	float PctMana();
	int MaxMana();

	// Endurance
	int Endurance();
	float PctEndurance();
	int MaxEndurance();

	// Tribute
	int64_t CurrentFavor();
	int64_t CareerFavor();
	bool TributeActive();
	int TributeTimer();
	int ActiveFavorCost();

	// Statistics - Hunger
	int Hunger();
	int Thirst();
	int Drunk();

	// Statistics - Bonuses
	boost::python::dict StatBonuses();
	boost::python::dict Resists();
	boost::python::dict Stats();
	int AttackSpeed();
	int HPRegen();
	int ManaRegen();
	int EnduranceRegen();
	int DamageAbsorbRemaining();

	// Items
	boost::python::list Inventory();

	// Currency
	int Platinum();
	int Gold();
	int Silver();
	int Copper();
	int PlatinumShared();
	int PlatinumBank();
	int GoldBank();
	int SilverBank();
	int CopperBank();
	int Cash();
	int CashBank();
	int RadiantCrystals();
	int EbonCrystals();
	boost::python::dict AltCurrency();



};
