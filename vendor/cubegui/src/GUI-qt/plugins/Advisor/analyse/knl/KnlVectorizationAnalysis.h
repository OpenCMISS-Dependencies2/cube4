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


#ifndef KNL_VECTORIZATION_ANALYSIS_H
#define KNL_VECTORIZATION_ANALYSIS_H

#include <string>
#include <QObject>
#include <QToolBar>
#include "CubeProxy.h"


#include "PerformanceAnalysis.h"
#include "PerformanceTest.h"

#include "VPUIntensityTest.h"
#include "L1CompToDataTest.h"
#include "L2CompToDataTest.h"





namespace advisor
{
class KnlVectorizationAnalysis : public PerformanceAnalysis
{
    Q_OBJECT
private:
    QString VPU_ISSUE;
    QString L1_ISSUE;
    QString L2_ISSUE;

protected:
    VPUIntensityTest* vpu_test;
    L1Comp2DataTest*  l1_comp2data;
    L2Comp2DataTest*  l2_comp2data;

public:
    KnlVectorizationAnalysis( cube::CubeProxy* _cube );

    virtual
    ~KnlVectorizationAnalysis();


    inline
    const
    std::string
    name() const
    {
        return "KNL Vectorization";
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
