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


#include "config.h"

#include "ScrollArea.h"

using namespace cubegui;

ScrollArea::ScrollArea( QWidget* parent ) :
    QScrollArea( parent )
{
    this->setWidgetResizable( true );
    //bars should be always displayed
    setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
    setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
}

void
ScrollArea::resizeEvent( QResizeEvent* event )
{
    emit resized( QSize( viewport()->width(), viewport()->height() ) );
    QScrollArea::resizeEvent( event );
}

/** scroll the contents of the Scrollplane at position (x,y) to be placed
 * at current mousePosition
 * @param x,y position to scroll to, ignored if negative
 */
void
ScrollArea::scrollTo( int x, int y )
{
    QPoint mouse = this->mapFromGlobal( QCursor::pos() );

    // tab has changed => don't use mouse position (mouse is on tab control) but center position
    if ( mouse.y() < 0 )
    {
        mouse.setX( this->size().width() / 2 );
        mouse.setY( this->size().height() / 2 );
    }

    if ( x > -1 )
    {
        QScrollBar* sb = horizontalScrollBar();
        sb->setValue( x - mouse.x() );
    }
    if ( y > -1 )
    {
        QScrollBar* sb = verticalScrollBar();
        // scrolls val to uppermost
        sb->setValue( y - mouse.y() );
    }
}
