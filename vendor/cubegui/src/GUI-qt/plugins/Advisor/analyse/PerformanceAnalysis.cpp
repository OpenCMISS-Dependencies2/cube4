/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2015-2019                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include <config.h>
#include "PerformanceAnalysis.h"


using namespace cube;
using namespace cubepluginapi;
using namespace advisor;

void
PerformanceAnalysis::fillAdviceHeader()
{
    header << tr( "Callpath" ) << tr( "Issue" );
}
