/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2021                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include "config.h"

#include <QPainter>
#include <QDebug>
#include "Globals.h"
#include "ProgressBar.h"
#include "TabWidget.h"
#include "TreeView.h"

using namespace cubegui;

const static int barHeight               = 6;
QTimer*          ProgressBar::blinkTimer = new QTimer();

ProgressBar::ProgressBar( QStackedLayout* layout )
{
    this->layout = layout;
#ifdef CUBE_CONCURRENT_LIB
    futureWatcher = nullptr;
#endif
    drawBlink = false;
    if ( !blinkTimer->isActive() )
    {
        blinkTimer->start( 1000 );
    }
    connect( blinkTimer, SIGNAL( timeout() ), this, SLOT( blink() ) );
    noProgressCounter = 0;
    lastProgress      = 0;
    progress          = 0;
}

#ifdef CUBE_CONCURRENT_LIB
void
ProgressBar::setFutureWatcher( const QFutureWatcher<void>* future )
{
    if ( futureWatcher )
    {
        disconnect( futureWatcher, SIGNAL( progressValueChanged( int ) ), this, SLOT( setProgress( int ) ) );
    }
    this->progress      = -1;
    this->futureWatcher = future;
    connect( futureWatcher, SIGNAL( progressValueChanged( int ) ), this, SLOT( setProgress( int ) ) );
    connect( futureWatcher, SIGNAL( finished() ), this, SLOT( closeProgressBar() ) );
    minProgressValue = 0;
    maxProgressValue = 0;
}
#endif

/** for chained futures: set user defined range to show total progress, not one for each future
   @param max total number of tasks
   @param min already finished tasks from previous future
 */
void
ProgressBar::setProgressRange( int min, int max )
{
    minProgressValue = min;
    maxProgressValue = max;
}

void
ProgressBar::setProgress( int p )
{
    if ( !layout )
    {
        return;
    }

#ifdef CUBE_CONCURRENT_LIB
    int range = ( futureWatcher->progressMaximum() - futureWatcher->progressMinimum() );
#else
    int range = 100;
#endif
    if ( range <= 1 )
    {
        // special case: only one big task, so progress is 0% or 100%.
        // show small percentage on progress bar to indicate that calculation is running
        progress          = 50;
        noProgressCounter = 100; // show blinking progress bar
    }
    else
    {
        if ( maxProgressValue > 0 ) // special case -> setProgressRange()
        {
            p    += minProgressValue;
            range = maxProgressValue;
        }
        int percent = p * 100 / range;
        if ( percent == progress )
        {
            return; // no change to display
        }
        progress = percent;
    }
    if ( !this->isVisible() ) // show progress bar
    {
        this->setMinimumHeight( barHeight );
        layout->addWidget( this );
        layout->setCurrentWidget( this );
    }
    update(); // paint progress bar
}

void
ProgressBar::closeProgressBar()
{
#ifdef CUBE_CONCURRENT_LIB
    if ( futureWatcher )
    {
        this->disconnect( futureWatcher ); // disconnect this progress bar from future watcher
    }
#endif
    if ( !layout )
    {
        return;
    }
    layout->removeWidget( this );
}

void
ProgressBar::blink()
{
    int startBlinking = 3; // start blinking, if progress hasn't changed for the given number
    noProgressCounter = ( progress == lastProgress ) ? noProgressCounter + 1 : 0;
    lastProgress      = progress;
    if ( isVisible() && noProgressCounter > startBlinking )
    {
        drawBlink = !drawBlink;
        repaint();
    }
    else
    {
        drawBlink = false;
    }
}

void
ProgressBar::paintEvent( QPaintEvent* )
{
    // start painting
    QPainter painter;
    painter.begin( this );

    // draw a rectangle
    painter.fillRect( 0, 0, width() - 1, height() - 1, Qt::white );
    painter.drawRect( 0, 0, width() - 1, height() - 1 );

    int xmax = width() - 2;
    painter.drawLine( 0, height() - barHeight, width() - 1, height() - 6 );
    if ( !drawBlink )
    {
        painter.fillRect( 2, height() - barHeight + 1, xmax * progress / 100, 3, Qt::darkGreen );
    }
}
