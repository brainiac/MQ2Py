/* MQ2PyExt_Guild.cpp
 * Copyright (c) 2009 Stephen Raub.
 *
 * Distribution is not allowed without the consent of the author. 
 *
 * This module implements the guild info support
 */

#include "MQ2PyPCH.h"
#include "MQ2Py.h"
#include "MQ2PyExt.h"

using namespace boost;
using namespace boost::python;

class PythonGuild
{
public:
	static bool Exists()
	{
		return pGuild != NULL && GetCharInfo() != NULL
			&& GetCharInfo()->pSpawn->GuildID != 0xffffffff;
	}

	static std::string Name()
	{
		return Exists() ? NameByID(GetCharInfo()->pSpawn->GuildID) : "<none>";
	}

	static bool IsMember(std::string name)
	{
		return pGuild != NULL && pGuild->FindMemberByName((char*)name.c_str());
	}

	static std::string NameByID(int id)
	{
		return GetGuildByID((DWORD)id);
	}

	static int IDByName(std::string name)
	{
		return (int)GetGuildIDByName((PCHAR)name.c_str());
	}
};


void Init_Module_PyMQ2_Guild()
{
	class_<PythonGuild>("Guild")
		.def("Exists", &PythonGuild::Exists)
		.staticmethod("Exists")
		.def("Name", &PythonGuild::Name)
		.staticmethod("Name")
		.def("IsMember", &PythonGuild::IsMember, "Determines if name is a member of the current guild.")
		.staticmethod("IsMember")
		.def("NameByID", &PythonGuild::NameByID)
		.staticmethod("NameByID")
		.def("IDByName", &PythonGuild::IDByName)
		.staticmethod("IDByName");
}