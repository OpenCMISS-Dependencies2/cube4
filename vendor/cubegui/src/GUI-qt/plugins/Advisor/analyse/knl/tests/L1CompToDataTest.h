/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2015-2018                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef L1_COMP2DATA_TEST_H
#define L1_COMP2DATA_TEST_H

#include <string>
#include "PerformanceTest.h"


namespace advisor
{
class L1Comp2DataTest : public PerformanceTest
{
private:
    cube::Metric* l1_comp2data;


    void
    add_l1_compute_to_data_access_ratio_all( cube::CubeProxy* ) const;

    void
    add_l1_compute_to_data_access_ratio( cube::CubeProxy* ) const;

    void
    add_uops_all_loads_without_wait( cube::CubeProxy*  ) const;

    void
    add_uops_all_loads_loops_without_wait( cube::CubeProxy*  ) const;

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
               QString::fromStdString( "AdvisorKNLTestsL1Comp2DataTest.html" ) :
               QString::fromStdString( "AdvisorKNLTestsMissingL1Comp2DataTest.html" );
    }

    virtual
    void
    adjustForTest( cube::CubeProxy* cube ) const;

    virtual
    inline
    bool
    isPercent() const
    {
        return false;
    }

    virtual
    inline
    bool
    isNormalized() const // returns true, if range goes from 0 to 1. false -> otherwise
    {
        return false;
    }

public:
    L1Comp2DataTest( cube::CubeProxy* );

    virtual
    ~L1Comp2DataTest()
    {
    };
// ------ overview tests ---------

    bool
    isActive() const;

    bool
    isIssue() const;
};
};
#endif // ADVISER_RATING_WIDGET_H
