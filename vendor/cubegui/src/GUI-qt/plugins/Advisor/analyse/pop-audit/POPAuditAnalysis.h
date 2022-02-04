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


#ifndef POP_AUDIT_PERFORMANCE_ANALYSIS_H
#define POP_AUDIT_PERFORMANCE_ANALYSIS_H

#include <string>
#include <QObject>
#include <QToolBar>
#include "Cube.h"


#include "PerformanceAnalysis.h"
#include "PerformanceTest.h"

#include "POPCommunicationEfficiencyTest.h"
#include "POPImbalanceTest.h"
#include "POPParallelEfficiencyTest.h"
#include "POPStalledResourcesTest.h"
#include "POPIPCTest.h"
#include "POPNoWaitINSTest.h"
#include "POPSerialisationTest.h"
#include "POPTransferTest.h"
#include "POPComputationTimeTest.h"




namespace advisor
{
class POPAuditPerformanceAnalysis : public PerformanceAnalysis
{
    Q_OBJECT
private:
    QString COMM_EFF_ISSUE;
    QString LB_EFF_ISSUE;
    QString SER_EFF_ISSUE;
    QString TRANSFER_EFF_ISSUE;
    QString STL_ISSUE;
    QString IPC_ISSUE;


protected:
    POPCommunicationEfficiencyTest* comm_eff;
    POPImbalanceTest*               lb_eff;
    POPParallelEfficiencyTest*      par_eff;
    POPStalledResourcesTest*        stalled_resources;
    POPNoWaitINSTest*               no_wait_ins;
    POPIPCTest*                     ipc;
    POPComputationTime*             comp;
    POPSerialisationTest*           pop_ser_eff;
    POPTransferTest*                pop_transfer_eff;

    double max_ipc;

public:
    POPAuditPerformanceAnalysis( cube::CubeProxy* _cube );

    virtual
    ~POPAuditPerformanceAnalysis();


    inline
    const
    std::string
    name() const
    {
        return "POP Assessment";
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
