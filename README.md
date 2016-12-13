MQ2Py
=====

Adds python intepreter support for MacroQuest 2.

You need Visual Studio 2015 to build this plugin. To build it, open up your MacroQuest2 solution and add MQ2Py.vcxproj

Current dependencies include:
* boost_python
* python2.7

Both of these dependencies are now configured to pull from Nuget. If you are using visual studio I think it should just work. If you are building at the command line then you will need to run nuget to pull the new packages.

_Note_: You'll need to copy the Scripts directory to your MacroQuest2 output directory.
