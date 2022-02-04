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

#include "KnlLLCMissTest.h"

using namespace advisor;

KnlLLCMissTest::KnlLLCMissTest( cube::CubeProxy* cube ) : PerformanceTest( cube )
{
    setName( tr( "KNL Last Level Cache Miss" ).toUtf8().data() );
    setWeight( 1 );   // need to be adjusted
    maxValue     = 1.;
    knl_llc_miss = cube->getMetric( "LLC_MISSES" );
    if ( knl_llc_miss == NULL )
    {
        setWeight( 0.2 );
        setValue( 0. );
        return;
    }
    cube::metric_pair metric;
    metric.first  = knl_llc_miss;
    metric.second = cube::CUBE_CALCULATE_INCLUSIVE;
    lmetrics.push_back( metric );
}


void
KnlLLCMissTest::applyCnode( const cube::Cnode* cnode, const cube::CalculationFlavour cnf,
                            const bool direct_calculation  )
{
    ( void )direct_calculation; // not used here
    if ( knl_llc_miss == NULL )
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
    double knl_llc_miss_value = v->getDouble();
    delete v;
    setValue( knl_llc_miss_value );
}

void
KnlLLCMissTest::applyCnode( const cube::list_of_cnodes& lcnodes,
                            const bool                  direct_calculation  )
{
    ( void )direct_calculation; // not used here
    if ( knl_llc_miss == NULL )
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
        double knl_llc_miss_value = v->getDouble();
        delete v;
        setValue( knl_llc_miss_value );
    }
}



const std::string&
KnlLLCMissTest::getCommentText() const
{
    if ( knl_llc_miss == NULL )
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
KnlLLCMissTest::isActive() const
{
    return knl_llc_miss != NULL;
};

bool
KnlLLCMissTest::isIssue() const
{
    return false;
};
