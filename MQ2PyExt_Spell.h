/* MQ2PyExt_Spell.h
 * Copyright (c) 2009 Stephen Raub.
 *
 * Distribution is not allowed without the consent of the author. 
 *
 * Definitions and such for MQ2Py Extension spell component
 */

#pragma once

class PythonSpell
{
	PSPELL Spell;

public:
	PythonSpell();
	PythonSpell(int spellId);
	PythonSpell(PythonSpell const &);
	PythonSpell(std::string Name);

	PythonSpell& operator= (PythonSpell const&);
	bool operator== (PythonSpell const&);
	operator bool();

	// Properties
	int Id();
	std::string Name();
	int Mana();
	int ResistAdj();
	float Range();
	float AERange();
	float PushBack();
	float CastTime();
	float RecoveryTime();
	float RecastTime();

	float Duration();
	float ModifiedCastTime();
	float ModifiedRange();	

	boost::python::dict Level();
	boost::python::dict ValidLevel();

	std::string ResistType();
	std::string SpellType();
	std::string TargetType();
	std::string Skill();

	std::string CastOnYouMessage();
	std::string CastOnOtherMessage();
	std::string WearOffMessage();

	std::string CounterType();
	int Counters();

	// Methods

	// Check against whether this spell stacks with character or pet
	// good chance this should be migrated over to Character or Spawn
	bool Stacks();
	bool Stacks_Int(int duration);
	bool StacksPet();
	bool StacksPet_Int(int duration);

	bool StacksWith_Spell(PythonSpell& spell);
	bool StacksWith_String(std::string name);
	bool StacksWith_Int(int id);

	bool IsValid();
	std::string __repr__();

protected:
	void AssertIsValid();

};