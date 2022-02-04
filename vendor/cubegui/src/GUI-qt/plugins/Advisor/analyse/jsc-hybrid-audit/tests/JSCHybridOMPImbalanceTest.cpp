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
#include "JSCHybridOMPImbalanceTest.h"


using namespace advisor;

JSCOmpImbalanceTest::JSCOmpImbalanceTest( cube::CubeProxy* cube ) : PerformanceTest( cube )
{
    setName( "OpenMP Load Balance" );
    setWeight( 1 );   // need to be adjusted
    pop_omp_imbalance = cube->getMetric( "jsc_lb_omp_eff" );
    if ( pop_omp_imbalance == nullptr )
    {
        adjustForTest( cube );
    }
    pop_omp_imbalance = cube->getMetric( "jsc_lb_omp_eff" );
    if ( pop_omp_imbalance == nullptr )
    {
        setWeight( 0.1 );
        setValue( 0. );
        return;
    }
    cube::list_of_sysresources lsysres = getRootsOfSystemTree();
    cube::metric_pair          metric;
    metric.first  = pop_omp_imbalance;
    metric.second = cube::CUBE_CALCULATE_INCLUSIVE;
    lmetrics.push_back( metric );
}



void
JSCOmpImbalanceTest::applyCnode( const cube::Cnode* cnode, const cube::CalculationFlavour cnf,
                                 const bool direct_calculation  )
{
    ( void )direct_calculation; // not used here
    if ( pop_omp_imbalance == nullptr )
    {
        return;
    }
    cube::value_container      inclusive_values;
    cube::value_container      exclusive_values;
    cube::list_of_sysresources lsysres;
    cube::list_of_cnodes       lcnodes;

    cube::cnode_pair pcnode = std::make_pair( const_cast<cube::Cnode*>( cnode ), cnf );
    lcnodes.push_back( pcnode );

    cube->getSystemTreeValues( lmetrics,
                               lcnodes,
                               inclusive_values,
                               exclusive_values );

    const std::vector<cube::LocationGroup*>& _lgs                  = cube->getLocationGroups();
    double                                   pop_omp_imbalance_sum = 0.;
    double                                   pop_omp_imbalance_min = std::numeric_limits<double>::max();
    double                                   pop_omp_imbalance_max = std::numeric_limits<double>::lowest();
    for ( std::vector<cube::LocationGroup*>::const_iterator iter = _lgs.begin(); iter != _lgs.end(); ++iter )
    {
        double _v =  ( cnf == cube::CUBE_CALCULATE_INCLUSIVE ) ? inclusive_values[ ( *iter )->get_sys_id() ]->getDouble() : exclusive_values[ ( *iter )->get_sys_id() ]->getDouble();
        pop_omp_imbalance_sum += _v;
        pop_omp_imbalance_min  = std::min( pop_omp_imbalance_min, _v );
        pop_omp_imbalance_max  = std::max( pop_omp_imbalance_max, _v );
    }
    double pop_imbalance_value = pop_omp_imbalance_sum / _lgs.size();
    setValues( pop_imbalance_value, pop_omp_imbalance_min, pop_omp_imbalance_max );
}

void
JSCOmpImbalanceTest::applyCnode( const cube::list_of_cnodes& cnodes,
                                 const bool                  direct_calculation  )
{
    ( void )direct_calculation; // not used here
    if ( pop_omp_imbalance == nullptr )
    {
        return;
    }
    cube::value_container inclusive_values;
    cube::value_container exclusive_values;
    cube->getSystemTreeValues( lmetrics,
                               cnodes,
                               inclusive_values,
                               exclusive_values );
    const std::vector<cube::LocationGroup*>& _lgs                  = cube->getLocationGroups();
    double                                   pop_omp_imbalance_sum = 0.;
    double                                   pop_omp_imbalance_min = std::numeric_limits<double>::max();
    double                                   pop_omp_imbalance_max = std::numeric_limits<double>::lowest();
    for ( std::vector<cube::LocationGroup*>::const_iterator iter = _lgs.begin(); iter != _lgs.end(); ++iter )
    {
        double _v =  inclusive_values[ ( *iter )->get_sys_id() ]->getDouble();
        pop_omp_imbalance_sum += _v;
        pop_omp_imbalance_min  = std::min( pop_omp_imbalance_min, _v );
        pop_omp_imbalance_max  = std::max( pop_omp_imbalance_max, _v );
    }
    double pop_imbalance_value = pop_omp_imbalance_sum / _lgs.size();
    setValues(  pop_imbalance_value, pop_omp_imbalance_min, pop_omp_imbalance_max );
}

double
JSCOmpImbalanceTest::analyze( const cube::list_of_cnodes& cnodes,
                              cube::LocationGroup*        _lg  ) const
{
    if ( pop_omp_imbalance == nullptr )
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
JSCOmpImbalanceTest::getCommentText() const
{
    if ( pop_omp_imbalance == nullptr )
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
JSCOmpImbalanceTest::isActive() const
{
    return pop_omp_imbalance != nullptr;
};

bool
JSCOmpImbalanceTest::isIssue() const
{
    return false;
};

void
JSCOmpImbalanceTest::adjustForTest( cube::CubeProxy* cube ) const
{
    cube::Metric* _omp_time = cube->getMetric( "omp_time" );
    if ( _omp_time->isInactive() )
    {
        return;
    }
    add_max_omp_comp_time( cube );
    add_avg_omp_comp_time( cube );
    add_omp_lb_eff( cube );
}


void
JSCOmpImbalanceTest::add_omp_lb_eff( cube::CubeProxy* ) const
{
    cube::Metric* _met = cube->getMetric( "jsc_lb_omp_eff" );
    if ( _met == nullptr )
    {
        _met = cube->defineMetric(
            "Omp Load balance efficiency",
            "jsc_lb_omp_eff",
            "DOUBLE",
            "",
            "",
            JSC_LB_EFF_METRIC_URL,
            "Load balance efficiency reflects how well the distribution of work to threads is done in the OpenMP regions of he application. It is computed as a ratio between the average time of a thread spend in OpenMP computation and the maximum time a thread spends in computation, i.e. ( avg_omp_comp_time / max_omp_comp_time ). The value varies from 0 to 100%, where 0 shows that some thread doesn't perform any computation. Do not consider values in the flat view and the system tree.",
            nullptr,
            cube::CUBE_METRIC_POSTDERIVED,
            "metric::jsc_avg_omp_comp_time() / metric::jsc_max_omp_comp_time()",
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
JSCOmpImbalanceTest::add_avg_omp_comp_time( cube::CubeProxy* ) const
{
    add_comp_time( cube );
    cube::Metric* _met = cube->getMetric( "jsc_avg_omp_comp_time" );
    if ( _met == nullptr )
    {
        _met = cube->defineMetric(
            "JSC AVG Omp computation time",
            "jsc_avg_omp_comp_time",
            "DOUBLE",
            "sec",
            "",
            JSC_LB_EFF_METRIC_URL,
            "AVG computation time, ( comp / number_of_processes )",
            nullptr,
            cube::CUBE_METRIC_POSTDERIVED,
            "metric::omp_non_wait_time() / metric::__service_counter_metric(e)",
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
JSCOmpImbalanceTest::add_max_omp_comp_time( cube::CubeProxy* ) const
{
    add_comp_time( cube );
    cube::Metric* _met = cube->getMetric( "jsc_max_omp_comp_time" );
    if ( _met == nullptr )
    {
        _met = cube->defineMetric(
            "JSC Maximal OMP computation time",
            "jsc_max_omp_comp_time",
            "DOUBLE",
            "sec",
            "",
            JSC_LB_EFF_METRIC_URL,
            "Maximal computation time, max( comp )",
            nullptr,
            cube::CUBE_METRIC_POSTDERIVED,
            "metric::omp_non_wait_time()",
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
