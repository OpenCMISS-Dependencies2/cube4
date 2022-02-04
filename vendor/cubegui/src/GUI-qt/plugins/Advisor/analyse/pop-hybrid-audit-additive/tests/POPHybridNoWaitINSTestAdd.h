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


#ifndef POP_HYBRID_NO_WAIT_INS_TEST_ADD_H
#define POP_HYBRID_NO_WAIT_INS_TEST_ADD_H

#include <string>
#include "PerformanceTest.h"


namespace advisor
{
class POPHybridNoWaitINSTestAdd : public PerformanceTest
{
private:
    cube::Metric* no_wait_ins;
    double        maxValue;

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
               QString::fromStdString( "AdvisorPOPHybridAddTestsNoWaitINS_efficiency.html" ) :
               QString::fromStdString( "AdvisorPOPHybridAddTestsMissingNoWaitINS_efficiency.html" );
    }

    virtual
    void
    adjustForTest( cube::CubeProxy* cube ) const;

public:
    POPHybridNoWaitINSTestAdd( cube::CubeProxy* );

    virtual
    ~POPHybridNoWaitINSTestAdd()
    {
    };
// ------ overview tests ---------

    virtual
    inline
    double
    getMaximum() const
    {
        return maxValue;
    }

    virtual
    inline
    bool
    isPercent() const
    {
        return false;
    }

    bool
    isActive() const;

    bool
    isIssue() const;

    virtual
    inline
    bool
    isNormalized() const // returns true, if range goes from 0 to 1. false -> otherwise
    {
        return false;
    }

    inline
    virtual
    const
    QString
    units()
    {
        return "#";
    }
};
};
#endif // POPHybrid_ipc_TEST_H
