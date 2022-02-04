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


#ifndef JSC_AUDIT_PERFORMANCE_ANALYSIS_H
#define JSC_AUDIT_PERFORMANCE_ANALYSIS_H

#include <string>
#include <QObject>
#include <QToolBar>
#include "Cube.h"


#include "PerformanceAnalysis.h"
#include "PerformanceTest.h"

#include "JSCHybridCommunicationEfficiencyTest.h"
#include "JSCHybridImbalanceTest.h"
#include "JSCHybridStalledResourcesTest.h"
#include "JSCHybridIPCTest.h"
#include "JSCHybridNoWaitINSTest.h"
#include "JSCHybridSerialisationTest.h"
#include "JSCHybridAmdahlTest.h"
#include "JSCHybridOMPSerialisationTest.h"
#include "JSCHybridOMPTransferTest.h"
#include "JSCHybridOMPImbalanceTest.h"
#include "JSCHybridTransferTest.h"
#include "JSCHybridComputationTimeTest.h"



namespace advisor
{
class JSCAuditPerformanceAnalysis : public PerformanceAnalysis
{
    Q_OBJECT
private:
    QString COMM_EFF_ISSUE;
    QString LB_EFF_ISSUE;
    QString SER_EFF_ISSUE;
    QString TRANSFER_EFF_ISSUE;
    QString STL_ISSUE;
    QString OMP_LB_ISSUE;
    QString OMP_SER_ISSUE;
    QString OMP_TRANSFER_ISSUE;
    QString IPC_ISSUE;


protected:
    JSCStalledResourcesTest* stalled_resources;
    JSCNoWaitINSTest*        no_wait_ins;
    JSCIPCTest*              ipc;
    JSCComputationTime*      comp;

    JSCTransferTest*                pop_transfer_eff;
    JSCSerialisationTest*           pop_ser_eff;
    JSCCommunicationEfficiencyTest* comm_eff;
    JSCImbalanceTest*               lb_eff;

    JSCOmpTransferTest*      pop_omp_transfer_eff;
    JSCOmpSerialisationTest* pop_omp_ser_eff;
    JSCOmpImbalanceTest*     lb_omp_eff;

    JSCAmdahlTest* pop_amdahl_eff;

    double max_ipc;

public:
    JSCAuditPerformanceAnalysis( cube::CubeProxy* _cube );

    virtual
    ~JSCAuditPerformanceAnalysis();


    inline
    const
    std::string
    name() const
    {
        return "JSC Hybrid Assessment";
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
