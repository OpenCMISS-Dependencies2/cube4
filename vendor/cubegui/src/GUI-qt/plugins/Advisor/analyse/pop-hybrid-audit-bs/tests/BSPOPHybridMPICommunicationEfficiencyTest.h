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


#ifndef BSPOP_HYBRID_MPI_COMMUNICATION_EFFICIENCY_TEST_H
#define BSPOP_HYBRID_MPI_COMMUNICATION_EFFICIENCY_TEST_H

#include <string>
#include "PerformanceTest.h"

namespace advisor
{
class BSPOPHybridMPICommunicationEfficiencyTest : public PerformanceTest
{
private:

    cube::Metric* max_runtime;
    cube::Metric* max_non_mpi_time;

    cube::list_of_metrics max_non_mpi_metrics;

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
               QString::fromStdString( "AdvisorBSPOPHybridTestsMPICommunication_efficiency.html" ) :
               QString::fromStdString( "AdvisorBSPOPHybridTestsMissing_MPIcommunication_efficiency.html" );
    }

    virtual
    void
    adjustForTest( cube::CubeProxy* cube ) const;


public:
    BSPOPHybridMPICommunicationEfficiencyTest(      cube::CubeProxy* );

    virtual
    ~BSPOPHybridMPICommunicationEfficiencyTest()
    {
    };
// ------ overview tests ---------

    bool
    isActive() const;

    bool
    isIssue() const;
};
};
#endif // ADVISER_RATING_WIDGET_H
