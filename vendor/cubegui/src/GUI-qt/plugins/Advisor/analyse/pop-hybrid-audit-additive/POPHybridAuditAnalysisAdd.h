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


#ifndef POP_HYBRID_AUDIT_PERFORMANCE_ANALYSIS_ADD_H
#define POP_HYBRID_AUDIT_PERFORMANCE_ANALYSIS_ADD_H

#include <string>
#include <QObject>
#include <QToolBar>
#include "Cube.h"


#include "PerformanceAnalysis.h"
#include "PerformanceTest.h"

#include "POPHybridAmdahlTestAdd.h"
#include "POPHybridCommunicationEfficiencyTestAdd.h"
#include "POPHybridComputationTimeTestAdd.h"
#include "POPHybridIPCTestAdd.h"
#include "POPHybridImbalanceTestAdd.h"
#include "POPHybridNoWaitINSTestAdd.h"
#include "POPHybridOMPRegionEfficiencyTestAdd.h"
#include "POPHybridParallelEfficiencyTestAdd.h"
#include "POPHybridProcessEfficiencyTestAdd.h"
#include "POPHybridSerialisationTestAdd.h"
#include "POPHybridStalledResourcesTestAdd.h"
#include "POPHybridTransferTestAdd.h"
#include "POPHybridThreadEfficiencyTestAdd.h"




namespace advisor
{
class POPHybridAuditPerformanceAnalysisAdd : public PerformanceAnalysis
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

    POPHybridStalledResourcesTestAdd* stalled_resources;
    POPHybridNoWaitINSTestAdd*        no_wait_ins;
    POPHybridIPCTestAdd*              ipc;
    POPHybridComputationTimeAdd*      comp;

    POPHybridTransferTestAdd*                pop_transfer_eff;
    POPHybridSerialisationTestAdd*           pop_ser_eff;
    POPHybridCommunicationEfficiencyTestAdd* comm_eff;
    POPHybridImbalanceTestAdd*               lb_eff;
    POPHybridProcessEfficiencyTestAdd*       proc_eff;

    POPHybridOmpRegionEfficiencyTestAdd* omp_region_eff;
    POPHybridAmdahlTestAdd*              pop_amdahl_eff;

    POPHybridThreadEfficiencyTestAdd* thread_eff;

    POPHybridParallelEfficiencyTestAdd* par_eff;



    double max_ipc;

public:
    POPHybridAuditPerformanceAnalysisAdd( cube::CubeProxy* _cube );

    virtual
    ~POPHybridAuditPerformanceAnalysisAdd();


    inline
    const
    std::string
    name() const
    {
        return "NAG POP Hybrid Assessment (Additive)";
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
