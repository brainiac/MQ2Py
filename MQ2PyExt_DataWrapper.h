/* MQ2PyExt_DataWrapper.h
 * Copyright (c) 2009 Stephen Raub.
 *
 * Distribution is not allowed without the consent of the author. 
 *
 * Definitions and such for the MQ2 Data API Wrapper class library
 */

#pragma once

boost::python::object MQ2TypeToPythonType(const MQ2TYPEVAR& value);

class DataMemberWrapper
{
	MQ2Type* type;
	MQ2TYPEVAR value;
	std::string member;

public:
	DataMemberWrapper(MQ2Type* t, std::string name, const MQ2TYPEVAR& value);

	std::string __repr__();
	boost::python::object __getattr__(std::string name);
	boost::python::object __call__();
	boost::python::object __call_str__(std::string);
	boost::python::object __call_int__(int);
};


class DataTypeWrapper
{
	MQ2Type* type;
	MQ2TYPEVAR value;

public:
	DataTypeWrapper(MQ2Type* t, const MQ2TYPEVAR& value);

	std::string Name();
	std::string __repr__();

	std::string GetMemberName(int);
	boost::python::object __getattr__(std::string name);
};

class TLOWrapper
{
	fMQData function;
	std::string name;
	PyObject* self;

	void InstallAttributes();

public:
	TLOWrapper(PyObject*, std::string);
	TLOWrapper(PyObject*, const TLOWrapper & other);

	std::string __repr__();
	boost::python::object __call__();
	boost::python::object __call_str__(std::string args);
	boost::python::object __call_int__(int value);

	boost::python::object __getattr__(std::string name);
};