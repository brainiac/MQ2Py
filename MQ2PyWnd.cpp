/* MQ2PyWnd.cpp
 * Copyright (c) 2009 Stephen Raub.
 *
 * Distribution is not allowed without the consent of the author. 
 *
 * This module implements the ingame python interpreter and status
 * window that is used for I/O with python.
 */

#include "MQ2PyPCH.h"
#include "MQ2Py.h"

using namespace boost;
using namespace boost::python;

#define MAX_CHAT_SIZE 250
#define CHAT_SIZE_RESET 200


CMQPyWnd::CMQPyWnd(CXStr *Template) : CCustomWnd(Template)
{
	// Python Window Position Defaults
	ChatTop			= 10;
	ChatBottom		= 210;
	ChatLeft		= 10;
	ChatRight		= 410;
	Locked			= 0;
	Fades			= 1;
	DelayTime		= 2000;
	Duration		= 500;
	Alpha			= 255;
	FadeToAlpha		= 255;
	BGType			= 1;
	BGTintRed		= 255;
	BGTintGreen		= 255;
	BGTintBlue		= 255;
	SavedFontSize	= 9;

	SetWndNotification(CMQPyWnd);
	InputBox = (CTextEntryWnd*)GetChildItem("CWChatInput");
	InputBox->WindowStyle |= 0x800C0;
	BitOff(WindowStyle, CWS_CLOSE);

	InputBox->UnknownCW |= 0xFFFFFFFF;
	InputBox->SetMaxChars(512);

	OutputBox = (CStmlWnd*)GetChildItem("CWChatOutput");
	OutBoxLines = 0;
	*(DWORD*)&(((PCHAR)OutputBox)[EQ_CHAT_HISTORY_OFFSET]) = 400;

	OutputBox->Clickable = 1;

	this->LoadChatFromINI();
	this->SaveChatToINI();

	CurrentCommand = -1;
	MoreCommands = false;

	try {
		python::object Code = import("code");

		object main_module = import("__main__");
		object main_namespace = main_module.attr("__dict__");

		InteractiveInterpreter = Code.attr("InteractiveInterpreter")(main_namespace);
	} catch (...) {
		PyErr_Print();
	}
	
	// Ok now we're ready to go.
	this->Write("\ayPython %s on %s", Py_GetVersion(), Py_GetPlatform());
}

CMQPyWnd::~CMQPyWnd()
{
	this->SaveChatToINI();
}

int CMQPyWnd::WndNotification(CXWnd *pWnd, unsigned int Message, void *data)
{    
	if (pWnd == (CXWnd*)InputBox)
	{
		if (Message == XWM_HITENTER)
		{
			// Copy contents of input box into a buffer
			char szBuffer[2048];
			GetCXStr((PCXSTR)InputBox->InputText, szBuffer, 2047);

			this->HandleCommand(szBuffer);
			if (szBuffer[0]) {
				this->AddTextToCommandHistory(szBuffer);
			}
		}
		else if (Message == 0x16) 
		{
			if (data != NULL)
			{
				int KeyPress = ((int*)data)[1];
				if (KeyPress == 200)		// KeyUp: 0xC8
				{
					if (CommandHistory.size() > 0) {
						CurrentCommand++;
						if (CurrentCommand < (int)CommandHistory.size() && CurrentCommand >= 0) {
							string s = CommandHistory[CurrentCommand];
							//SetCXStr(&InputBox->InputText, (PCHAR)s.c_str());
							((CXWnd*)this->InputBox)->SetWindowTextA(CXStr(s.c_str()));
						} else {
							CurrentCommand = CommandHistory.size() - 1;
						}
					}
				}
				else if (KeyPress == 208)	// KeyDown: 0xD0
				{
					if (CommandHistory.size() > 0) {
						CurrentCommand--;
						if (CurrentCommand >= 0 && CommandHistory.size() > 0) {
							string s = CommandHistory[CurrentCommand];
							SetCXStr(&InputBox->InputText, (PCHAR)s.c_str());
						} else if (CurrentCommand < 0) {
							CurrentCommand = -1;
							//SetCXStr(&InputBox->InputText, "");
							((CXWnd*)this->InputBox)->SetWindowTextA(CXStr(""));
						}
					}
				}
			}
		}
	}
	else if (pWnd == 0)
	{
		if (Message == XWM_CLOSE)
		{
			dShow = 1;
			return 1;
		}
	} 
	else if (Message == XWM_LINK)
	{
		class CChatWindow *p = (class CChatWindow *)this;

		if (OutputBox != (CStmlWnd *)pWnd) {
			CStmlWnd *tmp;
			int ret;

			DebugSpew("PyWnd: 0x%X, Msg: 0x%X, value: %Xh", pWnd, Message, data);
			DebugSpew("PyWnd: pWnd 0x%x != OutputBox 0x%x\n", pWnd, OutputBox);
			tmp = OutputBox;
			OutputBox = (CStmlWnd *)pWnd;
			ret = p->WndNotification(pWnd, Message, data);
			OutputBox = tmp;

			return ret;
		}
		return p->WndNotification(pWnd, Message, data);
	} else {
		DebugSpew("MQ2ChatWnd: 0x%X, Msg: 0x%X, value: %Xh", pWnd, Message, data);
	}

	return CSidlScreenWnd::WndNotification(pWnd, Message, data);
};

void CMQPyWnd::SetChatFont(int size)
{
	struct FONTDATA
	{
		DWORD NumFonts;
		PCHAR* Fonts; 
	};
	FONTDATA* Fonts;    // font array structure
	DWORD* SelFont;     // selected font

	// get fonts structure -- this offset can be found by looking at
	// SetChatfont which is called from the /chatfontsize function
	Fonts = (FONTDATA*)&(((char*)pWndMgr)[EQ_CHAT_FONT_OFFSET]);

	// check font array bounds and pointers
	if (size < 0 || size >= (int) Fonts->NumFonts) {
		return;
	}
	if (Fonts->Fonts == NULL) {
		return;
	}
	//DebugSpew("Setting Size: %i", size);

	SelFont = (DWORD*)Fonts->Fonts[size];

	// Save the text, change the font, then restore the text
	CXStr str(this->OutputBox->GetSTMLText());
	((CXWnd*)this->OutputBox)->SetFont(SelFont);
	((CXWnd*)this->InputBox)->SetFont(SelFont);
	this->OutputBox->SetSTMLText(str, 1, 0);
	this->OutputBox->ForceParseNow();

	// scroll to bottom of chat window
	((CXWnd*)this->OutputBox)->SetVScrollPos(this->OutputBox->VScrollMax);

	this->FontSize = size;
};

void CMQPyWnd::HandleCommand(const char* szBuffer)
{
	this->Write("%s %s\ax", MoreCommands ? "\a#7ff47f..." : "\a#67a7e3>>>", szBuffer);
	SetCXStr(&InputBox->InputText, "");

	std::string command(szBuffer);

	try {
		// Construct the command string
		ostringstream oss;
		for (std::vector<std::string>::const_iterator it = CommandBuffer.begin(); it != CommandBuffer.end(); it++) {
			oss << *it << "\n";				
		}
		oss << command;
				
		//WriteChatf("CMQPyWnd::HandleCommand about to try to run: %s", oss.str().c_str());
		bool result = extract<bool>(InteractiveInterpreter.attr("runsource")(oss.str()));
		//WriteChatf("CMQPyWnd::HandleCommand got result : %d", result);

		if (result == true) {
			// Append this line of text to the array
			CommandBuffer.push_back(command);

			// Indicate that there is more coming
			MoreCommands = true;
			SetCXStr(&this->WindowText, "Python (Continued)");
		} else {
			// The code was executed, clear the array
			CommandBuffer.clear();

			// remove the "MoreCommands" status indication
			MoreCommands = false;
			SetCXStr(&this->WindowText, "Python");
		}
	}
	catch (std::exception e) {
		// Error occurred, reset the command status and window title
		WriteChatf("CMQPyWnd::HandleCommand got internal exception : %s", e.what());
		MoreCommands = false;
		CommandBuffer.clear();

		SetCXStr(&this->WindowText, "Python");
		PyErr_Print();
	}
}

void CMQPyWnd::AddTextToCommandHistory(const char* szBuffer)
{
	if (!szBuffer || *szBuffer == '\0')
		return;

	if (CommandHistory.size() == 0 || CommandHistory.front().compare(szBuffer) != 0) {
		if (CommandHistory.size() > CMD_HIST_MAX)
			CommandHistory.pop_back();
		CommandHistory.insert(CommandHistory.begin(), string(szBuffer));
	}

	CurrentCommand = -1;
}


int CMQPyWnd::LoadDefaultChatSettings(PCHAR Key, int setting)
{
	return GetPrivateProfileInt("default", Key, setting, INIFileName);
}

void CMQPyWnd::LoadChatFromINI()
{
	ChatTop			= LoadDefaultChatSettings("ChatTop", ChatTop);
	ChatBottom		= LoadDefaultChatSettings("ChatBottom", ChatBottom);
	ChatLeft		= LoadDefaultChatSettings("ChatLeft" ,ChatLeft);
	ChatRight		= LoadDefaultChatSettings("ChatRight", ChatRight);
	Locked			= LoadDefaultChatSettings("Locked", Locked);
	Fades			= LoadDefaultChatSettings("Fades", Fades);
	DelayTime		= LoadDefaultChatSettings("Delay", DelayTime);
	Duration		= LoadDefaultChatSettings("Duration", Duration);
	Alpha			= LoadDefaultChatSettings("Alpha", Alpha);
	FadeToAlpha		= LoadDefaultChatSettings("FadeToAlpha", FadeToAlpha);
	BGType			= LoadDefaultChatSettings("BGType", BGType);
	BGTintRed		= LoadDefaultChatSettings("BGTint.red", BGTintRed);
	BGTintGreen		= LoadDefaultChatSettings("BGTint.green", BGTintGreen);
	BGTintBlue		= LoadDefaultChatSettings("BGTint.blue", BGTintBlue);
	SavedFontSize	= LoadDefaultChatSettings("FontSize", SavedFontSize);

	CHAR Buffer[MAX_STRING] = {0};
	sprintf(szChatINISection, "%s.%s", EQADDR_SERVERNAME, ((PCHARINFO)pCharData)->Name);

	this->Location.top		= GetPrivateProfileInt(szChatINISection, "ChatTop",		ChatTop, INIFileName);
	this->Location.bottom	= GetPrivateProfileInt(szChatINISection, "ChatBottom",	ChatBottom, INIFileName);
	this->Location.left		= GetPrivateProfileInt(szChatINISection, "ChatLeft",	ChatLeft, INIFileName);
	this->Location.right 	= GetPrivateProfileInt(szChatINISection, "ChatRight",	ChatRight, INIFileName);
	this->Locked		 	= GetPrivateProfileInt(szChatINISection, "Locked",		Locked, INIFileName);
	this->Fades			 	= GetPrivateProfileInt(szChatINISection, "Fades",		Fades, INIFileName);
	this->TimeMouseOver	 	= GetPrivateProfileInt(szChatINISection, "Delay",		DelayTime, INIFileName);
	this->FadeDuration	 	= GetPrivateProfileInt(szChatINISection, "Duration",	Duration, INIFileName);
	this->Alpha				= GetPrivateProfileInt(szChatINISection, "Alpha",		Alpha, INIFileName);
	this->FadeToAlpha		= GetPrivateProfileInt(szChatINISection, "FadeToAlpha",	FadeToAlpha, INIFileName);
	this->BGType			= GetPrivateProfileInt(szChatINISection, "BGType",		BGType, INIFileName);
	this->BGColor.R			= GetPrivateProfileInt(szChatINISection, "BGTint.red",	BGTintRed, INIFileName);
	this->BGColor.G			= GetPrivateProfileInt(szChatINISection, "BGTint.green",BGTintGreen, INIFileName);
	this->BGColor.B			= GetPrivateProfileInt(szChatINISection, "BGTint.blue",	BGTintBlue, INIFileName);

	GetPrivateProfileString(szChatINISection, "WindowTitle", "Python", Buffer, MAX_STRING, INIFileName);
	SetCXStr(&this->WindowText, Buffer);

	this->SetChatFont(GetPrivateProfileInt(szChatINISection, "FontSize", SavedFontSize, INIFileName));
}

void CMQPyWnd::DoSaveChatToDefault()
{
	CHAR szTemp[MAX_STRING] = {0};
	CHAR szChatINISection[MAX_STRING] = {0};
	sprintf(szChatINISection, "default");

	if (_chmod(INIFileName, _S_IREAD | _S_IWRITE) == -1) {
		if (errno != ENOENT) {
			WriteChatf("\arWARNING: Could not set write permission on MQ2Py.ini");
			DebugSpewAlways("WARNING: Could not set write permission on MQ2Py.ini");
		}
	}

	if (this->Minimized) {
		WritePrivateProfileString(szChatINISection, "ChatTop",		itoa(this->OldLocation.top,		szTemp, 10), INIFileName);
		WritePrivateProfileString(szChatINISection, "ChatBottom",	itoa(this->OldLocation.bottom,	szTemp, 10), INIFileName);
		WritePrivateProfileString(szChatINISection, "ChatLeft",		itoa(this->OldLocation.left,	szTemp, 10), INIFileName);
		WritePrivateProfileString(szChatINISection, "ChatRight",	itoa(this->OldLocation.right,	szTemp, 10), INIFileName);
	} else {
		WritePrivateProfileString(szChatINISection, "ChatTop",		itoa(this->Location.top,		szTemp, 10), INIFileName);
		WritePrivateProfileString(szChatINISection, "ChatBottom",	itoa(this->Location.bottom,		szTemp, 10), INIFileName);
		WritePrivateProfileString(szChatINISection, "ChatLeft",		itoa(this->Location.left,		szTemp, 10), INIFileName);
		WritePrivateProfileString(szChatINISection, "ChatRight",	itoa(this->Location.right,		szTemp, 10), INIFileName);
	}

	GetCXStr(this->WindowText, szTemp);
	WritePrivateProfileString(szChatINISection, "WindowTitle", szTemp, INIFileName);

	WritePrivateProfileString(szChatINISection, "Locked",		itoa(this->Locked, szTemp,10), INIFileName);
	WritePrivateProfileString(szChatINISection, "Fades",		itoa(this->Fades, szTemp,10), INIFileName);
	WritePrivateProfileString(szChatINISection, "Delay",		itoa(this->MouseOver, szTemp,10), INIFileName);
	WritePrivateProfileString(szChatINISection, "Duration",		itoa(this->FadeDuration,	szTemp,10), INIFileName);
	WritePrivateProfileString(szChatINISection, "Alpha",		itoa(this->Alpha, szTemp,10), INIFileName);
	WritePrivateProfileString(szChatINISection, "FadeToAlpha",	itoa(this->FadeToAlpha, szTemp,10), INIFileName);
	WritePrivateProfileString(szChatINISection, "BGType",		itoa(this->BGType, szTemp,10), INIFileName);
	WritePrivateProfileString(szChatINISection, "BGTint.red",	itoa(this->BGColor.R, szTemp,10), INIFileName);
	WritePrivateProfileString(szChatINISection, "BGTint.green",	itoa(this->BGColor.G, szTemp,10), INIFileName);
	WritePrivateProfileString(szChatINISection, "BGTint.blue",	itoa(this->BGColor.B, szTemp,10), INIFileName);
	WritePrivateProfileString(szChatINISection, "FontSize",		itoa(this->FontSize, szTemp,10), INIFileName);

	this->SetChatFont(GetPrivateProfileInt(szChatINISection, "FontSize", SavedFontSize, INIFileName));
}


void CMQPyWnd::SaveChatToINI()
{
	CHAR szTemp[MAX_STRING] = {0};

	if (_chmod(INIFileName, _S_IREAD | _S_IWRITE) == -1) {
		if (errno != ENOENT) {
			WriteChatf("\arWARNING: Could not set write permission on MQ2Py.ini");
			DebugSpewAlways("WARNING: Could not set write permission on MQ2Py.ini");
		}
	}

	if (this->Minimized)
	{
		WritePrivateProfileString(szChatINISection,"ChatTop",		itoa(this->OldLocation.top, szTemp, 10), INIFileName);
		WritePrivateProfileString(szChatINISection,"ChatBottom",	itoa(this->OldLocation.bottom, szTemp, 10), INIFileName);
		WritePrivateProfileString(szChatINISection,"ChatLeft",		itoa(this->OldLocation.left, szTemp, 10), INIFileName);
		WritePrivateProfileString(szChatINISection,"ChatRight",		itoa(this->OldLocation.right, szTemp, 10), INIFileName);
	}
	else
	{
		WritePrivateProfileString(szChatINISection,"ChatTop",		itoa(this->Location.top, szTemp, 10), INIFileName);
		WritePrivateProfileString(szChatINISection,"ChatBottom",	itoa(this->Location.bottom, szTemp, 10), INIFileName);
		WritePrivateProfileString(szChatINISection,"ChatLeft",		itoa(this->Location.left, szTemp, 10), INIFileName);
		WritePrivateProfileString(szChatINISection,"ChatRight",		itoa(this->Location.right, szTemp, 10), INIFileName);
	}

	GetCXStr(this->WindowText, szTemp);
	WritePrivateProfileString(szChatINISection, "WindowTitle", szTemp, INIFileName);

	WritePrivateProfileString(szChatINISection, "Locked",			itoa(this->Locked, szTemp, 10), INIFileName);
	WritePrivateProfileString(szChatINISection, "Fades",			itoa(this->Fades, szTemp,10), INIFileName);
	WritePrivateProfileString(szChatINISection, "Delay",			itoa(this->MouseOver, szTemp, 10), INIFileName);

	WritePrivateProfileString(szChatINISection, "Duration",			itoa(this->FadeDuration, szTemp, 10), INIFileName);
	WritePrivateProfileString(szChatINISection, "Alpha",			itoa(this->Alpha, szTemp, 10), INIFileName);
	WritePrivateProfileString(szChatINISection, "FadeToAlpha",		itoa(this->FadeToAlpha, szTemp, 10), INIFileName);
	WritePrivateProfileString(szChatINISection, "BGType",			itoa(this->BGType, szTemp,10), INIFileName);
	WritePrivateProfileString(szChatINISection, "BGTint.red",		itoa(this->BGColor.R, szTemp, 10), INIFileName);
	WritePrivateProfileString(szChatINISection, "BGTint.green",		itoa(this->BGColor.G, szTemp, 10), INIFileName);
	WritePrivateProfileString(szChatINISection, "BGTint.blue",		itoa(this->BGColor.B, szTemp, 10), INIFileName);
	WritePrivateProfileString(szChatINISection, "FontSize",			itoa(this->FontSize, szTemp, 10), INIFileName);
}

CHAR LineBuffer[MAX_STRING * 10] = {0};
CHAR ProcessedBuffer[MAX_STRING * 10] = {0};

void CMQPyWnd::AppendText(const CXStr& text) {

	if (text.Ptr->Length > 0) {
		//CXStr empty;
		//this->OutputBox->SetSTMLText(empty, true, NULL);
		this->OutputBox->AppendSTML(text);
		if (++this->OutBoxLines >= MAX_CHAT_SIZE) {
			DWORD diff = this->OutBoxLines - CHAT_SIZE_RESET;
			//WriteChatf("CMQPyWnd::AppendText: %d, %d", this->OutBoxLines, diff);
			this->OutputBox->StripFirstSTMLLines(diff);
			this->OutBoxLines = CHAT_SIZE_RESET;
		}
		((CXWnd*)this->OutputBox)->SetVScrollPos(this->OutputBox->VScrollMax);
		//this->OutputBox->ForceParseNow();
		//WriteChatf("content: --");
		//CXStr content = this->OutputBox->GetSTMLText();
		//CHAR buffer[1024];
		//GetCXStr(content.Ptr, buffer, 1024);
		//WriteChatf("%d: %s", strlen(buffer), buffer);
		//WriteChatf("--");
		//this->OutputBox->StripFirstSTMLLines(1);
		//this->OutputBox->ForceParseNow();
		//content = this->OutputBox->GetSTMLText();
		//GetCXStr(content.Ptr, buffer, 1024);
		//WriteChatf("%d: %s", strlen(buffer), buffer);
		//WriteChatf("--");
	}
}


void CMQPyWnd::Write(const char* msg, ...)
{
	va_list valist;
	va_start(valist, msg);
	vsprintf_s(LineBuffer, MAX_STRING * 10, msg, valist);

	this->dShow = 1;
	
	MQToSTML(LineBuffer, ProcessedBuffer, MAX_STRING * 10, 0xffffffff);
	strcat_s(ProcessedBuffer, MAX_STRING, "<br>");

	CXStr Text(ProcessedBuffer);
	ConvertItemTags(Text, 0);	

	this->AppendText(Text);
}

void CMQPyWnd::Write_NoBreak(const char* msg, ...)
{
	va_list valist;
	va_start(valist, msg);
	vsprintf_s(LineBuffer, MAX_STRING * 10, msg, valist);

	this->dShow = 1;
	
	MQToSTML(LineBuffer, ProcessedBuffer, MAX_STRING * 10, 0xffffffff);

	// Replace file path with <scripts>
	string temp = ProcessedBuffer;
	size_t found = temp.find(szScriptPath);
	if (found != string::npos) {
		temp.replace(found, strlen(szScriptPath), "<scripts>");
		strcpy_s(ProcessedBuffer, MAX_STRING * 10, temp.c_str());
	}

	CXStr Text(ProcessedBuffer);
	ConvertItemTags(Text, 0);	

	this->AppendText(Text);
	this->OutputBox->ForceParseNow();
}

CMQPyWnd* CMQPyWnd::Create()
{
	DebugSpew("Python Window: Creating Window");

	CXStr ChatWnd("ChatWindow");
	CMQPyWnd* pyWnd = new CMQPyWnd(&ChatWnd);
	if (!pyWnd)
		return NULL;

	return pyWnd;
}

void CMQPyWnd::Clear()
{
	((CChatWindow*)this)->Clear();
	OutBoxLines = 0;
}