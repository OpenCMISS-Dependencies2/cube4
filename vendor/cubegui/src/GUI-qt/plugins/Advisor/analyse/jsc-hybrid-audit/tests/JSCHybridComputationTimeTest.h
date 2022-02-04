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


#ifndef JSC_COMPUTATION_TIME_H
#define JSC_COMPUTATION_TIME_H

#include <string>
#include "PerformanceTest.h"


namespace advisor
{
class JSCComputationTime : public PerformanceTest
{
private:
    cube::Metric* comp;

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
               QString::fromStdString( "AdvisorJSCTestsComputationTime.html" ) :
               QString::fromStdString( "AdvisorJSCTestsMissingComputationTime.html" );
    }

    virtual
    void
    adjustForTest( cube::CubeProxy* cube ) const;

public:
    JSCComputationTime( cube::CubeProxy* );

    virtual
    ~JSCComputationTime()
    {
    };
// ------ overview tests ---------


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
        return "sec";
    }
};
};
#endif // JSC_ipc_TEST_H
