"""
	MQ2 Python Module -- Plugin Initialization
"""

# ----------------------------------------------------------------------------

from time import ctime

from MQ2Internal import Plugin as PluginBase
from MQ2Internal import AddCommand, RemoveCommand

# ----------------------------------------------------------------------------


class Plugin(PluginBase):

	def __init__(self):
		super(Plugin, self).__init__()
		self._commands = dict()

	def Destroy(self):
		# cleanup commands that were registered
		for name in self._commands.keys():
			self.RemoveCommand(name)


	def AddCommand(self, name, callback):
		print "AddCommand: %s" % (name)
		self._commands[name] = callback
		AddCommand(name, callback)

	def RemoveCommand(self, name):
		del self._commands[name]
		RemoveCommand(name)