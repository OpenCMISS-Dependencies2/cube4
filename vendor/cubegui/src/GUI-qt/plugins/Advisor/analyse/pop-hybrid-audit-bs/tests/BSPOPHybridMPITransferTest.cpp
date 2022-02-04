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

#include "BSPOPHybridMPITransferTest.h"


using namespace advisor;

BSPOPHybridMPITransferTest::BSPOPHybridMPITransferTest( cube::CubeProxy* cube ) : PerformanceTest( cube )
{
    setName( tr( " * * * MPI Transfer Efficiency" ).toUtf8().data() );
    setWeight( 1 );   // need to be adjusted
    pop_transfer = cube->getMetric( "transf_eff" );
    if ( pop_transfer == nullptr )
    {
        adjustForTest( cube );
    }
    pop_transfer = cube->getMetric( "transf_eff" );
    if ( pop_transfer == nullptr )
    {
        setWeight( 0.1 );
        setValue( 0. );
        return;
    }
    cube::list_of_sysresources lsysres = getRootsOfSystemTree();
    cube::metric_pair          metric;
    metric.first  = pop_transfer;
    metric.second = cube::CUBE_CALCULATE_INCLUSIVE;
    lmetrics.push_back( metric );
}



void
BSPOPHybridMPITransferTest::applyCnode( const cube::Cnode*, const cube::CalculationFlavour,
                                        const bool )
{
}

void
BSPOPHybridMPITransferTest::applyCnode( const cube::list_of_cnodes& cnodes,
                                        const bool                  direct_calculation  )
{
    ( void )direct_calculation; // not used here
    setValue(  analyze( cnodes ) );
}

double
BSPOPHybridMPITransferTest::analyze( const cube::list_of_cnodes& cnodes,
                                     cube::LocationGroup* ) const
{
    if ( pop_transfer == nullptr )
    {
        return 0.;
    }
    cube::value_container      inclusive_values;
    cube::value_container      exclusive_values;
    cube::list_of_sysresources lsysres;
    cube::IdIndexMap           metric_id_indices;
    cube->getMetricSubtreeValues( cnodes,
                                  lsysres,
                                  *pop_transfer,
                                  0,
                                  metric_id_indices,
                                  &inclusive_values,
                                  &exclusive_values );
    return inclusive_values[ 0 ]->getDouble();
}


const std::string&
BSPOPHybridMPITransferTest::getCommentText() const
{
    if ( pop_transfer == nullptr )
    {
        return no_comment;
    }
    else
    {
        return no_comment;
    }
}


// ------ overview tests ---------

bool
BSPOPHybridMPITransferTest::isActive() const
{
    return pop_transfer != nullptr;
};

bool
BSPOPHybridMPITransferTest::isIssue() const
{
    return false;
};

void
BSPOPHybridMPITransferTest::adjustForTest( cube::CubeProxy* cube ) const
{
    if ( scout_metrics_available( cube ) )
    {
        add_max_total_time_ideal( cube );
        add_max_total_time( cube );
        add_transfer_eff( cube );
    }
}


void
BSPOPHybridMPITransferTest::add_transfer_eff( cube::CubeProxy* ) const
{
    cube::Metric* _met = cube->getMetric( "transf_eff" );
    if ( _met == nullptr )
    {
        _met = cube->defineMetric(
            tr( "MPITransfer efficiency" ).toUtf8().data(),
            "transf_eff",
            "DOUBLE",
            "",
            "",
            BSPOP_TRANSFER_EFF_METRIC_URL,
            tr( "MPITransfer efficiency reflects the loss of efficiency due to actual data transfer, i.e. ( max_total_time_ideal / max_total_time ). The value varies from 0 to 100%, where 100% shows that routine is a pure computation routine and if MPI does not include waiting time. Do not consider values in the flat view and the system tree." ).toUtf8().data(),
            nullptr,
            cube::CUBE_METRIC_POSTDERIVED,
            "metric::max_total_time_ideal() / metric::max_total_time()",
            "",
            "",
            "",
            "",
            true,
            cube::CUBE_METRIC_GHOST
            );
        if ( _met != nullptr )
        {
            _met->setConvertible( false );
        }
        advisor_services->addMetric( _met, nullptr );
    }
}

void
BSPOPHybridMPITransferTest::add_max_total_time_ideal( cube::CubeProxy* ) const
{
    add_execution_time( cube );
    cube::Metric* _transfer_time_mpi = cube->getMetric( "transfer_time_mpi" );
    if ( _transfer_time_mpi == nullptr )
    {
        return;
    }

    cube::Metric* _met = cube->getMetric( "max_total_time_ideal" );
    if ( _met == nullptr )
    {
        _met = cube->defineMetric(
            tr( "Maximal total time in ideal network" ).toUtf8().data(),
            "max_total_time_ideal",
            "DOUBLE",
            tr( "sec" ).toUtf8().data(),
            "",
            BSPOP_TRANSFER_EFF_METRIC_URL,
            tr( "Maximal total time in ideal network, ( execution - transfer_time_mpi )" ).toUtf8().data(),
            nullptr,
            cube::CUBE_METRIC_PREDERIVED_INCLUSIVE,
            "metric::execution() - metric::transfer_time_mpi()",
            "",
            "",
            "",
            "max(arg1, arg2)",
            true,
            cube::CUBE_METRIC_GHOST
            );
        if ( _met != nullptr )
        {
            _met->setConvertible( false );
        }
        advisor_services->addMetric( _met, nullptr );
    }
}


void
BSPOPHybridMPITransferTest::add_max_total_time( cube::CubeProxy* ) const
{
    add_execution_time( cube );
    cube::Metric* _met = cube->getMetric( "max_total_time" );
    if ( _met == nullptr )
    {
        _met = cube->defineMetric(
            tr( "Maximal total time" ).toUtf8().data(),
            "max_total_time",
            "DOUBLE",
            tr( "sec" ).toUtf8().data(),
            "",
            BSPOP_TRANSFER_EFF_METRIC_URL,
            tr( "Maximal total time, max( execution )" ).toUtf8().data(),
            nullptr,
            cube::CUBE_METRIC_PREDERIVED_INCLUSIVE,
            "metric::execution()",
            "",
            "",
            "",
            "max(arg1, arg2)",
            true,
            cube::CUBE_METRIC_GHOST
            );
        if ( _met != nullptr )
        {
            _met->setConvertible( false );
        }
        advisor_services->addMetric( _met, nullptr );
    }
}
