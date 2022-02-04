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


#ifndef KNL_MEMORY_BANDWIDTH_TEST_H
#define KNL_MEMORY_BANDWIDTH_TEST_H

#include <string>
#include "PerformanceTest.h"


namespace advisor
{
class KnlMemoryBandwidthTest : public PerformanceTest
{
private:
    cube::Metric* knl_memory_bandwidth;
    double        maxValue;

    void
    add_knl_memory_bandwidth( cube::CubeProxy* ) const;

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
               QString::fromStdString( "AdvisorKNLTestsMemoryBandwidth.html" ) :
               QString::fromStdString( "AdvisorKNLTestsMissingMemoryBandwidth.html" );
    }

    inline
    virtual
    const
    QString
    units()
    {
        return tr( "bytes/s" );
    }

    virtual
    void
    adjustForTest( cube::CubeProxy* cube ) const;


    virtual
    inline
    bool
    isNormalized() const // returns true, if range goes from 0 to 1. false -> otherwise
    {
        return false;
    }
public:
    KnlMemoryBandwidthTest( cube::CubeProxy* );

    virtual
    ~KnlMemoryBandwidthTest()
    {
    };

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


// ------ overview tests ---------

    bool
    isActive() const;

    bool
    isIssue() const;
};
};
#endif // ADVISER_RATING_WIDGET_H
