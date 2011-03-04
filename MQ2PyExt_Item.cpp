/* MQ2PyExt_Item.cpp
 * Copyright (c) 2009 Stephen Raub.
 *
 * Distribution is not allowed without the consent of the author. 
 *
 * This module implements the character info support
 */

#include "MQ2PyPCH.h"
#include "MQ2Py.h"
#include "MQ2PyExt.h"
#include "MQ2PyExt_Item.h"

using namespace std;
using namespace boost;
using namespace boost::python;


//----------------------------------------------------------------------------

void PythonItem::RegisterPythonClass()
{
	class_<PythonItem>("Item", no_init)

		// Fields of ItemData
		.def_readonly("NoTrade", &PythonItem::noTrade)
		.add_property("Temporary", &PythonItem::Temporary)
		.add_property("Lore", &PythonItem::Lore)
		.add_property("Magic", &PythonItem::Magic)
		.add_property("QuestItem", &PythonItem::QuestItem)
		.add_property("Range", &PythonItem::Range)
		.add_property("Weight", &PythonItem::Weight)
		.add_property("StackSize", &PythonItem::StackSize)
		.def_readonly("Stackable", &PythonItem::stackable)
		.add_property("Size", &PythonItem::Size)
		.add_property("SizeCapacity", &PythonItem::SizeCapacity)
		.add_property("Attuneable", &PythonItem::Attuneable)

		// Properties
		.add_property("ID", &PythonItem::ItemId)
		.add_property("Name", &PythonItem::Name)
		.add_property("LoreName", &PythonItem::LoreName)
		.add_property("IDFile", &PythonItem::IDFile)

		.add_property("Type", &PythonItem::Type)
		.add_property("Contents", &PythonItem::ItemContents)
		.add_property("IsContainer", &PythonItem::IsContainer)
		.add_property("IsItem", &PythonItem::IsItem)

		.add_property("DmgBonusType", &PythonItem::DmgBonusType)

		// Fields of PythonItem - Evolving Item
		.def_readonly("IsEvolvingItem", &PythonItem::IsEvolvingItem)
		.def_readonly("EvolvingCurrentLevel", &PythonItem::EvolvingCurrentLevel)
		.def_readonly("EvolvingMaxLevel", &PythonItem::EvolvingMaxLevel)
		.def_readonly("EvolvingExperienceEnabled", &PythonItem::EvolvingExpOn)
		.def_readonly("EvolvingExperiencePercent", &PythonItem::EvolvingExpPct)
		.def_readonly("EvolvingLoreGroup", &PythonItem::EvolvingLoreGroup)

		// Fields of PythonItem - Merchant Info
		.def_readonly("Price", &PythonItem::Price)
		.def_readonly("MerchantQuantity", &PythonItem::MerchantQuantity)
		.def_readonly("MerchantSlot", &PythonItem::MerchantSlot)

		// Fields of PythonItem - Misc Counters, etc
		.def_readonly("ItemSlot", &PythonItem::ItemSlot)
		.def_readonly("Charges", &PythonItem::Charges)
		.def_readonly("Open", &PythonItem::Open)
		.def_readonly("Power", &PythonItem::Power)
		.def_readonly("StackCount", &PythonItem::StackCount)

		// Class Properties
		.def("__repr__", &PythonItem::__repr__);
	;
}

//----------------------------------------------------------------------------

PythonItem::PythonItem(PCONTENTS item)
{
	if (item == NULL) {
		PyErr_SetString(PyExc_AssertionError, "Item is Invalid");
		throw_error_already_set();
	}

	memcpy(&this->ItemData, item->Item, sizeof(_ITEMINFO));

	for (int i = 0; i < 10; i++) {
		if (item->pContentsArray->Contents[i] != NULL) {
			PythonItem* subitem = new PythonItem(item->pContentsArray->Contents[i]);
			this->Contents.push_back(subitem);
		}
	}

	noTrade = !((EQ_Item*)item)->CanDrop(1, 0);

	EvolvingCurrentLevel	= item->EvolvingCurrentLevel;
	EvolvingMaxLevel		= item->EvolvingMaxLevel;
	EvolvingExpOn			= item->EvolvingExpOn;
	EvolvingExpPct			= (float)item->EvolvingExpPct;
	EvolvingLoreGroup		= item->EvolvingLoreGroup;
	IsEvolvingItem			= item->IsEvolvingItem;

	Price					= item->Price;
	MerchantQuantity		= item->MerchantQuantity;
	MerchantSlot			= item->MerchantSlot;

	ItemSlot				= item->ItemSlot;

	if (ItemData.Type != ITEMTYPE_NORMAL)
		Charges = 1;
	else
		Charges = item->Charges;

	Open					= item->Open;
	Power					= item->Power;

	// Check stack count
	stackable = ((EQ_Item*)item)->IsStackable();
	if (ItemData.Type != ITEMTYPE_NORMAL || !stackable)
		StackCount = 1;
	else
		StackCount = item->StackCount;

}

//----------------------------------------------------------------------------

std::string PythonItem::Name()
{
	return ItemData.Name;
}

std::string PythonItem::LoreName()
{
	return ItemData.LoreName;
}

std::string PythonItem::IDFile()
{
	return ItemData.IDFile;
}

int PythonItem::ItemId()
{
	return ItemData.ItemNumber;
}

bool PythonItem::Magic()
{
	return (ItemData.Type == ITEMTYPE_NORMAL) && (ItemData.Magic != 0);
}

bool PythonItem::QuestItem()
{
	return ItemData.QuestItem != 0;
}

bool PythonItem::Attuneable()
{
	return ItemData.Attuneable;
}

int PythonItem::Range()
{
	return ItemData.Range;
}

float PythonItem::Weight()
{
	return ItemData.Weight;
}

int PythonItem::StackSize()
{
	if (ItemData.Type == ITEMTYPE_NORMAL || !stackable)
		return 1;

	return ItemData.StackSize;
}

int PythonItem::Size()
{
	return ItemData.Size;
}

int PythonItem::SizeCapacity()
{
	return ItemData.SizeCapacity;
}

std::string PythonItem::Type()
{
	std::ostringstream oss;

	if (ItemData.Type == ITEMTYPE_NORMAL)
	{
		const char* str = NULL;
		if (ItemData.ItemType < MAX_ITEMTYPES)
			str = szItemTypes[ItemData.ItemType];
		if (str != NULL)
			return str;

		oss << "*UnknownType" << ItemData.ItemType;
		return oss.str();
	}
	else if (ItemData.Type == ITEMTYPE_PACK)
	{
		const char* str = NULL;
		if (ItemData.Combine < MAX_COMBINES)
			str = szCombineTypes[ItemData.Combine];
		if (str != NULL)
			return str;

		oss << "*UnknownCombine" << ItemData.Combine;
		return oss.str();
	}
	else if (ItemData.Type == ITEMTYPE_BOOK)
	{
		return "Book";
	}

	oss << "*UnknownType" << (int)ItemData.Type;
	return oss.str();
}

bool PythonItem::IsContainer()
{
	return ItemData.Type == ITEMTYPE_PACK;
}

bool PythonItem::IsItem()
{
	return ItemData.Type == ITEMTYPE_NORMAL;
}

std::string PythonItem::DmgBonusType()
{
	return szDmgBonusType[ItemData.DmgBonusType];
}


python::list PythonItem::ItemContents()
{
	python::list contents = python::list();

	for (size_t idx = 0; idx < Contents.size(); idx++) {
		contents.append(Contents[idx]);
	}

	return contents;
}

std::string PythonItem::__repr__()
{
	ostringstream ss;
	ss << "Item: " << ItemData.Name;
	
	return ss.str();
}

//----------------------------------------------------------------------------
//

void Init_Module_PyMQ2_Item()
{
	enum_<PythonItem::ItemType>("ItemType")
		.value("Normal", PythonItem::Normal)
		.value("Bag", PythonItem::Pack)
		.value("Book", PythonItem::Book)
	;
	
	enum_<PythonItem::ItemSlotType>("ItemSlot")
		.value("None",			PythonItem::None)
		.value("Charm", 		PythonItem::Charm)
		.value("LeftEar",		PythonItem::LeftEar)
		.value("Head",			PythonItem::Head)
		.value("Face",			PythonItem::Face)
		.value("RightEar", 		PythonItem::RightEar)
		.value("Neck",			PythonItem::Neck)
		.value("Shoulder",		PythonItem::Shoulder)
		.value("Arms",			PythonItem::Arms)
		.value("Back",			PythonItem::Back)
		.value("LeftWrist",		PythonItem::LeftWrist)
		.value("RightWrist",	PythonItem::RightWrist)
		.value("Ranged",		PythonItem::Ranged)
		.value("Hands",			PythonItem::Hands)
		.value("MainHand",		PythonItem::MainHand)
		.value("OffHand",		PythonItem::OffHand)
		.value("LeftFinger",	PythonItem::LeftFinger)
		.value("RightFinger",	PythonItem::RightFinger)
		.value("Chest",			PythonItem::Chest)
		.value("Legs",			PythonItem::Legs)
		.value("Feet",			PythonItem::Feet)
		.value("Waist",			PythonItem::Waist)
		.value("PowerSource",	PythonItem::PowerSource)
		.value("Ammo",			PythonItem::Ammo)
		.value("Pack1",			PythonItem::Pack1)
		.value("Pack2",			PythonItem::Pack2)
		.value("Pack3",			PythonItem::Pack3)
		.value("Pack4",			PythonItem::Pack4)
		.value("Pack5",			PythonItem::Pack5)
		.value("Pack6",			PythonItem::Pack6)
		.value("Pack7",			PythonItem::Pack7)
		.value("Pack8",			PythonItem::Pack8)
	;

	PythonItem::RegisterPythonClass();
}

void Shutdown_Module_PyMQ2_Item()
{
}