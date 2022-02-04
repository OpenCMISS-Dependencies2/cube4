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

#include "JSCHybridTransferTest.h"


using namespace advisor;

JSCTransferTest::JSCTransferTest( cube::CubeProxy* cube ) : PerformanceTest( cube )
{
    setName( " -> Transfer Efficiency" );
    setWeight( 1 );   // need to be adjusted
    max_total_time       = cube->getMetric( "max_total_time" );
    max_total_time_ideal = cube->getMetric( "max_total_time_ideal" );
    if ( max_total_time_ideal == nullptr || max_total_time == nullptr  )
    {
        adjustForTest( cube );
    }
    max_total_time       = cube->getMetric( "max_total_time" );
    max_total_time_ideal = cube->getMetric( "max_total_time_ideal" );
    if ( max_total_time_ideal == nullptr || max_total_time == nullptr )
    {
        setWeight( 0.1 );
        setValue( 0. );
        return;
    }
    cube::list_of_sysresources lsysres = getRootsOfSystemTree();
    cube::metric_pair          metric;
    metric.first  = max_total_time;
    metric.second = cube::CUBE_CALCULATE_INCLUSIVE;
    lmetrics.push_back( metric );

    metric.first  = max_total_time_ideal;
    metric.second = cube::CUBE_CALCULATE_INCLUSIVE;
    lmax_total_time_ideal.push_back( metric );
}



void
JSCTransferTest::applyCnode( const cube::Cnode*, const cube::CalculationFlavour,
                             const bool )
{
    if ( max_total_time_ideal == nullptr || max_total_time == nullptr )
    {
        return;
    }
}

void
JSCTransferTest::applyCnode( const cube::list_of_cnodes& cnodes,
                             const bool                  direct_calculation  )
{
    ( void )direct_calculation; // not used here
    if ( max_total_time_ideal == nullptr || max_total_time == nullptr )
    {
        return;
    }
    setValue( analyze( cnodes ) );
}

double
JSCTransferTest::analyze( const cube::list_of_cnodes& cnodes,
                          cube::LocationGroup*          ) const
{
    cube::value_container inclusive_values1;
    cube::value_container exclusive_values1;
    cube->getSystemTreeValues( lmetrics,
                               cnodes,
                               inclusive_values1,
                               exclusive_values1 );

    cube::value_container inclusive_values2;
    cube::value_container exclusive_values2;
    cube->getSystemTreeValues( lmax_total_time_ideal,
                               cnodes,
                               inclusive_values2,
                               exclusive_values2 );

    const std::vector<cube::LocationGroup*>& _lgs                       = cube->getLocationGroups();
    double                                   max_total_time_value       = std::numeric_limits<double>::lowest();
    double                                   max_total_time_ideal_value = std::numeric_limits<double>::lowest();

    for ( std::vector<cube::LocationGroup*>::const_iterator iter = _lgs.begin(); iter != _lgs.end(); ++iter )
    {
        double _v1 =  inclusive_values1[ ( *iter )->get_sys_id() ]->getDouble();
        double _v2 =  inclusive_values2[ ( *iter )->get_sys_id() ]->getDouble();
        max_total_time_value       = std::max( max_total_time_value, _v1 );
        max_total_time_ideal_value = std::max( max_total_time_ideal_value, _v2 );
    }
    return max_total_time_ideal_value / max_total_time_value;
}


const std::string&
JSCTransferTest::getCommentText() const
{
    if ( max_total_time_ideal == nullptr || max_total_time == nullptr  )
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
JSCTransferTest::isActive() const
{
    return ( max_total_time_ideal != nullptr ) && ( max_total_time != nullptr );
};

bool
JSCTransferTest::isIssue() const
{
    return false;
};

void
JSCTransferTest::adjustForTest( cube::CubeProxy* cube ) const
{
    cube::Metric* _mpi_time = cube->getMetric( "mpi" );
    if ( _mpi_time->isInactive() )
    {
        return;
    }
    if ( scout_metrics_available( cube ) )
    {
        add_max_total_time_ideal( cube );
        add_max_total_time( cube );
    }
}


void
JSCTransferTest::add_max_total_time_ideal( cube::CubeProxy* ) const
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
            POP_TRANSFER_EFF_METRIC_URL,
            tr( "Maximal total time in ideal network, ( execution - transfer_time_mpi )" ).toUtf8().data(),
            nullptr,
            cube::CUBE_METRIC_PREDERIVED_INCLUSIVE,
            "metric::execution() - metric::transfer_time_mpi()",
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
JSCTransferTest::add_max_total_time( cube::CubeProxy* ) const
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
            POP_TRANSFER_EFF_METRIC_URL,
            tr( "Maximal total time, max( execution )" ).toUtf8().data(),
            nullptr,
            cube::CUBE_METRIC_PREDERIVED_INCLUSIVE,
            "metric::execution()",
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
