/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2021                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/



#ifndef CUBEGUI_VERSION_H
#define CUBEGUI_VERSION_H


/*-------------------------------------------------------------------------*/
/**
 *  @file  cubegui-version.h
 *  @brief Provides the Cube package version information.
 **/
/*-------------------------------------------------------------------------*/


/// @name Cube version information
/// @{

/// Cube revision number
#define CUBEGUI_REVISION "712b7830"

/// Cube revision number
#define CUBEGUI_REVISION_NUMBER 712b7830

/// Cube revision number
#define CUBEGUI_REVISION_STRING CUBEGUI_REVISION



/// Cube major version number
#define CUBEGUI_MAJOR_NUM   4

/// Cube minor version number
#define CUBEGUI_MINOR_NUM   6

/// Cube bugfix version number
#define CUBEGUI_BUGFIX_NUM  0

/// Cube major version number
#define CUBEGUI_MAJOR   "4"

/// Cube minor version number
#define CUBEGUI_MINOR   "6"

/// Cube bugfix version number
#define CUBEGUI_BUGFIX   "0"

/// Cube suffix
#define CUBEGUI_SUFFIX   ""

/// Cube suffix_internally
#define CUBEGUI_SUFFIX_INTERNAL   

/// Cube version number (<i>major</i>.<i>minor</i>)
#define CUBEGUI_SHORT_VERSION   CUBEGUI_MAJOR "." CUBEGUI_MINOR

/// Cube full version number (<i>major</i>.<i>minor</i>.<i>bugfix</i>)
#define CUBEGUI_FULL_VERSION   CUBEGUI_SHORT_VERSION "." CUBEGUI_BUGFIX

/// Cube version number
#define CUBEGUI_VERSION "4.6"

/// Cube package name "cube"
#define CUBEGUI_SHORT_NAME  "CubeGUI"

/// Cube package name "cube-4.x.1-XX"
#define CUBEGUI_FULL_NAME   CUBEGUI_SHORT_NAME "-" CUBEGUI_FULL_VERSION CUBEGUI_SUFFIX

/// Cube package name "cube-4.x.1-XX"
#define CUBEGUI_FULL_NAME   CUBEGUI_SHORT_NAME "-" CUBEGUI_FULL_VERSION CUBEGUI_SUFFIX

/// usage: #if (CUBEGUI_VERSION_NUMBER >= CUBEGUI_VERSION_CHECK(4, 5, 0))'
/// Macro to build a single number out of major.minor.bugfix
#define CUBEGUI_VERSION_CHECK( major, minor, bugfix ) ( ( major << 16 ) | ( minor << 8 ) | ( bugfix ) )

/// usage: #if (CUBEGUI_VERSION_NUMBER >= CUBEGUI_VERSION_CHECK(4, 5, 0))'
/// Cube package version as a single number
#define CUBEGUI_VERSION_NUMBER CUBEGUI_VERSION_CHECK( CUBEGUI_MAJOR_NUM, CUBEGUI_MINOR_NUM, CUBEGUI_BUGFIX_NUM )

/// Cube library interface information
#define LIBRARY_CURRENT "0"
#define LIBRARY_REVISION "0"
#define LIBRARY_AGE "0"

/// @}


#endif   // !CUBEGUI_VERSION_H
