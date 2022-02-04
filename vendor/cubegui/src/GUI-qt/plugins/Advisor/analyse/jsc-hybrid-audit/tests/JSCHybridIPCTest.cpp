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

#include "JSCHybridIPCTest.h"


using namespace advisor;

JSCIPCTest::JSCIPCTest( cube::CubeProxy* cube ) : PerformanceTest( cube )
{
    setName( "IPC" );
    setWeight( 1 );   // need to be adjusted
    pop_ipc = cube->getMetric( "ipc" );
    if ( pop_ipc == nullptr )
    {
        adjustForTest( cube );
    }
    pop_ipc  = cube->getMetric( "ipc" );
    maxValue = 1.;
    if ( pop_ipc == nullptr )
    {
        setWeight( 0.1 );
        setValue( 0. );
        return;
    }
    cube::list_of_sysresources lsysres = getRootsOfSystemTree();
    cube::metric_pair          metric;
    metric.first  = pop_ipc;
    metric.second = cube::CUBE_CALCULATE_INCLUSIVE;
    lmetrics.push_back( metric );
}


void
JSCIPCTest::applyCnode( const cube::Cnode* cnode, const cube::CalculationFlavour cnf,
                        const bool )
{
    if ( pop_ipc == nullptr )
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
    double pop_ipc_value = v->getDouble();
    delete v;
    setValue( pop_ipc_value );
}

void
JSCIPCTest::applyCnode( const cube::list_of_cnodes& lcnodes,
                        const bool                   )
{
    if ( pop_ipc == nullptr )
    {
        return;
    }
    cube::list_of_sysresources lsysres;
    cube::Value*               v = cube->calculateValue( lmetrics,
                                                         lcnodes,
                                                         lsysres );
    double pop_ipc_value = v->getDouble();
    delete v;
    setValue( pop_ipc_value );
}

const std::string&
JSCIPCTest::getCommentText() const
{
    if ( pop_ipc == nullptr )
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
JSCIPCTest::isActive() const
{
    return pop_ipc != nullptr;
};

bool
JSCIPCTest::isIssue() const
{
    return false;
};


void
JSCIPCTest::adjustForTest( cube::CubeProxy* cube ) const
{
    add_tot_ins_without_wait( cube );
    add_tot_cyc_without_wait( cube );
    add_ipc( cube );
}



void
JSCIPCTest::add_ipc( cube::CubeProxy* cube ) const
{
    cube::Metric* _met = cube->getMetric( "ipc" );
    if ( _met == nullptr )
    {
        if ( cube->getMetric( "tot_ins_without_wait" ) != nullptr  && cube->getMetric( "tot_cyc_without_wait" ) != nullptr )
        {
            _met = cube->defineMetric(
                "IPC",
                "ipc",
                "DOUBLE",
                "",
                "",
                JSC_IPC_METRIC_URL,
                "Value of IPC (instructions per cycle) without busy-wait in MPI and OpenMP, computed as tot_ins_without_wait() / tot_cyc_without_wait()",
                nullptr,
                cube::CUBE_METRIC_POSTDERIVED,
                "metric::tot_ins_without_wait() / metric::tot_cyc_without_wait()",
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
JSCIPCTest::add_tot_ins_without_wait( cube::CubeProxy* cube ) const
{
    cube::Metric* PAPI_TOT_INS = cube->getMetric( "PAPI_TOT_INS" );
    if ( PAPI_TOT_INS == nullptr )
    {
        return;
    }
    cube::Metric* _met = cube->getMetric( "tot_ins_without_wait" );
    if ( _met == nullptr )
    {
        _met = cube->defineMetric(
            "PAPI_TOT_INS without busy-wait",
            "tot_ins_without_wait",
            "DOUBLE",
            "",
            "",
            JSC_IPC_METRIC_URL,
            "Here is PAPI_TOT_INS without busy-wait in MPI and OpenMP.",
            nullptr,
            cube::CUBE_METRIC_PREDERIVED_EXCLUSIVE,
            "${without_wait_state}[${calculation::callpath::id}] * metric::PAPI_TOT_INS()",
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
JSCIPCTest::add_tot_cyc_without_wait( cube::CubeProxy* cube )  const
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
