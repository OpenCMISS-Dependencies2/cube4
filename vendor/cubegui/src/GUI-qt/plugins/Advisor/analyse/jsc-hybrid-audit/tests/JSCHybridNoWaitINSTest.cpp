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

#include "JSCHybridNoWaitINSTest.h"


using namespace advisor;

JSCNoWaitINSTest::JSCNoWaitINSTest( cube::CubeProxy* cube ) : PerformanceTest( cube )
{
    setName( "Instructions (only computation)" );
    setWeight( 1 );   // need to be adjusted
    no_wait_ins = cube->getMetric( "tot_ins_without_wait" );
    if ( no_wait_ins == nullptr )
    {
        setWeight( 0.1 );
        setValue( 0. );
        return;
    }
    maxValue = 1.;
    cube::list_of_sysresources lsysres = getRootsOfSystemTree();
    cube::metric_pair          metric;
    metric.first  = no_wait_ins;
    metric.second = cube::CUBE_CALCULATE_INCLUSIVE;
    lmetrics.push_back( metric );
}


void
JSCNoWaitINSTest::applyCnode( const cube::Cnode* cnode, const cube::CalculationFlavour cnf,
                              const bool direct_calculation  )
{
    ( void )direct_calculation; // not used here
    if ( no_wait_ins == nullptr )
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
    double no_wait_ins_value = v->getDouble();
    delete v;
    setValue( no_wait_ins_value );
}

void
JSCNoWaitINSTest::applyCnode( const cube::list_of_cnodes& lcnodes,
                              const bool                  direct_calculation  )
{
    ( void )direct_calculation; // not used here
    if ( no_wait_ins == nullptr )
    {
        return;
    }
    cube::list_of_sysresources lsysres;
    cube::Value*               v = cube->calculateValue( lmetrics,
                                                         lcnodes,
                                                         lsysres );
    double no_wait_ins_value = v->getDouble();
    delete v;
    setValue( no_wait_ins_value );
}

const std::string&
JSCNoWaitINSTest::getCommentText() const
{
    if ( no_wait_ins == nullptr )
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
JSCNoWaitINSTest::isActive() const
{
    return no_wait_ins != nullptr;
};

bool
JSCNoWaitINSTest::isIssue() const
{
    return false;
};



void
JSCNoWaitINSTest::adjustForTest( cube::CubeProxy* cube ) const
{
    ( void )cube;
}
