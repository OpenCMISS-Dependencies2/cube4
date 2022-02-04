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


#ifndef BSPOP_HYBRID_LOAD_BALANCE_TEST_H
#define BSPOP_HYBRID_LOAD_BALANCE_TEST_H

#include <string>
#include "PerformanceTest.h"
#include "BSPOPHybridCommunicationEfficiencyTest.h"


namespace advisor
{
class BSPOPHybridLoadBalanceTest : public PerformanceTest
{
private:
    cube::Metric* pop_avg_comp;
    cube::Metric* max_comp;

    cube::list_of_metrics max_comp_metrics;

    BSPOPHybridCommunicationEfficiencyTest* hyb_comm_eff;
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
               QString::fromStdString( "AdvisorBSPOPHybridTestsLoadBalance_efficiency.html" ) :
               QString::fromStdString( "AdvisorBSPOPHybridTestsMissing_loadbalance_efficiency.html" );
    }

    virtual
    void
    adjustForTest( cube::CubeProxy* cube ) const;

    void
    add_avg_comp( cube::CubeProxy* ) const;


public:
    BSPOPHybridLoadBalanceTest( cube::CubeProxy*,
                                BSPOPHybridCommunicationEfficiencyTest* _hyb_comm_eff );

    virtual
    ~BSPOPHybridLoadBalanceTest()
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
        if ( hyb_comm_eff != nullptr )
        {
            prereqs << hyb_comm_eff;
        }
        return prereqs;
    }
};
};
#endif // BSPOPHybrid_IMBALANCE_TEST_H
