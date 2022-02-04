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


#ifndef JSC_OMP_SERIALISATION_TEST_H
#define JSC_OMP_SERIALISATION_TEST_H

#include <string>
#include "PerformanceTest.h"



namespace advisor
{
class JSCOmpSerialisationTest : public PerformanceTest
{
private:
    cube::Metric* omp_pop_serialisation;

    void
    add_omp_ser_eff( cube::CubeProxy* ) const;

    void
    add_total_omp_runtime( cube::CubeProxy* ) const;

    void
    add_total_omp_runtime_ideal( cube::CubeProxy* ) const;

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
               QString::fromStdString( "AdvisorJSCTestsOmpSerialisation_efficiency.html" ) :
               QString::fromStdString( "AdvisorJSCTestsMissing_omp_serialisation_efficiency.html" );
    }

    virtual
    void
    adjustForTest( cube::CubeProxy* cube ) const;

public:
    JSCOmpSerialisationTest( cube::CubeProxy* );

    virtual
    ~JSCOmpSerialisationTest()
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
