/* MQ2PyExt_DataWrapper.cpp
 * Copyright (c) 2009 Stephen Raub.
 *
 * Distribution is not allowed without the consent of the author. 
 *
 * This module implements the MQ2 Data API Wrapper class library
 */

#include "MQ2PyPCH.h"
#include "MQ2Py.h"
#include "MQ2PyExt.h"
#include "MQ2PyExt_DataWrapper.h"

using namespace boost;
using namespace boost::python;

//----------------------------------------------------------------------------

python::object MQ2TypeToPythonType(const MQ2TYPEVAR& value)
{
	/* Select return type based on result */
	if (value.Type == pIntType)
		return python::object(value.Int);

	else if (value.Type == pFloatType)
		return python::object(value.Float);

	else if (value.Type == pBoolType)
		return python::object((bool)value.DWord);

	else if (value.Type == pStringType)
		return python::str((const char*)value.Ptr);

	else {
		/* Unknown datatype. Return the python-wrapper of that 
		 * datatype. */
		MQ2Type* type = value.Type;
		return python::object(DataTypeWrapper(type, value));
	}

	return python::object();
}

//----------------------------------------------------------------------------

DataMemberWrapper::DataMemberWrapper(MQ2Type* t, std::string n, const MQ2TYPEVAR& v)
{
	type = t;
	member = n;
	value = v;

	if (type == NULL) {
		PyErr_SetString(PyExc_AssertionError, "Invalid Type");
		throw_error_already_set();
	}
}

std::string DataMemberWrapper::__repr__()
{
	ostringstream oss;
	oss << "<MQ2Type: " << type->GetName() << ", Member: " << member << ">";

	return oss.str();
}

python::object DataMemberWrapper::__getattr__(std::string attr)
{
	MQ2TYPEVAR newValue;
	memcpy(&newValue, &value, sizeof(value));

	type->GetMember(value.VarPtr, (PCHAR)member.c_str(), "", newValue);

	return python::object(DataMemberWrapper(newValue.Type, attr, newValue));
}

python::object DataMemberWrapper::__call_str__(std::string args)
{
	MQ2TYPEVAR newValue;
	memcpy(&newValue, &value, sizeof(value));

	type->GetMember(value.VarPtr, (PCHAR)member.c_str(), (PCHAR)args.c_str(), newValue);

	return MQ2TypeToPythonType(newValue);
}

python::object DataMemberWrapper::__call_int__(int arg)
{
	ostringstream oss;
	oss << arg;

	return __call_str__(oss.str());
}

python::object DataMemberWrapper::__call__()
{
	return __call_str__("");
}

//----------------------------------------------------------------------------

DataTypeWrapper::DataTypeWrapper(MQ2Type* t, const MQ2TYPEVAR& v)
{
	if (t == NULL) {
		PyErr_SetString(PyExc_AssertionError, "Type is Invalid");
		throw_error_already_set();
	}
	type = t;
	value = v;
}

std::string DataTypeWrapper::Name()
{
	return type->GetName();
}

std::string DataTypeWrapper::__repr__()
{
	ostringstream oss;
	oss << "<MQ2Type: " << type->GetName() << ">";

	return oss.str();
}

std::string DataTypeWrapper::GetMemberName(int index)
{
	PCHAR result = type->GetMemberName(index);

	if (result == NULL) {
		PyErr_SetString(PyExc_IndexError, "Index is out of range");
		throw_error_already_set();
	}

	return result;
}

python::object DataTypeWrapper::__getattr__(std::string name)
{
	if (!type->FindMember((PCHAR)name.c_str()) && !type->InheritedMember((PCHAR)name.c_str())) {
		PyErr_SetString(PyExc_AttributeError, name.c_str());
		throw_error_already_set();
	}

	return python::object(DataMemberWrapper(type, name, value));
}


//----------------------------------------------------------------------------

namespace boost { namespace python
{
	template <>
	struct has_back_reference<TLOWrapper>
		: mpl::true_
	{};
}}

TLOWrapper::TLOWrapper(PyObject* _self, std::string _name)
{
	PMQ2DATAITEM item = FindMQ2Data((PCHAR)_name.c_str());
	
	if (item == NULL) {
		PyErr_SetString(PyExc_AssertionError, "Invalid Data Name");
		throw_error_already_set();
	}

	name = item->Name;
	function = item->Function;
	self = _self;
}

TLOWrapper::TLOWrapper(PyObject* _self, const TLOWrapper& other)
{
	self = _self;
	function = other.function;
	name = other.name;
}

std::string TLOWrapper::__repr__()
{
	ostringstream oss;
	oss << "<TopLevelObject: " << name << ">";

	return oss.str();
}

python::object TLOWrapper::__call_str__(std::string args)
{
	MQ2TYPEVAR value;
	memset(&value, 0, sizeof(value));
	BOOL result = function((PCHAR)args.c_str(), value);

	/* If no result, return None */
	if (result == FALSE)
		return python::object();

	return MQ2TypeToPythonType(value);
}

python::object TLOWrapper::__call_int__(int value)
{
	ostringstream oss;
	oss << value;

	return __call_str__(oss.str());
}

python::object TLOWrapper::__call__()
{
	return __call_str__("");
}

python::object TLOWrapper::__getattr__(std::string name)
{
	/* this is a combination of __call__ and DataTypeWrapper::__getattr__ */
	MQ2TYPEVAR value;
	memset(&value, 0, sizeof(value));
	BOOL result = function("", value);

	/* If no result, return None */
	if (result == FALSE)
		return python::object();

	return DataTypeWrapper(value.Type, value).__getattr__(name);
}


//----------------------------------------------------------------------------

void Init_Module_PyMQ2_DataWrapper()
{
	class_<DataMemberWrapper>("DataTypeMember", no_init)
		.def("__repr__", &DataMemberWrapper::__repr__)
		.def("__getattr__", &DataMemberWrapper::__getattr__)
		.def("__call__", &DataMemberWrapper::__call__)
		.def("__call__", &DataMemberWrapper::__call_str__)
		.def("__call__", &DataMemberWrapper::__call_int__)
	;

	class_<DataTypeWrapper>("DataType", no_init)
		.def("GetName", &DataTypeWrapper::Name)
		.def("GetMemberName", &DataTypeWrapper::GetMemberName)
		.def("__repr__", &DataTypeWrapper::__repr__)
		.def("__getattr__", &DataTypeWrapper::__getattr__)
	;

	class_<TLOWrapper>("TopLevelObject", no_init)
		.def(init<std::string>())
		.def("__repr__", &TLOWrapper::__repr__)
		.def("__call__", &TLOWrapper::__call__)
		.def("__call__", &TLOWrapper::__call_str__)
		.def("__call__", &TLOWrapper::__call_int__)
		.def("__getitem__", &TLOWrapper::__call_str__)
		.def("__getitem__", &TLOWrapper::__call_int__)
		.def("__getattr__", &TLOWrapper::__getattr__)
	;
}
