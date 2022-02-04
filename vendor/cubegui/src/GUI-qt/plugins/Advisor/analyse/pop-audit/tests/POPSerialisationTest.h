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


#ifndef POP_SERIALISATION_TEST_H
#define POP_SERIALISATION_TEST_H

#include <string>
#include "PerformanceTest.h"


namespace advisor
{
class POPSerialisationTest : public PerformanceTest
{
private:
    cube::Metric* max_total_time_ideal;
    cube::Metric* comp_time;

    cube::list_of_metrics lmax_total_time_ideal;

    void
    add_transfer_time_mpi( cube::CubeProxy* ) const;

    void
    add_max_total_time_ideal( cube::CubeProxy* ) const;

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
               QString::fromStdString( "AdvisorPOPTestsSerialisation_efficiency.html" ) :
               QString::fromStdString( "AdvisorPOPTestsMissing_serialisation_efficiency.html" );
    }

    virtual
    void
    adjustForTest( cube::CubeProxy* cube ) const;

public:
    POPSerialisationTest( cube::CubeProxy* );

    virtual
    ~POPSerialisationTest()
    {
    };

    virtual
    double
    analyze( const cube::list_of_cnodes& cnodes,
             cube::LocationGroup*        _lg = nullptr ) const;

    bool
    isActive() const;

    bool
    isIssue() const;
};
};
#endif // POP_IMBALANCE_TEST_H
