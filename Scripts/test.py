from mq2 import *

class PluginTest(Plugin):
	pulse = 0
	
	def CmdTest(self, line):
		p = SpawnList()
		i = 0
		while p.IsValid():
			i = i + 1
			p = p.Next
			
		print "%i spawns counted" % (i)

	def Initialize(self):
		print "It Works!"
		AddCommand("/cmdtest", self.CmdTest)
		self.spawn = SpawnList()

	def Shutdown(self):
		RemoveCommand("/cmdtest")
		
	def OnPulse(self):
		self.pulse = self.pulse + 1
		
		if self.pulse % 100 == 0:
			# print "Pulse: %i" % self.pulse
			pass
		
		if self.spawn.IsValid():
			#print "Hit Spawn: %s" % (self.spawn.CleanName)		
			self.spawn = self.spawn.Next
	
	def OnWriteChatColor(self, line, color, filter):
		#print "OnWriteChatColor: %s, %i, %i" % (line, color, filter)
		return 0
		
	def OnIncomingChat(self, line, color):
		#print "OnIncomingChat: %s %i" % (line, color)
		return 0
		
	def OnAddSpawn(self, spawn):
		print "New Spawn: \ay%s" % (spawn.Name)
		
	def OnRemoveSpawn(self, spawn):
		print "Spawn Removed: \ay%s" % (spawn.Name)
