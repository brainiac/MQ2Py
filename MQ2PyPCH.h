/* MQ2PyPCH.h
 * Copyright (c) 2009 Stephen Raub.
 *
 * Distribution is not allowed without the consent of the author. 
 *
 * Precompiled Header
 */

#pragma once

#include "../MQ2Plugin.h"

#include <stdlib.h>
#include <stdarg.h>

// Annoyingly, the python nuget package does not supply a debug python_d.lib
// and the release version of the headers #define some functions into macros
// meaning they don't exist... undef Py_DEBUG so they stay defined as functions,
// and ponder the day when an actual nuget package with debug libs is introduced
// that actually works.
#include <pyconfig.h>
#ifdef _DEBUG
#undef Py_DEBUG
#endif

#include <Python.h>

#define BOOST_PYTHON_STATIC_LIB

#include <boost/python.hpp>

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <sstream>
#include <list>

#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/suite/indexing/map_indexing_suite.hpp>
