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

#include <limits>
#include "JSCHybridOMPTransferTest.h"


using namespace advisor;

JSCOmpTransferTest::JSCOmpTransferTest( cube::CubeProxy* cube ) : PerformanceTest( cube )
{
    setName( "OpenMP Management Efficiency" );
    setWeight( 1 );   // need to be adjusted
    omp_pop_transfer = cube->getMetric( "jsc_omp_transf_eff" );
    if ( omp_pop_transfer == nullptr )
    {
        adjustForTest( cube );
    }
    omp_pop_transfer = cube->getMetric( "jsc_omp_transf_eff" );
    if ( omp_pop_transfer == nullptr )
    {
        setWeight( 0.1 );
        setValue( 0. );
        return;
    }
    cube::list_of_sysresources lsysres = getRootsOfSystemTree();
    cube::metric_pair          metric;
    metric.first  = omp_pop_transfer;
    metric.second = cube::CUBE_CALCULATE_INCLUSIVE;
    lmetrics.push_back( metric );
}



void
JSCOmpTransferTest::applyCnode( const cube::Cnode* cnode, const cube::CalculationFlavour cnf,
                                const bool  )
{
    if ( omp_pop_transfer == nullptr )
    {
        return;
    }
    cube::value_container inclusive_values;
    cube::value_container exclusive_values;
    cube::list_of_cnodes  lcnodes;

    cube::cnode_pair pcnode = std::make_pair( const_cast<cube::Cnode*>( cnode ), cnf );
    lcnodes.push_back( pcnode );

    cube->getSystemTreeValues( lmetrics,
                               lcnodes,
                               inclusive_values,
                               exclusive_values );

    const std::vector<cube::LocationGroup*>& _lgs                 = cube->getLocationGroups();
    double                                   pop_omp_transfer_sum = 0.;
    double                                   pop_omp_transfer_min = std::numeric_limits<double>::max();
    double                                   pop_omp_transfer_max = std::numeric_limits<double>::lowest();
    for ( std::vector<cube::LocationGroup*>::const_iterator iter = _lgs.begin(); iter != _lgs.end(); ++iter )
    {
        double _v = ( cnf == cube::CUBE_CALCULATE_INCLUSIVE ) ? inclusive_values[ ( *iter )->get_sys_id() ]->getDouble() : exclusive_values[ ( *iter )->get_sys_id() ]->getDouble();
        pop_omp_transfer_sum += _v;

        pop_omp_transfer_min = std::min( pop_omp_transfer_min, _v );
        pop_omp_transfer_max = std::max( pop_omp_transfer_max, _v );
    }
    double pop_omp_transfer_value = pop_omp_transfer_sum / _lgs.size();
    setValues( pop_omp_transfer_value, pop_omp_transfer_min, pop_omp_transfer_max );
}

void
JSCOmpTransferTest::applyCnode( const cube::list_of_cnodes& cnodes,
                                const bool                  direct_calculation  )
{
    ( void )direct_calculation; // not used here
    if ( omp_pop_transfer == nullptr )
    {
        return;
    }
    cube::value_container inclusive_values;
    cube::value_container exclusive_values;
    cube->getSystemTreeValues( lmetrics,
                               cnodes,
                               inclusive_values,
                               exclusive_values );

    const std::vector<cube::LocationGroup*>& _lgs                 = cube->getLocationGroups();
    double                                   pop_omp_transfer_sum = 0.;
    double                                   pop_omp_transfer_min = std::numeric_limits<double>::max();
    double                                   pop_omp_transfer_max = std::numeric_limits<double>::lowest();
    for ( std::vector<cube::LocationGroup*>::const_iterator iter = _lgs.begin(); iter != _lgs.end(); ++iter )
    {
        double _v = inclusive_values[ ( *iter )->get_sys_id() ]->getDouble();
        pop_omp_transfer_sum += _v;
        pop_omp_transfer_min  = std::min( pop_omp_transfer_min, _v );
        pop_omp_transfer_max  = std::max( pop_omp_transfer_max, _v );
    }
    double pop_omp_transfer_value = pop_omp_transfer_sum / _lgs.size();
    setValues( pop_omp_transfer_value, pop_omp_transfer_min, pop_omp_transfer_max );
}

double
JSCOmpTransferTest::analyze( const cube::list_of_cnodes& cnodes,
                             cube::LocationGroup*        _lg  ) const
{
    if ( omp_pop_transfer == nullptr )
    {
        return 0.;
    }
    cube::value_container inclusive_values;
    cube::value_container exclusive_values;
    cube->getSystemTreeValues( lmetrics,
                               cnodes,
                               inclusive_values,
                               exclusive_values );
    return inclusive_values[ _lg->get_sys_id() ]->getDouble();
}



const std::string&
JSCOmpTransferTest::getCommentText() const
{
    if ( omp_pop_transfer == nullptr )
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
JSCOmpTransferTest::isActive() const
{
    return omp_pop_transfer != nullptr;
};

bool
JSCOmpTransferTest::isIssue() const
{
    return false;
};

void
JSCOmpTransferTest::adjustForTest( cube::CubeProxy* cube ) const
{
    cube::Metric* _omp_time = cube->getMetric( "omp_time" );
    if ( _omp_time->isInactive() )
    {
        return;
    }

    if ( cube->getMetric( "omp_management" ) == nullptr )
    {
        return;
    }
    add_omp_max_total_time_ideal( cube );
    add_omp_max_total_time( cube );
    add_omp_transfer_eff( cube );
}


void
JSCOmpTransferTest::add_omp_transfer_eff( cube::CubeProxy* ) const
{
    cube::Metric* _met = cube->getMetric( "jsc_omp_transf_eff" );
    if ( _met == nullptr )
    {
        _met = cube->defineMetric(
            "JSC Omp Transfer efficiency",
            "jsc_omp_transf_eff",
            "DOUBLE",
            "",
            "",
            JSC_TRANSFER_EFF_METRIC_URL,
            "Transfer efficiency reflects the loss of efficiency due to actual data transfer, i.e. ( max_total_time_ideal / max_total_time ). The value varies from 0 to 100%, where 100% shows that routine is a pure computation routine and if MPI does not include waiting time. Do not consider values in the flat view and the system tree.",
            nullptr,
            cube::CUBE_METRIC_POSTDERIVED,
            "metric::jsc_max_omp_total_time_ideal() / metric::jsc_max_omp_total_time()",
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
JSCOmpTransferTest::add_omp_max_total_time_ideal( cube::CubeProxy* ) const
{
    add_omp_comp_time( cube );
    add_omp_max_total_time( cube );
    cube::Metric* _met = cube->getMetric( "max_omp_total_time_ideal" );
    if ( _met == nullptr )
    {
        _met = cube->defineMetric(
            "JSC Maximal ideal omp time ",
            "jsc_max_omp_total_time_ideal",
            "DOUBLE",
            "sec",
            "",
            JSC_TRANSFER_EFF_METRIC_URL,
            "Maximal total time in ideal network, ( omp computation + omp time - omp management )",
            nullptr,
            cube::CUBE_METRIC_PREDERIVED_INCLUSIVE,
            "metric::jsc_max_omp_total_time() - metric::omp_management()",
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
JSCOmpTransferTest::add_omp_max_total_time( cube::CubeProxy* ) const
{
    add_omp_comp_time( cube );

    cube::Metric* _met = cube->getMetric( "jsc_max_omp_total_time" );
    if ( _met == nullptr )
    {
        _met = cube->defineMetric(
            "JSC Maximal omp total time",
            "jsc_max_omp_total_time",
            "DOUBLE",
            "sec",
            "",
            JSC_TRANSFER_EFF_METRIC_URL,
            "Maximal total time, max( execution )",
            nullptr,
            cube::CUBE_METRIC_PREDERIVED_INCLUSIVE,
            "metric::omp_non_wait_time() + metric::omp_time()",
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
