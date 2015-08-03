/*
	\file		setup_python.cpp

	\remarks	Choose Python Interpreter with environment variable

	\author		Michael Loeffler @ Burrows (c) 2015
	\email		Michael.Loeffler@burrows.yr.com

	\license	This software is released under the GNU General Public License.  For more info, visit: http://www.gnu.org/
*/

#include "imports.h"
#include "stringUtils.h"


// Get Python Interpreter from environment variable
// If "3DSMAX_PYTHON" is not set, the plugin will 
// work with the default python interpreter

char* PY_HOME        = getenv("BURROWS_PYTHON");
char* PY_VIRTUAL_ENV = getenv("PYTHON_VENV");
char* MAX_SLAVE      = getenv("3DSMAX_SLAVE");

int setup_py() {
	//mprintf ( _T("Setup Python: \n"));
	//mprintf ( char2wchar_t(PY_HOME) );
	if (PY_HOME!=NULL){
		//mprintf ( _T("Path given: "));
		// Set Python executable path and Python HOME path to be used
		char* PY_EXECUTABLE = combine(PY_HOME,"\\python.exe");
		//mprintf ( char2wchar_t(PY_EXECUTABLE) );
		Py_SetProgramName(PY_EXECUTABLE);
		Py_SetPythonHome(PY_HOME);
		}
	//else {mprintf ( _T("\nPath NOT exists \n"));}

	// Initialize python
	Py_Initialize();
	return 0;
	}

int setup_virtualenv(){
	// Setup the given virtualenv for python, if given
	if (PY_VIRTUAL_ENV!=NULL){
		//char* cmd = "import os\nenv_path=u'";
		//cmd = combine(cmd, PY_VIRTUAL_ENV);
		//cmd = combine(cmd, "'\nif os.path.exists(env_path):\n"
		//			"    execfile(u'%s/activate_this.py'%(env_path),"
		//			"               dict(__file__=u'%s/activate_this.py'%(env_path)))");
		//PyRun_SimpleString(cmd);
		//PyRun_SimpleFile
		char* activateFile = combine(PY_VIRTUAL_ENV,"\\activate_this.py");
		PyObject* PyFileObject = PyFile_FromString(activateFile, "r");
		PyRun_SimpleFileEx(PyFile_AsFile(PyFileObject), activateFile, 1);
		}
	return 0;
	}