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

#include "KnlMemoryBandwidthTest.h"

using namespace advisor;

KnlMemoryBandwidthTest::KnlMemoryBandwidthTest( cube::CubeProxy* cube ) : PerformanceTest( cube )
{
    setName( tr( "KNL Memory Bandwidth" ).toUtf8().data() );
    setWeight( 1 );   // need to be adjusted
    knl_memory_bandwidth = cube->getMetric( "knl_memory_bandwidth" );
    if ( knl_memory_bandwidth == NULL )
    {
        adjustForTest( cube );
    }
    knl_memory_bandwidth = cube->getMetric( "knl_memory_bandwidth" );
    maxValue             = 1.;
    if ( knl_memory_bandwidth == NULL )
    {
        setWeight( 0.2 );
        setValue( 0. );
    }
    else
    {
        cube::metric_pair metric;
        metric.first  = knl_memory_bandwidth;
        metric.second = cube::CUBE_CALCULATE_INCLUSIVE;
        lmetrics.push_back( metric );
    }
}


void
KnlMemoryBandwidthTest::applyCnode( const cube::Cnode* cnode, const cube::CalculationFlavour cnf,
                                    const bool direct_calculation  )
{
    ( void )direct_calculation; // not used here
    if ( knl_memory_bandwidth == NULL )
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
    double knl_memory_bandwidth_value = v->getDouble();
    delete v;

    setValue( knl_memory_bandwidth_value );
}

void
KnlMemoryBandwidthTest::applyCnode( const cube::list_of_cnodes& lcnodes,
                                    const bool                  direct_calculation  )
{
    ( void )direct_calculation; // not used here
    if ( knl_memory_bandwidth == NULL )
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
        double knl_memory_bandwidth_value = v->getDouble();
        delete v;
        setValue( knl_memory_bandwidth_value );
    }
}


const std::string&
KnlMemoryBandwidthTest::getCommentText() const
{
    if ( knl_memory_bandwidth == NULL )
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
KnlMemoryBandwidthTest::isActive() const
{
    return knl_memory_bandwidth != NULL;
};

bool
KnlMemoryBandwidthTest::isIssue() const
{
    return false;
};


void
KnlMemoryBandwidthTest::adjustForTest( cube::CubeProxy* cube ) const
{
    cube::Metric* knl_memory_transfer = cube->getMetric( "knl_memory_transfer" );
    cube::Metric* max_time            = cube->getMetric( "max_time" );

    if ( knl_memory_transfer != NULL && max_time != NULL )
    {
        add_knl_memory_bandwidth( cube );
    }
}



void
KnlMemoryBandwidthTest::add_knl_memory_bandwidth( cube::CubeProxy* ) const
{
    cube::Metric* _met = cube->getMetric( "knl_memory_bandwidth" );
    if ( _met == NULL )
    {
        cube::Metric* _met = cube->defineMetric(
            tr( "Memory Bandwidth (DDR) " ).toUtf8().data(),
            "knl_memory_bandwidth",
            "DOUBLE",
            tr( "bytes/s" ).toUtf8().data(),
            "",
            KNL_MEMORY_BANDWIDTH_URL,
            tr( "Displays the utilized memory bandwidth." ).toUtf8().data(),
            NULL,
            cube::CUBE_METRIC_POSTDERIVED,
            "metric::knl_memory_transfer()/metric::fixed::max_time(i)",
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
