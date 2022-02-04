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




#ifndef _SCROLLAREA_H
#define _SCROLLAREA_H

#include <QScrollArea>
#include <QScrollBar>

namespace cubegui
{
class ScrollArea : public QScrollArea
{
    Q_OBJECT
public:
    explicit
    ScrollArea( QWidget* parent = 0 );

signals:
    void
    resized( const QSize& );

public slots:
    void
    scrollTo( int x,
              int y );

protected:
    void
    resizeEvent( QResizeEvent* event );
};
}
#endif
