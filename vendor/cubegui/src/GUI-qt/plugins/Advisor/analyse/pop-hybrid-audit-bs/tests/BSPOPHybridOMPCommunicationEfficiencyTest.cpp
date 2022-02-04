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
#include "BSPOPHybridOMPCommunicationEfficiencyTest.h"


using namespace advisor;

BSPOPHybridOMPCommunicationEfficiencyTest::BSPOPHybridOMPCommunicationEfficiencyTest( cube::CubeProxy*                           cube,
                                                                                      BSPOPHybridCommunicationEfficiencyTest*    _par_comm_eff,
                                                                                      BSPOPHybridMPICommunicationEfficiencyTest* _mpi_comm_eff ) : PerformanceTest( cube ),
    par_comm_eff( _par_comm_eff ),
    mpi_comm_eff( _mpi_comm_eff )
{
    setName( " * OpenMP Communication Efficiency" );
    setWeight( 1 );   // need to be adjusted
    if ( par_comm_eff == nullptr || mpi_comm_eff == nullptr  ||
         ( !par_comm_eff->isActive() && !mpi_comm_eff->isActive() )
         )
    {
        setWeight( 0.2 );
        setValue( 0. );
        return;
    }
}

void
BSPOPHybridOMPCommunicationEfficiencyTest::calculate( const cube::list_of_cnodes&  )
{
}



void
BSPOPHybridOMPCommunicationEfficiencyTest::calculate()
{
    if ( par_comm_eff == nullptr || mpi_comm_eff == nullptr  ||
         ( !par_comm_eff->isActive() && !mpi_comm_eff->isActive()  ) )
    {
        return;
    }
    double par_comm_eff_value = par_comm_eff->value();
    double mpi_comm_eff_value = mpi_comm_eff->value();
    setValue( ( par_comm_eff->isActive() ? par_comm_eff_value : 1. ) /
              ( mpi_comm_eff->isActive() ? mpi_comm_eff_value : 1. ) );
}


void
BSPOPHybridOMPCommunicationEfficiencyTest::applyCnode( const cube::Cnode* cnode, const cube::CalculationFlavour cnf,
                                                       const bool direct_calculation  )
{
    ( void )direct_calculation;  // not used here
    ( void )cnode;
    ( void )cnf;
    calculate();
}

void
BSPOPHybridOMPCommunicationEfficiencyTest::applyCnode( const cube::list_of_cnodes& cnodes,
                                                       const bool                  direct_calculation  )
{
    ( void )direct_calculation; // not used here
    ( void )cnodes;
    calculate();
}





const std::string&
BSPOPHybridOMPCommunicationEfficiencyTest::getCommentText() const
{
    if ( par_comm_eff == nullptr || par_comm_eff == nullptr  ||
         ( !par_comm_eff->isActive() && !par_comm_eff->isActive()  ) )
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
BSPOPHybridOMPCommunicationEfficiencyTest::isActive() const
{
    return par_comm_eff != nullptr &&
           mpi_comm_eff != nullptr &&
           (
        ( par_comm_eff->isActive() )  || ( mpi_comm_eff->isActive() ) )
    ;
};

bool
BSPOPHybridOMPCommunicationEfficiencyTest::isIssue() const
{
    return false;
};

void
BSPOPHybridOMPCommunicationEfficiencyTest::adjustForTest( cube::CubeProxy* cube ) const
{
    ( void )cube;
}
