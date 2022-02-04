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


#include "config.h"
#include "DefaultMarkerLabel.h"
#include "TreeItem.h"

using namespace cubegui;

DefaultMarkerLabel::DefaultMarkerLabel( const QString& label )
{
    this->label = label;
}

QString
DefaultMarkerLabel::getLabel( const TreeItem* item )
{
    QString itemLabel = labelHash.value( item );
    return QString( itemLabel.isNull() ? label : itemLabel );
}

void
DefaultMarkerLabel::setLabel( TreeItem* item, const QString& label )
{
    labelHash.insert( item, label );
}
