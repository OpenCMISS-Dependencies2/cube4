/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2015-2018                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef VPU_INTENSITY_TEST_H
#define VPU_INTENSITY_TEST_H

#include <string>
#include "PerformanceTest.h"


namespace advisor
{
class VPUIntensityTest : public PerformanceTest
{
private:
    cube::Metric* vpu_intensity;

    void
    add_vpu_intensity_all( cube::CubeProxy* ) const;

    void
    add_vpu_intensity( cube::CubeProxy* ) const;

    void
    add_uops_packed_simd_without_wait( cube::CubeProxy*  ) const;

    void
    add_uops_scalar_simd_without_wait( cube::CubeProxy*  ) const;

    void
    add_uops_packed_simd_loops_without_wait( cube::CubeProxy*  ) const;

    void
    add_uops_scalar_simd_loops_without_wait( cube::CubeProxy*  ) const;


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
               QString::fromStdString( "AdvisorKNLTestsVPUIntensity.html" ) :
               QString::fromStdString( "AdvisorKNLTestsMissingVPUIntensity.html" );
    }

    virtual
    void
    adjustForTest( cube::CubeProxy* cube ) const;

    virtual
    inline
    bool
    isPercent() const
    {
        return true;
    }
public:
    VPUIntensityTest( cube::CubeProxy* );

    virtual
    ~VPUIntensityTest()
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
