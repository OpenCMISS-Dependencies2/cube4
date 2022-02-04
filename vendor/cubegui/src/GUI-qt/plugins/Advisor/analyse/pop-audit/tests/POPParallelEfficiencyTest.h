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


#ifndef PARALLEL_EFFICIENCY_TEST_H
#define PARALLEL_EFFICIENCY_TEST_H

#include <string>
#include "PerformanceTest.h"
#include "POPImbalanceTest.h"
#include "POPCommunicationEfficiencyTest.h"

namespace advisor
{
class POPParallelEfficiencyTest : public PerformanceTest
{
private:
    cube::Metric* pop_pareff;

    POPImbalanceTest*               pop_lb;
    POPCommunicationEfficiencyTest* pop_commeff;

    void
    calculate();


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
               QString::fromStdString( "AdvisorPOPTestsParallel_efficiency.html" ) :
               QString::fromStdString( "AdvisorPOPTestsMissing_parallel_efficiency.html" );
    }

    virtual
    void
    adjustForTest( cube::CubeProxy* cube ) const;


public:
    POPParallelEfficiencyTest( POPImbalanceTest*               pop_lb,
                               POPCommunicationEfficiencyTest* pop_commeff );

    virtual
    ~POPParallelEfficiencyTest()
    {
    };

    bool
    isActive() const;

    bool
    isIssue() const;


    virtual
    QList<PerformanceTest*>
    getPrereqs()
    {
        QList<PerformanceTest*> prereqs;
        if ( pop_lb != nullptr && pop_commeff != nullptr )
        {
            prereqs << pop_lb << pop_commeff;
        }
        return prereqs;
    }
};
};
#endif // ADVISER_RATING_WIDGET_H
