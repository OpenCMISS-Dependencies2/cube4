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




#ifndef _PRETTY_H
#define _PRETTY_H


#include <vector>

namespace cubegui
{
void
Pretty( double               pmin,
        double               pmax,
        int                  pno,
        std::vector<double>& vec );
}
#endif
