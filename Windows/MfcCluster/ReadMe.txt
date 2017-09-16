
To run this project :
1. Install Visual Studio 2015 Community
2. Open solution "Cluster.sln"
3. Rebuild and Run

WARNING: this is only a demo application. Use at your own risks.


File description :
- MfcCluster.cpp			
The main demo application

- MfcClusterDlg.cpp		
The main dialog

- ClusterDB.cpp			
The database manager, keeping all sequences in memory, also managing memory caching.
The size of the case is defined in function ClusterDB::AllocateCache()
If you don't want result caching, simply comment this line: 
#define CACHING
in file ClusterDB.h

- Cluster.cpp
Contains all clustering code for four algorithms: UClust, CCBC, Single-level clustering (SLC) and Multi-level clustering (MLC)
To simplify debugging and comprehension of the algorithm, they are also proposed as single-threaded functions.

DLL Dependencies :
- This project depend in BioScience.x64.dll or BioScience.x86.dll for DNA comparisons.
This was mandatory because the DNA comparison code is not of public domain.
To use your own DNA comparison algorithm, simply replace function 	ComputeSimilarity(const TNFieldBase * p_Srce, const TNFieldBase * p_Ref);
by your own code.
There is a single call to this function from ClusterDB::Compare(const TNFieldBase * srce, const TNFieldBase * ref)
Your class containing the sequences and the sequence comparison code must derive from class TNFieldBase.
The returned value is the similarity value, from 0.0 to 1.0.



================================================================================
    MICROSOFT FOUNDATION CLASS LIBRARY : MfcCluster Project Overview
===============================================================================

The application wizard has created this MfcCluster application for
you.  This application not only demonstrates the basics of using the Microsoft
Foundation Classes but is also a starting point for writing your application.

This file contains a summary of what you will find in each of the files that
make up your MfcCluster application.

MfcCluster.vcxproj
    This is the main project file for VC++ projects generated using an application wizard.
    It contains information about the version of Visual C++ that generated the file, and
    information about the platforms, configurations, and project features selected with the
    application wizard.

MfcCluster.vcxproj.filters
    This is the filters file for VC++ projects generated using an Application Wizard. 
    It contains information about the assciation between the files in your project 
    and the filters. This association is used in the IDE to show grouping of files with
    similar extensions under a specific node (for e.g. ".cpp" files are associated with the
    "Source Files" filter).

MfcCluster.h
    This is the main header file for the application.  It includes other
    project specific headers (including Resource.h) and declares the
    MfcCluster application class.

MfcCluster.cpp
    This is the main application source file that contains the application
    class MfcCluster.

MfcCluster.rc
    This is a listing of all of the Microsoft Windows resources that the
    program uses.  It includes the icons, bitmaps, and cursors that are stored
    in the RES subdirectory.  This file can be directly edited in Microsoft
    Visual C++. Your project resources are in 1033.

res\MfcCluster.ico
    This is an icon file, which is used as the application's icon.  This
    icon is included by the main resource file MfcCluster.rc.

res\MfcCluster.rc2
    This file contains resources that are not edited by Microsoft
    Visual C++. You should place all resources not editable by
    the resource editor in this file.


/////////////////////////////////////////////////////////////////////////////

The application wizard creates one dialog class:

MfcClusterDlg.h, MfcClusterDlg.cpp - the dialog
    These files contain your MfcClusterDlg class.  This class defines
    the behavior of your application's main dialog.  The dialog's template is
    in MfcCluster.rc, which can be edited in Microsoft Visual C++.

/////////////////////////////////////////////////////////////////////////////

Help Support:

hlp\MfcCluster.hhp
    This file is a help project file. It contains the data needed to
    compile the help files into a .chm file.

hlp\MfcCluster.hhc
    This file lists the contents of the help project.

hlp\MfcCluster.hhk
    This file contains an index of the help topics.

hlp\afxcore.htm
    This file contains the standard help topics for standard MFC
    commands and screen objects. Add your own help topics to this file.

makehtmlhelp.bat
    This file is used by the build system to compile the help files.

hlp\Images\*.gif
    These are bitmap files required by the standard help file topics for
    Microsoft Foundation Class Library standard commands.


/////////////////////////////////////////////////////////////////////////////

Other Features:

ActiveX Controls
    The application includes support to use ActiveX controls.

/////////////////////////////////////////////////////////////////////////////

Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named MfcCluster.pch and a precompiled types file named StdAfx.obj.

Resource.h
    This is the standard header file, which defines new resource IDs.
    Microsoft Visual C++ reads and updates this file.

MfcCluster.manifest
	Application manifest files are used by Windows XP to describe an applications
	dependency on specific versions of Side-by-Side assemblies. The loader uses this
	information to load the appropriate assembly from the assembly cache or private
	from the application. The Application manifest  maybe included for redistribution
	as an external .manifest file that is installed in the same folder as the application
	executable or it may be included in the executable in the form of a resource.
/////////////////////////////////////////////////////////////////////////////

Other notes:

The application wizard uses "TODO:" to indicate parts of the source code you
should add to or customize.

If your application uses MFC in a shared DLL, you will need
to redistribute the MFC DLLs. If your application is in a language
other than the operating system's locale, you will also have to
redistribute the corresponding localized resources MFC100XXX.DLL.
For more information on both of these topics, please see the section on
redistributing Visual C++ applications in MSDN documentation.

/////////////////////////////////////////////////////////////////////////////
