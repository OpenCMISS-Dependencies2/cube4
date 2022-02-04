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


#ifndef POP_COMMUNICATION_EFFICENCY_TEST_H
#define POP_COMMUNICATION_EFFICENCY_TEST_H

#include <string>
#include "PerformanceTest.h"
#include "POPSerialisationTest.h"
#include "POPTransferTest.h"

namespace advisor
{
class POPCommunicationEfficiencyTest : public PerformanceTest
{
private:
    cube::Metric*         max_comp_time;
    cube::Metric*         max_runtime;
    POPSerialisationTest* pop_ser;
    POPTransferTest*      pop_transeff;
    bool                  scout_cubex;
    bool                  original_scout_cubex;

    cube::list_of_metrics lmax_comp_time;

    double
    calculateForScout( const cube::list_of_cnodes& cnodes  ) const;

protected:

    void
    applyCnode( const cube::list_of_cnodes& cnodes,
                const bool                  direct_calculation = false   );

    void
    applyCnode( const cube::Cnode*             cnode,
                const cube::CalculationFlavour cnf = cube::CUBE_CALCULATE_INCLUSIVE,
                const bool                     direct_calculation = false  );

    const std::string&
    getCommentText() const;

    inline
    virtual
    const
    QString
    getHelpUrl()
    {
        return ( isActive() ) ?
               QString::fromStdString( "AdvisorPOPTestsCommunication_efficiency.html" ) :
               QString::fromStdString( "AdvisorPOPTestsMissing_communication_efficiency.html" );
    }

    virtual
    void
    adjustForTest( cube::CubeProxy* cube ) const;


public:
    POPCommunicationEfficiencyTest(
        cube::CubeProxy*,
        POPSerialisationTest* pop_ser,
        POPTransferTest*      pop_transef );

    virtual
    ~POPCommunicationEfficiencyTest()
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

    virtual
    QList<PerformanceTest*>
    getPrereqs()
    {
        QList<PerformanceTest*> prereqs;
        if ( pop_ser != nullptr && pop_transeff != nullptr )
        {
            prereqs << pop_transeff << pop_ser;
        }
        return prereqs;
    }
};
};
#endif // ADVISER_RATING_WIDGET_H
