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


#ifndef BSPOP_HYBRID_OMP_LOAD_BALANCE_EFFICIENCY_TEST_H
#define BSPOP_HYBRID_OMP_LOAD_BALANCE_EFFICIENCY_TEST_H

#include <string>
#include "PerformanceTest.h"
#include "BSPOPHybridMPILoadBalanceTest.h"
#include "BSPOPHybridLoadBalanceTest.h"


namespace advisor
{
class BSPOPHybridOMPLoadBalanceEfficiencyTest : public PerformanceTest
{
private:

    BSPOPHybridLoadBalanceTest*    par_lb_eff;
    BSPOPHybridMPILoadBalanceTest* mpi_lb_eff;

    void
    calculate();

    void
    calculate( const cube::list_of_cnodes& );

protected:

    void
    applyCnode( const cube::list_of_cnodes& cnodes,
                const bool                  direct_calculation = false   );

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
               QString::fromStdString( "AdvisorBSPOPHybridTestsOMPLoadBalance_efficiency.html" ) :
               QString::fromStdString( "AdvisorBSPOPHybridTestsMissing_OMPloadbalance_efficiency.html" );
    }

    virtual
    void
    adjustForTest( cube::CubeProxy* cube ) const;


public:
    BSPOPHybridOMPLoadBalanceEfficiencyTest(     cube::CubeProxy*,
                                                 BSPOPHybridLoadBalanceTest*,
                                                 BSPOPHybridMPILoadBalanceTest*
                                                 );

    virtual
    ~BSPOPHybridOMPLoadBalanceEfficiencyTest()
    {
    };
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
        if ( par_lb_eff != nullptr && mpi_lb_eff != nullptr )
        {
            prereqs << par_lb_eff << mpi_lb_eff;
        }
        return prereqs;
    }
};
};
#endif // ADVISER_RATING_WIDGET_H
