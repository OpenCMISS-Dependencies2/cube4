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

#include "BSPOPHybridOMPLoadBalanceEfficiencyTest.h"

using namespace advisor;

BSPOPHybridOMPLoadBalanceEfficiencyTest::BSPOPHybridOMPLoadBalanceEfficiencyTest(   cube::CubeProxy*               cube,
                                                                                    BSPOPHybridLoadBalanceTest*    _par_lb_eff,
                                                                                    BSPOPHybridMPILoadBalanceTest* _mpi_lb_eff
                                                                                    ) :
    PerformanceTest( cube ),
    par_lb_eff( _par_lb_eff ),
    mpi_lb_eff( _mpi_lb_eff )
{
    setName( " * OpenMP Load Balance Efficiency" );
    setWeight( 1 );   // need to be adjusted
    if ( par_lb_eff == nullptr || mpi_lb_eff == nullptr  ||
         ( !par_lb_eff->isActive() && !mpi_lb_eff->isActive() )
         )
    {
        setWeight( 0.2 );
        setValue( 0. );
        return;
    }
}


void
BSPOPHybridOMPLoadBalanceEfficiencyTest::calculate( const cube::list_of_cnodes&  )
{
}



void
BSPOPHybridOMPLoadBalanceEfficiencyTest::calculate()
{
    if ( par_lb_eff == nullptr || mpi_lb_eff == nullptr  ||
         ( !par_lb_eff->isActive() && !mpi_lb_eff->isActive()  ) )
    {
        return;
    }
    double par_lb_eff_value = par_lb_eff->value();
    double mpi_lb_eff_value = mpi_lb_eff->value();
    setValue( ( par_lb_eff->isActive() ? par_lb_eff_value : 1. ) /
              ( mpi_lb_eff->isActive() ? mpi_lb_eff_value : 1. ) );
}

void
BSPOPHybridOMPLoadBalanceEfficiencyTest::applyCnode( const cube::Cnode* cnode, const cube::CalculationFlavour cnf,
                                                     const bool direct_calculation  )
{
    ( void )direct_calculation; // not used here
    ( void )cnode;
    ( void )cnf;
    calculate();
}

void
BSPOPHybridOMPLoadBalanceEfficiencyTest::applyCnode( const cube::list_of_cnodes& cnodes,
                                                     const bool                  direct_calculation  )
{
    ( void )direct_calculation; // not used here
    ( void )cnodes;
    calculate();
}


const std::string&
BSPOPHybridOMPLoadBalanceEfficiencyTest::getCommentText() const
{
    if ( par_lb_eff == nullptr || mpi_lb_eff == nullptr  ||
         ( !par_lb_eff->isActive() && !mpi_lb_eff->isActive()  ) )
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
BSPOPHybridOMPLoadBalanceEfficiencyTest::isActive() const
{
    return par_lb_eff != nullptr &&
           mpi_lb_eff != nullptr &&
           (
        ( par_lb_eff->isActive() )  || ( mpi_lb_eff->isActive() ) )
    ;
};

bool
BSPOPHybridOMPLoadBalanceEfficiencyTest::isIssue() const
{
    return false;
};



void
BSPOPHybridOMPLoadBalanceEfficiencyTest::adjustForTest( cube::CubeProxy* cube ) const
{
    ( void )cube;
}
