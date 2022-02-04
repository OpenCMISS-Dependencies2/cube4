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

#include "JSCHybridSerialisationTest.h"


using namespace advisor;

JSCSerialisationTest::JSCSerialisationTest( cube::CubeProxy* cube ) : PerformanceTest( cube )
{
    setName( " -> Serialisation Efficiency" );
    setWeight( 1 );   // need to be adjusted
    comp_time            = cube->getMetric( "mpi_comp" );
    max_total_time_ideal = cube->getMetric( "max_total_time_ideal" );
    if ( max_total_time_ideal == nullptr || comp_time == nullptr )
    {
        adjustForTest( cube );
    }
    comp_time            = cube->getMetric( "mpi_comp" );
    max_total_time_ideal = cube->getMetric( "max_total_time_ideal" );

    if ( max_total_time_ideal == nullptr || comp_time == nullptr )
    {
        setWeight( 0.1 );
        setValue( 0. );
        return;
    }
    cube::list_of_sysresources lsysres = getRootsOfSystemTree();
    cube::metric_pair          metric;
    metric.first  = comp_time;
    metric.second = cube::CUBE_CALCULATE_INCLUSIVE;
    lmetrics.push_back( metric );


    metric.first  = max_total_time_ideal;
    metric.second = cube::CUBE_CALCULATE_INCLUSIVE;
    lmax_total_time_ideal.push_back( metric );
}



void
JSCSerialisationTest::applyCnode( const cube::Cnode*, const cube::CalculationFlavour,
                                  const bool )
{
    if ( max_total_time_ideal == nullptr || comp_time == nullptr )
    {
        return;
    }
}

void
JSCSerialisationTest::applyCnode( const cube::list_of_cnodes& cnodes,
                                  const bool                  direct_calculation  )
{
    ( void )direct_calculation; // not used here
    if ( max_total_time_ideal == nullptr || comp_time == nullptr  )
    {
        return;
    }
    setValue( analyze( cnodes ) );
}

double
JSCSerialisationTest::analyze( const cube::list_of_cnodes& cnodes,
                               cube::LocationGroup*         ) const
{
    if ( max_total_time_ideal == nullptr || comp_time == nullptr  )
    {
        return 0.;
    }
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
    double                                   max_comp_time_value        = std::numeric_limits<double>::lowest();
    double                                   max_total_time_ideal_value = std::numeric_limits<double>::lowest();

    for ( std::vector<cube::LocationGroup*>::const_iterator iter = _lgs.begin(); iter != _lgs.end(); ++iter )
    {
        double _v1 =  inclusive_values1[ ( *iter )->get_sys_id() ]->getDouble();
        double _v2 =  inclusive_values2[ ( *iter )->get_sys_id() ]->getDouble();
        max_comp_time_value        = std::max( max_comp_time_value, _v1 );
        max_total_time_ideal_value = std::max( max_total_time_ideal_value, _v2 );
    }
    return max_comp_time_value / max_total_time_ideal_value;
}




const std::string&
JSCSerialisationTest::getCommentText() const
{
    if ( max_total_time_ideal == nullptr || comp_time == nullptr  )
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
JSCSerialisationTest::isActive() const
{
    return ( max_total_time_ideal != nullptr ) && ( comp_time != nullptr );
};

bool
JSCSerialisationTest::isIssue() const
{
    return false;
};

void
JSCSerialisationTest::adjustForTest( cube::CubeProxy* cube ) const
{
    if ( scout_metrics_available( cube ) )
    {
        add_mpi_comp_time( cube );
        add_transfer_time_mpi( cube );
        add_max_total_time_ideal( cube );
    }
}



void
JSCSerialisationTest::add_transfer_time_mpi( cube::CubeProxy* ) const
{
    add_mpi_time( cube );
    add_mpi_io_time( cube );
    add_wait_time_mpi( cube );

    cube::Metric* _met = cube->getMetric( "jsc_transfer_time_mpi" );
    if ( _met == nullptr )
    {
        _met = cube->defineMetric(
            "JSC Transfer time in MPI",
            "jsc_transfer_time_mpi",
            "DOUBLE",
            "sec",
            "",
            JSC_SER_EFF_METRIC_URL,
            "Transfer time in MPI, (mpi - wait_time_mpi - mpi_io)",
            nullptr,
            cube::CUBE_METRIC_PREDERIVED_EXCLUSIVE,
            "metric::mpi() - metric::wait_time_mpi() - metric::mpi_io()",
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
JSCSerialisationTest::add_max_total_time_ideal( cube::CubeProxy* ) const
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
