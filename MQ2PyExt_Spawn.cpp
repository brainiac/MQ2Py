/* MQ2PyExt_Spawn.cpp
 * Copyright (c) 2009 Stephen Raub.
 *
 * Distribution is not allowed without the consent of the author. 
 *
 * This module implements the spawn info support
 */

#include "MQ2PyPCH.h"
#include "MQ2Py.h"
#include "MQ2PyExt.h"
#include "MQ2PyExt_Spawn.h"

#include <unordered_map>

using namespace boost;
using namespace boost::python;

// Debug info
int PySpawn_CurCount = 0;
int PySpawn_MaxCount = 0;
int PySpawn_TotalCount = 0;

bool debugSpawns = false;

PSPAWNINFO HoverSpawn = NULL;
PSPAWNINFO LastTarget = NULL;

// TODO: Should the events pass the pointer to the spawn object?

//----------------------------------------------------------------------------
// Spawn consistency tracking

struct PythonSpawnList
{
	PythonSpawn* spawn;
	PythonSpawnList* next;
};

struct PythonSpawnInfo
{
	PythonSpawnList* list;

	bool isDead;

	eSpawnType LastSpawnType;
};


typedef std::unordered_map<PSPAWNINFO, PythonSpawnInfo> SpawnMap;
SpawnMap TrackedSpawns;

static PythonSpawnInfo s_emptySpawnInfo;

PythonSpawnInfo& FindPythonSpawns(PSPAWNINFO spawnToFind)
{
	auto it = TrackedSpawns.find(spawnToFind);
	if (it == TrackedSpawns.end())
		return s_emptySpawnInfo;

	return it->second;
}

void RegisterSpawn(PythonSpawn* pySpawn)
{
	if (debugSpawns) {
		PyPrint("Register Spawn: %p => %p", pySpawn, pySpawn->GetSpawn());
	}

	PSPAWNINFO pSpawn = pySpawn->GetSpawn();
	if (pSpawn != NULL) {
		// If spawn is null none of this needs to occur
		PythonSpawnInfo& info = TrackedSpawns[pSpawn];
		if (info.list == NULL) {
			info.isDead = false;
		}

		// Create new spawn list entry and append it to the linked list
		PythonSpawnList* list = new PythonSpawnList;
		list->spawn = pySpawn;
		list->next = info.list;
		info.list = list;
	}
}

void UnregisterSpawn(PythonSpawn* pySpawn)
{
	if (debugSpawns) {
		PyPrint("UnregisterSpawn: %p => %p", pySpawn, pySpawn->GetSpawn());
	}

	// Remove pySpawn from the spawn map
	PythonSpawnInfo& info = TrackedSpawns[pySpawn->GetSpawn()];
	PythonSpawnList* last = NULL;

	for (PythonSpawnList* ptr = info.list; ptr != NULL; ptr = ptr->next) {
		if (ptr->spawn == pySpawn) {
			if (last)
				last->next = ptr->next;	
			else
				info.list = ptr->next;

			delete ptr;
			break;
		}
		last = ptr;
	}
}

void RemoveTrackedSpawn(PSPAWNINFO pSpawn)
{
	// delete this node of tracked spawns
	PythonSpawnInfo& info = TrackedSpawns[pSpawn];
	PythonSpawnList* ptr = info.list;

	while (ptr != NULL) {
		PythonSpawnList* cur = ptr;
		ptr = ptr->next;

		PythonSpawn* spawn = cur->spawn;
		try {
			if (spawn->OnInvalidate)
				spawn->OnInvalidate();
		} catch (error_already_set const &) {
			PyErr_Print();
		}

		spawn->Invalidate();
		delete cur;
	}

	// remove list entry to indicate no tracked spawn
	auto it = TrackedSpawns.find(pSpawn);
	if (it != TrackedSpawns.end())
		TrackedSpawns.erase(it);
}

void UpdateTrackedSpawns()
{
	// No spawn tracking occurs unless we're in game
	if (gGameState != GAMESTATE_INGAME)
		return;

	// Update the hover spawn
	if (*pMouseLook == FALSE) {
		PSPAWNINFO pSpawn = (PSPAWNINFO)pEverQuest->ClickedPlayer(EQADDR_MOUSE->X, EQADDR_MOUSE->Y);

		// execute OnMouseLeave event
		if (HoverSpawn != pSpawn) {
			PythonSpawnInfo& psi = FindPythonSpawns(HoverSpawn);
			for (PythonSpawnList* ptr = psi.list; ptr != NULL; ptr = ptr->next) {
				try {
					if (ptr->spawn->OnMouseLeave)
						ptr->spawn->OnMouseLeave();
				} catch (error_already_set const &) {
					PyErr_Print();
				}
			}
		}

		// execute OnMouseOver event
		if (HoverSpawn == pSpawn) {
			PythonSpawnInfo& psi = FindPythonSpawns(HoverSpawn);
			for (PythonSpawnList* ptr = psi.list; ptr != NULL; ptr = ptr->next) {
				try {
					if (ptr->spawn->OnMouseOver)
						ptr->spawn->OnMouseOver();
				} catch (error_already_set const &) {
					PyErr_Print();
				}
			}
		}

		// execute OnMouseEnter event
		if (HoverSpawn != pSpawn && pSpawn != NULL) {
			PythonSpawnInfo& psi = FindPythonSpawns(pSpawn);
			for (PythonSpawnList* ptr = psi.list; ptr != NULL; ptr = ptr->next) {
				try {
					if (ptr->spawn->OnMouseEnter)
						ptr->spawn->OnMouseEnter();
				} catch (error_already_set const &) {
					PyErr_Print();
				}
			}
		}
		HoverSpawn = pSpawn;
	}

	// Update the target spawn
	PSPAWNINFO CurrentTarget = (PSPAWNINFO)pTarget;
	if (CurrentTarget != LastTarget && CurrentTarget != NULL) {
		PythonSpawnInfo& psi = FindPythonSpawns(CurrentTarget);
		for (PythonSpawnList* ptr = psi.list; ptr != NULL; ptr = ptr->next) {
			try {
				if (ptr->spawn->OnTarget)
					ptr->spawn->OnTarget();
			} catch (error_already_set const &) {
				PyErr_Print();
			}
		}
	}
	LastTarget = CurrentTarget;

	// Update Spawn Status
	auto it = TrackedSpawns.begin();
	while (it != TrackedSpawns.end())
	{
		PSPAWNINFO curSpawn = it->first;
		if (curSpawn != NULL) {
			eSpawnType spawnType = GetSpawnType(curSpawn);
			PythonSpawnInfo& sli = it->second;

			if (spawnType != sli.LastSpawnType) {
				if (spawnType == CORPSE) {
					sli.isDead = true;

					PythonSpawnList* ptr = sli.list;
					while (ptr != NULL) {
						try {
							if (ptr->spawn->OnDeath)
								ptr->spawn->OnDeath();
						} catch (error_already_set const &) {
							PyErr_Print();
						}
						ptr = ptr->next;
					}
				}
				sli.LastSpawnType = spawnType;
			}
		}
		it++;
	}
}

//----------------------------------------------------------------------------
// Construction / Deconstruction
PythonSpawn::PythonSpawn()
{
	SetSpawn(NULL);
	UpdateDebugCounts();
}

PythonSpawn::PythonSpawn(int SpawnID)
{
	SetSpawn((PSPAWNINFO)GetSpawnByID(SpawnID));
	UpdateDebugCounts();
}

PythonSpawn::PythonSpawn(std::string Name)
{
	SetSpawn((PSPAWNINFO)GetSpawnByName((PCHAR)Name.c_str()));
	UpdateDebugCounts();
}

PythonSpawn::PythonSpawn(PythonSpawn const& Spawn)
{
	SetSpawn(Spawn.pSpawn);
	UpdateDebugCounts();
}

PythonSpawn::PythonSpawn(PSPAWNINFO pSpawn)
{
	SetSpawn(pSpawn);
	UpdateDebugCounts();
}

PythonSpawn::~PythonSpawn()
{
	UnregisterSpawn(this);
	PySpawn_CurCount--;
}

PythonSpawn& PythonSpawn::operator= (PythonSpawn const& Spawn)
{
	SetSpawn(Spawn.pSpawn);
	return *this;
}

bool PythonSpawn::operator== (PythonSpawn const& right)
{
	AssertIsValid();
	return right.pSpawn == pSpawn;
}

//----------------------------------------------------------------------------
// Linked list properties
PythonSpawn PythonSpawn::Next()
{
	AssertIsValid();
	return PythonSpawn(pSpawn->pNext);
}

bool PythonSpawn::HasNext()
{
	AssertIsValid();
	return pSpawn->pNext != NULL;
}


PythonSpawn PythonSpawn::Prev()
{
	AssertIsValid();
	return PythonSpawn(pSpawn->pPrev);
}

bool PythonSpawn::HasPrev()
{
	AssertIsValid();
	return pSpawn->pPrev != NULL;
}

//----------------------------------------------------------------------------
// Properties
std::string PythonSpawn::Name()
{
	AssertIsValid();
	return pSpawn->Name;
}

std::string PythonSpawn::Surname()
{
	AssertIsValid();
	return pSpawn->Lastname;
}

std::string PythonSpawn::CleanName()
{
	AssertIsValid();

	char buffer[MAX_STRING];
	strcpy_s(buffer, MAX_STRING, pSpawn->Name);
	CleanupName(buffer, MAX_STRING, false, false);

	return buffer;
}

std::string PythonSpawn::DisplayedName()
{
	AssertIsValid();
	return pSpawn->DisplayedName;
}

std::string PythonSpawn::Title()
{
	AssertIsValid();
	return pSpawn->Title;
}

std::string PythonSpawn::Suffix()
{
	AssertIsValid();
	return pSpawn->Suffix;
}

//----------------------------------------------------------------------------
// General Properties
int PythonSpawn::Level()
{
	AssertIsValid();
	return pSpawn->Level;
}

int PythonSpawn::ID()
{
	AssertIsValid();
	return pSpawn->SpawnID;
}

int PythonSpawn::CurrentHP()
{
	AssertIsValid();
	return pSpawn->HPCurrent;
}

int PythonSpawn::MaxHP()
{
	AssertIsValid();
	return pSpawn->HPMax;
}

float PythonSpawn::PctHP()
{
	AssertIsValid();
	return (float)(pSpawn->HPCurrent * 100) / (float)pSpawn->HPMax;
}

eSpawnType PythonSpawn::Type()
{
	AssertIsValid();
	return GetSpawnType(pSpawn);
}

PlayerClass PythonSpawn::Class()
{
	AssertIsValid();

	if (pSpawn->mActorClient.Class > TotalClasses || pSpawn->mActorClient.Class < Unknown)
		return Unknown;

	return (PlayerClass)pSpawn->mActorClient.Class;
}

//----------------------------------------------------------------------------
// Location Properties

boost::python::tuple PythonSpawn::Position()
{
	AssertIsValid();
	return boost::python::make_tuple(pSpawn->Y, pSpawn->X);
}

boost::python::tuple PythonSpawn::Position3D()
{
	AssertIsValid();
	return boost::python::make_tuple(pSpawn->Y, pSpawn->X, pSpawn->Z);
}

float PythonSpawn::X()
{
	AssertIsValid();
	return pSpawn->X;
}

float PythonSpawn::Y()
{
	AssertIsValid();
	return pSpawn->Y;
}

float PythonSpawn::Z()
{
	AssertIsValid();
	return pSpawn->Z;
}

//----------------------------------------------------------------------------
// Pets
bool PythonSpawn::HasPet()
{
	AssertIsValid();
	return (int)pSpawn->PetID != -1;
}

int PythonSpawn::PetID()
{
	AssertIsValid();
	return pSpawn->PetID;
}

PythonSpawn PythonSpawn::Pet()
{
	AssertIsValid();
	return PythonSpawn(pSpawn->PetID);
}

bool PythonSpawn::HasMaster()
{
	AssertIsValid();
	return pSpawn->MasterID != 0;
}

int PythonSpawn::MasterID()
{
	AssertIsValid();
	return pSpawn->MasterID;
}

PythonSpawn PythonSpawn::Master()
{
	AssertIsValid();
	return PythonSpawn(pSpawn->MasterID);
}

//----------------------------------------------------------------------------
// Methods
bool PythonSpawn::IsGroupMember()
{
	AssertIsValid();
	return ::IsGroupMember(pSpawn);
}

bool PythonSpawn::IsRaidMember()
{
	AssertIsValid();
	return ::IsRaidMember(pSpawn);
}

bool PythonSpawn::LineOfSight()
{
	AssertIsValid();
	return ::LineOfSight(GetCharInfo()->pSpawn, pSpawn);
}

bool PythonSpawn::IsFleeing()
{
	AssertIsValid();
	return ::IsMobFleeing(GetCharInfo()->pSpawn, pSpawn);
}

void PythonSpawn::Target()
{
	if (pSpawn != NULL) {
		AssertIsValid();
	}
	pTarget = (EQPlayer*)pSpawn;
}

void PythonSpawn::Clear()
{
	SetSpawn(NULL);
}

//----------------------------------------------------------------------------
// Internal Details
//

void PythonSpawn::Invalidate()
{
	Valid = false;
	pSpawn = NULL;
}

bool PythonSpawn::IsValid()
{
	return Valid && pSpawn != NULL;
}

void PythonSpawn::AssertIsValid()
{
	if (!IsValid()) {
		PyErr_SetString(PyExc_AssertionError, "Spawn is Invalid");
		throw_error_already_set();
	}
}

void PythonSpawn::SetSpawn(PSPAWNINFO pSpawn)
{
	this->pSpawn = pSpawn;
	Valid = (this->pSpawn != NULL);

	RegisterSpawn(this);
}

std::string PythonSpawn::__repr__()
{
	if (IsValid()) {
		ostringstream ss;
		ss << "Spawn: " << pSpawn->Name << " (" << pSpawn->SpawnID << ")";
		return ss.str();
	}

	return "Spawn: <Invalid>";
}

void PythonSpawn::UpdateDebugCounts()
{
	PySpawn_CurCount++;
	PySpawn_TotalCount++;

	if (PySpawn_CurCount > PySpawn_MaxCount)
		PySpawn_MaxCount = PySpawn_CurCount;
}

//----------------------------------------------------------------------------
// Target subclass - represents current target
class PythonTargetType : public PythonSpawn
{
public:
	PythonTargetType()
	{
		SetSpawn((PSPAWNINFO)pTarget);
	}
};

class PythonSpawnListType : public PythonSpawn
{
public:
	PythonSpawnListType()
	{
		SetSpawn((PSPAWNINFO)pSpawnList);
	}
};

void Init_Module_PyMQ2_Spawn()
{
	class_<PythonSpawn>("Spawn")
		// Constructors
		.def(init<int>())
		.def(init<std::string>())
		.def(init<PythonSpawn const&>())
		.def(self == self)
		
		.add_property("Next", &PythonSpawn::Next)
		.add_property("HasNext", &PythonSpawn::HasNext)
		.add_property("Prev", &PythonSpawn::Prev)
		.add_property("HasPrev", &PythonSpawn::HasPrev)

		// Properties
		.add_property("Name", &PythonSpawn::Name)
		.add_property("Surname", &PythonSpawn::Surname)
		.add_property("CleanName", &PythonSpawn::CleanName)
		.add_property("DisplayedName", &PythonSpawn::DisplayedName)
		.add_property("Title", &PythonSpawn::Title)
		.add_property("Suffix", &PythonSpawn::Suffix)

		.add_property("Level", &PythonSpawn::Level)
		.add_property("ID", &PythonSpawn::ID)
		.add_property("CurrentHP", &PythonSpawn::CurrentHP)
		.add_property("MaxHP", &PythonSpawn::MaxHP)
		.add_property("PctHP", &PythonSpawn::PctHP)
		.add_property("Type", &PythonSpawn::Type)
		.add_property("Class", &PythonSpawn::Class)

		.add_property("Position", &PythonSpawn::Position)
		.add_property("Position3D", &PythonSpawn::Position3D)
		.add_property("X", &PythonSpawn::X)
		.add_property("Y", &PythonSpawn::Y)
		.add_property("Z", &PythonSpawn::Z)

		.add_property("HasPet", &PythonSpawn::HasPet)
		.add_property("PetID", &PythonSpawn::PetID)
		.add_property("Pet", &PythonSpawn::Pet)
		.add_property("HasMaster", &PythonSpawn::HasMaster)
		.add_property("MasterID", &PythonSpawn::MasterID)
		.add_property("Master", &PythonSpawn::Master)


		// Methods
		.def("IsGroupMember", &PythonSpawn::IsGroupMember)
		.def("IsRaidMember", &PythonSpawn::IsRaidMember)
		.def("LineOfSight", &PythonSpawn::LineOfSight)
		.def("IsFleeing", &PythonSpawn::IsFleeing)

		.def("Target", &PythonSpawn::Target)
		.def("Clear", &PythonSpawn::Clear)

		.def("IsValid", &PythonSpawn::IsValid)
		.def("__repr__", &PythonSpawn::__repr__)
	
		// Events
		.def_readwrite("OnTarget", &PythonSpawn::OnTarget)
		.def_readwrite("OnDeath", &PythonSpawn::OnDeath)

		.def_readwrite("OnMouseOver", &PythonSpawn::OnMouseOver)
		.def_readwrite("OnMouseEnter", &PythonSpawn::OnMouseEnter)
		.def_readwrite("OnMouseLeave", &PythonSpawn::OnMouseLeave)

		.def_readwrite("OnInvalidate", &PythonSpawn::OnInvalidate)
	;

	class_<PythonTargetType, bases<PythonSpawn> >("Target");
	class_<PythonSpawnListType, bases<PythonSpawn> >("SpawnList");
}