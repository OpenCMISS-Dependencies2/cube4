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

#include "BSPOPHybridOMPParallelEfficiencyTest.h"

using namespace advisor;

BSPOPHybridOMPParallelEfficiencyTest::BSPOPHybridOMPParallelEfficiencyTest( cube::CubeProxy*                      cube,
                                                                            BSPOPHybridParallelEfficiencyTest*    _par_eff,
                                                                            BSPOPHybridMPIParallelEfficiencyTest* _mpi_eff
                                                                            ) :
    PerformanceTest( cube ),
    par_eff( _par_eff ),
    mpi_eff( _mpi_eff )

{
    setName( tr( "OpenMP Parallel Efficiency" ).toUtf8().data() );
    setWeight( 1 );   // need to be adjusted

    if ( par_eff == nullptr || mpi_eff == nullptr  ||
         ( !par_eff->isActive() && !mpi_eff->isActive() )
         )
    {
        setWeight( 0.2 );
        setValue( 0. );
        return;
    }
}

void
BSPOPHybridOMPParallelEfficiencyTest::calculate( const cube::list_of_cnodes&  )
{
}

void
BSPOPHybridOMPParallelEfficiencyTest::calculate()
{
    if ( par_eff == nullptr || mpi_eff == nullptr  )
    {
        return;
    }
    double par_eff_value = par_eff->value();
    double mpi_eff_value = mpi_eff->value();
    setValue(
        ( ( par_eff->isActive() ) ? par_eff_value : 1. ) /
        ( ( mpi_eff->isActive() ) ? mpi_eff_value : 1. )
        );
}

void
BSPOPHybridOMPParallelEfficiencyTest::applyCnode( const cube::Cnode*             cnode,
                                                  const cube::CalculationFlavour cnf,
                                                  const bool                     direct_calculation  )
{
    ( void )direct_calculation; // not used here
    ( void )cnode;
    ( void )cnf;
    calculate();
}

void
BSPOPHybridOMPParallelEfficiencyTest::applyCnode( const cube::list_of_cnodes& cnodes,
                                                  const bool                  direct_calculation  )
{
    ( void )direct_calculation; // not used here
    ( void )cnodes;
    calculate();
}


const std::string&
BSPOPHybridOMPParallelEfficiencyTest::getCommentText() const
{
    if ( par_eff == nullptr || mpi_eff == nullptr  )
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
BSPOPHybridOMPParallelEfficiencyTest::isActive() const
{
    return par_eff != nullptr &&
           mpi_eff != nullptr;
};

bool
BSPOPHybridOMPParallelEfficiencyTest::isIssue() const
{
    return false;
};



void
BSPOPHybridOMPParallelEfficiencyTest::adjustForTest( cube::CubeProxy* cube ) const
{
    ( void )cube;
}
