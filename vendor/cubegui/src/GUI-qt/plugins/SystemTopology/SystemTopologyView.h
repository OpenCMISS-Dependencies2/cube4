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





#ifndef   _SYSTEMTOPOLOGYVIEW_H
#define   _SYSTEMTOPOLOGYVIEW_H

#include <QWidget>
#include "PluginServiceExports.h"

class SystemTopologyView : public QWidget
{
    Q_OBJECT

public:
    virtual void
    updateDrawing() = 0;
    virtual void
    rescaleDrawing() = 0;
    virtual void
    changeZoom( double zoomFactor ) = 0;
    virtual void
    setXAngle( int xangle ) = 0;
    virtual void
    setYAngle( int yangle ) = 0;
    virtual void
    setSize( const QSize& visibleRegion ) = 0;
    virtual void
    move( int dx,
          int dy ) = 0;
    virtual void
    scrolledTo( int x,
                int y ) = 0;

signals:
    // selectItem is emitted, if an element is selected with left mouse click
    void
    selectItem( cubepluginapi::TreeItem* item,
                bool                     add );
    void
    scrollTo( int x,
              int y );
};

#endif
