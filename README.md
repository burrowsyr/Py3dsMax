Py3dsMax allows two way access between the maxscript and python interpreters. In maxscript you can import python modules and classes create objects set values etc. In python you can access maxscript commands get and set values. 

Some non-pythonic functionality does not translate to python like the by-reference character "&". To work around these limitations you can generally define a helper maxscript struct like the pyhelper class.

# Compile Settings

There are several environment variables you will need to set to be able to compile. Most of them contain the year of the max version you are trying to comile for. For example to compile for Max 2015 you need to create a environment variable "MAX2015SDK" pointing to the maxsdk folder of the sdk(C:\Program Files\Autodesk\3ds Max 2015 SDK\maxsdk).


Required Environment variables:
-------------------------------

1. MAX[year]SDK: The Max SDK's location. should point to the maxsdk folder. year is the year number of the max build you are trying to build.

2. PYTHON[ver][_64]: The folder containing python.exe. ver is the python version without a decimal. _64 should be specified for 64bit builds, and should be omitted for 32bit builds.

3. MAX[Year]OUT: If defined as a post-build process the .dlx file will be copied to this folder. This variable is optional, and is only supported in Max 2015 and newer.

Plugin Load Required Changes:
-----------------------------

1. If you choose to load the plugin with a `plugin.ini` file, like:

    Example `plugin.ini`:
     ```    
     [Directories]
     ;=;
     blur = C:\max2015\plugins\blur
     ```
 
     Please use `LibInit()` for the `dllexport` and comment the `LibInitialize()` out or remove it:
     
     ```
     __declspec( dllexport ) void           LibInit()              { setup_py(); init_module(); }
     //__declspec( dllexport ) void           LibInitialize()        { setup_py();	init_module(); }
    
     ```

2. To use a startup script with the `LoadDllsFromDir DIR_PATH "*.dlx"` command to load the plugin, please
   make sure that the plugin will not be loaded with any `*ini` file. You should use `LibInitialize()`
   instead of `LibInit()` for the `dllexport`:
 
     ```
      //__declspec( dllexport ) void           LibInit()              { setup_py(); init_module(); }
      __declspec( dllexport ) void           LibInitialize()        { setup_py();	init_module(); }
     ```

Here is a breakdown of the Visual Studio requirements required to build this package. 
If a binary compatible version is listed you can use the same plugin compiled for that version.

| 3ds Max Version | Visual Studio        | Binary Compatible | 32bit Python  | 64bit Python  | Config/Platform                                               |
|-----------------|----------------------|-------------------|---------------|---------------|---------------------------------------------------------------|
| 2012            | 2008 SP1 / 2010 SP1  |                   | PYTHON24      | PYTHON26_64 /PYTHON27_64 | Max2012_Python24 / Win32 Max2012x64_Python26 / x64 |
| 2013            | 2010 SP1             |                   | PYTHON27      | PYTHON27_64              | Max2013x32_Python27 / Win32 Max2013x64_Python27 / x64 |
| 2014            | 2012 Update 4        |                   | Not Supported | PYTHON27_64              | Max2014_Python27 / x64 |
| 2015            | 2012 Update 4        |                   | Not Supported | PYTHON27_64              | Max2015x64_Python27 / x64 |
| 2016            | 2012 Update 4        | 2015              | Not Supported | PYTHON27_64              | Max2015x64_Python27 / x64 |



Compiled output goes in [project]/bin/[platform]/Max[year]_Python[ver]/ or for older versions it may go into [project]/bin/[platform]/[configuration]/

# Using a different Python Interpreter

To use a separate python interpreter it is necessary to setup an additional system environment variable.
This variable has to be named `3DSMAX_PYTHON` and pointed to the python directory you want to use.
The Python directory could be a local installed version of python or a python copy somewhere in your network.

```
+--------------------------------+
| 3DS Max Application            |
+--------------------------------+
| Plug-ins:  +---------------+   |
|            |  blurPythonXX |---------------------------+
|            +---------------+   |                       |
|                  ||            |                       |
|      connected: ----!NO!       <----------------+      |
|                  ||            |                |      |
|                  ▽            |                △      ▽
|    +--------------------- +    |        +--------------------- +     
|    | Internal Python      |    |        | External Python      |     
|    +----------------------+    |        +----------------------+     
|    | [X] MaxPlus          |    |        | [X] MaxPlus          |     
|    | [ ] Py3dsMax         |    |        | [X] Py3dsMax         |     
|    | [X] PySide           |    |        | [X] PySide           |
|    | [ ] PyQtX            |  connected: | [X] PyQtX            |
|    | [ ] Threading        |<====|X|====>| [X] Threading        |  
|    | [ ] Remembers        |    !NO!     | [X] Remembers        |    
|    |     environment      |    |        |     environment      |    
|    | [X] Could run        |    |        | [X] Could run        |
|    |     MaxScript        |    |        |     MaxScript        |
|    |                      |    |        |                      |
|    | MaxScript command:   |    |        | MaxScript command:   |     
|    | `python.execute`     |    |        | `pymax.exec`         |     
|    | `python.executeFile` |    |        | `pymax.run`          |     
|    | ...                  |    |        | ...                  |     
|    +----------------------+    |        +----------------------+     
|                                |
+--------------------------------+
```
