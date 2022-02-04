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
#include "JSCHybridAmdahlTest.h"


using namespace advisor;

JSCAmdahlTest::JSCAmdahlTest( cube::CubeProxy* cube ) : PerformanceTest( cube )
{
    setName( "OpenMP Amdahl Efficiency" );
    setWeight( 1 );   // need to be adjusted
    pop_amdahl = cube->getMetric( "jsc_amdahl_eff" );
    if ( pop_amdahl == nullptr )
    {
        adjustForTest( cube );
    }
    pop_amdahl = cube->getMetric( "jsc_amdahl_eff" );
    if ( pop_amdahl == nullptr )
    {
        setWeight( 0.1 );
        setValue( 0. );
        return;
    }
    cube::list_of_sysresources lsysres = getRootsOfSystemTree();
    cube::metric_pair          metric;
    metric.first  = pop_amdahl;
    metric.second = cube::CUBE_CALCULATE_INCLUSIVE;
    lmetrics.push_back( metric );
}



void
JSCAmdahlTest::applyCnode( const cube::Cnode* cnode, const cube::CalculationFlavour cnf,
                           const bool direct_calculation   )
{
    ( void )direct_calculation; // not used here
    if ( pop_amdahl == nullptr )
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

    const std::vector<cube::LocationGroup*>& _lgs                 = cube->getLocationGroups();
    double                                   pop_amdahl_value_sum = 0.;
    double                                   pop_amdahl_value_min = std::numeric_limits<double>::max();
    double                                   pop_amdahl_value_max = std::numeric_limits<double>::lowest();

    for ( std::vector<cube::LocationGroup*>::const_iterator iter = _lgs.begin(); iter != _lgs.end(); ++iter )
    {
        double _v =  ( cnf == cube::CUBE_CALCULATE_INCLUSIVE ) ? inclusive_values[ ( *iter )->get_sys_id() ]->getDouble() : exclusive_values[ ( *iter )->get_sys_id() ]->getDouble();
        pop_amdahl_value_sum += _v;
        pop_amdahl_value_min  = std::min( pop_amdahl_value_min, _v );
        pop_amdahl_value_max  = std::max( pop_amdahl_value_max, _v );
    }
    double pop_amdahl_value = pop_amdahl_value_sum / _lgs.size();
    setValues( pop_amdahl_value, pop_amdahl_value_min, pop_amdahl_value_max );
}

void
JSCAmdahlTest::applyCnode( const cube::list_of_cnodes& cnodes,
                           const bool                  direct_calculation  )
{
    ( void )direct_calculation; // not used here
    if ( pop_amdahl == nullptr )
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
    double                                   pop_amdahl_value_sum = 0.;
    double                                   pop_amdahl_value_min = std::numeric_limits<double>::max();
    double                                   pop_amdahl_value_max = std::numeric_limits<double>::lowest();
    for ( std::vector<cube::LocationGroup*>::const_iterator iter = _lgs.begin(); iter != _lgs.end(); ++iter )
    {
        double _v =  inclusive_values[ ( *iter )->get_sys_id() ]->getDouble();
        pop_amdahl_value_sum +=   _v;
        pop_amdahl_value_min  = std::min( pop_amdahl_value_min, _v );
        pop_amdahl_value_max  = std::max( pop_amdahl_value_max, _v );
    }
    double pop_amdahl_value = pop_amdahl_value_sum / _lgs.size();
    setValues(  pop_amdahl_value, pop_amdahl_value_min, pop_amdahl_value_max  );
}


const std::string&
JSCAmdahlTest::getCommentText() const
{
    if ( pop_amdahl == nullptr )
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
JSCAmdahlTest::isActive() const
{
    return pop_amdahl != nullptr;
};

bool
JSCAmdahlTest::isIssue() const
{
    return false;
};

void
JSCAmdahlTest::adjustForTest( cube::CubeProxy* cube ) const
{
    cube::Metric* _omp_time = cube->getMetric( "omp_time" );
    if ( _omp_time->isInactive() )
    {
        return;
    }
    cube::Metric* amdahl_eff = cube->getMetric( "jsc_amdahl_eff" );
    if ( amdahl_eff == nullptr )
    {
        add_amdahl( cube );
    }
}


void
JSCAmdahlTest::add_amdahl( cube::CubeProxy* ) const
{
    add_omp_comp_time( cube );
    cube::Metric* _met = cube->getMetric( "jsc_amdahl_eff" );
    if ( _met == nullptr )
    {
        _met = cube->defineMetric(
            tr( "JSC Amdahl efficiency" ).toUtf8().data(),
            "jsc_amdahl_eff",
            "DOUBLE",
            "",
            "",
            JSC_TRANSFER_EFF_METRIC_URL,
            "Calculates JSC Amdahl efficiency.",
            nullptr,
            cube::CUBE_METRIC_POSTDERIVED,
            "metric::par_execution_time()/metric::execution() ",
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
