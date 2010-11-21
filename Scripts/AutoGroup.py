from mq2 import *

def WriteChat(str):
	print str

class AutoGroup(Plugin):
	autoGroup = False
	autoGroupGuild = False
	autoAdd = False
	namesLoaded = 0

	def AutoGroupCommand(self, line):
		
		if line == "on":
			self.autoGroup = True
			WriteChat("\atMQ2AutoGroup:\ax: AutoGroup (\aynames\ax) now \agON\ax.")
		elif line == "off":
			self.autoGroup = False
			WriteChat("\atMQ2AutoGroup\ax: AutoGroup (\aynames\ax) now \ayOFF\ax.")
		elif line == "guild":
			self.autoGroupGuild = not self.autoGroupGuild
			WriteChat("\atMQ2AutoGroup\ax: AutoGroup (\ayguild\ax) now %s\ax." 
				% ("\agOn" if self.autoGroupGuild else "\arOff"))
		elif line == "auto":
			self.autoAdd = not self.autoAdd
			WriteChat("\atMQ2AutoGroup\ax: Auto (\ayadd\ax) now %s\ax."
				% ("\agOn" if self.autoAdd else "\arOff"))
		elif line == "load":
			self.LoadSettings()
		elif line == "save":
			self.SaveSettings()
		elif line == "add":
			pass
		elif line == "del":
			pass
		elif line == "clear":
			pass
		elif line == "list":
			pass
		elif line == "":
			WriteChat(
			 """\arMQ2AutoGroup: \atAutoGroup is %s\ax.
				\arMQ2AutoGroup: \atAutoGroup guild is %s\ax.
				\arMQ2AutoGroup: \atNames loaded: \ag%i\at.
				\arMQ2AutoGroup: \agType \ay/autogroup help \agfor usage.""" 
					% ("\agON" if self.autoGroup else "\arOFF", "\agON" if self.autoGroupGuild else "\arOFF", self.namesLoaded))
		else:
			# show help
			WriteChat(
			 """\atUsage\ax: \ag/autogroup [help | on | off | guild | load | save | add | del | clear | list]
				\ag  Automatically accepts group/raid invite from specified chars, or guild.
				\ag  Names are NOT case sensitive.
				\ag  Using \ar/autogroup\ag with no options will show current status.
				\at  on \ax= \ayTurns automatic group/raid invite accept from specified chars on.
				\at  off \ax= \ayTurns automatic group/raid invite accept from specified chars off.
				\at  guild \ax= \ayToggles automatic group/raid invite accept from your guild members.
				\at  load \ax= \ayLoads options and names from .ini file
				\ay             (any current unsaved names will be lost!).
				\at  save \ax= \ayUpdates .ini to match current options and names.
				\at  auto \ax= \ayToggles automatic toon adding to INI, if on, any toon you log in will be added to auth list.
				\at  add \ax= \ayAdd a new char name (ex: \ar/autogroup add bubbawar\ay).
				\at  del \ax= \ayDelete a char name (ex: \ar/autogroup del 15\ay deletes name #15).
				\at  clear \ax= \ayClears all char names.
				\at  list \ax= \ayLists current char names.""")

	def CheckName(self, name):
		pass

	def LoadSettings(self):
		pass
		
	def SaveSettings(self):
		pass


	# plugin callbacks
	#
	
	def Initialize(self):
		AddCommand("/autogroup", self.AutoGroupCommand)
		self.autoGroup = False
		self.autoGroupGuild = False
		self.autoAdd = False

	def Shutdown(self):
		RemoveCommand("/autogroup")

	def OnIncomingChat(self, line, color):
		return 0
		
	def SetGameState(self, state):
		pass
