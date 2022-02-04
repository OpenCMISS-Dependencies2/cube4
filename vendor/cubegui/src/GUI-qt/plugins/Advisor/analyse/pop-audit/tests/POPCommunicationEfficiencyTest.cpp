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

#include "POPCommunicationEfficiencyTest.h"


using namespace advisor;


POPCommunicationEfficiencyTest::POPCommunicationEfficiencyTest( cube::CubeProxy*      cube,
                                                                POPSerialisationTest* _pop_ser,
                                                                POPTransferTest*      _pop_transeff ) : PerformanceTest( cube ),
    pop_ser( _pop_ser ),
    pop_transeff( _pop_transeff )
{
    original_scout_cubex = scout_metrics_available( cube );
    setName( tr( "    Communication Efficiency" ).toUtf8().data() );
    setWeight( 1 );   // need to be adjusted
    max_comp_time = nullptr;
    max_runtime   = nullptr;

    max_comp_time = cube->getMetric( "comp" );
    if ( max_comp_time == nullptr )
    {
        adjustForTest( cube );
    }
    max_comp_time = cube->getMetric( "comp" );
    if ( max_comp_time == nullptr )
    {
        setWeight( 0.2 );
        setValue( 0. );
        return;
    }
    max_runtime = cube->getMetric( "execution" );

    cube::metric_pair metric;
    metric.first  = max_runtime;
    metric.second = cube::CUBE_CALCULATE_INCLUSIVE;
    lmetrics.push_back( metric );

    metric.first  = max_comp_time;
    metric.second = cube::CUBE_CALCULATE_INCLUSIVE;
    lmax_comp_time.push_back( metric );
}

double
POPCommunicationEfficiencyTest::calculateForScout( const cube::list_of_cnodes& cnodes ) const
{
    if ( pop_ser == nullptr || pop_transeff == nullptr )
    {
        return 0.;
    }
    double pop_ser_value     = pop_ser->analyze( cnodes );
    double pop_transef_value = pop_transeff->analyze( cnodes );
    return pop_ser_value *   pop_transef_value;
}


void
POPCommunicationEfficiencyTest::applyCnode( const cube::Cnode*, const cube::CalculationFlavour,
                                            const bool )
{
}

void
POPCommunicationEfficiencyTest::applyCnode( const cube::list_of_cnodes& cnodes,
                                            const bool                  direct_calculation  )
{
    scout_cubex = original_scout_cubex && !direct_calculation;
    setValue(  analyze( cnodes ) );
}


double
POPCommunicationEfficiencyTest::analyze( const cube::list_of_cnodes& cnodes,
                                         cube::LocationGroup*          ) const
{
    if ( !scout_cubex )
    {
        if ( max_runtime == nullptr || max_comp_time == nullptr )
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
        cube->getSystemTreeValues( lmax_comp_time,
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
            max_total_time_ideal_value = std::max( max_total_time_ideal_value, _v1 );
            max_comp_time_value        = std::max( max_comp_time_value, _v2 );
        }
        return max_comp_time_value / max_total_time_ideal_value;
    }
    else
    {
        return calculateForScout( cnodes );
    }
}



const std::string&
POPCommunicationEfficiencyTest::getCommentText() const
{
    if ( pop_ser == nullptr || pop_transeff == nullptr )
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
POPCommunicationEfficiencyTest::isActive() const
{
    return ( pop_ser != nullptr && pop_transeff != nullptr )
           ||
           ( max_comp_time != nullptr && max_runtime != nullptr );
};

bool
POPCommunicationEfficiencyTest::isIssue() const
{
    return false;
};

void
POPCommunicationEfficiencyTest::adjustForTest( cube::CubeProxy* cube ) const
{
    add_execution_time( cube );
    add_comp_time( cube );
}
