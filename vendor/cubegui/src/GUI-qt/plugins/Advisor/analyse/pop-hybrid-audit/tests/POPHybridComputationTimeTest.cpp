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

#include "POPHybridComputationTimeTest.h"


using namespace advisor;

POPHybridComputationTime::POPHybridComputationTime( cube::CubeProxy* cube ) : PerformanceTest( cube )
{
    setName( tr( "Computation time" ).toUtf8().data() );
    setWeight( 1 );   // need to be adjusted
    comp = cube->getMetric( "comp" );
    if ( comp == nullptr )
    {
        setWeight( 0.1 );
        setValue( 0. );
        return;
    }
    cube::list_of_sysresources lsysres = getRootsOfSystemTree();
    cube::metric_pair          metric;
    metric.first  = comp;
    metric.second = cube::CUBE_CALCULATE_INCLUSIVE;
    lmetrics.push_back( metric );
}


void
POPHybridComputationTime::applyCnode( const cube::Cnode* cnode, const cube::CalculationFlavour cnf,
                                      const bool direct_calculation  )
{
    ( void )direct_calculation; // not used here
    if ( comp == nullptr )
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
    double comp_value = v->getDouble();
    delete v;
    setValue( comp_value );
}

void
POPHybridComputationTime::applyCnode( const cube::list_of_cnodes& lcnodes,
                                      const bool                  direct_calculation  )
{
    ( void )direct_calculation; // not used here
    if ( comp == nullptr )
    {
        return;
    }
    cube::list_of_sysresources lsysres;
    cube::Value*               v = cube->calculateValue( lmetrics,
                                                         lcnodes,
                                                         lsysres );
    double comp_value = v->getDouble();
    delete v;
    setValue( comp_value );
}

const std::string&
POPHybridComputationTime::getCommentText() const
{
    if ( comp == nullptr )
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
POPHybridComputationTime::isActive() const
{
    return comp != nullptr;
};

bool
POPHybridComputationTime::isIssue() const
{
    return false;
};



void
POPHybridComputationTime::adjustForTest( cube::CubeProxy* cube ) const
{
    ( void )cube;
}
