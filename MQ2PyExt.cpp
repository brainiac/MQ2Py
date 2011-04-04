/* MQ2PyExt.cpp
 * Copyright (c) 2009 Stephen Raub.
 *
 * Distribution is not allowed without the consent of the author. 
 *
 * This module implements the general aspects of the python extension
 */

#include "MQ2PyPCH.h"
#include "MQ2Py.h"
#include "MQ2PyExt.h"

using namespace std;
using namespace boost;
using namespace boost::python;

// Notes:
// Can Override Py_GetPath, Py_getPrefix, Py_GetExecPrefix to shortcut the paths for python
//----------------------------------------------------------------------------

void Cmd_PyDebug(PSPAWNINFO spawn, PCHAR line)
{
	std::string command = PyMQ2_GetArg(line, 1);

	if (command == "")
	{
		WriteChatf("\aoMQ2Py Debug Info");
		WriteChatf("\ao==================================");
		WriteChatf("\ag*\ao Spawn Counters:");
		WriteChatf("\ay**\ao Current Live Count: \ag%i", PySpawn_CurCount);
		WriteChatf("\ay**\ao Maximum Live Count: \ag%i", PySpawn_MaxCount);
		WriteChatf("\ay**\ao Total Live Count: \ag%i", PySpawn_TotalCount);
	}

}

//----------------------------------------------------------------------------
// Python MQ2 Module Implementation

struct OutputHandler
{
	void write(std::string msg)
	{
		if (PyWnd) {
			if (msg.length() > 0) {
				PyWnd->Write_NoBreak("%s", msg.c_str());
			}
		} else
			WriteChatf("\ayPy>\ax %s", msg.c_str());
	}
};

struct ErrorHandler
{
	void write(std::string msg)
	{
		if (PyWnd) {
			if (msg.length() > 0)
				PyWnd->Write_NoBreak("\ar%s", msg.c_str());
		} else
			WriteChatf("\arPy>\ax %s", msg.c_str());
	}
};

struct PythonQuitter
{
	void quit()
	{
		DoCommand(NULL, "/plugin mq2py unload");
	}
};

void PyPrint(const char* format, ...)
{
	CHAR szOutput[MAX_STRING] = {0};
	va_list valist;
	va_start(valist, format);
	vsprintf(szOutput, format, valist);

	if (PyWnd)
		PyWnd->Write("%s", szOutput);
	else
		WriteChatf("%s", szOutput);
}

void PyMQ2_WriteChat(std::string msg)
{
	WriteChatColor((PCHAR)msg.c_str());
}

void PyMQ2_DoCommand(std::string msg, bool delayed = true)
{
	HideDoCommand(GetCharInfo()->pSpawn, (PCHAR)msg.c_str(), delayed);
}
BOOST_PYTHON_FUNCTION_OVERLOADS(DoCommand_Overloads, PyMQ2_DoCommand, 1, 2);

std::string PyMQ2_GetNextArg(std::string line, int number, bool csv, char separator)
{
	char* ret = GetNextArg(line.c_str(), number, csv, separator);
	return std::string(ret);
}
BOOST_PYTHON_FUNCTION_OVERLOADS(GetNextArg_Overloads, PyMQ2_GetNextArg, 1, 4);

std::string PyMQ2_GetArg(std::string src, int number, bool leaveQuotes, bool toParen, 
						 char separator, bool anyNonAlphaNum)
{
	CHAR temp[MAX_STRING];
	GetArg(temp, src.c_str(), number, leaveQuotes, toParen, separator, anyNonAlphaNum);

	return std::string(temp);
}
BOOST_PYTHON_FUNCTION_OVERLOADS(GetArg_Overloads, PyMQ2_GetArg, 2, 6);

std::string PyMQ2_ParseMacroData(std::string data)
{
	char buff[MAX_STRING];
	strcpy_s(buff, MAX_STRING, data.c_str());

	ParseMacroParameter(NULL, buff);

	return buff;
}

void PyMQ2_DebugSpew(std::string text)
{
	DebugSpew("%s", text.c_str());
}

// Initialize the extension module
void Init_Module_PyMQ2_DataWrapper();
void Init_Module_PyMQ2_Plugin();
void Init_Module_PyMQ2_Command();
void Init_Module_PyMQ2_Enum();
void Init_Module_PyMQ2_Guild();
void Init_Module_PyMQ2_Character();
void Init_Module_PyMQ2_Spawn();
void Init_Module_PyMQ2_Spell();
void Init_Module_PyMQ2_Item();

void Shutdown_Module_PyMQ2_Plugin();
void Shutdown_Module_PyMQ2_Command();
void Shutdown_Module_PyMQ2_Item();

void Init_Module_PyMQ2()
{
	class_<OutputHandler>("OutputHandler")
		.def("write", &OutputHandler::write);
	class_<ErrorHandler>("ErrorHandler")
		.def("write", &ErrorHandler::write);
	class_<PythonQuitter>("PythonQuitter")
		.def("__call__", &PythonQuitter::quit);

	// Utilities
	def("script", &PyMQ2_RunScript);
	def("WriteChat", &PyMQ2_WriteChat);
	def("DoCommand", &PyMQ2_DoCommand, DoCommand_Overloads());
	def("GetNextArg", &PyMQ2_GetNextArg, GetNextArg_Overloads());
	def("GetArg", &PyMQ2_GetArg, GetArg_Overloads());
	def("GetGameState", &GetGameState);
	def("Parse", &PyMQ2_ParseMacroData);
	def("DebugSpew", &PyMQ2_DebugSpew);

	Init_Module_PyMQ2_DataWrapper();
	Init_Module_PyMQ2_Spell();
	Init_Module_PyMQ2_Item();
	Init_Module_PyMQ2_Spawn();
	Init_Module_PyMQ2_Character();
	Init_Module_PyMQ2_Guild();
	Init_Module_PyMQ2_Plugin();
	Init_Module_PyMQ2_Command();
	Init_Module_PyMQ2_Enum();
}

void Shutdown_Module_PyMQ2()
{
	Shutdown_Module_PyMQ2_Item();
	Shutdown_Module_PyMQ2_Plugin();
	Shutdown_Module_PyMQ2_Command();
}
