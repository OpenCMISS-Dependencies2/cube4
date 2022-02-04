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


#ifndef KNL_LLC_MISS_TEST_H
#define KNL_LLC_MISS_TEST_H

#include <string>
#include "PerformanceTest.h"


namespace advisor
{
class KnlLLCMissTest : public PerformanceTest
{
private:
    cube::Metric* knl_llc_miss;
    double        maxValue;
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
               QString::fromStdString( "AdvisorKNLTestsLLCMiss.html" ) :
               QString::fromStdString( "AdvisorKNLTestsMissingLLCMiss.html" );
    }

    inline
    virtual
    const
    QString
    units()
    {
        return "";
    }

    virtual
    inline
    bool
    isNormalized() const // returns true, if range goes from 0 to 1. false -> otherwise
    {
        return false;
    }

public:
    KnlLLCMissTest( cube::CubeProxy* );

    virtual
    ~KnlLLCMissTest()
    {
    };
// ------ overview tests ---------

    inline
    double
    getMaximum() const
    {
        return maxValue;
    }


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
};
};
#endif // ADVISER_RATING_WIDGET_H
