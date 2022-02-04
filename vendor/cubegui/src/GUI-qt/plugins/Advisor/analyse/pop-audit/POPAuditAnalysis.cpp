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
#include "CubeAdvisorProgress.h"
#include "POPAuditAnalysis.h"
#include "TreeItem.h"
#include "CubeAdvice.h"
#include "PerformanceTest.h"


using namespace cube;
using namespace cubegui;
using namespace cubepluginapi;
using namespace advisor;



POPAuditPerformanceAnalysis::POPAuditPerformanceAnalysis( cube::CubeProxy* _cube ) : PerformanceAnalysis( _cube )
{
    stalled_resources  = new POPStalledResourcesTest( cube );
    ipc                = new POPIPCTest( cube );
    no_wait_ins        = new POPNoWaitINSTest( cube );
    comp               = new POPComputationTime( cube );
    pop_ser_eff        = new POPSerialisationTest( cube );
    pop_transfer_eff   = new POPTransferTest( cube );
    comm_eff           = new POPCommunicationEfficiencyTest( cube, pop_ser_eff, pop_transfer_eff );
    lb_eff             = new POPImbalanceTest( cube );
    par_eff            = new POPParallelEfficiencyTest( lb_eff, comm_eff );
    max_ipc            = ipc->getMaximum();
    COMM_EFF_ISSUE     = tr( "This call path has very low communication efficiency (%1) " );
    LB_EFF_ISSUE       = tr( "This call not sufficient load balance efficiency (%1)" );
    SER_EFF_ISSUE      = tr( "This call not sufficient serialisation efficiency (%1)" );
    TRANSFER_EFF_ISSUE = tr( "This call not sufficient transfer efficiency (%1)" );
    STL_ISSUE          = tr( "This call waits for stalled resources (%1)" );
    IPC_ISSUE          = tr( "This call has a little ratio of instructions per cycle (%1 of %2)" );
}


POPAuditPerformanceAnalysis::~POPAuditPerformanceAnalysis()
{
    delete comm_eff;
    delete lb_eff;
    delete par_eff;
    delete stalled_resources;
    delete ipc;
    delete no_wait_ins;
    delete comp;
    delete pop_ser_eff;
    delete pop_transfer_eff;
};


static
bool
exclude_from_analysis( cube::Cnode* cnode )
{
    if ( cnode->get_callee()->get_paradigm().compare( "compiler" ) == 0 )
    {
        return false;
    }
    if ( cnode->get_callee()->get_paradigm().compare( "openmp" ) == 0 )
    {
        if ( cnode->get_callee()->get_role().compare( "parallel" ) == 0 )
        {
            return false;
        }
        if ( cnode->get_callee()->get_role().compare( "loop" ) == 0 )
        {
            return false;
        }
    }

    return true;
}


QList<AdvisorAdvice>
POPAuditPerformanceAnalysis::getCandidates( const QList<TreeItem*>& _list )
{
    QList<AdvisorAdvice> to_return;
    int                  progress = 0;
    foreach( TreeItem * item, _list )
    {
        cube::Cnode* cnode = static_cast<cube::Cnode*>( item->getCubeObject() );
        if ( exclude_from_analysis( cnode ) )
        {
            continue;
        }
        list_of_cnodes _cnodes;
        cnode_pair     pair;
        pair.first  = cnode;
        pair.second = cube::CUBE_CALCULATE_INCLUSIVE;
        _cnodes.push_back( pair );

        pop_ser_eff->apply( _cnodes );

        pop_transfer_eff->apply( _cnodes );

        stalled_resources->apply( _cnodes );

        lb_eff->apply( _cnodes );

        comm_eff->apply( _cnodes );

        par_eff->apply( _cnodes );

        PROGRESS_BUSY_WAIT( item->getName(), ( int )( ( double )progress * 100. / ( double )_list.size() ) );
        progress++;


//         ipc->apply( cnode, cube::CUBE_CALCULATE_INCLUSIVE );

        double comm_eff_value          = comm_eff->value();
        double lb_eff_value            = lb_eff->value();
        double pop_ser_eff_value       = pop_ser_eff->value();
        double pop_transfer_eff_value  = pop_ser_eff->value();
        double stalled_resources_value = stalled_resources->value();
//         double ipc_value               = ipc->value();

        // if the values are exactly 0 they are most likely deactivated (e.g., MPI/OpenMP)
        // don't mark in these cases
        AdvisorAdvice advice;
        bool          mark          = false;
        QString       advice_string = tr( "POP Analysis: \n" );
        if ( comm_eff->isActive() && ( comm_eff_value < 0.8 )  )
        {
            mark           |= true;
            advice.callpath = item;
            advice_string  += COMM_EFF_ISSUE.arg( comm_eff_value ) + "\n";
        }
        if (  lb_eff->isActive() && ( lb_eff_value <= 0.8 ) )
        {
            mark           |= true;
            advice.callpath = item;
            advice_string  += LB_EFF_ISSUE.arg( lb_eff_value ) + "\n";
        }
        if ( pop_ser_eff->isActive() && ( pop_ser_eff_value < 0.8 )  )
        {
            mark           |= true;
            advice.callpath = item;
            advice_string  += SER_EFF_ISSUE.arg( pop_ser_eff_value ) + "\n";
        }
        if ( pop_transfer_eff->isActive() && ( pop_transfer_eff_value < 0.8 )  )
        {
            mark           |= true;
            advice.callpath = item;
            advice_string  += TRANSFER_EFF_ISSUE.arg( pop_transfer_eff_value ) + "\n";
        }
        if (  stalled_resources->isActive() && ( stalled_resources_value >= 0.50 ) )
        {
            mark           |= true;
            advice.callpath = item;
            advice_string  += STL_ISSUE.arg( stalled_resources_value ) + "\n";
        }
//         if ( ipc->isActive() &&  ( ipc_value / max_ipc <= 0.85 ) )
//         {
//             mark           |= true;
//             advice.callpath = item;
//             advice_string  += IPC_ISSUE.arg( ipc_value ).arg( max_ipc ) + "\n";
//         }
        if ( mark )
        {
            advice.comments << advice_string;
            to_return << advice;
        }
    }
    return to_return;
}

QList<PerformanceTest*>
POPAuditPerformanceAnalysis::getPerformanceTests()
{
    QList<PerformanceTest*> to_return;
    to_return << par_eff
              << lb_eff
              << comm_eff
              << pop_ser_eff
              << pop_transfer_eff
              << stalled_resources
              << ipc
              << no_wait_ins
              << comp;
    return to_return;
}


// ------ overview tests ---------

bool
POPAuditPerformanceAnalysis::isActive() const
{
    return true;
}

QString
POPAuditPerformanceAnalysis::getAnchorHowToMeasure()
{
    return "MeasurementForPOPAnalysis";
}
