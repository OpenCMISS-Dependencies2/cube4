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


#ifndef JSC_OMP_IMBALANCE_TEST_H
#define JSC_OMP_IMBALANCE_TEST_H

#include <string>
#include "PerformanceTest.h"


namespace advisor
{
class JSCOmpImbalanceTest : public PerformanceTest
{
private:
    cube::Metric* pop_omp_imbalance;

    void
    add_omp_lb_eff( cube::CubeProxy* ) const;

    void
    add_avg_omp_comp_time( cube::CubeProxy* ) const;

    void
    add_max_omp_comp_time( cube::CubeProxy* ) const;

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
               QString::fromStdString( "AdvisorJSCTestsOmpLoad_balance.html" ) :
               QString::fromStdString( "AdvisorJSCTestsMissing_omp_load_balance.html" );
    }

    virtual
    void
    adjustForTest( cube::CubeProxy* cube ) const;

public:
    JSCOmpImbalanceTest( cube::CubeProxy* );

    virtual
    ~JSCOmpImbalanceTest()
    {
    };

    virtual
    double
    analyze( const cube::list_of_cnodes& cnodes,
             cube::LocationGroup*        _lg = NULL ) const;

// ------ overview tests ---------

    bool
    isActive() const;

    bool
    isIssue() const;
};
};
#endif // JSC_IMBALANCE_TEST_H
