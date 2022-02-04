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


#ifndef BSPOP_HYBRID_AUDIT_PERFORMANCE_ANALYSIS_H
#define BSPOP_HYBRID_AUDIT_PERFORMANCE_ANALYSIS_H

#include <string>
#include <QObject>
#include <QToolBar>
#include "Cube.h"


#include "PerformanceAnalysis.h"
#include "PerformanceTest.h"

#include "BSPOPHybridComputationTimeTest.h"
#include "BSPOPHybridIPCTest.h"
#include "BSPOPHybridMPICommunicationEfficiencyTest.h"
#include "BSPOPHybridMPILoadBalanceTest.h"
#include "BSPOPHybridMPIParallelEfficiencyTest.h"
#include "BSPOPHybridMPISerialisationTest.h"
#include "BSPOPHybridMPITransferTest.h"
#include "BSPOPHybridNoWaitINSTest.h"
#include "BSPOPHybridOMPCommunicationEfficiencyTest.h"
#include "BSPOPHybridOMPLoadBalanceEfficiencyTest.h"
#include "BSPOPHybridOMPParallelEfficiencyTest.h"
#include "BSPOPHybridParallelEfficiencyTest.h"
#include "BSPOPHybridCommunicationEfficiencyTest.h"
#include "BSPOPHybridLoadBalanceTest.h"
#include "BSPOPHybridStalledResourcesTest.h"



namespace advisor
{
class BSPOPHybridAuditPerformanceAnalysis : public PerformanceAnalysis
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

    BSPOPHybridStalledResourcesTest* stalled_resources;
    BSPOPHybridNoWaitINSTest*        no_wait_ins;
    BSPOPHybridIPCTest*              ipc;
    BSPOPHybridComputationTime*      comp;

    BSPOPHybridMPITransferTest*                mpi_transfer_eff;
    BSPOPHybridMPISerialisationTest*           mpi_ser_eff;
    BSPOPHybridMPICommunicationEfficiencyTest* mpi_comm_eff;
    BSPOPHybridMPILoadBalanceTest*             mpi_lb_eff;
    BSPOPHybridMPIParallelEfficiencyTest*      mpi_pareff;

    BSPOPHybridOMPCommunicationEfficiencyTest* omp_comm_eff;
    BSPOPHybridOMPLoadBalanceEfficiencyTest*   omp_lb_eff;
    BSPOPHybridOMPParallelEfficiencyTest*      omp_par_eff;

    BSPOPHybridCommunicationEfficiencyTest* hyb_comm_eff;
    BSPOPHybridLoadBalanceTest*             hyb_lb_eff;
    BSPOPHybridParallelEfficiencyTest*      hyb_par_eff;



    double max_ipc;

public:
    BSPOPHybridAuditPerformanceAnalysis( cube::CubeProxy* _cube );

    virtual
    ~BSPOPHybridAuditPerformanceAnalysis();


    inline
    const
    std::string
    name() const
    {
        return "BSC POP Hybrid Assessment";
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
