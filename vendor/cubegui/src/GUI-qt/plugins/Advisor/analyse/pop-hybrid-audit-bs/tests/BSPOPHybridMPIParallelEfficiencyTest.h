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


#ifndef BSPOP_HYBRID_MPI_PARALLEL_EFFICIENCY_TEST_H
#define BSPOP_HYBRID_MPI_PARALLEL_EFFICIENCY_TEST_H

#include <string>
#include "PerformanceTest.h"
#include "BSPOPHybridMPICommunicationEfficiencyTest.h"
#include "BSPOPHybridMPILoadBalanceTest.h"

namespace advisor
{
class BSPOPHybridMPIParallelEfficiencyTest : public PerformanceTest
{
private:

    cube::Metric* non_mpi_time;
    cube::Metric* max_runtime;

    cube::list_of_metrics                      max_runtime_metrics;
    BSPOPHybridMPICommunicationEfficiencyTest* mpi_comm_eff;
    BSPOPHybridMPILoadBalanceTest*             mpi_lb_eff;


    void
    calculate();

    void
    calculate( const cube::list_of_cnodes& );

protected:

    void
    applyCnode( const cube::list_of_cnodes& cnodes,
                const bool                  direct_calculation = false  );

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
               QString::fromStdString( "AdvisorBSPOPHybridTestsMPIParallel_efficiency.html" ) :
               QString::fromStdString( "AdvisorBSPOPHybridTestsMissing_MPIparallel_efficiency.html" );
    }

    virtual
    void
    adjustForTest( cube::CubeProxy* cube ) const;


public:
    BSPOPHybridMPIParallelEfficiencyTest(   cube::CubeProxy*,
                                            BSPOPHybridMPICommunicationEfficiencyTest*,
                                            BSPOPHybridMPILoadBalanceTest*              );

    virtual
    ~BSPOPHybridMPIParallelEfficiencyTest()
    {
    };
// ------ overview tests ---------

    bool
    isActive() const;

    bool
    isIssue() const;

//     virtual
//     QList<PerformanceTest*>
//     getPrereqs()
//     {
//         QList<PerformanceTest*> prereqs;
//         if ( mpi_comm_eff != nullptr && mpi_lb_eff != nullptr )
//         {
//             prereqs << mpi_comm_eff << mpi_lb_eff;
//         }
//         return prereqs;
//     }
};
};
#endif // ADVISER_RATING_WIDGET_H
