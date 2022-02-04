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


#ifndef ProgressBar_H
#define ProgressBar_H

#include "cubegui-concurrent.h"
#include <QWidget>
#ifdef CUBE_CONCURRENT_LIB
#include <QFutureWatcher>
#endif
#include <QStackedLayout>
#include "TabInterface.h"

namespace cubegui
{
class TabWidget;

/**
 * widget which shows the progress of a process
 * inserts the progress bar into the StackedLayout and removes it if the process is finished
 */
class ProgressBar : public QWidget
{
    Q_OBJECT
public:
    ProgressBar( QStackedLayout* layout );
    void
    paintEvent( QPaintEvent* );

    void
    setProgressRange( int min,
                      int max );

public slots:
#ifdef CUBE_CONCURRENT_LIB
    /** sets the futureWatcher whose progress should be displayed */
    void
    setFutureWatcher( const QFutureWatcher<void>* futureWatcher );

#endif

private slots:
    void
    setProgress( int progress );
    void
    closeProgressBar();
    void
    blink();

private:
#ifdef CUBE_CONCURRENT_LIB
    const QFutureWatcher<void>* futureWatcher;
#endif
    int progress;
    int             maxProgressValue;
    int             minProgressValue;
    QStackedLayout* layout;

    // blinking progress bar, if progress doesn't change
    bool           drawBlink;
    int            lastProgress;
    int            noProgressCounter;
    static QTimer* blinkTimer;
};
}
#endif
