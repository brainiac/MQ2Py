/* MQ2PyExt_Item.h
 * Copyright (c) 2009 Stephen Raub.
 *
 * Distribution is not allowed without the consent of the author. 
 *
 * Definitions and such for the Item python object and related classes
 */

#pragma once

class PythonItem
{
public:
	static void RegisterPythonClass();

	PythonItem(PCONTENTS item);

	enum ItemType {
		Normal		= ITEMTYPE_NORMAL,
		Pack		= ITEMTYPE_PACK,
		Book		= ITEMTYPE_BOOK
	};

	enum ItemSlotType {
		None,
		Charm,
		LeftEar,
		Head,
		Face,
		RightEar,
		Neck,
		Shoulder,
		Arms,
		Back,
		LeftWrist,
		RightWrist,
		Ranged,
		Hands,
		MainHand,
		OffHand,
		LeftFinger,
		RightFinger,
		Chest,
		Legs,
		Feet,
		Waist,
		PowerSource,
		Ammo,
		Pack1,
		Pack2,
		Pack3,
		Pack4,
		Pack5,
		Pack6,
		Pack7,
		Pack8
	};

private:
	struct _ITEMINFO			ItemData;
	std::vector<PythonItem*>	Contents;

	// Evolving Item Data
	int EvolvingCurrentLevel;
	int EvolvingMaxLevel;
	bool EvolvingExpOn;
	float EvolvingExpPct;
	int EvolvingLoreGroup;
	bool IsEvolvingItem;

	int Price;
	int MerchantQuantity;
	int MerchantSlot;

	int ItemSlot;
	int Charges;
	int Open;
	int Power;
	int StackCount;

	bool stackable;
	bool noTrade;

public:		// Properties

	std::string Name();
	std::string LoreName();
	std::string IDFile();

	int ItemId();

	// Flags
	bool Lore() { return ItemData.Lore != 0; }
	bool Temporary() { return ItemData.NoRent; }
	bool Magic();
	bool QuestItem();
	bool Attuneable();
	
	int Range();
	float Weight();
	int StackSize();
	int Size();
	int SizeCapacity();
	int Value();

	std::string Type();
	boost::python::list ItemContents();
	bool IsContainer();
	bool IsItem();

	// std::string LDoNTheme()		-- who cares? seriously
	std::string DmgBonusType();

	std::string __repr__();
};

