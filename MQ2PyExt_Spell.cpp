/* MQ2PyExt_Spell.cpp
 * Copyright (c) 2009 Stephen Raub.
 *
 * Distribution is not allowed without the consent of the author. 
 *
 * This module implements the spell info support
 */

#include "MQ2PyPCH.h"
#include "MQ2Py.h"
#include "MQ2PyExt.h"
#include "MQ2PyExt_Spell.h"

using namespace std;
using namespace boost;
using namespace boost::python;

//----------------------------------------------------------------------------
//

PythonSpell::PythonSpell()
{
	Spell = NULL;
}

PythonSpell::PythonSpell(int spellId)
{
	Spell = GetSpellByID(spellId);
}

PythonSpell::PythonSpell(std::string Name)
{
	Spell = GetSpellByName((PCHAR)Name.c_str());
}

PythonSpell::PythonSpell(PythonSpell const& spell)
{
	Spell = spell.Spell;
}

PythonSpell& PythonSpell::operator=(PythonSpell const& spell)
{
	Spell = spell.Spell;
	return *this;
}

bool PythonSpell::operator== (PythonSpell const& spell)
{
	return Spell == spell.Spell;
}

PythonSpell::operator bool()
{
	return IsValid();
}

//----------------------------------------------------------------------------
//

int PythonSpell::Id()
{
	AssertIsValid();
	return Spell->ID;
}

std::string PythonSpell::Name()
{
	AssertIsValid();
	return Spell->Name;
}

int PythonSpell::Mana()
{
	AssertIsValid();
	return Spell->Mana;
}

int PythonSpell::ResistAdj()
{
	AssertIsValid();
	return Spell->ResistAdj;
}

float PythonSpell::Range()
{
	AssertIsValid();
	return Spell->Range;
}

float PythonSpell::AERange()
{
	AssertIsValid();
	return Spell->AERange;
}

float PythonSpell::PushBack()
{
	AssertIsValid();
	return Spell->PushBack;
}

float PythonSpell::CastTime()
{
	AssertIsValid();
	return (float)Spell->CastTime / 1000.0f;
}

float PythonSpell::RecoveryTime()
{
	AssertIsValid();
	return (float)Spell->FizzleTime / 1000.0f;
}

float PythonSpell::RecastTime()
{
	AssertIsValid();
	return (float)Spell->RecastTime / 1000.0f;
}

float PythonSpell::Duration()
{
	AssertIsValid();

	PSPAWNINFO pSpawn = (PSPAWNINFO)pCharSpawn;
	if (pSpawn == NULL) {
		PyErr_SetString(PyExc_RuntimeError, "Character is Invalid");
		throw_error_already_set();
	}

	return (float)GetSpellDuration(Spell, (PSPAWNINFO)pCharSpawn);
}

float PythonSpell::ModifiedCastTime()
{
	AssertIsValid();

	PSPAWNINFO pSpawn = (PSPAWNINFO)pCharSpawn;
	if (pSpawn == NULL) {
		PyErr_SetString(PyExc_RuntimeError, "Character is Invalid");
		throw_error_already_set();
	}

	float mct = (float)(pCharData1->GetAACastingTimeModifier((EQ_Spell*)Spell) 
		+ pCharData1->GetFocusCastingTimeModifier((EQ_Spell*)Spell, 0, 0)
		+ Spell->CastTime) / 1000.0f;

	if (mct < 0.50f * ((float)Spell->CastTime / 1000.0f))
		return 0.50f * ((float)Spell->CastTime / 1000.0f);
	
	return mct;
}

float PythonSpell::ModifiedRange()
{
	AssertIsValid();

	PSPAWNINFO pSpawn = (PSPAWNINFO)pCharSpawn;
	if (pSpawn == NULL) {
		PyErr_SetString(PyExc_RuntimeError, "Character is Invalid");
		throw_error_already_set();
	}

	return Spell->Range + (float)pCharData1->GetFocusRangeModifier((EQ_Spell*)Spell, 0);
}

python::dict PythonSpell::Level()
{
	AssertIsValid();

	dict levels;

	levels[Warrior]			= Spell->Level[Warrior - 1];
	levels[Cleric]			= Spell->Level[Cleric - 1];
	levels[Paladin]			= Spell->Level[Paladin - 1];
	levels[Ranger]			= Spell->Level[Ranger - 1];
	levels[Shadowknight]	= Spell->Level[Shadowknight - 1];
	levels[Druid]			= Spell->Level[Druid - 1];
	levels[Monk]			= Spell->Level[Monk - 1];
	levels[Bard]			= Spell->Level[Bard - 1];
	levels[Rogue]			= Spell->Level[Rogue - 1];
	levels[Shaman]			= Spell->Level[Shaman - 1];
	levels[Necromancer]		= Spell->Level[Necromancer - 1];
	levels[Wizard]			= Spell->Level[Wizard - 1];
	levels[Mage]			= Spell->Level[Mage - 1];
	levels[Enchanter]		= Spell->Level[Enchanter - 1];
	levels[Beastlord]		= Spell->Level[Beastlord - 1];
	levels[Berserker]		= Spell->Level[Berserker - 1];

	return levels;
}

python::dict PythonSpell::ValidLevel()
{
	AssertIsValid();

	dict levels;

#define ADD_SPELL_LEVEL(c) \
		if (Spell->Level[c - 1] != 255) \
			levels[c] = Spell->Level[c - 1];

	ADD_SPELL_LEVEL(Warrior);
	ADD_SPELL_LEVEL(Cleric);
	ADD_SPELL_LEVEL(Paladin);
	ADD_SPELL_LEVEL(Ranger);
	ADD_SPELL_LEVEL(Shadowknight);
	ADD_SPELL_LEVEL(Druid);
	ADD_SPELL_LEVEL(Monk);
	ADD_SPELL_LEVEL(Bard);
	ADD_SPELL_LEVEL(Rogue);
	ADD_SPELL_LEVEL(Shaman);
	ADD_SPELL_LEVEL(Necromancer);
	ADD_SPELL_LEVEL(Wizard);
	ADD_SPELL_LEVEL(Mage);
	ADD_SPELL_LEVEL(Enchanter);
	ADD_SPELL_LEVEL(Beastlord);
	ADD_SPELL_LEVEL(Berserker);

	return levels;
}

std::string PythonSpell::ResistType()
{
	AssertIsValid();

	switch (Spell->Resist)
	{
		case 9:	return "Corruption";
		case 7:	return "Prismatic";
		case 6:	return "Chromatic";
		case 5: return "Disease";
		case 4: return "Poison";
		case 3: return "Cold";
		case 2: return "Fire";
		case 1: return "Magic";
		case 0: return "Unresistable";
	}
	
	return "Unknown";
}

std::string PythonSpell::SpellType()
{
	AssertIsValid();

	switch (Spell->SpellType)
	{
		case 2: return "Beneficial (Group)";
		case 1: return "Beneficial";
		case 0: return "Detrimental";
	}

	return "Unknown";
}

std::string PythonSpell::TargetType()
{
	AssertIsValid();

	switch (Spell->TargetType)
	{
		case 41: return "Group v2";
		case 40: return "AE PC v2";
		case 25: return "AE Summoned";
		case 24: return "AE Undead";
		case 20: return "Targetted AE Tap";
		case 18: return "Uber Dragons";
		case 17: return "Uber Giants";
		case 16: return "Plant";
		case 15: return "Corpse";
		case 14: return "Pet";
		case 13: return "LifeTap";
		case 11: return "Summoned";
		case 10: return "Undead";
		case  9: return "Animal";
		case  8: return "Targetted AE";
		case  6: return "Self";
		case  5: return "Single";
		case  4: return "PB AE";
		case  3: return "Group v1";
		case  2: return "AE PC v1";
		case  1: return "Line of Sight";
	}

	return "Unknown";
}

std::string PythonSpell::Skill()
{
	AssertIsValid();

	if (Spell->Skill >= 133)
		return "Unknown";

	return szSkills[Spell->Skill];
}

std::string PythonSpell::CastOnYouMessage()
{
	AssertIsValid();
	return Spell->CastOnYou;
}

std::string PythonSpell::CastOnOtherMessage()
{
	AssertIsValid();
	return Spell->CastOnAnother;
}

std::string PythonSpell::WearOffMessage()
{
	AssertIsValid();
	return Spell->WearOff;
}

std::string PythonSpell::CounterType()
{
	AssertIsValid();

	for (int i = 0; i < 12; i++) {
		switch (Spell->Attrib[i]) {
			case 35: return "Disease";
			case 36: return "Poison";
			case 116: return "Curse";
			case 369: return "Corruption";
		}
	}

	return "None";
}

int PythonSpell::Counters()
{
	AssertIsValid();

	for (int i = 0; i < 12; i++) {
		switch (Spell->Attrib[i]) {
			case 35:
			case 36:
			case 116:
			case 369:
				return (int)Spell->Base[i];
		}
	}

	return 0;
}

//----------------------------------------------------------------------------
// Methods

bool PythonSpell::Stacks()
{
	AssertIsValid();
	return Stacks_Int(99999);
}

bool PythonSpell::Stacks_Int(int duration)
{
	AssertIsValid();

	PCHARINFO2 pChar = GetCharInfo2();
	if (pChar == NULL) {
		PyErr_SetString(PyExc_RuntimeError, "Character is Invalid");
		throw_error_already_set();
	}

	// Check against all current buffs to see if this spell would stack.
	for (int buff = 0; buff < 25; buff++) {
		if (pChar->Buff[buff].SpellID > 0) {
			PSPELL spell = GetSpellByID(pChar->Buff[buff].SpellID);
			int thisduration = pChar->Buff[buff].Duration;

			if (GetSpellDuration(spell, (PSPAWNINFO)pCharSpawn) > 0xfffffffe)
				thisduration = 99999 + 1;
			if (!BuffStackTest(Spell, spell) || ((Spell == spell) && (thisduration > duration)))
				return false;
		}
	}
	return true;
}

bool PythonSpell::StacksPet()
{
	AssertIsValid();
	return StacksPet_Int(99999);
}

bool PythonSpell::StacksPet_Int(int duration)
{
	AssertIsValid();

	PCHARINFO2 pChar = GetCharInfo2();
	if (pChar == NULL) {
		PyErr_SetString(PyExc_RuntimeError, "Character is Invalid");
		throw_error_already_set();
	}

	PEQPETINFOWINDOW pPet = (PEQPETINFOWINDOW)pPetInfoWnd;
	for (int buff = 0; buff < NUM_BUFF_SLOTS; buff++) {
		if (pPet->Buff[buff] > 0 && !(pPet->Buff[buff] == 0xffffffff || pPet->Buff[buff] == 0)) {
			PSPELL spell = GetSpellByID(pPet->Buff[buff]);
			int petbuffduration = ((pPet->BuffFadeETA[buff] + 5999) / 1000) / 6;

			if (GetSpellDuration(spell, (PSPAWNINFO)pCharSpawn) >= 0xfffffffe)
				petbuffduration = 99999 + 1;
			if (!BuffStackTest(Spell, spell) || ((Spell == spell) && (petbuffduration > duration)))
				return false;
		}
	}

	return true;
}

bool PythonSpell::StacksWith_Spell(PythonSpell& spell)
{
	AssertIsValid();

	if (spell.Spell == NULL) {
		PyErr_SetString(PyExc_RuntimeError, "Parameter is Invalid");
		throw_error_already_set();
	}

	return BuffStackTest(Spell, spell.Spell);
}

bool PythonSpell::StacksWith_String(std::string name)
{
	AssertIsValid();

	PSPELL spell = GetSpellByName((PCHAR)name.c_str());
	if (spell == NULL) {
		PyErr_SetString(PyExc_RuntimeError, "Parameter is Invalid");
		throw_error_already_set();
	}

	return BuffStackTest(Spell, spell);
}

bool PythonSpell::StacksWith_Int(int id)
{
	AssertIsValid();

	PSPELL spell = GetSpellByID(id);
	if (spell == NULL) {
		PyErr_SetString(PyExc_RuntimeError, "Parameter is Invalid");
		throw_error_already_set();
	}

	return BuffStackTest(Spell, spell);
}

//----------------------------------------------------------------------------
//

bool PythonSpell::IsValid()
{
	return Spell != NULL;
}

void PythonSpell::AssertIsValid()
{
	if (!IsValid()) {
		PyErr_SetString(PyExc_AssertionError, "Spell is Invalid");
		throw_error_already_set();
	}
}

std::string PythonSpell::__repr__()
{
	if (IsValid()) {
		ostringstream ss;
		ss << "Spell: " << Spell->Name << " (" << Spell->ID << ")";
		return ss.str();
	}

	return "Spell: <Invalid>";
}

void Init_Module_PyMQ2_Spell()
{
	class_<PythonSpell>("Spell")
		// constructors
		.def(init<int>())
		.def(init<std::string>())
		.def(init<PythonSpell const &>())
		.def(self == self)

		// properties
		.add_property("ID", &PythonSpell::Id)
		.add_property("Name", &PythonSpell::Name)
		.add_property("Mana", &PythonSpell::Mana)
		.add_property("ResistAdj", &PythonSpell::ResistAdj)
		.add_property("Range", &PythonSpell::Range)
		.add_property("AERange", &PythonSpell::AERange)
		.add_property("PushBack", &PythonSpell::PushBack)
		.add_property("CastTime", &PythonSpell::CastTime)
		.add_property("RecoveryTime", &PythonSpell::RecoveryTime)
		.add_property("RecastTime", &PythonSpell::RecastTime)
		.add_property("Duration", &PythonSpell::Duration)
		.add_property("ModifiedCastTime", &PythonSpell::ModifiedCastTime)
		.add_property("ModifiedRange", &PythonSpell::ModifiedRange)
		.add_property("Level", &PythonSpell::Level)
		.add_property("ValidLevel", &PythonSpell::ValidLevel)

		.add_property("ResistType", &PythonSpell::ResistType)
		.add_property("SpellType", &PythonSpell::SpellType)
		.add_property("TargetType", &PythonSpell::TargetType)
		.add_property("Skill", &PythonSpell::Skill)

		.add_property("CastOnYouMessage", &PythonSpell::CastOnYouMessage)
		.add_property("CastOnOtherMessage", &PythonSpell::CastOnOtherMessage)
		.add_property("WearOffMessage", &PythonSpell::WearOffMessage)

		.add_property("CounterType", &PythonSpell::CounterType)
		.add_property("Counters", &PythonSpell::Counters)

		.def("Stacks", &PythonSpell::Stacks)
		.def("Stacks", &PythonSpell::Stacks_Int)
		.def("StacksPet", &PythonSpell::StacksPet)
		.def("StacksPet", &PythonSpell::StacksPet_Int)

		.def("StacksWith", &PythonSpell::StacksWith_Spell)
		.def("StacksWith", &PythonSpell::StacksWith_String)
		.def("StacksWith", &PythonSpell::StacksWith_Int)

		// Utils
		.def("IsValid", &PythonSpell::IsValid)
		.def("__repr__", &PythonSpell::__repr__)
	;		
}