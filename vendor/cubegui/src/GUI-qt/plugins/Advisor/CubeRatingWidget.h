/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2015-2020                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef ADVISER_RATING_WIDGET_H
#define ADVISER_RATING_WIDGET_H
#include <QtGui>
#include <QLabel>
#include <QWidget>
#include <QGroupBox>
#include <QMutex>
#include <QProgressBar>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QGridLayout>
#include "TreeItem.h"
#include "CubeTestWidget.h"
#include "CubeAdvice.h"

namespace advisor
{
class PerformanceAnalysis;
class PerformanceTest;
class ParallelCalculation;

class CubeRatingWidget : public QWidget
{
    Q_OBJECT

private:
    QString title;
    QLabel*                                      title_label;
    QGridLayout*                                 main_grid;
    QList<CubeTestWidget* >                      list_of_tests;
    PerformanceAnalysis*                         analysis;
    QGroupBox*                                   callpath_frame;
    QTableWidget*                                table;
    bool                                         analysis_possible;
    cubepluginapi::TabInterface*                 tab;
    QHash<ParallelCalculation*, CubeTestWidget*> calculations;
    QTimer*                                      updatetimer;
    QElapsedTimer                                elapsedStageTimer;
    QElapsedTimer                                elapsedTimer;
    cubepluginapi::Future*                       future;
    QMutex                                       guard;

    bool calculating_in_progress;

    void
    addPerformanceTest( PerformanceTest* t );

private slots:

    void
    tableItemClicked( QTableWidgetItem* _item );

    void
    calculationFinished();

    void
    calculationStepFinished();

    void
    calculationProgress();

public:
    CubeRatingWidget( cubepluginapi::TabInterface*,
                      QString  title,
                      PerformanceAnalysis*,
                      QWidget* parent = NULL );



    ~CubeRatingWidget();

    void
    apply( const cube::list_of_cnodes&,
           const bool direct_calculation = false );

    void
    copyMetricsValues( const cube::list_of_cnodes& );

    QList<AdvisorAdvice>
    runAnalysis( const QList<cubegui::TreeItem*>& cnodes );

    void
    invalidateAnalysis();

    bool
    isCalculating() const
    {
        return calculating_in_progress;
    }
};


/**
 * The class ParallelCalculation has to provide all data that is used for one calculation task, which is triggerd with
 * calculate(). It calculates the product of x_ and y_ and writes the result to result_.
 */
class ParallelCalculation : public cubepluginapi::Task
{
public:
    ParallelCalculation( QHash<ParallelCalculation*, CubeTestWidget*>* hash, PerformanceTest* _test, const cube::list_of_cnodes& _c, const bool d_calculation = false  ) : test( _test ), cnodes( _c ), calculations( hash ), direct_calculation( d_calculation )
    {
    };
    ParallelCalculation( const cube::list_of_cnodes& _c, const bool d_calculation = false  ) : test( NULL ), cnodes( _c ), calculations( NULL ), direct_calculation( d_calculation )
    {
    };


    virtual bool
    isReady()
    {
        return notifiers.size() == 0;
    }

    virtual void
    calculate();

    inline
    void
    addPrereq( ParallelCalculation* _pr )
    {
        notifiers.insert( _pr );
        _pr->toNotifier( this );
    }

    inline
    void
    toNotifier( ParallelCalculation* _pr )
    {
        toNotify.insert( _pr );
    }

    inline
    void
    notifiedBy( ParallelCalculation* _pr )
    {
        notifiers.remove( _pr );
    }

protected:

    PerformanceTest*                              test;
    QSet    <ParallelCalculation*>                notifiers;
    QSet    <ParallelCalculation*>                toNotify;
    const cube::list_of_cnodes                    cnodes;
    QHash<ParallelCalculation*, CubeTestWidget*>* calculations;
    QMutex                                        guard;

    // specifies if the efficiency is calculated as derivation from sub efficiencies or via direct formula
    bool direct_calculation;
};


class AnalysisUpdateTask : public ParallelCalculation
{
public:
    AnalysisUpdateTask( QTableWidget* _table, const cube::list_of_cnodes& _c ) : ParallelCalculation( _c ), table( _table )
    {
    }
    virtual void
    calculate();

protected:
    QTableWidget* table;
};
};

#endif // ADVISER_RATING_WIDGET_H
