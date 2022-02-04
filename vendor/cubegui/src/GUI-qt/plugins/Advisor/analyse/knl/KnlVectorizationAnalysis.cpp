/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2015-2019                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include <config.h>
#include <QList>
#include "KnlVectorizationAnalysis.h"
#include "TreeItem.h"
#include "CubeAdvice.h"
#include "PerformanceTest.h"



using namespace cube;
using namespace cubegui;
using namespace cubepluginapi;
using namespace advisor;

KnlVectorizationAnalysis::KnlVectorizationAnalysis( cube::CubeProxy* _cube ) : PerformanceAnalysis( _cube )
{
    vpu_test     = new VPUIntensityTest( cube );
    l1_comp2data = new L1Comp2DataTest( cube );
    l2_comp2data = new L2Comp2DataTest( cube );
    VPU_ISSUE    = tr( "This call path should be vectorized: VPU (%1 < %2)" );
    L1_ISSUE     = tr( "This call path should be vectorized: L1 (%1 < %2)" );
    L2_ISSUE     = tr( "This call path should be vectorized L2 (%1 < %2 * 100)" );
}


KnlVectorizationAnalysis::~KnlVectorizationAnalysis()
{
    delete vpu_test;
    delete l1_comp2data;
    delete l2_comp2data;
};



QList<AdvisorAdvice>
KnlVectorizationAnalysis::getCandidates( const QList<TreeItem*>& _list )
{
    QList<AdvisorAdvice> to_return;
    foreach( TreeItem * item, _list )
    {
        cube::Cnode* cnode = static_cast<cube::Cnode*>( item->getCubeObject() );
        vpu_test->apply( cnode,  cube::CUBE_CALCULATE_EXCLUSIVE );
        l1_comp2data->apply( cnode, cube::CUBE_CALCULATE_EXCLUSIVE );
        l2_comp2data->apply( cnode,  cube::CUBE_CALCULATE_EXCLUSIVE );

        double vpu = vpu_test->value();
        double l1  = l1_comp2data->value();
        double l2  = l2_comp2data->value();

        // if the values are exactly 0 they are most likely deactivated (e.g., MPI/OpenMP)
        // don't mark in these cases
        AdvisorAdvice advice;
        bool          mark          = false;
        QString       advice_string = tr( "KNL Vectorization analysis: \n" );
        if ( vpu_test->isActive() && ( ( vpu < 0.5 ) && ( vpu != 0 ) ) )
        {
            mark           |= true;
            advice.callpath = item;
            advice_string  += VPU_ISSUE.arg( vpu ).arg( 0.5 ) + QString( "\n" );
        }
        if ( l1_comp2data->isActive() && ( ( l1 < 1 ) && ( l1 != 0 ) ) )
        {
            mark           |= true;
            advice.callpath = item;
            advice_string  += L1_ISSUE.arg( l1 ).arg( 1 ) + QString( "\n" );
        }
        if ( l2_comp2data->isActive() && (  ( l2 < 100 * l1 ) && ( l2 != 0 ) ) )
        {
            mark           |= true;
            advice.callpath = item;
            advice_string  += L2_ISSUE.arg( l2 ).arg( l1 ) + QString( "\n" );
        }
        if ( mark )
        {
            advice.comments << advice_string;
            to_return << advice;
        }
    }
    return to_return;
}

QList<PerformanceTest*>
KnlVectorizationAnalysis::getPerformanceTests()
{
    QList<PerformanceTest*> to_return;
    to_return << vpu_test
              << l1_comp2data
              << l2_comp2data;
    return to_return;
}


// ------ overview tests ---------

bool
KnlVectorizationAnalysis::isActive() const
{
    return true;
}

QString
KnlVectorizationAnalysis::getAnchorHowToMeasure()
{
    return "MeasurementForKnlVectorizationAnalysis";
}
