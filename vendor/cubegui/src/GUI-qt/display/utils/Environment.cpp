/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2019                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include "config.h"

#include <cctype>
#include <cstring>
#include <cstdlib>

#include "Environment.h"

using namespace std;


/**
 * Converts a string read from an environment variable to a boolean value.
 * Any positive integer as well as the character strings "yes" and "true"
 * (case-insensitive) are are recognized as @c true, any other value as
 * @c false.
 */
bool
cubegui::env_str2bool( const char* str )
{
    char  strbuf[ 128 ];
    char* ptr = strbuf;

    if ( !str )
    {
        return false;
    }

    memset( strbuf, 0, sizeof( strbuf ) );
    strncpy( strbuf, str, sizeof( strbuf ) - 1 );
    while ( *ptr )
    {
        *ptr = tolower( *ptr );
        ++ptr;
    }

    if ( strcmp( strbuf, "yes" ) == 0  ||
         strcmp( strbuf, "true" ) == 0 ||
         atoi( strbuf ) > 0 )
    {
        return true;
    }

    return false;
}
