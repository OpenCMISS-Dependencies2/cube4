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

#include "JSCHybridStalledResourcesTest.h"


using namespace advisor;

JSCStalledResourcesTest::JSCStalledResourcesTest( cube::CubeProxy* cube ) : PerformanceTest( cube )
{
    setName( "Resource stall cycles" );
    setWeight( 1 );   // need to be adjusted
    pop_stalled_resources = cube->getMetric( "stalled_resources" );
    if ( pop_stalled_resources == nullptr )
    {
        adjustForTest( cube );
    }
    pop_stalled_resources = cube->getMetric( "stalled_resources" );
    if ( pop_stalled_resources == nullptr )
    {
        setWeight( 0.1 );
        setValue( 0. );
        return;
    }

    cube::list_of_sysresources lsysres = getRootsOfSystemTree();
    cube::metric_pair          metric;
    metric.first  = pop_stalled_resources;
    metric.second = cube::CUBE_CALCULATE_INCLUSIVE;
    lmetrics.push_back( metric );
}



void
JSCStalledResourcesTest::applyCnode( const cube::Cnode* cnode, const cube::CalculationFlavour cnf,
                                     const bool direct_calculation  )
{
    ( void )direct_calculation; // not used here
    if ( pop_stalled_resources == nullptr )
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
    double pop_stalled_resources_value = v->getDouble();
    delete v;
    setValue( pop_stalled_resources_value  );
}

void
JSCStalledResourcesTest::applyCnode( const cube::list_of_cnodes& cnodes,
                                     const bool                  direct_calculation  )
{
    ( void )direct_calculation; // not used here
    setValue( analyze( cnodes ) );
}

double
JSCStalledResourcesTest::analyze( const cube::list_of_cnodes& lcnodes,
                                  cube::LocationGroup*          ) const
{
    if ( pop_stalled_resources == nullptr )
    {
        return 0.;
    }
    cube::list_of_sysresources lsysres;
    cube::Value*               v = cube->calculateValue( lmetrics,
                                                         lcnodes,
                                                         lsysres );
    double pop_stalled_resources_value = v->getDouble();
    delete v;
    return pop_stalled_resources_value;
}



const std::string&
JSCStalledResourcesTest::getCommentText() const
{
    if ( pop_stalled_resources == nullptr )
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
JSCStalledResourcesTest::isActive() const
{
    return pop_stalled_resources != nullptr;
};

bool
JSCStalledResourcesTest::isIssue() const
{
    return false;
};


void
JSCStalledResourcesTest::adjustForTest( cube::CubeProxy* cube ) const
{
    add_res_stl_without_wait( cube );
    add_tot_cyc_without_wait( cube );
    add_stalled_resources( cube );
}



void
JSCStalledResourcesTest::add_stalled_resources( cube::CubeProxy* cube ) const
{
    cube::Metric* _met = cube->getMetric( "stalled_resources" );
    if ( _met == nullptr )
    {
        if ( cube->getMetric( "res_stl_without_wait" ) != nullptr && cube->getMetric( "tot_cyc_without_wait" ) )
        {
            _met = cube->defineMetric(
                "Stalled resources",
                "stalled_resources",
                "DOUBLE",
                "",
                "",
                JSC_STALLED_METRIC_URL,
                "Amount of stalled resources, computed as res_stl_without_wait() / tot_cyc_without_wait()",
                nullptr,
                cube::CUBE_METRIC_POSTDERIVED,
                " metric::res_stl_without_wait() / metric::tot_cyc_without_wait()",
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
}



void
JSCStalledResourcesTest::add_res_stl_without_wait( cube::CubeProxy* cube ) const
{
    cube::Metric* PAPI_TOT_INS = cube->getMetric( "PAPI_RES_STL" );
    if ( PAPI_TOT_INS == nullptr )
    {
        return;
    }
    cube::Metric* _met = cube->getMetric( "res_stl_without_wait" );
    if ( _met == nullptr )
    {
        _met = cube->defineMetric(
            "PAPI_RES_STL without busy-wait, res_stl_without_wait",
            "res_stl_without_wait",
            "DOUBLE",
            "",
            "",
            JSC_IPC_METRIC_URL,
            "Here is PAPI_RES_STL without busy-wait in MPI and OpenMP.",
            nullptr,
            cube::CUBE_METRIC_PREDERIVED_EXCLUSIVE,
            "${without_wait_state}[${calculation::callpath::id}] * metric::PAPI_RES_STL()",
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
JSCStalledResourcesTest::add_tot_cyc_without_wait( cube::CubeProxy* cube )  const
{
    cube::Metric* PAPI_TOT_CYC = cube->getMetric( "PAPI_TOT_CYC" );
    if ( PAPI_TOT_CYC == nullptr )
    {
        return;
    }
    cube::Metric* _met = cube->getMetric( "tot_cyc_without_wait" );
    if ( _met == nullptr )
    {
        _met = cube->defineMetric(
            "PAPI_TOT_CYC without busy-wait",
            "tot_cyc_without_wait",
            "DOUBLE",
            "",
            "",
            JSC_IPC_METRIC_URL,
            "Here is PAPI_TOT_CYC without busy-wait in MPI and OpenMP.",
            nullptr,
            cube::CUBE_METRIC_PREDERIVED_EXCLUSIVE,
            "${without_wait_state}[${calculation::callpath::id}] * metric::PAPI_TOT_CYC()",
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
