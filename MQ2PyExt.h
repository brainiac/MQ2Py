/* MQ2PyExt.h
 * Copyright (c) 2009 Stephen Raub.
 *
 * Distribution is not allowed without the consent of the author. 
 *
 * Definitions and such for MQ2Py Extension
 */

#pragma once

// Notifies spawn tracker that a spawn is no longer valid
void RemoveTrackedSpawn(PSPAWNINFO pSpawn);
void UpdateTrackedSpawns();

// Utility functions
std::string PyMQ2_GetArg(std::string src, int number, bool leaveQuotes = false, bool toParen = false, 
						 char separator = '\0', bool anyNonAlphaNum = false);
std::string PyMQ2_GetNextArg(std::string line, int number = 1, bool csv = false, char separator = '\0');

// Debug variables
extern int PySpawn_CurCount;
extern int PySpawn_MaxCount;
extern int PySpawn_TotalCount;


// Plugin base class
class Plugin
{
public:
	virtual void Initialize();
	virtual void Shutdown();

	virtual void OnZoned();
	virtual void OnCleanUI();
	virtual void OnReloadUI();
	virtual void OnDrawHUD();
	virtual void SetGameState(int NewGameState);
	virtual void OnPulse();
	virtual int OnWriteChatColor(std::string, int Color, int Filter);
	virtual int OnIncomingChat(std::string Line, int Color);

	virtual void OnAddSpawn(class PythonSpawn& NewSpawn);
	virtual void OnRemoveSpawn(class PythonSpawn& OldSpawn);
#if 0	// Not yet Implemented callpacks
	virtual void OnAddGroundItem(PyGroundItem NewItem);
	virtual void OnRemoveGroundItem(PyGroundItem OldItem);
#endif

	virtual ~Plugin();

	boost::python::object instance;
	std::string classname;
	std::string filename;
};

// Python Command class
class PythonCommand
{
	std::string command;
	boost::python::object function;
	unsigned char* buffer;

	std::string classname;
	bool isclass;

public:
	PythonCommand(std::string command, boost::python::object function);
	PythonCommand(std::string command, boost::python::object function, boost::python::object self);
	~PythonCommand();

private:
	unsigned char* CreateFunction();
	void CallCommand(const char* line);

	static unsigned int GetFunctionAddress(int dummy, ...);
};
