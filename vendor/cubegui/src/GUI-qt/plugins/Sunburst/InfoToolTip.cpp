/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2016-2017                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2015-2016                                                **
**  RWTH Aachen University, JARA-HPC                                       **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


/*-------------------------------------------------------------------------*/
/**
 *  @file    InfoToolTip.cpp
 *  @ingroup CubeSunburst
 *  @brief   Definition of the class InfoToolTip.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>
#include "InfoToolTip.h"
#include <QHBoxLayout>

using namespace cube_sunburst;

InfoToolTip::InfoToolTip()
{
    setWindowFlags( Qt::ToolTip );
    left_label  = new QLabel();
    right_label = new QLabel();
    QHBoxLayout* layout = new QHBoxLayout();
    setLayout( layout );
    layout->addWidget( left_label );
    layout->addWidget( right_label );

    // modifications:
    // enable MouseMoveEvents on the infoToolTip
    setMouseTracking( true );
    left_label->setMouseTracking( true );
    right_label->setMouseTracking( true );

    // handle the child events like they are events of this widget
    left_label->installEventFilter( this );
    right_label->installEventFilter( this );
    installEventFilter( this );

    // initialization
    moveCursorCache = QPoint( -1, -1 );
}


void
InfoToolTip::showInfo( const QPoint&      pos,
                       const QStringList& tooltipText )
{
    move( pos );
    left  = tooltipText.at( 0 );
    right = tooltipText.at( 1 );
    left_label->setText( left );
    right_label->setText( right );
    show();

    // modifications:
    // cache the cursor position
    moveCursorCache = cursor().pos();
}


bool
InfoToolTip::eventFilter( QObject*, QEvent* event )
{
    if (    ( event->type() == QEvent::MouseButtonPress ) ||
            ( event->type() == QEvent::MouseButtonRelease ) ||
            ( event->type() == QEvent::MouseMove ) )
    {
        // avoid that the initial MouseEvent hides the tooltip
        if ( moveCursorCache == cursor().pos() )
        {
            return false;
        }
        hide();
        return true;
    }
    return false;
}
