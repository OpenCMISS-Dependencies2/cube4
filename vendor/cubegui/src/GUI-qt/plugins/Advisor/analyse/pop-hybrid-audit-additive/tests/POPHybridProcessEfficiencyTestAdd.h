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


#ifndef POP_HYBRID_PROCESS_EFFICIENCY_TEST_ADD_H
#define POP_HYBRID_PROCESS_EFFICIENCY_TEST_ADD_H

#include <string>
#include "PerformanceTest.h"
#include "POPHybridImbalanceTestAdd.h"
#include "POPHybridCommunicationEfficiencyTestAdd.h"

namespace advisor
{
class POPHybridProcessEfficiencyTestAdd : public PerformanceTest
{
private:


    POPHybridImbalanceTestAdd*               pop_lb;
    POPHybridCommunicationEfficiencyTestAdd* pop_commeff;


    cube::Metric* max_runtime;
    cube::Metric* pop_avg_omp;
    cube::Metric* pop_avg_ser_comp;

    cube::list_of_metrics max_runtime_metrics;
    cube::list_of_metrics lavg_omp_metrics;
    cube::list_of_metrics lavg_ser_metrics;

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
               QString::fromStdString( "AdvisorPOPHybridAddTestsProcess_efficiency.html" ) :
               QString::fromStdString( "AdvisorPOPHybridAddTestsMissing_process_efficiency.html" );
    }

    virtual
    void
    adjustForTest( cube::CubeProxy* cube ) const;


public:
    POPHybridProcessEfficiencyTestAdd(      cube::CubeProxy*,
                                            POPHybridImbalanceTestAdd*               pop_lb,
                                            POPHybridCommunicationEfficiencyTestAdd* pop_commeff );

    virtual
    ~POPHybridProcessEfficiencyTestAdd()
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
        if ( pop_lb != nullptr && pop_commeff != nullptr )
        {
            prereqs << pop_lb << pop_commeff;
        }
        return prereqs;
    }
};
};
#endif // ADVISER_RATING_WIDGET_H
