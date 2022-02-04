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


#ifndef POP_STALLED_RESOURCES_TEST_H
#define POP_STALLED_RESOURCES_TEST_H

#include <string>
#include "PerformanceTest.h"


namespace advisor
{
class POPStalledResourcesTest : public PerformanceTest
{
private:
    cube::Metric* pop_stalled_resources;

    void
    add_stalled_resources( cube::CubeProxy* ) const;

    void
    add_res_stl_without_wait( cube::CubeProxy* ) const;

    void
    add_tot_cyc_without_wait( cube::CubeProxy* ) const;

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
               QString::fromStdString( "AdvisorPOPTestsStalled_resources.html" ) :
               QString::fromStdString( "AdvisorPOPTestsMissing_stalled_resources.html" );
    }

    virtual
    void
    adjustForTest( cube::CubeProxy* cube ) const;

public:
    POPStalledResourcesTest( cube::CubeProxy* );

    virtual
    ~POPStalledResourcesTest()
    {
    };
// ------ overview tests ---------

    bool
    isActive() const;

    bool
    isIssue() const;

    virtual
    inline
    bool
    isRegular() const // returns true, if 0 to 1 indicates improvement, false - otherwise
    {
        return false;
    }
};
};
#endif // POP_stalled_resources_TEST_H
