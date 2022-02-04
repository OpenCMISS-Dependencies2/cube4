/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2015-2021                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include <config.h>
#include <QList>
#include "CubeAdvisorProgress.h"
#include "POPHybridAuditAnalysisAdd.h"
#include "TreeItem.h"
#include "CubeAdvice.h"
#include "PerformanceTest.h"


using namespace cube;
using namespace cubegui;
using namespace cubepluginapi;
using namespace advisor;



POPHybridAuditPerformanceAnalysisAdd::POPHybridAuditPerformanceAnalysisAdd( cube::CubeProxy* _cube ) : PerformanceAnalysis( _cube )
{
    stalled_resources = new POPHybridStalledResourcesTestAdd( cube );
    ipc               = new POPHybridIPCTestAdd( cube );
    no_wait_ins       = new POPHybridNoWaitINSTestAdd( cube );
    comp              = new POPHybridComputationTimeAdd( cube );

    pop_ser_eff      = new POPHybridSerialisationTestAdd( cube );
    pop_transfer_eff = new POPHybridTransferTestAdd( cube );
    comm_eff         = new POPHybridCommunicationEfficiencyTestAdd( cube, pop_ser_eff, pop_transfer_eff );
    lb_eff           = new POPHybridImbalanceTestAdd( cube );
    proc_eff         = new POPHybridProcessEfficiencyTestAdd( cube, lb_eff, comm_eff );

    omp_region_eff = new POPHybridOmpRegionEfficiencyTestAdd( cube );
    pop_amdahl_eff = new POPHybridAmdahlTestAdd( cube );

    thread_eff = new POPHybridThreadEfficiencyTestAdd( cube, pop_amdahl_eff,  omp_region_eff );
    par_eff    = new POPHybridParallelEfficiencyTestAdd( cube, proc_eff, thread_eff );


    max_ipc            = ipc->getMaximum();
    COMM_EFF_ISSUE     = tr( "This call path has very low communication efficiency (%1) " );
    LB_EFF_ISSUE       = tr( "This call not sufficient load balance efficiency (%1)" );
    SER_EFF_ISSUE      = tr( "This call not sufficient serialisation efficiency (%1)" );
    TRANSFER_EFF_ISSUE = tr( "This call not sufficient transfer efficiency (%1)" );
    STL_ISSUE          = tr( "This call waits for stalled resources (%1)" );
    IPC_ISSUE          = tr( "This call has a little ratio of instructions per cycle (%1 of %2)" );
    OMP_ISSUE          = tr( "This call has not sufficient OMP region efficiency in MPI rank %1 (%2 )" );
}


POPHybridAuditPerformanceAnalysisAdd::~POPHybridAuditPerformanceAnalysisAdd()
{
    delete stalled_resources;
    delete ipc;
    delete no_wait_ins;
    delete comp;

    delete comm_eff;
    delete lb_eff;
    delete pop_ser_eff;
    delete pop_transfer_eff;
    delete proc_eff;

    delete omp_region_eff;
    delete pop_amdahl_eff;

    delete thread_eff;

    delete par_eff;
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

/*
   static
   bool
   exclude_from_omp_analysis( cube::CubeProxy* cube, cube::Cnode* cnode )
   {
    list_of_metrics lmetrics;
    metric_pair     metric;
    metric.second = cube::CUBE_CALCULATE_INCLUSIVE;
    cube::Metric* _ser = cube->getMetric( "__service_serial_marker" );
    metric.first = _ser;
    lmetrics.push_back( metric );
    cube::list_of_sysresources lsysres;
    cube::list_of_cnodes       lcnodes;

    cube::cnode_pair pcnode = std::make_pair( const_cast<cube::Cnode*>( cnode ), cube::CUBE_CALCULATE_INCLUSIVE );
    lcnodes.push_back( pcnode );

    cube::Value* v = cube->calculateValue( lmetrics,
                                           lcnodes,
                                           lsysres );
    double serival = v->getDouble();
    if ( serival != 0 )
    {
        return true;
    }

    if ( cnode->get_callee()->get_paradigm().compare( "compiler" ) == 0 )
    {
        return false;
    }
    if ( cnode->get_callee()->get_paradigm().compare( "openmp" ) == 0 )
    {
        if ( cnode->get_callee()->get_role().compare( "implicit barrier" ) == 0 )
        {
            return true;
        }
        return false;
    }
    return true;
   }
 */


QList<AdvisorAdvice>
POPHybridAuditPerformanceAnalysisAdd::getCandidates( const QList<TreeItem*>& _list )
{
    QList<AdvisorAdvice> to_return;
    int                  progress = 0;
    //const std::vector<cube::LocationGroup*>& _lgs     = cube->getLocationGroups();

    foreach( TreeItem * item, _list )
    {
        // if the values are exactly 0 they are most likely deactivated (e.g., MPI/OpenMP)
        // don't mark in these cases
        AdvisorAdvice advice;
        bool          mark          = false;
        QString       advice_string = tr( "POP Analysis: \n" );


        cube::Cnode* cnode = static_cast<cube::Cnode*>( item->getCubeObject() );

        list_of_cnodes _cnodes;
        cnode_pair     pair;
        pair.first  = cnode;
        pair.second = cube::CUBE_CALCULATE_INCLUSIVE;


        _cnodes.push_back( pair );

        // first MPI efficiencies


        if ( !exclude_from_analysis( cnode ) )
        {
            double pop_lb_eff_value = lb_eff->analyze( _cnodes );
            if (  lb_eff->isActive() && ( pop_lb_eff_value <= 0.8 ) )
            {
                mark           |= true;
                advice.callpath = item;
                advice_string  += LB_EFF_ISSUE.arg( pop_lb_eff_value ) + "\n";
            }

            double comm_eff_value = comm_eff->analyze( _cnodes );
            if ( comm_eff->isActive() && ( comm_eff_value < 0.8 )  )
            {
                mark           |= true;
                advice.callpath = item;
                advice_string  += COMM_EFF_ISSUE.arg( comm_eff_value ) + "\n";
            }

            double pop_ser_eff_value = pop_ser_eff->analyze( _cnodes );
            if ( pop_ser_eff->isActive() && ( pop_ser_eff_value < 0.8 )  )
            {
                mark           |= true;
                advice.callpath = item;
                advice_string  += SER_EFF_ISSUE.arg( pop_ser_eff_value ) + "\n";
            }

            double pop_transfer_eff_value = pop_transfer_eff->analyze( _cnodes );
            if ( pop_transfer_eff->isActive() && ( pop_transfer_eff_value < 0.8 )  )
            {
                mark           |= true;
                advice.callpath = item;
                advice_string  += TRANSFER_EFF_ISSUE.arg( pop_transfer_eff_value ) + "\n";
            }

            double stalled_resources_value = stalled_resources->analyze( _cnodes );
            if (  stalled_resources->isActive() && ( stalled_resources_value >= 0.50 ) )
            {
                mark           |= true;
                advice.callpath = item;
                advice_string  += STL_ISSUE.arg( stalled_resources_value ) + "\n";
            }
        }
        // now omp efficiencies, but for every location group
/*
        if ( !exclude_from_omp_analysis( cube, cnode ) )
        {
            for ( std::vector<cube::LocationGroup*>::const_iterator lgiter = _lgs.begin(); lgiter != _lgs.end(); ++lgiter )
            {
                double pop_omp_lb_value = lb_omp_eff->analyze( _cnodes, *lgiter );
                if (  lb_omp_eff->isActive() && ( pop_omp_lb_value <= 0.8 ) )
                {
                    mark           |= true;
                    advice.callpath = item;
                    advice_string  += OMP_LB_ISSUE.arg( ( *lgiter )->get_rank() ).arg( pop_omp_lb_value ) + "\n";
                }
                double pop_omp_ser_value = pop_omp_ser_eff->analyze( _cnodes, *lgiter );
                if (  pop_omp_ser_eff->isActive() && ( pop_omp_ser_value <= 0.8 ) )
                {
                    mark           |= true;
                    advice.callpath = item;
                    advice_string  += OMP_SER_ISSUE.arg( ( *lgiter )->get_rank() ).arg( pop_omp_ser_value ) + "\n";
                }
                double pop_omp_transfer_value = pop_omp_transfer_eff->analyze( _cnodes, *lgiter );
                if (  pop_omp_transfer_eff->isActive() && ( pop_omp_transfer_value <= 0.8 ) )
                {
                    mark           |= true;
                    advice.callpath = item;
                    advice_string  += OMP_TRANSFER_ISSUE.arg( ( *lgiter )->get_rank() ).arg( pop_omp_transfer_value ) + "\n";
                }
            }
        }
 */

        PROGRESS_BUSY_WAIT( item->getName(), ( int )( ( double )progress * 100. / ( double )_list.size() ) );
        progress++;
        if ( mark )
        {
            advice.comments << advice_string;
            to_return << advice;
        }
    }
    return to_return;
}

QList<PerformanceTest*>
POPHybridAuditPerformanceAnalysisAdd::getPerformanceTests()
{
    QList<PerformanceTest*> to_return;
    to_return << par_eff
              << proc_eff
              << lb_eff
              << comm_eff
              << pop_ser_eff
              << pop_transfer_eff
              << thread_eff
              << pop_amdahl_eff
              << omp_region_eff
              << stalled_resources
              << ipc
              << no_wait_ins
              << comp;
    return to_return;
}


// ------ overview tests ---------

bool
POPHybridAuditPerformanceAnalysisAdd::isActive() const
{
    return true;
}

QString
POPHybridAuditPerformanceAnalysisAdd::getAnchorHowToMeasure()
{
    return "MeasurementForPOPHybridAnalysis";
}
