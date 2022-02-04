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

#include "L2CompToDataTest.h"

using namespace advisor;

L2Comp2DataTest::L2Comp2DataTest( cube::CubeProxy* cube ) : PerformanceTest( cube )
{
    setName( tr( "L2 Computation to Data Access" ).toUtf8().data() );
    setWeight( 1 );   // need to be adjusted
    l2_comp2data = cube->getMetric( "l2_compute_to_data_access_ratio" );
    if ( l2_comp2data == NULL )
    {
        adjustForTest( cube );
    }
    l2_comp2data = cube->getMetric( "l2_compute_to_data_access_ratio" );
    if ( l2_comp2data == NULL )
    {
        setWeight( 0.2 );
        setValue( 0. );
        return;
    }
    cube::metric_pair metric;
    metric.first  = l2_comp2data;
    metric.second = cube::CUBE_CALCULATE_INCLUSIVE;
    lmetrics.push_back( metric );
}

void
L2Comp2DataTest::applyCnode( const cube::Cnode* cnode, const cube::CalculationFlavour cnf,
                             const bool direct_calculation  )
{
    ( void )direct_calculation; // not used here
    if ( l2_comp2data == NULL )
    {
        return;
    }

    cube::list_of_sysresources lsysres;
    cube::list_of_cnodes       lcnodes;

    cube::cnode_pair pcnode = std::make_pair( const_cast<cube::Cnode*>( cnode ), cnf );
    lcnodes.push_back( pcnode );

    cube::Value* v = cube->calculateValue( lmetrics,
                                           lcnodes,
                                           lsysres );
    double l2_comp2data_value = v->getDouble();
    delete v;
    setValue( l2_comp2data_value );
}

void
L2Comp2DataTest::applyCnode( const cube::list_of_cnodes& lcnodes,
                             const bool                  direct_calculation  )
{
    ( void )direct_calculation; // not used here
    if ( l2_comp2data == NULL )
    {
        return;
    }
    if ( lcnodes.size() == 1 )
    {
        applyCnode( lcnodes[ 0 ].first, lcnodes[ 0 ].second );
    }
    else
    {
        cube::list_of_sysresources lsysres;
        cube::Value*               v = cube->calculateValue( lmetrics,
                                                             lcnodes,
                                                             lsysres );
        double l2_comp2data_value = v->getDouble();
        delete v;
        setValue( l2_comp2data_value );
    }
}

const std::string&
L2Comp2DataTest::getCommentText() const
{
    if ( l2_comp2data == NULL )
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
L2Comp2DataTest::isActive() const
{
    return l2_comp2data != NULL;
};

bool
L2Comp2DataTest::isIssue() const
{
    return false;
};



void
L2Comp2DataTest::adjustForTest( cube::CubeProxy* cube ) const
{
    cube::Metric* uops_packed_simd_without_wait   = cube->getMetric( "uops_packed_simd_without_wait" );
    cube::Metric* uops_l1_miss_loads_without_wait = cube->getMetric( "uops_l1_miss_loads_without_wait" );

    cube::Metric* uops_packed_simd_loops_without_wait   = cube->getMetric( "uops_packed_simd_loops_without_wait" );
    cube::Metric* uops_l1_miss_loads_loops_without_wait = cube->getMetric( "uops_l1_miss_loads_loops_without_wait" );

    if ( uops_l1_miss_loads_without_wait == NULL )
    {
        add_uops_l1_miss_loads_without_wait( cube );
    }
    if ( uops_l1_miss_loads_loops_without_wait == NULL )
    {
        add_uops_l1_miss_loads_loops_without_wait( cube );
    }

    uops_l1_miss_loads_without_wait       = cube->getMetric( "uops_l1_miss_loads_without_wait" );
    uops_l1_miss_loads_loops_without_wait = cube->getMetric( "uops_l1_miss_loads_loops_without_wait" );


    if ( uops_packed_simd_without_wait != NULL && uops_l1_miss_loads_without_wait != NULL )
    {
        add_l2_compute_to_data_access_ratio_all( cube );
    }
    if ( uops_packed_simd_loops_without_wait != NULL && uops_l1_miss_loads_loops_without_wait != NULL )
    {
        add_l2_compute_to_data_access_ratio( cube );
    }
}





void
L2Comp2DataTest::add_l2_compute_to_data_access_ratio_all( cube::CubeProxy* ) const
{
    cube::Metric* _met = cube->getMetric( "l2_compute_to_data_access_ratio_all" );
    if ( _met == NULL )
    {
        cube::Metric* _met = cube->defineMetric(
            tr( "L2 compute-to-data-access ratio all callpaths" ).toUtf8().data(),
            "l2_compute_to_data_access_ratio_all",
            "DOUBLE",
            "",
            "",
            KNL_L1_COMPUTE_TO_DATA_ACCESS_RATIO_URL,
            tr( "L2 compute to data access ratio." ).toUtf8().data(),
            NULL,
            cube::CUBE_METRIC_POSTDERIVED,
            "metric::uops_packed_simd_without_wait() / metric::uops_l1_miss_loads_without_wait() ",
            "",
            "",
            "",
            "",
            true,
//             cube::CUBE_METRIC_NORMAL
            cube::CUBE_METRIC_GHOST
            );
        if ( _met != NULL )
        {
            _met->setConvertible( false );
        }
//         advisor_services->addMetric( _met, NULL );
    }
}


void
L2Comp2DataTest::add_l2_compute_to_data_access_ratio( cube::CubeProxy* ) const
{
    cube::Metric* _met = cube->getMetric( "l2_compute_to_data_access_ratio" );
    if ( _met == NULL )
    {
        cube::Metric* _met = cube->defineMetric(
            tr( "L2 compute-to-data-access rati" ).toUtf8().data(),
            "l2_compute_to_data_access_ratio",
            "DOUBLE",
            "",
            "",
            KNL_L1_COMPUTE_TO_DATA_ACCESS_RATIO_URL,
            tr( "L2 compute to data access ratio." ).toUtf8().data(),
            NULL,
            cube::CUBE_METRIC_POSTDERIVED,
            "metric::uops_packed_simd_loops_without_wait() / metric::uops_l1_miss_loads_loops_without_wait()",
            "",
            "",
            "",
            "",
            true,
            cube::CUBE_METRIC_GHOST
            );
        if ( _met != NULL )
        {
            _met->setConvertible( false );
        }
        advisor_services->addMetric( _met, NULL );
    }
}


void
L2Comp2DataTest::add_uops_l1_miss_loads_without_wait( cube::CubeProxy* ) const
{
    cube::Metric* _met = cube->getMetric( "MEM_UOPS_RETIRED:L1_MISS_LOADS" );
    if ( _met != NULL )
    {
        cube::Metric* _met = cube->defineMetric(
            "MEM_UOPS_RETIRED:L1_MISS_LOADS_WITHOUT_WAIT",
            "uops_l1_miss_loads_without_wait",
            "DOUBLE",
            "",
            "",
            KNL_UOPS_RETIRED_SCALAR_SMD_URL,
            tr( "Here is MEM_UOPS_RETIRED:L1_MISS_LOADS  without busy-wait in MPI and OpenMP." ).toUtf8().data(),
            NULL,
            cube::CUBE_METRIC_PREDERIVED_EXCLUSIVE,
            "${without_wait_state}[${calculation::callpath::id}]  * metric::MEM_UOPS_RETIRED:L1_MISS_LOADS()",
            "",
            "",
            "",
            "",
            true,
            cube::CUBE_METRIC_GHOST
            );
        if ( _met != NULL )
        {
            _met->setConvertible( false );
        }
        advisor_services->addMetric( _met, NULL );
    }
}

void
L2Comp2DataTest::add_uops_l1_miss_loads_loops_without_wait( cube::CubeProxy* ) const
{
    cube::Metric* _met = cube->getMetric( "MEM_UOPS_RETIRED:L1_MISS_LOADS" );
    if ( _met != NULL )
    {
        cube::Metric* _met = cube->defineMetric(
            "MEM_UOPS_RETIRED:L1_MISS_LOADS_LOOPS_WITHOUT_WAIT",
            "uops_l1_miss_loads_loops_without_wait",
            "DOUBLE",
            "",
            "",
            KNL_UOPS_RETIRED_SCALAR_SMD_URL,
            tr( "Here is MEM_UOPS_RETIRED:L1_MISS_LOADS  without busy-wait in MPI and OpenMP." ).toUtf8().data(),
            NULL,
            cube::CUBE_METRIC_PREDERIVED_EXCLUSIVE,
            "${without_wait_state}[${calculation::callpath::id}] * ${loop}[${calculation::callpath::id}] *  metric::MEM_UOPS_RETIRED:L1_MISS_LOADS()",
            "",
            "",
            "",
            "",
            true,
            cube::CUBE_METRIC_GHOST
            );
        if ( _met != NULL )
        {
            _met->setConvertible( false );
        }
        advisor_services->addMetric( _met, NULL );
    }
}
