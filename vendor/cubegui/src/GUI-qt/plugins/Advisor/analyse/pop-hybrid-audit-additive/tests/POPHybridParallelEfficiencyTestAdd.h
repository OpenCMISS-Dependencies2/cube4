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


#ifndef POP_HYBRID_PARALLEL_EFFICIENCY_TEST_ADD_H
#define POP_HYBRID_PARALLEL_EFFICIENCY_TEST_ADD_H

#include <string>
#include "PerformanceTest.h"
#include "POPHybridProcessEfficiencyTestAdd.h"
#include "POPHybridThreadEfficiencyTestAdd.h"

namespace advisor
{
class POPHybridParallelEfficiencyTestAdd : public PerformanceTest
{
private:
    cube::Metric* pop_avg_comp;
    cube::Metric* max_runtime;

    cube::list_of_metrics max_runtime_metrics;


    POPHybridProcessEfficiencyTestAdd* proc_eff;
    POPHybridThreadEfficiencyTestAdd*  thread_eff;

    void
    calculate();

    void
    calculate( const cube::list_of_cnodes& );


protected:

    void
    applyCnode( const cube::list_of_cnodes& cnodes,
                const bool                  direct_calculation = false   );

    void
    applyCnode( const cube::Cnode*             cnode,
                const cube::CalculationFlavour cnf = cube::CUBE_CALCULATE_INCLUSIVE,
                const bool                     direct_calculation = false  );

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
               QString::fromStdString( "AdvisorPOPHybridAddTestsParallel_efficiency.html" ) :
               QString::fromStdString( "AdvisorPOPHybridAddTestsMissing_parallel_efficiency.html" );
    }

    virtual
    void
    adjustForTest( cube::CubeProxy* cube ) const;


public:
    POPHybridParallelEfficiencyTestAdd( cube::CubeProxy*,
                                        POPHybridProcessEfficiencyTestAdd* proc_eff,
                                        POPHybridThreadEfficiencyTestAdd*  thread_eff );

    virtual
    ~POPHybridParallelEfficiencyTestAdd()
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
