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


#ifndef KNL_MEMORY_TRANSFER_TEST_H
#define KNL_MEMORY_TRANSFER_TEST_H

#include <string>
#include "PerformanceTest.h"


namespace advisor
{
class KnlMemoryTransferTest : public PerformanceTest
{
private:
    cube::Metric* knl_memory_transfer;
    double        maxValue;

    void
    add_knl_memory_transfer( cube::CubeProxy* ) const;

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
               QString::fromStdString( "AdvisorKNLTestsMemoryTransfer.html" ) :
               QString::fromStdString( "AdvisorKNLTestsMissingMemoryTransfer.html" );
    }

    inline
    virtual
    const
    QString
    units()
    {
        return tr( "bytes" );
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
    KnlMemoryTransferTest( cube::CubeProxy* );

    virtual
    ~KnlMemoryTransferTest()
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
