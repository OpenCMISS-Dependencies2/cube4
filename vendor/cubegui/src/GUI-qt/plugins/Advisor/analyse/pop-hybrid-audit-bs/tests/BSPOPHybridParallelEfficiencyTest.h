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


#ifndef BSPOP_HYBRID_PARALLEL_EFFICIENCY_TEST_H
#define BSPOP_HYBRID_PARALLEL_EFFICIENCY_TEST_H

#include <string>
#include "PerformanceTest.h"
#include "BSPOPHybridCommunicationEfficiencyTest.h"
#include "BSPOPHybridLoadBalanceTest.h"

namespace advisor
{
class BSPOPHybridParallelEfficiencyTest : public PerformanceTest
{
private:
    cube::Metric* pop_avg_comp;
    cube::Metric* max_runtime;

    cube::list_of_metrics max_runtime_metrics;

    BSPOPHybridCommunicationEfficiencyTest* hyb_comm_eff;
    BSPOPHybridLoadBalanceTest*             hyb_lb_eff;

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
               QString::fromStdString( "AdvisorBSPOPHybridTestsParallel_efficiency.html" ) :
               QString::fromStdString( "AdvisorBSPOPHybridTestsMissing_parallel_efficiency.html" );
    }

    virtual
    void
    adjustForTest( cube::CubeProxy* cube ) const;


public:
    BSPOPHybridParallelEfficiencyTest(  cube::CubeProxy*,
                                        BSPOPHybridCommunicationEfficiencyTest*,
                                        BSPOPHybridLoadBalanceTest*
                                        );

    virtual
    ~BSPOPHybridParallelEfficiencyTest()
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
//         if ( hyb_comm_eff != nullptr && hyb_lb_eff != nullptr )
//         {
//             prereqs << hyb_comm_eff << hyb_lb_eff;
//         }
//         return prereqs;
//     }
};
};
#endif // ADVISER_RATING_WIDGET_H
