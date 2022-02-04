/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2021                                                     **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef COMPATIBILITY_H
#define COMPATIBILITY_H

#if QT_VERSION >= QT_VERSION_CHECK( 6, 0, 0 ) // at least 5.0.0
#define HAS_QREGULAR_EXPRESSION
#endif

#if QT_VERSION >= QT_VERSION_CHECK( 6, 0, 0 ) // at least 5.15.0
#define HAS_QREGULAR_WILDCARD_EXPRESSION
#endif

#endif // COMPATIBILITY_H
