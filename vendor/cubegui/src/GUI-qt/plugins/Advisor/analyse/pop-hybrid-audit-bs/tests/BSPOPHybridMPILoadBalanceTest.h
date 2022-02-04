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


#ifndef BSPOP_HYBRID_MPI_LOAD_BALANCE_TEST_H
#define BSPOP_HYBRID_MPI_LOAD_BALANCE_TEST_H

#include <string>
#include "PerformanceTest.h"
#include "BSPOPHybridMPICommunicationEfficiencyTest.h"


namespace advisor
{
class BSPOPHybridMPILoadBalanceTest : public PerformanceTest
{
private:

    cube::Metric* non_mpi_time;
    cube::Metric* max_non_mpi_time;

    cube::list_of_metrics                      max_non_mpi_metrics;
    BSPOPHybridMPICommunicationEfficiencyTest* mpi_comm_eff;



protected:

    void
    applyCnode( const cube::list_of_cnodes& cnodes,
                const bool                  direct_calculation = false );

    void
    applyCnode( const cube::Cnode*             cnode,
                const cube::CalculationFlavour cnf = cube::CUBE_CALCULATE_INCLUSIVE,
                const bool                     direct_calculation = false );



    virtual
    const std::string&
    getCommentText() const;

    inline
    virtual
    const
    QString
    getHelpUrl()
    {
        return ( isActive() ) ?
               QString::fromStdString( "AdvisorBSPOPHybridTestsMPILoad_balance_efficiency.html" ) :
               QString::fromStdString( "AdvisorBSPOPHybridTestsMissing_MPIload_balance_efficiency.html" );
    }

    virtual
    void
    adjustForTest( cube::CubeProxy* cube ) const;

public:
    BSPOPHybridMPILoadBalanceTest( cube::CubeProxy*,
                                   BSPOPHybridMPICommunicationEfficiencyTest* );

    virtual
    ~BSPOPHybridMPILoadBalanceTest()
    {
    };

    virtual
    double
    analyze( const cube::list_of_cnodes& cnodes,
             cube::LocationGroup*        _lg = nullptr ) const;




// ------ overview tests ---------

    bool
    isActive() const;

    bool
    isIssue() const;

    virtual
    QList<PerformanceTest*>
    getPrereqs()
    {
        QList<PerformanceTest*> prereqs;
        if ( mpi_comm_eff != nullptr )
        {
            prereqs << mpi_comm_eff;
        }
        return prereqs;
    }
};
};
#endif // BSPOPHybrid_IMBALANCE_TEST_H
