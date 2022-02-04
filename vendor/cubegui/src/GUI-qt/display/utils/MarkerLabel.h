/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2017                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/



#ifndef _MARKER_LABEL_H
#define _MARKER_LABEL_H

namespace cubegui
{
class TreeItem;

class MarkerLabel
{
public:
    virtual QString
    getLabel( const TreeItem* = 0 ) = 0;
};
}
#endif
