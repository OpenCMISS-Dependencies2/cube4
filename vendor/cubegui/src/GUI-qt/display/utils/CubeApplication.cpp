/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2021                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include "config.h"
#include "CubeApplication.h"
#include "MainWidget.h"
#include "PresentationCursor.h"

using namespace cubegui;

CubeApplication::CubeApplication( int& argc, char** argv )
    : QApplication( argc, argv )
{
    pcursor = nullptr;
}

void
CubeApplication::setPresentationMode( bool enabled )
{
    if ( enabled  && !pcursor )
    {
        pcursor = new PresentationCursor();
    }
    pcursor->setVisible( false );

    if ( enabled )
    {
        pcursor->move( QCursor::pos() );
        this->installEventFilter( this );
    }
    else
    {
        this->removeEventFilter( this );
    }
}

void
CubeApplication::setMain( MainWidget* m )
{
    mainWidget = m;
}

bool
CubeApplication::event( QEvent* event )
{
    if ( mainWidget && event->type() == QEvent::FileOpen )
    {
        QFileOpenEvent* openEvent = static_cast<QFileOpenEvent*>( event );
        mainWidget->loadFile( openEvent->file() );
    }
    return QApplication::event( event );
}

bool
CubeApplication::eventFilter( QObject* obj, QEvent* event )
{
    if ( !obj->isWidgetType() )
    {
        return false;
    }
    QWidget* widget = qobject_cast<QWidget*> ( obj );

    if ( event->type() == QEvent::MouseMove )
    {
        QMouseEvent* ev = static_cast<QMouseEvent*>( event );
        QPoint       p  =  ev->globalPos();
        p += QPoint( -4, 4 ); // PresentationCursor widget mustn't be placed over cursor, WA_TransparentForMouseEvents doesn't seem to work

        pcursor->move( p );
        pcursor->raise();
    }
    else if ( event->type() == QEvent::MouseButtonPress )
    {
        QMouseEvent* ev = static_cast<QMouseEvent*>( event );
        if ( ev->button() == Qt::LeftButton )
        {
            pcursor->setCursor( CursorType::LEFT );
        }
        else if ( ev->button() == Qt::RightButton )
        {
            pcursor->setCursor( CursorType::RIGHT );
        }
        // MouseButtonRelease is not emitted if context menu is active and mouse is clicked enywhere else. For that reason
        // set a delay after which the default mouse pointer is shown again.
        pcursor->setCursor( CursorType::DEFAULT, 1000 );
    }
    // else if ( event->type() == QEvent::MouseButtonRelease )
    else if ( event->type() == QEvent::Wheel )
    {
        pcursor->setCursor( CursorType::WHEEL );
        pcursor->setCursor( CursorType::DEFAULT, 1000 );
    }
    else if ( widget && widget->isWindow() && event->type() == QEvent::Enter )
    {
        if ( !pcursor->isVisible() )
        {
            pcursor->setVisible( true );
        }
        pcursor->raise();
    }
    else if ( widget && widget->isWindow() && event->type() == QEvent::Leave )
    {
        QWidget* nextWindow = qApp->widgetAt( QCursor::pos() );
        if ( !nextWindow ) // if cursor is not above a window from this app then hide the presentation cursor
        {
            pcursor->setVisible( false );
        }
    }
    else if ( event->type() == QEvent::Show ) // e.g. a popup menu has been shown -> raise cursor
    {
        pcursor->raise();
    }

    return false; // do not filter
}
