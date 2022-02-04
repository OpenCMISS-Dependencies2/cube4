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
#include <QDebug>
#include "Future.h"
#include "TabInterface.h"
#include "Globals.h"
#include "ProgressBar.h"
#include "TabWidget.h"

using namespace std;
using namespace cubegui;

QElapsedTimer Future::globalTimer;

Future::Future( cubegui::TabWidget* tab, TabInterface* tabInterface )
{
    this->tabWidget    = tab;
    this->tabInterface = tabInterface;
    this->progressBar  = 0;
    this->isRunning_   = false;

    calculationQueueSize = 0;

    // create progress bar
    if ( !progressBar && tabInterface && tabWidget )
    {
        progressBar = new ProgressBar( tabWidget->getTab( tabInterface )->getStatusLayout() );
    }

#ifdef CUBE_CONCURRENT_LIB
    connect( &futureWatcher, SIGNAL( finished() ), this, SLOT( calculationFinishedSlot() ) );
#endif
}

Future::~Future()
{
    delete progressBar;
}

void
Future::cancelCalculation( bool waitForFinished )
{
#ifdef CUBE_CONCURRENT_LIB
    if ( future.isRunning() )
    {
        future.cancel();
        if ( waitForFinished )
        {
            if ( progressBar )
            {
                progressBar->disconnect(); // prevent notification of progress bar after plugin has been closed
            }
            future.waitForFinished();
        }
    }
#endif
}

void
Future::startCalculation( bool dependsOnTreeSelection )
{
    bool single = Globals::optionIsSet( Single );
#ifndef CUBE_CONCURRENT_LIB
    single = true;
#endif
    // ----------  single-threaded execution
    if ( single )
    {
        calculationQueueSize = workerData.size();
        Globals::getMainWindow()->setCursor( Qt::WaitCursor );
        timer.start();
        for ( Task* data : workerData )
        {
            data->calculate();
        }
        calculationFinishedSlot();
        Globals::getMainWindow()->unsetCursor();
        return;
    }

    // ----------  multi-threaded execution

    this->dependsOnTreeSelection_ = dependsOnTreeSelection;
    queuedWorkerData              = workerData;
    workerData.clear();
    if ( isRunning() )
    {
        cancelCalculation();
        startAfterCancelation = true; // start after future has been successfully canceled
    }
    else
    {
        startCalculation_();
    }
}

void
Future::startCalculation_()
{
    startAfterCancelation = false;
    if ( queuedWorkerData.size() == 0 ) // no tasks available
    {
        return;
    }

    //qDebug() << "calculation started" << ( tabInterface ? tabInterface->label() : "?" );

    setRunning( true );

    calculationQueueSize = queuedWorkerData.size();
    nextWorkerData       = queuedWorkerData;
    queuedWorkerData.clear();

    timer.start();
    nextCalculationStep();

#ifdef CUBE_CONCURRENT_LIB
    if ( progressBar )
    {
        progressBar->setFutureWatcher( &futureWatcher );
        progressBar->setProgressRange( 0, calculationQueueSize );
    }
#endif
}

void
Future::nextCalculationStep()
{
    QList<Task*> tasks = nextWorkerData;
    currentWorkerData.clear();
    nextWorkerData.clear();
    foreach( Task * task, tasks )
    {
        if ( task->isReady() )
        {
            currentWorkerData.append( task );
        }
        else
        {
            nextWorkerData.append( task );
        }
    }
#ifdef CUBE_CONCURRENT_LIB
    future = QtConcurrent::map( currentWorkerData, [ & ]( Task* t ){
        calculate( t );
    } );
    futureWatcher.setFuture( future );
#endif
}

void
Future::calculationFinishedSlot()
{
    qint64 elapsed = timer.elapsed();
    emit   calculationStepFinished();

    bool single = Globals::optionIsSet( Single );
#ifndef CUBE_CONCURRENT_LIB
    single = true;
#endif

    if ( single )
    {
        workerData.clear();
        setRunning( false );
    }
    else
    {
        if ( !nextWorkerData.isEmpty() )        // go on with dependent calculations
        {
            elapsed = -1;
            if ( !currentWorkerData.isEmpty() ) // start dependent tasks                                                                                   // error -> invalid task conditions
            {
                if ( progressBar )
                {
                    progressBar->setProgressRange( calculationQueueSize - nextWorkerData.size(), calculationQueueSize );
                }
                nextCalculationStep();
            }
            else // error state
            {
                setRunning( false );
                QString msg = tr( "Future::startCalculation: state of all remaining tasks is \"not ready\" -> aborting calculation for " ) + tabInterface->label();
                Globals::setStatusMessage( msg, MessageType::Error );
                qDebug() << msg;
            }
        }
        else // all tasks have been finished
        {
            if ( startAfterCancelation ) // start next task, previous results are ignored
            {
                startCalculation_();
            }
            else
            {
                setRunning( false );
            }
        }
    }

    if ( elapsed >= 0 ) // show elapsed time if calculation has been finished
    {
        QString label = tabInterface ? tabInterface->label() : "?";
        Globals::debug( "Future" ) << ( single ? "serial" : "parallel" ) << " calculation finished " << label
                                   << " ( " << calculationQueueSize << " elements ) in " << timer.elapsed() << " ms"
                                   << ", since selection: " << globalTimer.elapsed() << " ms" << Qt::endl;
    }
    if ( !errorMessage.isEmpty() )
    {
        Globals::setStatusMessage( errorMessage, Error );
        errorMessage = "";
    }
}

#ifdef CUBE_CONCURRENT_LIB
const QFutureWatcher<void>&
Future::getFutureWatcher() const
{
    return futureWatcher;
}
#endif

/**
 * @brief Future::setRunning sets status of execution to true (running) or false.
 */
void
Future::setRunning( bool isRunning )
{
    isRunning_ = isRunning;
    if ( tabWidget )
    {
        if ( isRunning )
        {
#ifdef CUBE_CONCURRENT_LIB
            if ( progressBar )
            {
                progressBar->setFutureWatcher( &futureWatcher );
                progressBar->setProgressRange( 0, calculationQueueSize );
            }
#endif
            tabWidget->setCursor( Qt::BusyCursor );
        }
        else // canceled or finished
        {
            // ensure, that progress bar doesn't receive any events
            if ( progressBar )
            {
                progressBar->disconnect();
            }
            // if canceled, clear queued tasks (otherwise the queues are already empty)
            currentWorkerData.clear();
            nextWorkerData.clear();

            tabWidget->unsetCursor();

            emit calculationFinished();
        }
    }
}

bool
Future::dependsOnTreeSelection() const
{
    return dependsOnTreeSelection_;
}

bool
Future::isRunning()
{
    return isRunning_;
}

void
Future::startTimer()
{
    globalTimer.start();
}

void
Future::calculate( Task* data )
{
    try
    {
        data->calculate();
    }
    catch ( const cube::FatalError& e )
    {
        errorMessage = e.what();
    }

    delete data;
}
