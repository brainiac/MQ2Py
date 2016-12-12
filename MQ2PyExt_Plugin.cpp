/* MQ2PyExt_Plugin.cpp
 * Copyright (c) 2009 Stephen Raub.
 *
 * Distribution is not allowed without the consent of the author. 
 *
 * This module implements the aspects of the Python extension that
 * involve the management of a plugin.
 */

#include "MQ2PyPCH.h"
#include "MQ2Py.h"
#include "MQ2PyExt.h"
#include "MQ2PyExt_Spawn.h"

#include <fstream>

using namespace boost;
using namespace boost::python;

// Struct contains information about the plugin
class PluginInfo
{
public:
	std::string Name;
	std::string Author;
	std::string Description;
	std::string Version;

	PluginInfo()
	{
		static int AutoCount = 1;

		ostringstream ss;
		ss << "UnnamedPlugin" << AutoCount++;
		Name = ss.str();

		Author = "<Unknown>";
		Description = "";
		Version = "1.0";
	}
};

#pragma region Plugin base class implementation
Plugin::~Plugin()
{
	//PyPrint("Plugin %s Destroyed", classname.c_str());
}

void Plugin::Initialize() 
{
	//WriteChatf("%s -> Initialize()", Info.Name.c_str());
}

void Plugin::Shutdown()
{
	//WriteChatf("%s -> Shutdown()", Info.Name.c_str());
}

void Plugin::OnZoned() {}
void Plugin::OnCleanUI() {}
void Plugin::OnReloadUI() {}
void Plugin::OnDrawHUD() {}
void Plugin::SetGameState(int NewGameState) {}
void Plugin::OnPulse() {}

int Plugin::OnWriteChatColor(std::string Line, int Color, int Filter) 
{
	return 0;
}

int Plugin::OnIncomingChat(std::string Line, int Color)
{
	return 0;
}

void Plugin::OnAddSpawn(PythonSpawn& NewSpawn) {}
void Plugin::OnRemoveSpawn(PythonSpawn& OldSpawn) {}

#if 0
void Plugin::OnAddGroundItem(PythonGroundItem NewItem) {}
void Plugin::OnRemoveGroundItem(PythonGroundItem OldItem) {}
#endif

void Plugin::Destroy() {}
#pragma endregion

#pragma region Python Plugin Wrapper
class PluginWrapper : public Plugin, public wrapper<Plugin>
{
public:
	// Wrapper for Initialize()
	void Initialize()
	{
		if (override init = this->get_override("Initialize"))
			init();
		else
			Plugin::Initialize();
	}

	void Default_Initialize()
	{
		Plugin::Initialize();
	}

	// Wrapper for Shutdown()
	void Shutdown()
	{
		if (override shutdown = this->get_override("Shutdown"))
			shutdown();
		else
			Plugin::Shutdown();

		this->Destroy();
	}

	void Default_Shutdown()
	{
		Plugin::Shutdown();

		this->Destroy();
	}

	// Wrapper for OnZoned()
	void OnZoned()
	{
		if (override onZoned = this->get_override("OnZoned"))
			onZoned();
		else
			Plugin::OnZoned();
	}

	void Default_OnZoned()
	{
		Plugin::OnZoned();
	}

	// Wrapper for OnCleanUI
	void OnCleanUI()
	{
		if (override onCleanUI = this->get_override("OnCleanUI"))
			onCleanUI();
		else
			Plugin::OnCleanUI();
	}

	void Default_OnCleanUI()
	{
		Plugin::OnCleanUI();
	}

	// Wrapper for OnReloadUI()
	void OnReloadUI()
	{
		if (override onReloadUI = this->get_override("OnReloadUI"))
			onReloadUI();
		else
			Plugin::OnReloadUI();
	}

	void Default_OnReloadUI()
	{
		Plugin::OnReloadUI();
	}

	// Wrapper for OnDrawHUD()
	void OnDrawHUD()
	{
		if (override onDrawHUD = this->get_override("OnDrawHUD"))
			onDrawHUD();
		else
			Plugin::OnDrawHUD();
	}

	void Default_OnDrawHUD()
	{
		Plugin::OnDrawHUD();
	}

	// Wrapper for SetGameState()
	void SetGameState(int GameState)
	{
		if (override setGameState = this->get_override("SetGameState"))
			setGameState(GameState);
		else
			Plugin::SetGameState(GameState);
	}

	void Default_SetGameState(int GameState)
	{
		Plugin::SetGameState(GameState);
	}

	// Wrapper for OnPulse()
	void OnPulse()
	{
		if (override onPulse = this->get_override("OnPulse"))
			onPulse();
		else
			Plugin::OnPulse();
	}

	void Default_OnPulse()
	{
		Plugin::OnPulse();
	}

	// Wrapper for OnWriteChatColor()
	int OnWriteChatColor(std::string Line, int Color, int Filter)
	{
		if (override write = this->get_override("OnWriteChatColor"))
			return write(Line, Color, Filter);

		return Plugin::OnWriteChatColor(Line, Color, Filter);
	}

	int Default_OnWriteChatColor(std::string Line, int Color, int Filter)
	{
		return Plugin::OnWriteChatColor(Line, Color, Filter);
	}

	// Wrapper for OnIncomingChat()
	int OnIncomingChat(std::string Line, int Color)
	{
		if (override chat = this->get_override("OnIncomingChat"))
			return chat(Line, Color);

		return Plugin::OnIncomingChat(Line, Color);
	}

	int Default_OnIncomingChat(std::string Line, int Color)
	{
		return Plugin::OnIncomingChat(Line, Color);
	}

	void OnAddSpawn(PythonSpawn& spawn)
	{
		if (override addSpawn = this->get_override("OnAddSpawn"))
			addSpawn(spawn);
		else
			Plugin::OnAddSpawn(spawn);
	}

	void Default_OnAddSpawn(PythonSpawn& spawn)
	{
		Plugin::OnAddSpawn(spawn);
	}

	void OnRemoveSpawn(PythonSpawn& spawn)
	{
		if (override removeSpawn = this->get_override("OnRemoveSpawn"))
			removeSpawn(spawn);
		else
			Plugin::OnRemoveSpawn(spawn);
	}

	void Default_OnRemoveSpawn(PythonSpawn& spawn)
	{
		Plugin::OnRemoveSpawn(spawn);
	}

	// destroy is implemented on the python side to provide cleanup-like services
	void Destroy()
	{
		if (override destroy = this->get_override("Destroy"))
			destroy();
	}

	void Default_Destroy()
	{
		Plugin::Destroy();
	}
};
#pragma endregion

/* Links plugin class names with a plugin instance. */
std::map<std::string, Plugin*> PluginMap;
std::map<Plugin*, python::object> PluginInstanceMap;

/* Workhorse function. Does the brunt work of setting up and registering a new function for tracking with
 * the rest of the system. */
bool CreatePluginInstance(object plugin, std::string scriptname)
{
	/* We know this object is a subclass of Plugin, because thats how we found it in the first place. What
	 * we're going to do next is check it against the list of known plugins */

	std::string name = python::extract<std::string>(plugin.attr("__name__"));	// plugin is a type

	/* check to see if the plugin exists already */
	if (PluginMap.find(name) != PluginMap.end()) {
		WriteChatf("\ar*\ax Plugin class already exists: \a#ff9999%s\aw (try \"/script %s unload\" first)",
			name.c_str(), scriptname.c_str());
		return false;
	}

	/* new create an instance of this plugin */
	object main_module = import("__main__");
	object main_namespace = main_module.attr("__dict__");

	// Local scope
	object local_module = import("__main__");
	object local_namespace = local_module.attr("__dict__");

	// TODO: Wrap this with a try
	std::string command = name + "()";
	python::object instance;
	try {
		instance = python::eval(command.c_str(), main_namespace, local_namespace);
	} catch (...) {
		PyErr_Print();

		return false;
	}

	/* this is the new plugin pointer. We'll hold onto it so we can use it later. */
	Plugin* plugin_ptr = extract<Plugin*>(instance);
	plugin_ptr->filename = scriptname;
	plugin_ptr->classname = name;

	/* Insert plugin pointer into the plugin map and then initialize. */
	try {
		plugin_ptr->Initialize();
	} catch  (...) {
		PyErr_Print();

		// TODO: anything that was added will need to be removed
		try {
			plugin_ptr->Destroy();
		} catch (...) {
			PyErr_Print();
		}

		return false;
	}

	/* connect the name of the plugin with the pointer to the plugin */
	PluginMap[name] = plugin_ptr;
	

	/* Keep track of the instance object. Can't keep it with the plugin because
	 * of wierd deconstruction issues */
	PluginInstanceMap[plugin_ptr] = instance;

	return true;
}

#pragma region Plugin Callbacks
void Propogate_OnZoned()
{
	std::map<std::string, Plugin*>::iterator it = PluginMap.begin();
	while (it != PluginMap.end()) {
		try {
			it->second->OnZoned();
		} catch (error_already_set const &) {
			PyErr_Print();
		}
		it++;
	}
}

void Propogate_OnReloadUI()
{
	std::map<std::string, Plugin*>::iterator it = PluginMap.begin();
	while (it != PluginMap.end()) {
		try {
			it->second->OnReloadUI();
		} catch (error_already_set const &) {
			PyErr_Print();
		}
		it++;
	}
}

void Propogate_OnCleanUI()
{
	std::map<std::string, Plugin*>::iterator it = PluginMap.begin();
	while (it != PluginMap.end()) {
		try {
			it->second->OnCleanUI();
		} catch (error_already_set const &) {
			PyErr_Print();
		}
		it++;
	}
}

void Propogate_OnDrawHUD()
{
	std::map<std::string, Plugin*>::iterator it = PluginMap.begin();
	while (it != PluginMap.end()) {
		try {
			it->second->OnDrawHUD();
		} catch (error_already_set const &) {
			PyErr_Print();
		}
		it++;
	}
}

void Propogate_SetGameState(int NewGameState)
{
	std::map<std::string, Plugin*>::iterator it = PluginMap.begin();
	while (it != PluginMap.end()) {
		try {
			it->second->SetGameState(NewGameState);
		} catch (error_already_set const &) {
			PyErr_Print();
		}
		it++;
	}
}

void Propogate_OnPulse()
{
	UpdateTrackedSpawns();

	std::map<std::string, Plugin*>::iterator it = PluginMap.begin();
	while (it != PluginMap.end()) {
		try {
			it->second->OnPulse();
		} catch (error_already_set const &) {
			PyErr_Print();
		}
		it++;
	}

	try {
		PyRun_SimpleString("mq2.ProcessEvents()");
	} catch (error_already_set const &) {
		PyErr_Print();
	}
}

int Propogate_OnWriteChatColor(std::string Line, int Color, int Filter)
{
	int val = 0;

	std::map<std::string, Plugin*>::iterator it = PluginMap.begin();
	while (it != PluginMap.end()) {
		try {
			val |= it->second->OnWriteChatColor(Line, Color, Filter);
		} catch (error_already_set const &) {
			PyErr_Print();
		}
		it++;
	}

	return val != 0;
}

int Propogate_OnIncomingChat(std::string Line, int Color)
{
	int val = 0;

	std::map<std::string, Plugin*>::iterator it = PluginMap.begin();
	while (it != PluginMap.end()) {
		try {
			val |= it->second->OnIncomingChat(Line, Color);
		} catch (error_already_set const &) {
			PyErr_Print();
		}
		it++;
	}

	return val != 0;
}

// These aren't implemented yet
void Propogate_OnAddSpawn(PSPAWNINFO pSpawn)
{
	PythonSpawn pySpawn(pSpawn);

	std::map<std::string, Plugin*>::iterator it = PluginMap.begin();
	while (it != PluginMap.end()) {
		try {
			it->second->OnAddSpawn(pySpawn);
		} catch (error_already_set const &) {
			PyErr_Print();
		}

		it++;
	}
}

void Propogate_OnRemoveSpawn(PSPAWNINFO pSpawn)
{
	PythonSpawn pySpawn(pSpawn);

	std::map<std::string, Plugin*>::iterator it = PluginMap.begin();
	while (it != PluginMap.end()) {
		try {
			it->second->OnRemoveSpawn(pySpawn);
		} catch (error_already_set const &) {
			PyErr_Print();
		}

		it++;
	}

	RemoveTrackedSpawn(pSpawn);
}

void Propogate_OnAddGroundItem(PGROUNDITEM pItem)
{
}

void Propogate_OnRemoveGroundItem(PGROUNDITEM pItem)
{
}
#pragma endregion

void PyMQ2_RunScript(std::string line)
{
	/* for now, lets just set the plugin name to the strlwr of line. This is what
	 * will be used to track the plugin */
	std::transform(line.begin(), line.end(), line.begin(), tolower);

	// TODO: Parse arguments and ensure script file exists (with or without extension)
	CHAR szName[MAX_STRING] = {0};
	GetArg(szName, line.c_str(), 1);

	const char* szCommand = GetNextArg(line.c_str());

	if (!_stricmp(szName, "list")) {
		WriteChatf("\aoPython plugins Loaded:");
		WriteChatf("\ao==================================");

		int count = 0;
		std::map<std::string, Plugin*>::iterator it = PluginMap.begin();

		while (it != PluginMap.end()) {
			count++;
			WriteChatf("\ag*\ax %s", it->first.c_str());
			it++;
		}

		WriteChatf("%i Python plugin script(s) loaded.", count);
		return;
	}

	if (szName[0] == '\0') {
		WriteChatf("\ar*\ax \ayError.\ax Usage: /script name [unload] | list");
		return;
	}

	if (!_stricmp(szCommand, "unload") || !_stricmp(szCommand, "reload")) {
		/* search through the plugin classes for the plugin that is instantiated through this
		 * filename */
		int count = 0;
		std::map<std::string, Plugin*>::iterator it = PluginMap.begin();
		while (it != PluginMap.end()) {
			Plugin* plugin = it->second;
			if (plugin->filename == szName)
			{
				try {
					plugin->Shutdown();
				} catch (error_already_set const &) {
					PyErr_Print();
				}

				/* free the plugin from the map */
				std::map<std::string, Plugin*>::iterator itcopy = it++;
				PluginMap.erase(itcopy);

				/* free the instance */
				std::map<Plugin*, python::object>::iterator it2 = PluginInstanceMap.find(plugin);
				if (it2 != PluginInstanceMap.end()) {
					PluginInstanceMap.erase(it2);
					count++;
				}
			}
			else
			{
				it++;
			}
		}

		if (it == PluginMap.end() && count == 0) {
			WriteChatf("\ar*\ax Plugin not found: \a#ff9999'%s'", szName);
			return;
		}

		/* If we're reloading, we can just continue to the loading part... */
		if (_stricmp(szCommand, "reload")) {
			WriteChatf("\ar*\ax Plugin unloaded: \a#99ff99'%s'", szName);
			return;
		}
	}

	/* What remains is the code to load a new script plugin */
	try
	{
		std::string scriptname = szName;

		// Global scope
		object main_module = import("__main__");
		object main_namespace = main_module.attr("__dict__");

		// Local scope
		//object local_module = import(line.c_str());
		object local_module = import("__main__");
		object local_namespace = local_module.attr("__dict__");

		CHAR filename[MAX_STRING];
		sprintf_s(filename, MAX_STRING, "%s\\%s.py", szScriptPath, scriptname.c_str());

		/* automatic instantiation implementation:
		 * get list of all current subclasses of the plugin base class*/
		WriteChatf("Reading mq2 subclasses", filename);
		python::list first = extract<python::list>(python::eval("mq2.Plugin.__subclasses__()", main_namespace, local_namespace));

		WriteChatf("Trying to open file: \a#99ff99'%s'", filename);
		std::ifstream infile(filename);
		if (!infile.good()) {
			WriteChatf("\ar*\ax Can't open file: \a#99ff99'%s'", filename);
			throw std::invalid_argument(filename);
		}

		python::exec_file(filename, main_namespace, local_namespace);

		/* file has been executed. Now look for any new plugin class definitions */
		python::list second = extract<python::list>(python::eval("mq2.Plugin.__subclasses__()", main_namespace, local_namespace));

		/* the new subclasses are appended to the end of the list, so we'll simply perform the comparison 
		 * by using the size. */
		python::ssize_t count = boost::python::len(first);
		python::stl_input_iterator<object> begin(second), end;

		std::vector<python::object> type_list;

		while (begin != end) {
			object entry = *begin++;

			if (count <= 0) {
				type_list.push_back(entry);
			}
			count--;
		}

		if (type_list.size() > 0) {

			// Register class objects
			for (std::vector<python::object>::iterator it = type_list.begin(); it != type_list.end(); it++) {
				std::string name = extract<std::string>(it->attr("__name__"));
				if (!CreatePluginInstance(*it, scriptname)) {
					WriteChatf("\ar*\ax Error loading plugin \a#ff9999'%s'\ax", name.c_str());
				} else {
					if (!_stricmp(szCommand, "reload")) {
						WriteChatf("\ar*\ax Plugin Reloaded: \a#9999ff'%s'", name.c_str());
					} else {
						WriteChatf("\ar*\ax Plugin Loaded: \a#9999ff'%s'", name.c_str());
					}
				}
			}
		}
	}
	catch (error_already_set const &)
	{
		PyErr_Print();
	}
	catch (std::invalid_argument& arg)
	{
		WriteChatf("\ar*\ax Couldn't load plugin: \ax%s", arg.what());
	}
}

void Init_Module_PyMQ2_Plugin()
{
	// PluginData class
	class_<PluginInfo>("PluginInfo")
		.def_readwrite("Name", &PluginInfo::Name)
		.def_readwrite("Author", &PluginInfo::Author)
		.def_readwrite("Description", &PluginInfo::Description)
		.def_readwrite("Version", &PluginInfo::Version)
	;

	// Plugin class
	class_<PluginWrapper, boost::noncopyable>("Plugin")
		.def("Initialize", &Plugin::Initialize, &PluginWrapper::Default_Initialize)
		.def("Shutdown", &Plugin::Shutdown, &PluginWrapper::Default_Shutdown)
		.def("OnZoned", &Plugin::OnZoned, &PluginWrapper::Default_OnZoned)
		.def("OnCleanUI", &Plugin::OnCleanUI, &PluginWrapper::Default_OnCleanUI)
		.def("OnReloadUI", &Plugin::OnReloadUI, &PluginWrapper::Default_OnReloadUI)
		.def("OnDrawHUD", &Plugin::OnDrawHUD, &PluginWrapper::Default_OnDrawHUD)
		.def("SetGameState", &Plugin::SetGameState, &PluginWrapper::Default_SetGameState)
		.def("OnPulse", &Plugin::OnPulse, &PluginWrapper::Default_OnPulse)
		.def("OnWriteChatColor", &Plugin::OnWriteChatColor, &PluginWrapper::Default_OnWriteChatColor)
		.def("OnIncomingChat", &Plugin::OnIncomingChat, &PluginWrapper::Default_OnIncomingChat)
		.def("OnAddSpawn", &Plugin::OnAddSpawn, &PluginWrapper::Default_OnAddSpawn)
		.def("OnRemoveSpawn", &Plugin::OnRemoveSpawn, &PluginWrapper::Default_OnRemoveSpawn)

		.def("Destroy", &Plugin::Destroy, &PluginWrapper::Default_Destroy);
		//.def_readwrite("Info", &Plugin::Info);
	;
}

/* This function must unload all 'plugins' that we have currently loaded, for purposes
 * of cleanup and shutdown. Who knows what those crazy plugins might end up doing that
 * needs a bit of tidying up... */
void Shutdown_Module_PyMQ2_Plugin()
{
	std::map<std::string, Plugin*>::iterator it = PluginMap.begin();
	while (it != PluginMap.end()) {
		Plugin* plugin = it->second;
		try {
			plugin->Shutdown();
		} catch (error_already_set const &) {
			PyErr_Print();
		}		

		/* free the plugin from the map */
		std::map<std::string, Plugin*>::iterator itcopy = it++;
		PluginMap.erase(itcopy);

		/* free the instance */
		PluginInstanceMap.erase(PluginInstanceMap.find(plugin));
	}
	PluginMap.clear();
}