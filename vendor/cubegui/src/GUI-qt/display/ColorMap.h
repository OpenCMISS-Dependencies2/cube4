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


#ifndef ColorMap_H
#define ColorMap_H
#include <QObject>
#include <QColor>

namespace cubegui
{
class ColorMap : public QObject
{
    Q_OBJECT

public:
    QColor
    virtual
    getColor( double value,
              double minValue,
              double maxValue,
              bool   whiteForZero ) const = 0;

    virtual QString
    getMapName() const = 0;

    virtual bool
    dialogAvailable() const
    {
        return false;
    }

    virtual void
    showDialog()
    {
    }

signals:
    void
    colorMapChanged();
};
}
#endif
