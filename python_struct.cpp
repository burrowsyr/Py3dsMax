/*
	\file		python_struct.cpp

	\remarks	This file defines the 3dsMax struct called python

	\author		Blur Studio (c) 2010
	\email		beta@blur.com

	\edit		Michael Loeffler @ Burrows (c) 2015

	\license	This software is released under the GNU General Public License.  For more info, visit: http://www.gnu.org/
*/

#include "imports.h"

#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>

#include <import.h>
#include <graminit.h>
#include <pythonrun.h>

// updated to work with max 2012's new maxscript organization
#if __MAXSCRIPT_2012__ || __MAXSCRIPT_2013__ || __MAXSCRIPT_2015__

#include "macros/define_external_functions.h"
#include "macros/define_instantiation_functions.h"

// include settings for previous versions of 3dsmax
#else

#include "defextfn.h"
#include "definsfn.h"

#endif

#include "macros.h"
#include "wrapper.h"

// additional string conversion
#include "stringUtils.h"

// Define the python struct in 3dsMax
def_struct_primitive( import,  pymax, "import" );
def_struct_primitive( reload,  pymax, "reload" );
def_struct_primitive( run,     pymax, "run" );
def_struct_primitive( exec,    pymax, "exec" );

// Define additional python struct to check running python version
def_struct_primitive( sysPath, pymax, "sysPath" );
def_struct_primitive( environ, pymax, "environ" );
def_struct_primitive( version, pymax, "version" );
def_struct_primitive( home,    pymax, "home" );
def_struct_primitive( prefix,  pymax, "prefix" );

// python.home function: print the home dir location of python
Value* sysPath_cf(Value** anyArgs, int count) {
	// Step 1: make sure the arguments supplied are correct
	check_arg_count( python.sysPath, 0, count );

	// Step 2: protect the maxscript memory
	MXS_PROTECT( two_value_locals( mxs_check, mxs_return ) );

	// Step 3: print result into maxscript listener
	char* cmd = 
			"print u'\\n'.join([s for s in sys.path])";
	PyRun_SimpleString(cmd);
	return &ok;	
	}

// python.home function: print the home dir location of python
Value* environ_cf(Value** anyArgs, int count) {
	// Step 1: make sure the arguments supplied are correct
	check_arg_count( python.environ, 0, count );

	// Step 2: protect the maxscript memory
	MXS_PROTECT( two_value_locals( mxs_check, mxs_return ) );

	// Step 3: print result into maxscript listener
	//PyObject* main = PyImport_AddModule("__main__");
	//PyObject* globalDictionary = PyDict_New();// PyModule_GetDict(main);
	//PyObject* localDictionary = PyDict_New();
	char* cmd = "import os\n"
				"keys = os.environ.keys()\n"
				"keys.sort()\n"
				"print u'\\n'.join( [k+' :\\n'+(' '*24)+os.environ[k] for k in keys] )";
	//PyRun_String(cmd, Py_file_input, globalDictionary, localDictionary);
	//char* stringEnv = PyString_AsString(PyDict_GetItemString(localDictionary, "result"));
	//char* charEnv = PyString_AsString(stringEnv);
	//mprintf( char2wchar_t( combine( stringEnv, "\n") ) );
	PyRun_SimpleString(cmd);
	return &ok;	
	}

// python.version function: print the version of python which is used
Value* version_cf(Value** anyArgs, int count) {
	// Step 1: make sure the arguments supplied are correct
	check_arg_count( python.version, 0, count );

	// Step 2: protect the maxscript memory
	MXS_PROTECT( two_value_locals( mxs_check, mxs_return ) );

	// Step 3: print result into maxscript listener
	mprintf( constChar2wchar_t( Py_GetVersion() )); 
	mprintf( _T("\n"));
	return &ok;		
	}

// python.home function: print the home dir location of python
Value* home_cf(Value** anyArgs, int count) {
	// Step 1: make sure the arguments supplied are correct
	check_arg_count( python.home, 0, count );

	// Step 2: protect the maxscript memory
	MXS_PROTECT( two_value_locals( mxs_check, mxs_return ) );

	// Step 3: print result into maxscript listener
	mprintf( char2wchar_t( combine( Py_GetPythonHome(), "\n") ) );
	return &ok;	
	}

// python.prefix function: print the python executable which is used
Value* prefix_cf(Value** anyArgs, int count) {
	// Step 1: make sure the arguments supplied are correct
	check_arg_count( python.prefix, 0, count );

	// Step 2: protect the maxscript memory
	MXS_PROTECT( two_value_locals( mxs_check, mxs_return ) );

	// Step 3: print result into maxscript listener
	mprintf( char2wchar_t( combine( Py_GetPrefix(), "\n") ) );
	return &ok;	
	}
// ----------- END: Additional functions..


// python.import function: import a python module to maxscript
Value* import_cf( Value** arg_list, int count ) {
	// Step 1: make sure the arguments supplied are correct
	check_arg_count( python.import, 1, count );

	// Step 2: protect the maxscript memory
	MXS_PROTECT( two_value_locals( mxs_check, mxs_return ) );

	// Step 3: convert the maxscript value
	MXS_EVAL( arg_list[0], vl.mxs_check );

	// Step 4: calculate the python module name to import
	const MCHAR* module_name = NULL;
	try { module_name = vl.mxs_check->to_string(); }
	MXS_CATCHERRORS();

	// Step 5: import the module
	if ( module_name ) {
		MCharToPyString pys(module_name);
		if( pys.pyString() ) {
			PyObject* module = PyImport_Import( pys.pyString() );
			vl.mxs_return = ( module ) ? ObjectWrapper::intern( module ) : &undefined;
		}

		PY_ERROR_PROPAGATE_MXS_CLEANUP();
	}
	else {
		mprintf( _T("python.import() error: importing modules must be done with a string value\n") );
		vl.mxs_return = &undefined;
	}
	MXS_RETURN( vl.mxs_return );
}

// python.reload function: reload an existing module in maxscript
Value* reload_cf( Value** arg_list, int count ) {
	// Step 1: make sure the arguments supplied are correct in count
	check_arg_count( python.reload, 1, count );

	// Step 2: evaluate the input item
	MXS_PROTECT(one_value_local(mxs_check));
	MXS_EVAL( arg_list[0], vl.mxs_check );
	
	// Step 3: make sure the item is a proper type
	if ( is_objectwrapper(vl.mxs_check) ) {
		PyImport_ReloadModule( ((ObjectWrapper*) vl.mxs_check)->object() );
		PY_ERROR_PROPAGATE_MXS_CLEANUP();
	}
	else { mprintf( _T("python.reload() error: you need to supply a valid python module to reload\n") ); }

	MXS_CLEANUP();
	return &ok;	
}

// python.run function: run a python file from maxscript
Value* run_cf( Value** arg_list, int count ) {
	// Step 1: make sure the arguments supplied are correct in count
	check_arg_count( python.run, 1, count );

	// Step 2: protect the maxscript memory
	MXS_PROTECT(one_value_local(mxs_filename));
	MXS_EVAL( arg_list[0], vl.mxs_filename );

	// Step 2: create a python file based on the filename
	const MCHAR * filename	= vl.mxs_filename->to_string();
	//mprintf( _T("Got Filename to run: %s\n"), filename );
	
	MCharToPyString pys(filename);
	PyObject* args = PyTuple_New(2);
	PyTuple_SET_ITEM(args,0,pys.pyStringRef());
	PyTuple_SET_ITEM(args,1,PyString_FromString("r"));
	//mprintf( _T("Arg tuple created, creating python file object\n") );
	
	PyObject* py_file = PyObject_Call((PyObject*)&PyFile_Type, args, NULL);
	Py_DECREF(args);
	if( !py_file ) {
		mprintf( _T("Call to python file object creation failed\n") );
		PY_ERROR_PROPAGATE_MXS_CLEANUP();
		return &false_value;
	}
	
	//mprintf( _T("File opened, calling PyRun_SimpleFile\n") );
	// Step 4: run the file
	PyRun_SimpleFile( PyFile_AsFile(py_file), pys.data() );

	//mprintf( _T("File ran, cleaning up\n") );
	// Step 5: cleanup the memory
	Py_DECREF( py_file );
	PY_ERROR_PROPAGATE_MXS_CLEANUP();
	
	return &true_value;
}

// python.exec function: execute a python command through a maxscript string
Value* exec_cf( Value** arg_list, int count ) {
	// Step 1: make sure the arguments supplied are correct in count
	check_arg_count( python.exec, 1, count );

	// Step 2: protect the maxscript memory
	MXS_PROTECT(one_value_local(mxs_command));
	MXS_EVAL( arg_list[0], vl.mxs_command );

	// Step 2: create a python file based on the filename
	const MCHAR* command	= NULL;
	try { command	= vl.mxs_command->to_string(); }
	MXS_CATCHERRORS();

	// Step 3: check to make sure the command is valid
	if ( !command ) {
		MXS_CLEANUP();
		return &false_value;
	}

	{
		MCharToPyString ascii(command);
		if( ascii.pyString() )
			PyRun_SimpleString( ascii.data() );

		PY_ERROR_PROPAGATE_MXS_CLEANUP();
	}

	// Step 5: cleanup the memory
	MXS_CLEANUP();
	
	return &ok;
}

PyExcRuntimeError::PyExcRuntimeError( MCHAR * _error )
: RuntimeError( _error )
, error( _error )
{}

PyExcRuntimeError::~PyExcRuntimeError()
{
	delete error;
}

// Returns a new string
MCHAR * pythonExceptionTraceback( bool clearException )
{
	/*
		import traceback
		return '\n'.join(traceback.format_exc()).encode('ascii','replace')
	*/
	MCHAR * ret = 0;
	bool success = false;

	PyObject * type, * value, * traceback;
	/* Save the current exception */
	PyErr_Fetch(&type, &value, &traceback);
	if( type ) {
	
		PyObject * traceback_module = PyImport_ImportModule("traceback");
		if (traceback_module) {

			/* Call the traceback module's format_exception function, which returns a list */
			PyObject * traceback_list = PyObject_CallMethod(traceback_module, "format_exception", "OOO", type, value ? value : Py_None, traceback ? traceback : Py_None);
			if( traceback_list ) {

				PyObject * separator = PyString_FromString("");
				if( separator ) {

					PyObject * retUni = PyUnicode_Join(separator, traceback_list);
					if( retUni ) {
#ifdef UNICODE
						ret = _tcsdup( PyUnicode_AsUnicode(retUni) );
#else
						PyObject * retAscii = PyUnicode_AsEncodedString(retUni, "ascii", "replace");
						if( retAscii ) {
							Py_ssize_t len = 0;
							char * tmp;
							if( PyString_AsStringAndSize( retAscii, &tmp, &len ) != -1 ) {
								ret = strdup( tmp );//, len );
								success = true;
							} else {
								ret = _tcsdup( _T("Uhoh, failed to get pointer to ascii representation of the exception") );
								success = false;
							}
							Py_DECREF( retAscii );
						} else {
							ret = _tcsdup( _T("Uhoh, encoding exception to ascii failed") );
							success = false;
						}
#endif
						Py_DECREF(retUni);

					} else
						ret = _tcsdup(_T("PyUnicode_Join failed"));

					Py_DECREF(separator);
				} else
					ret = _tcsdup(_T("PyUnicode_FromString failed"));

				Py_DECREF(traceback_list);
			} else
				ret = _tcsdup(_T("Failure calling traceback.format_exception"));

			Py_DECREF(traceback_module);
		} else
			ret = _tcsdup(_T("Unable to load the traceback module, can't get exception text"));
	} else
		ret = _tcsdup(_T("pythonExceptionTraceback called, but no exception set"));

	if( clearException ) {
		Py_DECREF(type);
		Py_XDECREF(value);
		Py_XDECREF(traceback);
	} else
		PyErr_Restore(type,value,traceback);

	return ret;
}