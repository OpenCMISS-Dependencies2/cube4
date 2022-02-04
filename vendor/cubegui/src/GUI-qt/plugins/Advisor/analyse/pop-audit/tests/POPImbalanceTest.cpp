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

#include "POPImbalanceTest.h"


using namespace advisor;

POPImbalanceTest::POPImbalanceTest( cube::CubeProxy* cube ) : PerformanceTest( cube )
{
    setName( tr( "    Load Balance Efficiency" ).toUtf8().data() );
    setWeight( 1 );   // need to be adjusted
    pop_comp = cube->getMetric( "comp" );
    if ( pop_comp == nullptr )
    {
        adjustForTest( cube );
    }
    pop_comp = cube->getMetric( "comp" );
    if ( pop_comp == nullptr )
    {
        setWeight( 0.1 );
        setValue( 0. );
        return;
    }
    cube::list_of_sysresources lsysres = getRootsOfSystemTree();
    cube::metric_pair          metric;
    metric.first  = pop_comp;
    metric.second = cube::CUBE_CALCULATE_INCLUSIVE;
    lmetrics.push_back( metric );
}



void
POPImbalanceTest::applyCnode( const cube::Cnode*, const cube::CalculationFlavour,
                              const bool )
{
    if ( pop_comp == nullptr )
    {
        return;
    }
}

double
POPImbalanceTest::analyze( const cube::list_of_cnodes& cnodes,
                           cube::LocationGroup*          ) const
{
    if ( pop_comp == nullptr )
    {
        return 0.;
    }
    cube::value_container inclusive_values;
    cube::value_container exclusive_values;
    cube->getSystemTreeValues( lmetrics,
                               cnodes,
                               inclusive_values,
                               exclusive_values );

    const std::vector<cube::LocationGroup*>& _lgs                         = cube->getLocationGroups();
    double                                   pop_comp_imbalance_value_sum = 0.;
    double                                   pop_comp_imbalance_value_max = std::numeric_limits<double>::lowest();
    for ( std::vector<cube::LocationGroup*>::const_iterator iter = _lgs.begin(); iter != _lgs.end(); ++iter )
    {
        double _v =  inclusive_values[ ( *iter )->get_sys_id() ]->getDouble();
        pop_comp_imbalance_value_sum += _v;
        pop_comp_imbalance_value_max  = std::max(
            pop_comp_imbalance_value_max,
            inclusive_values[ ( *iter )->get_sys_id() ]->getDouble()
            );
    }
    return pop_comp_imbalance_value_sum / _lgs.size() / pop_comp_imbalance_value_max;
}




void
POPImbalanceTest::applyCnode( const cube::list_of_cnodes& cnodes,
                              const bool                  direct_calculation  )
{
    ( void )direct_calculation; // not used here
    if ( pop_comp == nullptr )
    {
        return;
    }
    setValue(  analyze( cnodes ) );
}


const std::string&
POPImbalanceTest::getCommentText() const
{
    if ( pop_comp == nullptr )
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
POPImbalanceTest::isActive() const
{
    return pop_comp != nullptr;
};

bool
POPImbalanceTest::isIssue() const
{
    return false;
};

void
POPImbalanceTest::adjustForTest( cube::CubeProxy* cube ) const
{
    cube::Metric* _comp = cube->getMetric( "comp" );
    if ( _comp == nullptr )
    {
        add_comp_time( cube );
    }
}
