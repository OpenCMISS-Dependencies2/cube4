/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2020                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/



#include "config.h"
#include "PresentationCursor.h"

cubegui::PresentationCursor::PresentationCursor()
{
    enum Qt::WindowType flag;
#if QT_VERSION >= QT_VERSION_CHECK( 5, 0, 0 )
    flag = Qt::BypassWindowManagerHint;
#else
    flag = Qt::X11BypassWindowManagerHint;
#endif
    setWindowFlags( Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | flag );
    setAttribute( Qt::WA_TranslucentBackground );
    setAttribute( Qt::WA_TransparentForMouseEvents );

    setLayout( new QVBoxLayout() );
    label = new QLabel();
    this->layout()->addWidget( label );

    QSize cursorSize( 80, 80 );
    pixmaps[ CursorType::DEFAULT ] = QPixmap( ":images/mouse.png" ).scaled( cursorSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
    pixmaps[ CursorType::LEFT ]    = QPixmap( ":images/mouse-left.png" ).scaled( cursorSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
    pixmaps[ CursorType::RIGHT ]   = QPixmap( ":images/mouse-right.png" ).scaled( cursorSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
    pixmaps[ CursorType::WHEEL ]   = QPixmap( ":images/mouse-wheel.png" ).scaled( cursorSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );

    label->setPixmap( pixmaps[ CursorType::DEFAULT ] );
}

void
cubegui::PresentationCursor::setCursor( cubegui::CursorType type, int delay )
{
    this->type = type;
    if ( delay == 0 )
    {
        updateCursor();
    }
    else
    {
        QTimer::singleShot( delay, this, SLOT( updateCursor() ) );
    }
}

void
cubegui::PresentationCursor::updateCursor()
{
    label->setPixmap( pixmaps[ type ] );
}
