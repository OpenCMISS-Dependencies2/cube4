/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2015-2021                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include <config.h>

#include "VPUIntensityTest.h"

using namespace advisor;

VPUIntensityTest::VPUIntensityTest( cube::CubeProxy* cube ) : PerformanceTest( cube )
{
    setName( tr( "VPU Intensity" ).toUtf8().data() );
    setWeight( 1 );   // need to be adjusted
    vpu_intensity = cube->getMetric( "vpu_intensity" );
    if ( vpu_intensity == NULL )
    {
        adjustForTest( cube );
    }
    vpu_intensity = cube->getMetric( "vpu_intensity" );
    if ( vpu_intensity == NULL )
    {
        setWeight( 0.2 );
        setValue( 0. );
        return;
    }
    cube::metric_pair metric;
    metric.first  = vpu_intensity;
    metric.second = cube::CUBE_CALCULATE_INCLUSIVE;
    lmetrics.push_back( metric );
}


void
VPUIntensityTest::applyCnode( const cube::Cnode* cnode, const cube::CalculationFlavour cnf,
                              const bool direct_calculation  )
{
    ( void )direct_calculation; // not used here
    if ( vpu_intensity == NULL )
    {
        return;
    }

    cube::list_of_sysresources lsysres;
    cube::list_of_cnodes       lcnodes;

    cube::cnode_pair pcnode = std::make_pair( const_cast<cube::Cnode*>( cnode ), cnf );
    lcnodes.push_back( pcnode );

    cube::Value* v = cube->calculateValue( lmetrics,
                                           lcnodes,
                                           lsysres );
    double vpu_intensity_value = v->getDouble();
    delete v;
    setValue( vpu_intensity_value );
}

void
VPUIntensityTest::applyCnode( const cube::list_of_cnodes& lcnodes,
                              const bool                  direct_calculation  )

{
    ( void )direct_calculation; // not used here
    if ( vpu_intensity == NULL )
    {
        return;
    }
    if ( lcnodes.size() == 1 )
    {
        applyCnode( lcnodes[ 0 ].first, lcnodes[ 0 ].second );
    }
    else
    {
        cube::list_of_sysresources lsysres;
        cube::Value*               v = cube->calculateValue( lmetrics,
                                                             lcnodes,
                                                             lsysres );
        double vpu_intensity_value = v->getDouble();
        delete v;
        setValue( vpu_intensity_value );
    }
}


const std::string&
VPUIntensityTest::getCommentText() const
{
    if ( vpu_intensity == NULL )
    {
        return no_comment;
    }
    else
    {
        return no_comment;
    }
}


// ------ overview tests ---------

bool
VPUIntensityTest::isActive() const
{
    return vpu_intensity != NULL;
};

bool
VPUIntensityTest::isIssue() const
{
    return false;
};

void
VPUIntensityTest::adjustForTest( cube::CubeProxy* cube ) const
{
    cube::Metric* uops_packed_simd_without_wait       = cube->getMetric( "uops_packed_simd_without_wait" );
    cube::Metric* uops_scalar_simd_without_wait       = cube->getMetric( "uops_scalar_simd_without_wait" );
    cube::Metric* uops_packed_simd_loops_without_wait = cube->getMetric( "uops_packed_simd_loops_without_wait" );
    cube::Metric* uops_scalar_simd_loops_without_wait = cube->getMetric( "uops_scalar_simd_loops_without_wait" );
    if ( uops_packed_simd_without_wait == NULL )
    {
        add_uops_packed_simd_without_wait( cube );
    }
    if ( uops_scalar_simd_without_wait == NULL )
    {
        add_uops_scalar_simd_without_wait( cube );
    }
    if ( uops_packed_simd_loops_without_wait == NULL )
    {
        add_uops_packed_simd_loops_without_wait( cube );
    }
    if ( uops_scalar_simd_loops_without_wait == NULL )
    {
        add_uops_scalar_simd_loops_without_wait( cube );
    }

    uops_packed_simd_without_wait       = cube->getMetric( "uops_packed_simd_without_wait" );
    uops_scalar_simd_without_wait       = cube->getMetric( "uops_scalar_simd_without_wait" );
    uops_packed_simd_loops_without_wait = cube->getMetric( "uops_packed_simd_loops_without_wait" );
    uops_scalar_simd_loops_without_wait = cube->getMetric( "uops_scalar_simd_loops_without_wait" );
    if ( uops_packed_simd_loops_without_wait != NULL && uops_scalar_simd_without_wait != NULL )
    {
        add_vpu_intensity_all( cube );
    }
    if ( uops_scalar_simd_loops_without_wait != NULL && uops_scalar_simd_without_wait != NULL )
    {
        add_vpu_intensity( cube );
    }
}

void
VPUIntensityTest::add_vpu_intensity_all( cube::CubeProxy* ) const
{
    cube::Metric* _met = cube->getMetric( "vpu_intensity_all" );
    if ( _met == NULL )
    {
        cube::Metric* _met = cube->defineMetric(
            tr( "VPU Intensity for all callpaths" ).toUtf8().data(),
            "vpu_intensity_all",
            "DOUBLE",
            "",
            "",
            KNL_VPU_INTENSITY_ALL_URL,
            tr( "VPU Intensity for all call paths" ).toUtf8().data(),
            NULL,
            cube::CUBE_METRIC_POSTDERIVED,
            "metric::uops_packed_simd_without_wait() / (metric::uops_packed_simd_without_wait() + metric::uops_scalar_simd_without_wait() )",
            "",
            "",
            "",
            "",
            true,
            cube::CUBE_METRIC_GHOST
            );
        if ( _met != NULL )
        {
            _met->setConvertible( false );
        }
        advisor_services->addMetric( _met, NULL );
    }
}



void
VPUIntensityTest::add_vpu_intensity( cube::CubeProxy* ) const
{
    cube::Metric* _met = cube->getMetric( "vpu_intensity" );
    if ( _met == NULL )
    {
        cube::Metric* _met = cube->defineMetric(
            tr( "VPU Intensity for loops" ).toUtf8().data(),
            "vpu_intensity",
            "DOUBLE",
            "",
            "",
            KNL_VPU_INTENSITY_ALL_URL,
            tr( "VPU Intensity for loops" ).toUtf8().data(),
            NULL,
            cube::CUBE_METRIC_POSTDERIVED,
            "metric::uops_packed_simd_loops_without_wait() / (metric::uops_packed_simd_loops_without_wait() + metric::uops_scalar_simd_loops_without_wait() )",
            "",
            "",
            "",
            "",
            true,
            cube::CUBE_METRIC_GHOST
            );
        if ( _met != NULL )
        {
            _met->setConvertible( false );
        }
        advisor_services->addMetric( _met, NULL );
    }
}



void
VPUIntensityTest::add_uops_packed_simd_without_wait( cube::CubeProxy* ) const
{
    cube::Metric* _met = cube->getMetric( "UOPS_RETIRED:PACKED_SIMD" );
    if ( _met != NULL )
    {
        cube::Metric* _met = cube->defineMetric(
            "UOPS_RETIRED_PACKED_SIMD_WITHOUT_WAIT",
            "uops_packed_simd_without_wait",
            "DOUBLE",
            "",
            "",
            KNL_UOPS_RETIRED_PACKED_SMD_URL,
            tr( "Here is UOPS_RETIRED:PACKED_SIMD without busy-wait in MPI and OpenMP." ).toUtf8().data(),
            NULL,
            cube::CUBE_METRIC_PREDERIVED_EXCLUSIVE,
            "${without_wait_state}[${calculation::callpath::id}] * metric::UOPS_RETIRED:PACKED_SIMD()",
            "",
            "",
            "",
            "",
            true,
            cube::CUBE_METRIC_GHOST
            );
        if ( _met != NULL )
        {
            _met->setConvertible( false );
        }
        advisor_services->addMetric( _met, NULL );
    }
}


void
VPUIntensityTest::add_uops_scalar_simd_without_wait( cube::CubeProxy* ) const
{
    cube::Metric* _met = cube->getMetric( "UOPS_RETIRED:SCALAR_SIMD" );
    if ( _met != NULL )
    {
        cube::Metric* _met = cube->defineMetric(
            "UOPS_RETIRED_SCALAR_SIMD_WITHOUT_WAIT",
            "uops_scalar_simd_without_wait",
            "DOUBLE",
            "",
            "",
            KNL_UOPS_RETIRED_SCALAR_SMD_URL,
            tr( "Here is UOPS_RETIRED:SCALAR_SIMD without busy-wait in MPI and OpenMP." ).toUtf8().data(),
            NULL,
            cube::CUBE_METRIC_PREDERIVED_EXCLUSIVE,
            "${without_wait_state}[${calculation::callpath::id}] * metric::UOPS_RETIRED:SCALAR_SIMD()",
            "",
            "",
            "",
            "",
            true,
            cube::CUBE_METRIC_GHOST
            );
        if ( _met != NULL )
        {
            _met->setConvertible( false );
        }
        advisor_services->addMetric( _met, NULL );
    }
}



void
VPUIntensityTest::add_uops_packed_simd_loops_without_wait( cube::CubeProxy* ) const
{
    cube::Metric* _met = cube->getMetric( "UOPS_RETIRED:PACKED_SIMD" );
    if ( _met != NULL )
    {
        cube::Metric* _met = cube->defineMetric(
            "UOPS_RETIRED_PACKED_SIMD_WITHOUT_WAIT_LOOPS",
            "uops_packed_simd_loops_without_wait",
            "DOUBLE",
            "",
            "",
            KNL_UOPS_RETIRED_PACKED_SMD_URL,
            tr( "Here is UOPS_RETIRED:PACKED_SIMD without busy-wait in MPI and OpenMP." ).toUtf8().data(),
            NULL,
            cube::CUBE_METRIC_PREDERIVED_EXCLUSIVE,
            "${without_wait_state}[${calculation::callpath::id}] * ${loop}[${calculation::callpath::id}] * metric::UOPS_RETIRED:PACKED_SIMD()",
            "",
            "",
            "",
            "",
            true,
            cube::CUBE_METRIC_GHOST
            );
        if ( _met != NULL )
        {
            _met->setConvertible( false );
        }
        advisor_services->addMetric( _met, NULL );
    }
}

void
VPUIntensityTest::add_uops_scalar_simd_loops_without_wait( cube::CubeProxy* ) const
{
    cube::Metric* _met = cube->getMetric( "UOPS_RETIRED:SCALAR_SIMD" );
    if ( _met != NULL )
    {
        cube::Metric* _met = cube->defineMetric(
            "UOPS_RETIRED_SCALAR_SIMD_WITHOUT_WAIT_LOOPS",
            "uops_scalar_simd_loops_without_wait",
            "DOUBLE",
            "",
            "",
            KNL_UOPS_RETIRED_SCALAR_SMD_URL,
            tr( "Here is UOPS_RETIRED:SCALAR_SIMD without busy-wait in MPI and OpenMP." ).toUtf8().data(),
            NULL,
            cube::CUBE_METRIC_PREDERIVED_EXCLUSIVE,
            "${without_wait_state}[${calculation::callpath::id}] * ${loop}[${calculation::callpath::id}] * metric::UOPS_RETIRED:SCALAR_SIMD()",
            "",
            "",
            "",
            "",
            true,
            cube::CUBE_METRIC_GHOST
            );
        if ( _met != NULL )
        {
            _met->setConvertible( false );
        }
        advisor_services->addMetric( _met, NULL );
    }
}
