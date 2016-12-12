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

//#define Py_NO_ENABLE_SHARED
//#define Py_BUILD_CORE

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
