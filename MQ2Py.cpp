/* MQ2Py.cpp
 * Copyright (c) 2009 Stephen Raub.
 *
 * Distribution is not allowed without the consent of the author. 
 *
 * Python support for MQ2
 */

#include "MQ2PyPCH.h"
#include "MQ2Py.h"

PreSetup("MQ2Py");

using namespace boost;

// Global Data
CHAR szScriptPath[MAX_STRING];
CMQPyWnd* PyWnd = NULL;

#if PY_VERSION_HEX >= 0x03000000
#pragma comment(lib, "python35.lib")
#else
#pragma comment(lib, "python27.lib")
#endif

PyObject* PyInit_MQ2()
{
#if PY_VERSION_HEX >= 0x03000000
	static PyModuleDef_Base initial_m_base = {
		PyObject_HEAD_INIT(NULL)
		0, /* m_init */
		0, /* m_index */
		0 /* m_copy */
	};
	static PyMethodDef initial_methods[] = { { 0, 0, 0, 0 } };
	static struct PyModuleDef moduledef = {
		initial_m_base,
		"MQ2Internal",
		0, /* m_doc */
		-1, /* m_size */
		initial_methods,
		0, /* m_reload */
		0, /* m_traverse */
		0, /* m_clear */
		0, /* m_free */
	};

	return boost::python::detail::init_module(
		moduledef, &Init_Module_PyMQ2);
#else
	return boost::python::detail::init_module(
		"MQ2Internal", &Init_Module_PyMQ2);
#endif
}

// Called once, when the plugin is to initialize
PLUGIN_API void InitializePlugin()
{
	// First things first. We need to initialize the python path to be our script directory
	sprintf_s(szScriptPath, MAX_STRING, "%s\\Scripts", gszINIPath);

	CHAR szEnvVar[MAX_STRING];
	sprintf_s(szEnvVar, MAX_STRING, "PYTHONPATH=%s;%s\\Lib", szScriptPath, szScriptPath);
	_putenv(szEnvVar);

	// Now, the python library can be initialized.
	Py_Initialize();

	// Initialize Python Window
	DebugSpewAlways("Python Window: Initializing");

	// Initialize the python extensions
	PyInit_MQ2();

	// This redirects the python output to the output classes below
	PyRun_SimpleString("import MQ2Internal");
	PyRun_SimpleString("exit = MQ2Internal.PythonQuitter()");
	PyRun_SimpleString("quit = MQ2Internal.PythonQuitter()");

	PyRun_SimpleString("import mq2");
	//PyRun_SimpleString("sys.stderr = MQ2.ErrorHandler()");
	//PyRun_SimpleString("sys.stdout = MQ2.OutputHandler()");

	// Now do something else
	int GameState = GetGameState();
	if (GameState == GAMESTATE_CHARSELECT || GameState == GAMESTATE_INGAME) {
		PyWnd = CMQPyWnd::Create();
	}

	AddCommand("/python", &Cmd_Python);
	AddCommand("/script", &Cmd_Script);
	AddCommand("/pyclear", &Cmd_PyClear);
	AddCommand("/pydebug", &Cmd_PyDebug);

	PyRun_SimpleString("print \"MQ2 Python by \a#7ff47fbrainiac\ax loaded successfully!\"");

	// Execute the startup script
}

// Called once, when the plugin is to shutdown
PLUGIN_API void ShutdownPlugin()
{
	DebugSpewAlways("Python Window: Shutting Down");

	RemoveCommand("/python");
	RemoveCommand("/script");
	RemoveCommand("/pyclear");

	/* Propogate the shutdown message to the plugin system. */
	Shutdown_Module_PyMQ2();

	if (PyWnd != NULL) {
		delete PyWnd;
		PyWnd = NULL;
	}

	// Cleanup python and shut down
	Py_Finalize();
}


// Called once directly before shutdown of the new ui system, and also
// every time the game calls CDisplay::CleanGameUI()
PLUGIN_API void OnCleanUI()
{
	DebugSpewAlways("Python Window: Cleaning UI");

	Propogate_OnCleanUI();

	if (PyWnd) {
		delete PyWnd;
		PyWnd = NULL;
	}
}

// Called once directly after the game ui is reloaded, after issuing /loadskin
PLUGIN_API void OnReloadUI()
{
	DebugSpewAlways("Python Window: Reloading UI");

	if (PyWnd == NULL) {
		PyWnd = CMQPyWnd::Create();
	}

	Propogate_OnReloadUI();
}

// Called once directly after initialization, and then every time the gamestate changes
PLUGIN_API void SetGameState(DWORD GameState)
{
	DebugSpew("Python Window: Changing Game State");

	if (GameState == GAMESTATE_INGAME && PyWnd == NULL)
	{
		PyWnd = CMQPyWnd::Create();
	}

	Propogate_SetGameState(GameState);
}


// This is called every time MQ pulses
PLUGIN_API void OnPulse()
{
	if (gGameState == GAMESTATE_CHARSELECT && PyWnd == NULL) {
		PyWnd = CMQPyWnd::Create();
	}

	if (PyWnd)
	{
		if (InHoverState())
		{
			((CXWnd*)PyWnd)->DoAllDrawing();
		}
	}

	Propogate_OnPulse();
}

#pragma region Plugin Function Stubs
// Called after entering a new zone
PLUGIN_API void OnZoned()
{
	Propogate_OnZoned();
}

// Called every frame that the "HUD" is drawn -- e.g. net status / packet loss bar
PLUGIN_API void OnDrawHUD()
{
}

// This is called every time WriteChatColor is called by MQ2Main or any plugin,
// IGNORING FILTERS, IF YOU NEED THEM MAKE SURE TO IMPLEMENT THEM. IF YOU DONT
// CALL CEverQuest::dsp_chat MAKE SURE TO IMPLEMENT EVENTS HERE (for chat plugins)
PLUGIN_API DWORD OnWriteChatColor(PCHAR Line, DWORD Color, DWORD Filter)
{
	return Propogate_OnWriteChatColor(Line, Color, Filter);
}

// This is called every time EQ shows a line of chat with CEverQuest::dsp_chat,
// but after MQ filters and chat events are taken care of.
PLUGIN_API DWORD OnIncomingChat(PCHAR Line, DWORD Color)
{
	return Propogate_OnIncomingChat(Line, Color);
}

// This is called each time a spawn is added to a zone (inserted into EQ's list of spawns),
// or for each existing spawn when a plugin first initializes
// NOTE: When you zone, these will come BEFORE OnZoned
PLUGIN_API void OnAddSpawn(PSPAWNINFO pNewSpawn)
{
	Propogate_OnAddSpawn(pNewSpawn);
}

// This is called each time a spawn is removed from a zone (removed from EQ's list of spawns).
// It is NOT called for each existing spawn when a plugin shuts down.
PLUGIN_API void OnRemoveSpawn(PSPAWNINFO pSpawn)
{
	Propogate_OnRemoveSpawn(pSpawn);
}

// This is called each time a ground item is added to a zone
// or for each existing ground item when a plugin first initializes
// NOTE: When you zone, these will come BEFORE OnZoned
PLUGIN_API void OnAddGroundItem(PGROUNDITEM pNewGroundItem)
{
	Propogate_OnAddGroundItem(pNewGroundItem);
}

// This is called each time a ground item is removed from a zone
// It is NOT called for each existing ground item when a plugin shuts down.
PLUGIN_API void OnRemoveGroundItem(PGROUNDITEM pGroundItem)
{
	Propogate_OnRemoveGroundItem(pGroundItem);
}
#pragma endregion

void Cmd_Python(PSPAWNINFO pChar, PCHAR line)
{
	// Execute a line of code as python
	if (PyWnd) {
		PyWnd->HandleCommand(line);
	} else {
		PyRun_SimpleString(line);
	}
}


void Cmd_PyClear(PSPAWNINFO pChar, PCHAR line)
{
	// Clear the python window
	if (PyWnd) {
		PyWnd->Clear();
	}
}

void Cmd_Script(PSPAWNINFO pChar, PCHAR line)
{
	PyMQ2_RunScript(line);
}
