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

#include "BSPOPHybridMPISerialisationTest.h"


using namespace advisor;

BSPOPHybridMPISerialisationTest::BSPOPHybridMPISerialisationTest( cube::CubeProxy* cube ) : PerformanceTest( cube )
{
    setName( tr( " * * * MPI Serialisation Efficiency" ).toUtf8().data() );
    setWeight( 1 );   // need to be adjusted
    max_omp_serial_comp_time = cube->getMetric( "max_omp_serial_comp_time" );
    max_total_time_ideal     = cube->getMetric( "max_total_time_ideal_hyb" );
    if ( max_omp_serial_comp_time == nullptr || max_total_time_ideal == nullptr )
    {
        adjustForTest( cube );
    }
    max_omp_serial_comp_time = cube->getMetric( "max_omp_serial_comp_time" );
    max_total_time_ideal     = cube->getMetric( "max_total_time_ideal_hyb" );
    if ( max_omp_serial_comp_time == nullptr || max_total_time_ideal == nullptr )
    {
        setWeight( 0.2 );
        setValue( 0. );
        return;
    }
    cube::metric_pair metric;
    metric.first  = max_total_time_ideal;
    metric.second = cube::CUBE_CALCULATE_INCLUSIVE;
    lmetrics.push_back( metric );

    metric.first  = max_omp_serial_comp_time;
    metric.second = cube::CUBE_CALCULATE_INCLUSIVE;
    lmax_omp_serial_comp_time.push_back( metric );
}



void
BSPOPHybridMPISerialisationTest::applyCnode( const cube::Cnode*, const cube::CalculationFlavour,
                                             const bool )
{
}

void
BSPOPHybridMPISerialisationTest::applyCnode( const cube::list_of_cnodes& cnodes,
                                             const bool                  direct_calculation  )
{
    ( void )direct_calculation; // not used here
    setValue(  analyze( cnodes ) );
}


double
BSPOPHybridMPISerialisationTest::analyze( const cube::list_of_cnodes& cnodes,
                                          cube::LocationGroup* ) const
{
    if ( max_omp_serial_comp_time == nullptr || max_total_time_ideal == nullptr )
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
    cube->getSystemTreeValues( lmax_omp_serial_comp_time,
                               cnodes,
                               inclusive_values2,
                               exclusive_values2 );

    double max_total_time_ideal  = inclusive_values1[ 0 ]->getDouble();
    double avg_max_omp_ser_value = inclusive_values2[ 0 ]->getDouble();
    return ( avg_max_omp_ser_value ) / ( max_total_time_ideal );
}





const std::string&
BSPOPHybridMPISerialisationTest::getCommentText() const
{
    if ( max_omp_serial_comp_time == nullptr || max_total_time_ideal == nullptr )
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
BSPOPHybridMPISerialisationTest::isActive() const
{
    return ( max_omp_serial_comp_time != nullptr ) && ( max_total_time_ideal != nullptr );
};

bool
BSPOPHybridMPISerialisationTest::isIssue() const
{
    return false;
};

void
BSPOPHybridMPISerialisationTest::adjustForTest( cube::CubeProxy* cube ) const
{
    if ( scout_metrics_available( cube ) )
    {
        add_max_omp_and_ser_execution( cube );
        add_max_total_time_ideal_hyb( cube );
    }
}




void
BSPOPHybridMPISerialisationTest::add_max_total_time_ideal_hyb( cube::CubeProxy* ) const
{
    add_execution_time( cube );
    add_transfer_time_mpi( cube );
    cube::Metric* _transfer_time_mpi = cube->getMetric( "transfer_time_mpi" );
    if ( _transfer_time_mpi == nullptr )
    {
        return;
    }

    cube::Metric* _met = cube->getMetric( "max_total_time_ideal_hyb" );
    if ( _met == nullptr )
    {
        _met = cube->defineMetric(
            tr( "Maximal total time in ideal network, hybrid" ).toUtf8().data(),
            "max_total_time_ideal_hyb",
            "DOUBLE",
            tr( "sec" ).toUtf8().data(),
            "",
            POP_TRANSFER_EFF_METRIC_URL,
            tr( "Maximal total time in ideal network, ( execution - transfer_time_mpi )" ).toUtf8().data(),
            nullptr,
            cube::CUBE_METRIC_POSTDERIVED,
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
BSPOPHybridMPISerialisationTest::add_transfer_time_mpi( cube::CubeProxy* ) const
{
    add_mpi_time( cube );
    add_mpi_io_time( cube );
    add_wait_time_mpi( cube );

    cube::Metric* _met = cube->getMetric( "transfer_time_mpi" );
    if ( _met == nullptr )
    {
        _met = cube->defineMetric(
            tr( "Transfer time in MPI" ).toUtf8().data(),
            "transfer_time_mpi",
            "DOUBLE",
            tr( "sec" ).toUtf8().data(),
            "",
            BSPOP_SER_EFF_METRIC_URL,
            tr( "Transfer time in MPI, (mpi - wait_time_mpi - mpi_io)" ).toUtf8().data(),
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
