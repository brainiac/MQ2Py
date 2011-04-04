"""
	MQ2 Python Module Initialization
	
	MQ2Internal module is implemented by the plugin. This module binds
	everything to the python namespace.
"""

# ----------------------------------------------------------------------------

import MQ2Internal

# Install in-game output & error handlers
import sys as __sys
__sys.stdout = MQ2Internal.OutputHandler()
__sys.stderr = MQ2Internal.ErrorHandler()

# ----------------------------------------------------------------------------
# import main game classes

from MQ2Internal import Character, Spell, Spawn, SpawnList, Item
from MQ2Internal import AddCommand, RemoveCommand, WriteChat, DoCommand, GetNextArg, GetArg, GetGameState, Parse, script

# ----------------------------------------------------------------------------
# "Me" is the global instance of the Character object

Me = Character()

# ----------------------------------------------------------------------------
# create TLO dispatcher object

class __TLODispatcher(object):
	def __getattr__(self, name):
		try:
			return MQ2Internal.TopLevelObject(name)
		except:
			raise AttributeError, name
			
	def __repr__(self):
		return "<mq2 TLO dispatcher>"

# Create an instance of the TLO dispatcher object
TLO = __TLODispatcher()

# ----------------------------------------------------------------------------
# import plugin base class

from mq2.plugin import Plugin

# ----------------------------------------------------------------------------	

def ProcessEvents():
	pass


# ----------------------------------------------------------------------------
