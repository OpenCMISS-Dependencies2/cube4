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


#ifndef KNL_MEMORY_ANALYSIS_H
#define KNL_MEMORY_ANALYSIS_H

#include <string>
#include <QObject>
#include <QToolBar>
#include "Cube.h"


#include "PerformanceAnalysis.h"
#include "PerformanceTest.h"

#include "KnlMemoryBandwidthTest.h"
#include "KnlMemoryTransferTest.h"
#include "KnlLLCMissTest.h"




namespace advisor
{
class KnlMemoryAnalysis : public PerformanceAnalysis
{
    Q_OBJECT
private:
    QString MCDRAM_ISSUE;

protected:
    KnlMemoryBandwidthTest* knl_memory_bandwidth_test;
    KnlMemoryTransferTest*  knl_memory_transdfer_test;
    KnlLLCMissTest*         knl_llc_miss_test;
    int                     number_candidates;

    void
    fillAdviceHeader();

private slots:

    void
    setTreashold( int );



public:
    KnlMemoryAnalysis( cube::CubeProxy* _cube );

    virtual
    ~KnlMemoryAnalysis();


    inline
    const
    std::string
    name() const
    {
        return "KNL Memory";
    }


    virtual
    QWidget*
    getToolBar();


    virtual
    QList<AdvisorAdvice>
    getCandidates( const QList<cubegui::TreeItem*>& );


    virtual
    QList<PerformanceTest*>
    getPerformanceTests();

    virtual
    QString
    getAnchorHowToMeasure();

// ------ overview tests ---------

    virtual
    bool
    isActive() const;
};
};

#endif // ADVISER_RATING_WIDGET_H
