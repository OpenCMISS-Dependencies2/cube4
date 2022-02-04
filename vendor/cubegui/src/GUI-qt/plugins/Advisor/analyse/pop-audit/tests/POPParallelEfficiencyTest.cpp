/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2015-2020                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include <config.h>

#include "POPParallelEfficiencyTest.h"

using namespace advisor;

POPParallelEfficiencyTest::POPParallelEfficiencyTest( POPImbalanceTest*               _pop_lb,
                                                      POPCommunicationEfficiencyTest* _pop_commeff ) :
    PerformanceTest( nullptr ),
    pop_lb( _pop_lb ),
    pop_commeff( _pop_commeff )

{
    setName( tr( "Parallel Efficiency" ).toUtf8().data() );
    setWeight( 1 );   // need to be adjusted
    if ( pop_lb == nullptr || pop_commeff == nullptr )
    {
        setWeight( 0.2 );
        setValue( 0. );
        return;
    }
}

void
POPParallelEfficiencyTest::calculate()
{
    if ( pop_lb == nullptr || pop_commeff == nullptr )
    {
        return;
    }
    double lb_eff_value   = pop_lb->value();
    double comm_eff_value = pop_commeff->value();
    setValue( lb_eff_value *   comm_eff_value );
}

void
POPParallelEfficiencyTest::applyCnode( const cube::Cnode* cnode, const cube::CalculationFlavour cnf,
                                       const bool direct_calculation  )
{
    ( void )direct_calculation; // not used here
    ( void )cnode;
    ( void )cnf;
    calculate();
}

void
POPParallelEfficiencyTest::applyCnode( const cube::list_of_cnodes& cnodes,
                                       const bool                  direct_calculation  )
{
    ( void )direct_calculation; // not used here
    ( void )cnodes;
    calculate();
}


const std::string&
POPParallelEfficiencyTest::getCommentText() const
{
    if ( pop_lb == nullptr || pop_commeff == nullptr )
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
POPParallelEfficiencyTest::isActive() const
{
    return pop_lb != nullptr &&
           pop_commeff != nullptr;
};

bool
POPParallelEfficiencyTest::isIssue() const
{
    return false;
};



void
POPParallelEfficiencyTest::adjustForTest( cube::CubeProxy* cube ) const
{
    ( void )cube;
}
