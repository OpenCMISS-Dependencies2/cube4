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
#include "JSCHybridOMPSerialisationTest.h"


using namespace advisor;

JSCOmpSerialisationTest::JSCOmpSerialisationTest( cube::CubeProxy* cube ) : PerformanceTest( cube )
{
    setName( "OpenMP Serialisation Efficiency" );
    setWeight( 1 );   // need to be adjusted
    omp_pop_serialisation = cube->getMetric( "jsc_omp_ser_eff" );
    if ( omp_pop_serialisation == nullptr )
    {
        adjustForTest( cube );
    }
    omp_pop_serialisation = cube->getMetric( "jsc_omp_ser_eff" );
    if ( omp_pop_serialisation == nullptr )
    {
        setWeight( 0.1 );
        setValue( 0. );
        return;
    }
    cube::list_of_sysresources lsysres = getRootsOfSystemTree();
    cube::metric_pair          metric;
    metric.first  = omp_pop_serialisation;
    metric.second = cube::CUBE_CALCULATE_INCLUSIVE;
    lmetrics.push_back( metric );
}



void
JSCOmpSerialisationTest::applyCnode( const cube::Cnode* cnode, const cube::CalculationFlavour cnf,
                                     const bool direct_calculation  )
{
    ( void )direct_calculation; // not used here
    if ( omp_pop_serialisation == nullptr )
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

    const std::vector<cube::LocationGroup*>& _lgs                      = cube->getLocationGroups();
    double                                   pop_omp_serialisation_sum = 0.;
    double                                   pop_omp_serialisation_min = std::numeric_limits<double>::max();
    double                                   pop_omp_serialisation_max = std::numeric_limits<double>::lowest();
    for ( std::vector<cube::LocationGroup*>::const_iterator iter = _lgs.begin(); iter != _lgs.end(); ++iter )
    {
        double _v =  ( cnf == cube::CUBE_CALCULATE_INCLUSIVE ) ? inclusive_values[ ( *iter )->get_sys_id() ]->getDouble() : exclusive_values[ ( *iter )->get_sys_id() ]->getDouble();
        pop_omp_serialisation_sum += _v;

        pop_omp_serialisation_min = std::min( pop_omp_serialisation_min, _v );
        pop_omp_serialisation_max = std::max( pop_omp_serialisation_max, _v );
    }
    double pop_omp_serialisation_value = pop_omp_serialisation_sum / _lgs.size();
    setValues( pop_omp_serialisation_value, pop_omp_serialisation_min, pop_omp_serialisation_max );
}

void
JSCOmpSerialisationTest::applyCnode( const cube::list_of_cnodes& cnodes,
                                     const bool                  direct_calculation  )
{
    ( void )direct_calculation; // not used here
    if ( omp_pop_serialisation == nullptr )
    {
        return;
    }
    cube::value_container inclusive_values;
    cube::value_container exclusive_values;
    cube->getSystemTreeValues( lmetrics,
                               cnodes,
                               inclusive_values,
                               exclusive_values );
    const std::vector<cube::LocationGroup*>& _lgs                      = cube->getLocationGroups();
    double                                   pop_omp_serialisation_sum = 0.;
    double                                   pop_omp_serialisation_min = std::numeric_limits<double>::max();
    double                                   pop_omp_serialisation_max = std::numeric_limits<double>::lowest();
    for ( std::vector<cube::LocationGroup*>::const_iterator iter = _lgs.begin(); iter != _lgs.end(); ++iter )
    {
        double _v =  inclusive_values[ ( *iter )->get_sys_id() ]->getDouble();
        pop_omp_serialisation_sum +=  _v;
        pop_omp_serialisation_min  = std::min( pop_omp_serialisation_min, _v );
        pop_omp_serialisation_max  = std::max( pop_omp_serialisation_max, _v );
    }
    double pop_omp_serialisation_value = pop_omp_serialisation_sum / _lgs.size();
    setValues( pop_omp_serialisation_value, pop_omp_serialisation_min, pop_omp_serialisation_max );
}



double
JSCOmpSerialisationTest::analyze( const cube::list_of_cnodes& cnodes,
                                  cube::LocationGroup*        _lg  ) const
{
    if ( omp_pop_serialisation == nullptr )
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
JSCOmpSerialisationTest::getCommentText() const
{
    if ( omp_pop_serialisation == nullptr )
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
JSCOmpSerialisationTest::isActive() const
{
    return omp_pop_serialisation != nullptr;
};

bool
JSCOmpSerialisationTest::isIssue() const
{
    return false;
};

void
JSCOmpSerialisationTest::adjustForTest( cube::CubeProxy* cube ) const
{
    cube::Metric* _omp_time = cube->getMetric( "omp_time" );
    if ( _omp_time->isInactive() )
    {
        return;
    }
    add_total_omp_runtime_ideal( cube );
    add_total_omp_runtime( cube );
    add_omp_ser_eff( cube );
}


void
JSCOmpSerialisationTest::add_omp_ser_eff( cube::CubeProxy* ) const
{
    add_total_omp_runtime_ideal( cube );
    cube::Metric* _met = cube->getMetric( "jsc_omp_ser_eff" );
    if ( _met == nullptr )
    {
        _met = cube->defineMetric(
            "Serialization OMP efficiency",
            "jsc_omp_ser_eff",
            "DOUBLE",
            "%",
            "",
            JSC_SER_EFF_METRIC_URL,
            "Serialization omp efficiency reflects the loss of efficiency due to dependencies among threads. It is computed as a maximum ratio of a process in computation time to total runtime on ideal runtime. The value varies from 0 to 100%, where 100% shows that routine is a pure computation routine, whereas 0 shows that it is a MPI routine. Do not consider values in the flat view and the system tree.",
            nullptr,
            cube::CUBE_METRIC_POSTDERIVED,
            "metric::omp_non_wait_time()/metric::jsc_total_omp_runtime_ideal()",
            "",
            "",
            "",
            "max(arg1,arg2)",
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
JSCOmpSerialisationTest::add_total_omp_runtime( cube::CubeProxy* ) const
{
    add_comp_time( cube );
    cube::Metric* _met = cube->getMetric( "jsc_total_omp_runtime" );
    if ( _met == nullptr )
    {
        _met = cube->defineMetric(
            "JSC Total omp run time",
            "jsc_total_omp_runtime",
            "DOUBLE",
            "sec",
            "",
            JSC_SER_EFF_METRIC_URL,
            "Run time of the OMP region",
            nullptr,
            cube::CUBE_METRIC_PREDERIVED_INCLUSIVE,
            "metric::omp_non_wait_time() + metric::omp_time()",
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
JSCOmpSerialisationTest::add_total_omp_runtime_ideal( cube::CubeProxy* ) const
{
    add_total_omp_runtime( cube );
    cube::Metric* _met = cube->getMetric( "jsc_total_omp_runtime_ideal" );
    if ( _met == nullptr )
    {
        _met = cube->defineMetric(
            "JSC Ideal total omp runtime",
            "jsc_total_omp_runtime_ideal",
            "DOUBLE",
            "sec",
            "",
            JSC_SER_EFF_METRIC_URL,
            "Run time of the OMP region in ideal OpenMP runtime system",
            nullptr,
            cube::CUBE_METRIC_PREDERIVED_INCLUSIVE,
            "metric::jsc_total_omp_runtime() - metric::omp_management()",
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
