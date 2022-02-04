/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2016                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/



/** @internal
 *
 *  @file
 *
 *  @brief      Helper class for CUBEW tools.
 */

#ifndef CUBEGUI_TOOLS_H
#define CUBEGUI_TOOLS_H

#ifndef CUBEGUI_TOOL_NAME
#error No CUBEGUI_TOOL_NAME given
#endif

/* ___ Output handling. _____________________________________________________ */

#define out( msg ) \
    do { \
        std::cout << CUBEGUI_TOOL_NAME << ": " << msg << std::endl; \
    } while ( 0 )

#define err( msg ) \
    do { \
        std::cerr << CUBEGUI_TOOL_NAME << ": " << msg << std::endl; \
    } while ( 0 )

#define warn( msg ) \
    do { \
        std::cerr << CUBEGUI_TOOL_NAME << ": warning: " << msg << std::endl; \
    } while ( 0 )

#define die( msg ) \
    do { \
        err( msg ); \
        std::cerr << "Try '" << CUBEGUI_TOOL_NAME << " --help' for more information." << std::endl; \
        exit( EXIT_FAILURE ); \
    } while ( 0 )

#endif // CUBEGUI_TOOLS_HPP
