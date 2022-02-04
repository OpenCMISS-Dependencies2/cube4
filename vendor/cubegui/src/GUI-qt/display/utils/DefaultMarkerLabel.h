/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2018                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef _DEFAULT_MARKER_LABEL_H
#define _DEFAULT_MARKER_LABEL_H

#include <QHash>
#include <QString>
#include "MarkerLabel.h"

namespace cubegui
{
class TreeItem;

class DefaultMarkerLabel : public MarkerLabel
{
public:
    DefaultMarkerLabel( const QString& label );
    virtual ~DefaultMarkerLabel()
    {
    }

    virtual QString
    getLabel( const TreeItem* = 0 );

    void
    setLabel( TreeItem*      item,
              const QString& label );

private:
    QHash<const TreeItem*, QString> labelHash;
    QString                         label;
};
}
#endif
