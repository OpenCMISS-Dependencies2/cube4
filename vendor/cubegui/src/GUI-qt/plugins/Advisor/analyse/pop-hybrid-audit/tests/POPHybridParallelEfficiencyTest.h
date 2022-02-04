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


#ifndef POP_HYBRID_PARALLEL_EFFICIENCY_TEST_H
#define POP_HYBRID_PARALLEL_EFFICIENCY_TEST_H

#include <string>
#include "PerformanceTest.h"
#include "POPHybridProcessEfficiencyTest.h"
#include "POPHybridThreadEfficiencyTest.h"

namespace advisor
{
class POPHybridParallelEfficiencyTest : public PerformanceTest
{
private:
    cube::Metric* pop_avg_comp;
    cube::Metric* max_runtime;

    cube::list_of_metrics max_runtime_metrics;


    POPHybridProcessEfficiencyTest* proc_eff;
    POPHybridThreadEfficiencyTest*  thread_eff;

    void
    calculate();

    void
    calculate( const cube::list_of_cnodes& );

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
               QString::fromStdString( "AdvisorPOPHybridTestsParallel_efficiency.html" ) :
               QString::fromStdString( "AdvisorPOPHybridTestsMissing_parallel_efficiency.html" );
    }

    virtual
    void
    adjustForTest( cube::CubeProxy* cube ) const;


public:
    POPHybridParallelEfficiencyTest(  cube::CubeProxy*,
                                      POPHybridProcessEfficiencyTest* proc_eff,
                                      POPHybridThreadEfficiencyTest*  thread_eff );

    virtual
    ~POPHybridParallelEfficiencyTest()
    {
    };
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
        if ( proc_eff != nullptr && thread_eff != nullptr )
        {
            prereqs << proc_eff << thread_eff;
        }
        return prereqs;
    }
};
};
#endif // ADVISER_RATING_WIDGET_H
