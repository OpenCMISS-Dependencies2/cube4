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


#ifndef POP_HYBRID_AUDIT_PERFORMANCE_ANALYSIS_H
#define POP_HYBRID_AUDIT_PERFORMANCE_ANALYSIS_H

#include <string>
#include <QObject>
#include <QToolBar>
#include "Cube.h"


#include "PerformanceAnalysis.h"
#include "PerformanceTest.h"

#include "POPHybridAmdahlTest.h"
#include "POPHybridCommunicationEfficiencyTest.h"
#include "POPHybridComputationTimeTest.h"
#include "POPHybridIPCTest.h"
#include "POPHybridImbalanceTest.h"
#include "POPHybridNoWaitINSTest.h"
#include "POPHybridOMPRegionEfficiencyTest.h"
#include "POPHybridParallelEfficiencyTest.h"
#include "POPHybridProcessEfficiencyTest.h"
#include "POPHybridSerialisationTest.h"
#include "POPHybridStalledResourcesTest.h"
#include "POPHybridTransferTest.h"
#include "POPHybridThreadEfficiencyTest.h"




namespace advisor
{
class POPHybridAuditPerformanceAnalysis : public PerformanceAnalysis
{
    Q_OBJECT
private:
    QString COMM_EFF_ISSUE;
    QString LB_EFF_ISSUE;
    QString SER_EFF_ISSUE;
    QString TRANSFER_EFF_ISSUE;
    QString STL_ISSUE;
    QString OMP_ISSUE;
    QString IPC_ISSUE;


protected:

    POPHybridStalledResourcesTest* stalled_resources;
    POPHybridNoWaitINSTest*        no_wait_ins;
    POPHybridIPCTest*              ipc;
    POPHybridComputationTime*      comp;

    POPHybridTransferTest*                pop_transfer_eff;
    POPHybridSerialisationTest*           pop_ser_eff;
    POPHybridCommunicationEfficiencyTest* comm_eff;
    POPHybridImbalanceTest*               lb_eff;
    POPHybridProcessEfficiencyTest*       proc_eff;

    POPHybridOmpRegionEfficiencyTest* omp_region_eff;
    POPHybridAmdahlTest*              pop_amdahl_eff;

    POPHybridThreadEfficiencyTest* thread_eff;

    POPHybridParallelEfficiencyTest* par_eff;



    double max_ipc;

public:
    POPHybridAuditPerformanceAnalysis( cube::CubeProxy* _cube );

    virtual
    ~POPHybridAuditPerformanceAnalysis();


    inline
    const
    std::string
    name() const
    {
        return "NAG POP Hybrid Assessment (Multiplicative)";
    }


    virtual
    QWidget*
    getToolBar()
    {
        return new QWidget();
    }


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
