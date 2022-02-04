/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2020                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/



#include <QGridLayout>
#include <QtPlugin>
#include "ParallelPlugin.h"
#include "PluginServices.h"
#include "Future.h"

/**
 * If the plugin doesn't load, start cube with -verbose to get detailed information.
 * If the error message "Plugin verification data mismatch" is printed, check if the
 * same compiler and Qt version have been used.
 */

using namespace cubepluginapi;
using namespace simpleexampleplugin;

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2( ParallelExamplePlugin, ParallelPlugin ); // ( PluginName, ClassName )
#endif

ParallelPlugin::ParallelPlugin()
{
    // The constructor should be empty, use cubeOpened to initialize. If Qt widgets or
    // signals/slots are used in constructor, they have to be deleted in destructor,
    // otherwise cube may crash if the plugin is unloaded.
}

// ====================================================================================================================

/* set a version number, the plugin with the highest version number will be loaded */
void
ParallelPlugin::version( int& major, int& minor, int& bugfix ) const
{
    major  = 1;
    minor  = 0;
    bugfix = 0;
}

/* unique plugin name */
QString
ParallelPlugin::name() const
{
    return "Parallel Example";
}

QString
ParallelPlugin::getHelpText() const
{
    return "Just a simple example.";
}

/* widget that will be placed into the tab */
QWidget*
ParallelPlugin::widget()
{
    return widget_;
}

/* tab label */
QString
ParallelPlugin::label() const
{
    return "Parallel Example";
}

// ====================================================================================================================

bool
ParallelPlugin::cubeOpened( PluginServices* service )
{
    this->service = service;

    widget_ = new QWidget();
    text    = new QTextEdit();
    text->setTextInteractionFlags( Qt::NoTextInteraction );
    QFont font = text->font();
    font.setFamily( "Courier" );
    font.setStyleHint( QFont::Monospace );
    font.setFixedPitch( true );
    text->setFont( font );

    QVBoxLayout* layout = new QVBoxLayout();
    widget_->setLayout( layout );
    layout->addWidget( text );

    // service->addTab( SYSTEM, this ); // system tab with value widget below
    service->addTab( SYSTEM, this, OTHER_PLUGIN_TAB ); // non-system tab without value widget

    future = service->createFuture( this );
    connect( future, SIGNAL( calculationFinished() ), this, SLOT( calculationFinished() ) );
    connect( &updateTimer, SIGNAL( timeout() ), this, SLOT( updateView() ) );

    return true; // initialisation is ok => plugin should be shown
}

void
ParallelPlugin::cubeClosed()
{
    updateTimer.disconnect();
    service->disconnect( future, SIGNAL( calculationFinished() ) );

    delete widget_;
}

/* shows the values, which have already been "calculated" */
void
ParallelPlugin::updateView()
{
    std::lock_guard<std::mutex> lockGuard( updateMutex );
    QString                     txt = "";
    for ( int i = 0; i < N; i++ )
    {
        for ( int j = 0; j < N; j++ )
        {
            int     val   = data[ i ][ j ];
            QString label = val > 0 ? QString( "%1" ).arg( val, 4 ) : "   ?";
            txt.append( label );
        }
        txt.append( "\n" );
    }
    text->setText( txt );
}

/* implemented from TabInterface: called if tree values have been changed and if tab is visible
 * valuesChanged is called if e.g. a tree item is selected an after the calculation of tree items has been finished
 */
void
ParallelPlugin::valuesChanged()
{
    future->cancelCalculation(); // ensure that previous tasks have been finished before values are initialized

    for ( int i = 0; i < N; i++ )
    {
        for ( int j = 0; j < N; j++ )
        {
            data[ i ][ j ] = 0;
            future->addCalculation( new ParallelCalculation( i + 1, j + 1, &data[ i ][ j ] ) ); // calculation data
        }
    }

    updateView();
    widget()->repaint();              // force immediate repaint of the view to show the initial values

    updateTimer.start( 500 );         // update view every 500 ms to display already calculated values
    future->startCalculation( true ); // start parallel calculation and abort tasks if another tree item is selected

    /*
     * start parallel calculation but
     * - don't show progress bar
     * - don't stop if new tree selection is done
     */
    // service->startCalculation( this, false, false );
}

void
ParallelPlugin::calculationFinished()
{
    updateTimer.stop();
    updateView();
    QString txt = text->toPlainText().append( "\nCalculation finished" );
    text->setText( txt );
}

/* all odd rows return false for the 1st call -> even rows are calculated first */
bool
ParallelCalculation::isReady()
{
    bool ready = ( x_ % 2 == 0 || !first );
    first = false;
    return ready;
}

/* one task, that can be calculated in parallel */
void
ParallelCalculation::calculate()
{
    // slow down the execution to simulate an expensive calculation, works in Qt4 and Qt5
    {
        QWaitCondition wc;
        QMutex         mutex;
        QMutexLocker   locker( &mutex );
        int            wait_factor = cubegui::Globals::optionIsSet( cubegui::Single ) ? 10 : 100;
        int            msecs       = ( std::rand() % 5 ) * wait_factor;
        wc.wait( &mutex, msecs );
    }

    // example for expensive calculation which should be done in parallel :-)
    *result_ = x_ * y_;
}
