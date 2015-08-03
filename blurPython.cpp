/*
	\file		blurPython.cpp

	\remarks	Entry point for the blurPython dlx

	\author		Blur Studio (c) 2010
	\email		beta@blur.com

	\edit		Michael Loeffler @ Burrows (c) 2015

	\license	This software is released under the GNU General Public License.  For more info, visit: http://www.gnu.org/
*/

#include "imports.h"
#pragma comment( lib, "comctl32.lib" )


//------------------------------------------------------------------------------------------------------
HMODULE hInstance	= NULL;
HINSTANCE g_hInst   = NULL;

BOOL WINAPI	DLLMain( HINSTANCE hModule, DWORD ul_reason, LPVOID lpReserved ) {
	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof( icex );
	icex.dwICC	= ICC_TREEVIEW_CLASSES | ICC_LISTVIEW_CLASSES;

	switch ( ul_reason ) {
		case DLL_PROCESS_ATTACH: {
			MaxSDK::Util::UseLanguagePackLocale();
			if	(g_hInst) return(FALSE);
			hInstance	= hModule;
			g_hInst		= hModule;

			// TODO: Do we really have to disable thread library calls?
			DisableThreadLibraryCalls( hModule );
			break;
		    }
		case DLL_PROCESS_DETACH: {
			// Kill the python system
			Py_Finalize();
			hModule	 =	NULL;
			g_hInst  =  NULL;
		    break;
		    }
	    }
	return TRUE;
    }

// the init_module function is ofund in the studiomax_module file
PyMODINIT_FUNC init_module();


// setup_py function is defined inside setup_python.cpp file.
// this function has to be called before Py_Initialize() to use a different python interpreter
int setup_py();
//int setup_virtualenv();

// Return version so can detect obsolete DLLs
__declspec( dllexport ) ULONG          LibVersion()           { return VERSION_3DSMAX; }
// Return description 
__declspec( dllexport ) const TCHAR*   LibDescription()       { return _T( "Py3dsMax Python Extension" ); }

// Original initialisation by blur 
__declspec( dllexport ) void           LibInit()              { setup_py(); init_module(); }

// Initialisation 3dsMax is expecting (works for manual load of the plugin
//       -> MaxScript command:  LoadDllsFromDir DIR_PATH "*.dlx"
//__declspec( dllexport ) void           LibInitialize()        { setup_py();	init_module(); }

                                                              
// Maxscript 2012 requires these additional exports                                                             
#if __MAXSCRIPT_2012__ || __MAXSCRIPT_2013__ || __MAXSCRIPT_2015__
	// we aren't defining any classes in this plugin, so this is pretty easy
	// other plugins should refer to samples/scriptplugin for an example of the new
	// registration system - that is the only plugin that does not error out of the samples
	// in max                                                        
	__declspec( dllexport ) int			   LibNumberClasses()     { return 0; }
	__declspec( dllexport ) ClassDesc*	   LibClassDesc( int i )  { return 0; }
#endif