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


#ifndef POP_HYBRID_THREAD_EFFICIENCY_TEST_ADD_H
#define POP_HYBRID_THREAD_EFFICIENCY_TEST_ADD_H

#include <string>
#include "PerformanceTest.h"
#include "POPHybridOMPRegionEfficiencyTestAdd.h"
#include "POPHybridAmdahlTestAdd.h"

namespace advisor
{
class POPHybridThreadEfficiencyTestAdd : public PerformanceTest
{
private:
    POPHybridAmdahlTestAdd*              amdahl_eff;
    POPHybridOmpRegionEfficiencyTestAdd* omp_region_eff;

    cube::Metric* pop_ser_comp_comp;
    cube::Metric* max_runtime;
    cube::Metric* pop_avg_omp;
    cube::Metric* pop_avg_omp_comp;

    cube::list_of_metrics lavg_omp_metrics;
    cube::list_of_metrics lavg_omp_comp_metrics;
    cube::list_of_metrics lavg_ser_metrics;
    cube::list_of_metrics lmax_runtime_metrics;
    void
    calculate();

    void
    calculate( const cube::list_of_cnodes& cnodes );

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
               QString::fromStdString( "AdvisorPOPHybridAddTestsThread_efficiency.html" ) :
               QString::fromStdString( "AdvisorPOPHybridAddTestsMissing_thread_efficiency.html" );
    }

    virtual
    void
    adjustForTest( cube::CubeProxy* cube ) const;


public:
    POPHybridThreadEfficiencyTestAdd(   cube::CubeProxy*,
                                        POPHybridAmdahlTestAdd*              amdahl_eff,
                                        POPHybridOmpRegionEfficiencyTestAdd* omp_region_eff
                                        );

    virtual
    ~POPHybridThreadEfficiencyTestAdd()
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
        if ( amdahl_eff != nullptr && omp_region_eff != nullptr )
        {
            prereqs << amdahl_eff << omp_region_eff;
        }
        return prereqs;
    }
};
};
#endif // ADVISER_RATING_WIDGET_H
