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


#ifndef POP_HYBRID_OMP_REGION_TEST_ADD_H
#define POP_HYBRID_OMP_REGION_TEST_ADD_H

#include <string>
#include "PerformanceTest.h"


namespace advisor
{
class POPHybridOmpRegionEfficiencyTestAdd : public PerformanceTest
{
private:
    cube::Metric* pop_avg_omp;
    cube::Metric* pop_avg_omp_comp;
    cube::Metric* max_runtime;

    cube::list_of_metrics lavg_omp_metrics;
    cube::list_of_metrics lmax_runtime_metrics;

    void
    add_omp_lb_eff( cube::CubeProxy* ) const;

    void
    add_avg_omp_comp_time( cube::CubeProxy* ) const;

    void
    add_max_omp_comp_time( cube::CubeProxy* ) const;

protected:

    void
    applyCnode( const cube::list_of_cnodes& cnodes,
                const bool                  direct_calculation = false    );

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
               QString::fromStdString( "AdvisorPOPHybridAddTestsOmpRegion_efficiency.html" ) :
               QString::fromStdString( "AdvisorPOPHybridAddTestsMissing_omp_region_efficiency.html" );
    }

    virtual
    void
    adjustForTest( cube::CubeProxy* cube ) const;

public:
    POPHybridOmpRegionEfficiencyTestAdd( cube::CubeProxy* );

    virtual
    ~POPHybridOmpRegionEfficiencyTestAdd()
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
#endif // POPHybrid_IMBALANCE_TEST_H
