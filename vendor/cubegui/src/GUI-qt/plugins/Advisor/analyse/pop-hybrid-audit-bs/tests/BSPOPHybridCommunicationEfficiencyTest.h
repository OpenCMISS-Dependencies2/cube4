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


#ifndef BSPOP_HYBRID_COMMUNICATION_EFFICENCY_TEST_H
#define BSPOP_HYBRID_COMMUNICATION_EFFICENCY_TEST_H

#include <string>
#include "PerformanceTest.h"

namespace advisor
{
class BSPOPHybridCommunicationEfficiencyTest : public PerformanceTest
{
private:

    cube::Metric* max_runtime;
    cube::Metric* max_comp;

    cube::list_of_metrics max_comp_metrics;
protected:

    void
    applyCnode( const cube::list_of_cnodes& cnodes,
                const bool                  direct_calculation = false  );

    void
    applyCnode( const cube::Cnode*             cnode,
                const cube::CalculationFlavour cnf = cube::CUBE_CALCULATE_INCLUSIVE,
                const bool                     direct_calculation = false );

    const std::string&
    getCommentText() const;

    inline
    virtual
    const
    QString
    getHelpUrl()
    {
        return ( isActive() ) ?
               QString::fromStdString( "AdvisorBSPOPHybridTestsCommunication_efficiency.html" ) :
               QString::fromStdString( "AdvisorBSPOPHybridTestsMissing_communication_efficiency.html" );
    }

    virtual
    void
    adjustForTest( cube::CubeProxy* cube ) const;


public:
    BSPOPHybridCommunicationEfficiencyTest(
        cube::CubeProxy* );

    virtual
    ~BSPOPHybridCommunicationEfficiencyTest()
    {
    };

    virtual
    double
    analyze( const cube::list_of_cnodes& cnodes,
             cube::LocationGroup*        _lg = nullptr ) const;

// ------ overview tests ---------

    bool
    isActive() const;

    bool
    isIssue() const;
};
};
#endif // ADVISER_RATING_WIDGET_H
