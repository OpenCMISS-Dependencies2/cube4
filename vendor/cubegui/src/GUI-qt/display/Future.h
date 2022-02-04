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


#ifndef FUTURE_H
#define FUTURE_H

#include "cubegui-concurrent.h"
#include "Task.h"
#include <QObject>
#include <QList>
#include <QElapsedTimer>
#include <QTimer>
#ifdef CUBE_CONCURRENT_LIB
#if QT_VERSION >= 0x050000
#include <QtConcurrent/QtConcurrent>
#else
#include <QtConcurrentMap>
#endif
#include <QFuture>
#include <QFutureWatcher>
#endif

namespace cubegui
{
class TabInterface;
class TabWidget;
class Task;
class ProgressBar;

/**
 * @brief The Future class represents the result of an asynchronous computation.
 * It is a wrapper to QFuture with QtConcurrent::map() to write multi-threaded plugins.
 * If QtConcurrent isn't available, the tasks are excuted serially.
 */
class Future : public QObject
{
    Q_OBJECT
public:
    /**
     * use Pluginservices::createFuture()
     * @param tabInterface the if tabInterface is given, a progress bar is displayed
     */
    Future( cubegui::TabWidget*    tab,
            cubegui::TabInterface* ti = 0 );
    ~Future();

    /** Adds a task to the task list. All tasks of that list are executed in parallel with startCalculation(). After execution, the
     * Task object is deleted */
    void
    addCalculation( Task* task )
    {
        workerData.append( task );
    }
    void
    addCalculations( const QList<Task*>& taskList )
    {
        workerData += taskList;
    }
    /**
     * @brief startCalculation starts all tasks which have been added with @ref addCalculation()
     * In general, startCalculation should be called from TabInterface::valuesChanged to ensure that calculations are
     * only executed for visible plugin.
     * @param dependsOnTreeSelection if true, the previous plugin calculations are cancelled, if tree selection changes.
     * The cancelation of plugin tasks is done before tree value calculation starts.
     */
    void
    startCalculation( bool dependsOnTreeSelection_ = false );

    /** Cancels the calculation.
     *  @param waitForFinished if true, blocks until the calculation has been finished. In mosts cases, it's preferable
     *  to use calculationFinished() signal instead, to prevent that the GUI gets blocked.
     */
    void
    cancelCalculation( bool waitForFinished = false );

    // method for use in QtConcurrent
    void
    calculate( Task* data );

    /** returns true, if the calculation depends on tree selections */
    bool
    dependsOnTreeSelection() const;

    /** returns true, if the computation is running */
    bool
    isRunning();

    static
    void
    startTimer();

#ifdef CUBE_CONCURRENT_LIB
    const QFutureWatcher<void>&
    getFutureWatcher() const;

#endif

signals:
    /** emitted, after the parallel excution started with @ref startCalculation() has been finished */
    void
    calculationFinished();

    /** emitted, after all tasks with isReady() == true have been finished and before the remaining tasks will be
        executed */
    void
    calculationStepFinished();

private slots:
    void
    calculationFinishedSlot();

private:
    QList<Task*>           workerData; // currenly filled data, not yet ordered to be started
    cubegui::TabWidget*    tabWidget;
    cubegui::TabInterface* tabInterface;
#ifdef CUBE_CONCURRENT_LIB
    QFutureWatcher<void> futureWatcher;
    QFuture<void>        future;
#endif
    QList<Task*>         queuedWorkerData;  // data of next job, which is waiting to be started
    QList<Task*>         currentWorkerData; // data of currently running tasks
    QList<Task*>         nextWorkerData;    // data of dependent tasks, which will be executed later
    QTimer               updateTimer;
    bool                 dependsOnTreeSelection_;
    bool                 startAfterCancelation;
    ProgressBar*         progressBar;
    int                  calculationQueueSize;
    QElapsedTimer        timer;
    static QElapsedTimer globalTimer;
    bool                 isRunning_;  // false, if calculation has been finished or sucessfully canceled
    QString              errorMessage;
    void
    nextCalculationStep();
    void
    startCalculation_();
    void
    setRunning( bool isRunning );
};
}

#endif // FUTURE_H
