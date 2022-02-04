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


#ifndef POP_HYBRID_AMDAHL_TEST_ADD_H
#define POP_HYBRID_AMDAHL_TEST_ADD_H

#include <string>
#include "PerformanceTest.h"


namespace advisor
{
class POPHybridAmdahlTestAdd : public PerformanceTest
{
private:
    cube::Metric* pop_ser_comp_comp;
    cube::Metric* max_runtime;

    cube::list_of_metrics lmax_runtime_metrics;

    void
    add_amdahl( cube::CubeProxy* ) const;

    void
    add_avg_comp( cube::CubeProxy* ) const;

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
               QString::fromStdString( "AdvisorPOPHybridAddTestsAmdahl_efficiency.html" ) :
               QString::fromStdString( "AdvisorPOPHybridAddTestsMissingAmdahl_transfer_efficiency.html" );
    }

    virtual
    void
    adjustForTest( cube::CubeProxy* cube ) const;

public:
    POPHybridAmdahlTestAdd( cube::CubeProxy* );

    virtual
    ~POPHybridAmdahlTestAdd()
    {
    };
// ------ overview tests ---------

    bool
    isActive() const;

    bool
    isIssue() const;
};
};
#endif // POPHybrid_IMBALANCE_TEST_H
