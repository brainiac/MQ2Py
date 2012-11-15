/* MQ2Py.h
 * Copyright (c) 2009 Stephen Raub.
 *
 * Distribution is not allowed without the consent of the author. 
 *
 * Python support for MQ2
 */

#pragma once

extern CHAR szScriptPath[MAX_STRING];

// Functions
void Init_Module_PyMQ2();
void Shutdown_Module_PyMQ2();

void Cmd_Python(PSPAWNINFO spawn, PCHAR line);
void Cmd_Script(PSPAWNINFO spawn, PCHAR line);
void Cmd_PyClear(PSPAWNINFO spawn, PCHAR line);
void Cmd_PyDebug(PSPAWNINFO spawn, PCHAR line);

void PyMQ2_RunScript(std::string line);

// Python Interpreter Window class
class CMQPyWnd : public CCustomWnd
{
public:
    CMQPyWnd(CXStr *Template);
    ~CMQPyWnd();

    int WndNotification(CXWnd *pWnd, unsigned int Message, void *unknown);
    void SetChatFont(int size);

	void HandleCommand(const char* szBuffer);
	void AddTextToCommandHistory(const char* szBuffer);

	int LoadDefaultChatSettings(PCHAR Key, int setting);
	void LoadChatFromINI();

	void DoSaveChatToDefault();
	void SaveChatToINI();

	void Write(const char*, ...);
	void Write_NoBreak(const char*, ...);

	void Clear();

	static CMQPyWnd* Create();

private:

	void AppendText(const CXStr& text);

    CTextEntryWnd *InputBox;
    CStmlWnd *OutputBox;
    DWORD OutBoxLines;
    DWORD FontSize;

	vector<string> CommandHistory;
	enum { CMD_HIST_MAX = 40 };
	int CurrentCommand;

	CHAR szChatINISection[MAX_STRING];

	// Python Window Position Defaults
	int ChatTop;
	int ChatBottom;
	int ChatLeft;
	int ChatRight;
	int Locked;
	int Fades;
	int DelayTime;
	int Duration;
	int Alpha;
	int FadeToAlpha;
	int BGType;
	int BGTintRed;
	int BGTintGreen;
	int BGTintBlue;
	int SavedFontSize;
	
	boost::python::object InteractiveInterpreter;
	std::vector<std::string> CommandBuffer;
	bool MoreCommands;
};

extern CMQPyWnd* PyWnd;

void PyPrint(const char*, ...);
void CreatePluginInstance(boost::python::object plugin);

void Propogate_OnZoned();
void Propogate_OnReloadUI();
void Propogate_OnCleanUI();
void Propogate_OnDrawHUD();
void Propogate_SetGameState(int NewGameState);
void Propogate_OnPulse();
int Propogate_OnWriteChatColor(std::string Line, int Color, int Filter);
int Propogate_OnIncomingChat(std::string Line, int Color);

// These aren't implemented yet
void Propogate_OnAddSpawn(PSPAWNINFO pSpawn);
void Propogate_OnRemoveSpawn(PSPAWNINFO pSpawn);
void Propogate_OnAddGroundItem(PGROUNDITEM pItem);
void Propogate_OnRemoveGroundItem(PGROUNDITEM pItem);
