/* MQ2PyExt_Command.cpp
* Copyright (c) 2009 Stephen Raub.
*
* Distribution is not allowed without the consent of the author. 
*
* This module implements the aspects of the Python extension that
* involve the management of a slash commands constructed from python.
*/

#include "MQ2PyPCH.h"
#include "MQ2Py.h"
#include "MQ2PyExt.h"

using namespace boost;
using namespace boost::python;

std::map<std::string, PythonCommand*> CommandMap;

PythonCommand::PythonCommand(std::string command, python::object function)
{
	this->command = command;
	this->function = function;

	buffer = CreateFunction();

	/* set this memory to be executable */
	DWORD OldProtect;
	VirtualProtectEx(GetCurrentProcess(), (LPVOID)buffer, 18, PAGE_EXECUTE_READWRITE, &OldProtect);

	AddCommand((PCHAR)command.c_str(), (fEQCommand)buffer);

	DebugSpew("MQ2Py: Created new command: %s", command.c_str());
}

PythonCommand::PythonCommand(std::string command, python::object function, python::object self)
{
}

PythonCommand::~PythonCommand()
{
	DebugSpew("MQ2Py: Removing command: %s", command.c_str());

	RemoveCommand((PCHAR)command.c_str());
	delete [] buffer;
}

unsigned char* PythonCommand::CreateFunction()
{
	unsigned char temp[] = {
		0x55,							// push ebp
		0x8b, 0xec,						// mov ebp, esp
		0xff, 0x75, 0x0C,				// push dword ptr [ebp+0xCh]
		0xb9, 0x00, 0x00, 0x00, 0x00,	// mov ecx, 0x00000000
		0xe8, 0x00, 0x00, 0x00, 0x00,	// call PythonCommand::CallCommand
		0xc9,							// leave
		0xc3							// retn
	};

	unsigned char* buffer = new unsigned char[18];
	memcpy(buffer, temp, 18);

	unsigned int this_ptr = (unsigned int)this;
	*((unsigned int*)(buffer+7)) = this_ptr;

	signed int diff = (int)GetFunctionAddress(0, &PythonCommand::CallCommand) - (int)(buffer + 16);
	*((signed int*)(buffer + 12)) = diff;

	return buffer;
}

void PythonCommand::CallCommand(const char* line)
{
	try {
		function(line);
	} catch (error_already_set const &) {
		PyErr_Print();
	}
}

unsigned int PythonCommand::GetFunctionAddress(int dummy, ...)
{
	va_list valist;
	va_start(valist, dummy);

	return va_arg(valist, unsigned int);
}


void PyMQ2_AddCommand(std::string command, python::object function)
{
	/* This should result in an exception if it fails... */
	if (CommandMap.find(command) != CommandMap.end()) {
		std::ostringstream msgs;
		msgs << "Command already exists: '" << command << "'";
		PyErr_SetString(PyExc_RuntimeError, msgs.str().c_str());
		throw_error_already_set();
		return;
	}

	CommandMap[command] = new PythonCommand(command, function);
}

void PyMQ2_RemoveCommand(std::string command)
{
	std::map<std::string, PythonCommand*>::iterator pos = CommandMap.find(command);

	/* this should result in an exception if it fails... */
	if (pos == CommandMap.end()) {
		std::ostringstream msgs;
		msgs << "Command not found: '" << command << "'";
		PyErr_SetString(PyExc_RuntimeError, msgs.str().c_str());
		throw_error_already_set();
		return;
	}

	PythonCommand* function = pos->second;
	delete function;

	CommandMap.erase(pos);
}

void Init_Module_PyMQ2_Command()
{
	def("AddCommand", &PyMQ2_AddCommand);
	def("RemoveCommand", &PyMQ2_RemoveCommand);
}

void Shutdown_Module_PyMQ2_Command()
{
	std::map<std::string, PythonCommand*>::iterator pos = CommandMap.begin();

	/* iterate through all the commands and delete them. */
	while (pos != CommandMap.end())
	{
		PythonCommand* function = pos->second;
		delete function;

		pos++;
	}

	CommandMap.clear();
}