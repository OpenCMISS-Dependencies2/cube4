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
#include "PerformanceTest.h"

using namespace advisor;

std::string PerformanceTest::no_comment = QObject::tr( "-- no comment --" ).toStdString();


void
PerformanceTest::adjustForTest( cube::CubeProxy* cube ) const
{
    cube::Metric* _met = cube->getMetric( "__time_hierarchy_ghost" );
    if ( _met == nullptr )
    {
        _met = cube->defineMetric(
            "__time_hierarchy_ghost",
            "__time_hierarchy_ghost",
            "DOUBLE",
            tr( "sec" ).toUtf8().data(),
            "",
            TIME_METRIC_URL "#non_existent",
            tr( "Dummy metric to execute init sequence" ).toUtf8().data(),
            nullptr,
            cube::CUBE_METRIC_PREDERIVED_EXCLUSIVE,
            "1",
            cubepl_time_init_str,
            "",
            "",
            "",
            true,
            cube::CUBE_METRIC_GHOST
            );
        advisor_services->addMetric( _met, nullptr );
    }


    _met = cube->getMetric( "__knl_loops_ghost" );
    if ( _met == nullptr )
    {
        _met = cube->defineMetric(
            "__knl_loops_ghost",
            "__knl_loops_ghost",
            "DOUBLE",
            tr( "sec" ).toUtf8().data(),
            "",
            TIME_METRIC_URL "#non_existent",
            tr( "Dummy metric to execute init sequence for knl analysis" ).toUtf8().data(),
            nullptr,
            cube::CUBE_METRIC_PREDERIVED_EXCLUSIVE,
            "1",
            cubepl_knl_init_str,
            "",
            "",
            "",
            true,
            cube::CUBE_METRIC_GHOST
            );
        advisor_services->addMetric( _met, nullptr );
    }

    _met = cube->getMetric( "__service_dummy_metric_mpi_wait_time" );
    if ( _met == nullptr )
    {
        _met = cube->defineMetric(
            "__service_dummy_metric_mpi_wait_time",
            "__service_dummy_metric_mpi_wait_time",
            "DOUBLE",
            tr( "sec" ).toUtf8().data(),
            "",
            TIME_METRIC_URL "#non_existent",
            tr( "Dummy metric to display ${without_wait_state} variable" ).toUtf8().data(),
            nullptr,
            cube::CUBE_METRIC_PREDERIVED_EXCLUSIVE,
            "${without_wait_state}[${calculation::callpath::id}]",
            "",
            "",
            "",
            "",
            true,
            cube::CUBE_METRIC_GHOST
            );
        advisor_services->addMetric( _met, nullptr );
    }

    _met = cube->getMetric( "__service_parallel_marker" );
    if ( _met == nullptr )
    {
        _met = cube->defineMetric(
            "__service_parallel_marker",
            "__service_parallel_marker",
            "DOUBLE",
            "sec",
            "",
            TIME_METRIC_URL "#non_existent",
            tr( "Dummy metric to display ${without_wait_state} variable" ).toUtf8().data(),
            nullptr,
            cube::CUBE_METRIC_PREDERIVED_EXCLUSIVE,
            "${omp_comp}[${calculation::callpath::id}]",
            " {"
            "        ${i} = 0;"
            "        global(omp_comp);"
            "        while (${i} < ${cube::#callpaths})"
            "        {"
            "        ${omp_comp}[${i}]=0;"
            "        ${callpath} = ${i};"
            "        while( (${callpath} != -1) and ( not( (${cube::region::role}[${cube::callpath::calleeid}[${callpath}] ] eq \"parallel\" ) or (${cube::region::role}[${cube::callpath::calleeid}[${callpath}] ] eq \"task\" ) or (${cube::region::role}[${cube::callpath::calleeid}[${callpath}] ] eq \"task create\" )) ) )"
            "        {"
            "        ${callpath} = ${cube::callpath::parent::id}[${callpath}];"
            "        };"
            "        if (${callpath} != -1)"
            "        {"
            "        ${omp_comp}[${i}]=1;"
            "        };"
            "        ${i} = ${i} + 1;"
            "        };"
            "        return 0;"
            "}",
            "",
            "",
            "",
            true,
            cube::CUBE_METRIC_GHOST
            );
        advisor_services->addMetric( _met, nullptr );
    }
    _met = cube->getMetric( "__service_serial_marker" );
    if ( _met == nullptr )
    {
        _met = cube->defineMetric(
            "__service_serial_marker",
            "__service_serial_marker",
            "DOUBLE",
            "sec",
            "",
            TIME_METRIC_URL "#non_existent",
            tr( "Dummy metric to display ${without_wait_state} variable" ).toUtf8().data(),
            nullptr,
            cube::CUBE_METRIC_PREDERIVED_EXCLUSIVE,
            "1-${omp_comp}[${calculation::callpath::id}]",
            "",
            "",
            "",
            "",
            true,
            cube::CUBE_METRIC_GHOST
            );
        advisor_services->addMetric( _met, nullptr );
    }




    _met = cube->getMetric( "__service_counter_metric" );
    if ( _met == nullptr )
    {
        _met = cube->defineMetric(
            "__service_counter_metric",
            "__service_counter_metric",
            "DOUBLE",
            "sec",
            "",
            TIME_METRIC_URL "#non_existent",
            "Dummy metric to count parts of the dimension trees",
            nullptr,
            cube::CUBE_METRIC_PREDERIVED_EXCLUSIVE,
            "1/${calculation::callpath::#elements}",
            "",
            "",
            "",
            "",
            true,
            cube::CUBE_METRIC_GHOST
            );
        advisor_services->addMetric( _met, nullptr );
    }

    add_comp_time( cube );


    _met = cube->getMetric( "__time_hierarchy_deactivating_ghost" );
    if ( _met == nullptr )
    {
        _met = cube->defineMetric(
            "__time_hierarchy_deactivating_ghost",
            "__time_hierarchy_deactivating_ghost",
            "DOUBLE",
            "sec",
            "",
            TIME_METRIC_URL "#non_existent",
            "Dummy metric to execute init sequence",
            nullptr,
            cube::CUBE_METRIC_PREDERIVED_EXCLUSIVE,
            "1",
            cubepl_time_set_str,
            "",
            "",
            "",
            true,
            cube::CUBE_METRIC_GHOST
            );
        advisor_services->addMetric( _met, nullptr );
    }
}

bool
PerformanceTest::scout_metrics_available( cube::CubeProxy* cube ) const
{
    if ( cube->getMetric( "mpi_latesender" ) != nullptr )
    {
        return true;
    }
    if ( cube->getMetric( "mpi_latereceiver" ) != nullptr )
    {
        return true;
    }
    if ( cube->getMetric( "mpi_earlyreduce" ) != nullptr )
    {
        return true;
    }
    if ( cube->getMetric( "mpi_earlyscan" ) != nullptr )
    {
        return true;
    }
    if ( cube->getMetric( "mpi_latebroadcast" ) != nullptr )
    {
        return true;
    }
    if ( cube->getMetric( "mpi_wait_nxn" ) != nullptr )
    {
        return true;
    }
    if ( cube->getMetric( "mpi_barrier_wait" ) != nullptr )
    {
        return true;
    }
    if ( cube->getMetric( "mpi_finalize_wait" ) != nullptr )
    {
        return true;
    }
    return false;
}




void
PerformanceTest::add_mpi_comp_time( cube::CubeProxy* cube ) const
{
    cube::Metric* _execution = cube->getMetric( "execution" );
    cube::Metric* _mpi       = cube->getMetric( "mpi" );
    if ( _execution == nullptr )
    {
        add_execution_time( cube );
    }
    if ( _mpi == nullptr )
    {
        add_mpi_time( cube );
    }
    cube::Metric* _comp = cube->getMetric( "mpi_comp" );
    if ( _comp == nullptr )
    {
        cube::Metric* _met = cube->defineMetric(
            tr( "MPI Computation" ).toUtf8().data(),
            "mpi_comp",
            "DOUBLE",
            "sec",
            "",
            TIME_METRIC_URL "#comp",
            tr( "Time spent OUTSIDE of MPI routines." ).toUtf8().data(),
            nullptr,
            cube::CUBE_METRIC_POSTDERIVED,
            "metric::execution() - metric::mpi()",
            "",
            "",
            "",
            "",
            true,
            cube::CUBE_METRIC_GHOST
            );
        advisor_services->addMetric( _met, nullptr );
    }
}

void
PerformanceTest::add_comp_time( cube::CubeProxy* cube ) const
{
    add_max_time( cube );
    add_execution_time( cube );
    add_omp_time( cube );
    add_mpi_time( cube );
    add_mpi_indicator( cube );
    add_shmem_time( cube );
    add_pthread_time( cube );
    add_opencl_time( cube );
    add_cuda_time( cube );
    add_libwrap_time( cube );

    cube::Metric* _comp = cube->getMetric( "comp" );
    //bool          omp_as_ghost = false;
    if ( _comp == nullptr )
    {
        cube::Metric* _met = cube->defineMetric(
            tr( "Computation" ).toUtf8().data(),
            "comp",
            "DOUBLE",
            tr( "sec" ).toUtf8().data(),
            "",
            TIME_METRIC_URL "#comp",
            tr( "Time spent on computation" ).toUtf8().data(),
            nullptr,
            cube::CUBE_METRIC_POSTDERIVED,
            "metric::execution() - metric::mpi() - metric::shmem_time() - metric::omp_time() - metric::pthread_time() - metric::opencl_time() - metric::cuda_time() - metric::libwrap_time()",
            "",
            "",
            "",
            "",
            true,
            cube::CUBE_METRIC_GHOST
            );
        advisor_services->addMetric( _met, nullptr );
        //omp_as_ghost = true;
    }


    add_max_comp_time( cube );
    add_omp_execution( cube );
    add_max_omp_time( cube );
    add_omp_comp_time( cube );
    add_omp_non_wait_time( cube );
}


void
PerformanceTest::add_ser_comp_time( cube::CubeProxy* cube ) const
{
    cube::Metric* _met = cube->getMetric( "ser_comp_time" );
    if ( _met == nullptr )
    {
        _met = cube->defineMetric(
            tr( "Serial computation time" ).toUtf8().data(),
            "ser_comp_time",
            "DOUBLE",
            "sec",
            "",
            TIME_METRIC_URL "#serial_comp_time",
            tr( "Time spent on computation in serial part of calculation " ).toUtf8().data(),
            nullptr,
            cube::CUBE_METRIC_PREDERIVED_EXCLUSIVE,
            "(1-${omp_comp}[${calculation::callpath::id}])*metric::comp()",
            "",
            "",
            "",
            "",
            true,
            cube::CUBE_METRIC_GHOST
            );
        advisor_services->addMetric( _met, nullptr );
    }
    add_max_omp_and_ser_execution( cube );
}



void
PerformanceTest::add_omp_comp_time( cube::CubeProxy* cube, bool as_ghost ) const
{
    cube::Metric* _omp_comp_time = cube->getMetric( "omp_comp_time" );
    cube::Metric* _comp          = nullptr; // cube->getMetric( "comp" );
    if ( _omp_comp_time == nullptr )
    {
        cube::Metric* _met = cube->defineMetric(
            tr( "OMP computation time" ).toUtf8().data(),
            "omp_comp_time",
            "DOUBLE",
            "sec",
            "",
            TIME_METRIC_URL "#omp_comp_time",
            tr( "Time spent on computation within OpenMP regions" ).toUtf8().data(),
            _comp,
            cube::CUBE_METRIC_PREDERIVED_EXCLUSIVE,
            "${omp_comp}[${calculation::callpath::id}]*metric::comp()",
            "",
            "",
            "",
            "",
            true,
            ( as_ghost ) ? cube::CUBE_METRIC_GHOST : cube::CUBE_METRIC_NORMAL
            );
        advisor_services->addMetric( _met, nullptr /* advisor_services->getMetricTreeItem( "comp" ) */ );
    }

    add_ser_comp_time(  cube );
    add_max_omp_comp_time( cube );
    add_avg_omp_comp_time( cube );
}



void
PerformanceTest::add_max_time( cube::CubeProxy* cube ) const
{
    cube::Metric* _met = cube->getMetric( "max_time" );
    if ( _met == nullptr )
    {
        _met = cube->defineMetric(
            tr( "Maximal time, max_time" ).toUtf8().data(),
            "max_time",
            "DOUBLE",
            tr( "sec" ).toUtf8().data(),
            "",
            TIME_METRIC_URL "#execution",
            tr( "aximal run time time, max( time )" ).toUtf8().data(),
            nullptr,
            cube::CUBE_METRIC_PREDERIVED_EXCLUSIVE,
            "metric::time()",
            "",
            "",
            "",
            "max(arg1, arg2)",
            true,
            cube::CUBE_METRIC_GHOST
            );
        advisor_services->addMetric( _met, nullptr );
    }
}


void
PerformanceTest::add_execution_time( cube::CubeProxy* cube ) const
{
    cube::Metric* _met = cube->getMetric( "execution" );
    if ( _met == nullptr )
    {
        _met = cube->defineMetric(
            tr( "Execution" ).toUtf8().data(),
            "execution",
            "DOUBLE",
            tr( "sec" ).toUtf8().data(),
            "",
            TIME_METRIC_URL "#execution",
            tr( "Execution time (does not include time allocated for idle threads)" ).toUtf8().data(),
            nullptr,
            cube::CUBE_METRIC_PREDERIVED_EXCLUSIVE,
            "${execution}[${calculation::callpath::id}] * ( metric::time(e) - metric::omp_idle_threads(e) )",
            "",
            "",
            "",
            "",
            true,
            cube::CUBE_METRIC_GHOST
            );
        advisor_services->addMetric( _met, nullptr );
    }

    _met = cube->getMetric( "max_runtime" );
    if ( _met == nullptr )
    {
        _met = cube->defineMetric(
            tr( "Max Runtime" ).toUtf8().data(),
            "max_runtime",
            "DOUBLE",
            "sec",
            "",
            TIME_METRIC_URL "#max_runtime",
            tr( "Execution time (does not include time allocated for idle threads)" ).toUtf8().data(),
            nullptr,
            cube::CUBE_METRIC_POSTDERIVED,
            "metric::execution()",
            "",
            "",
            "",
            "max(arg1, arg2)",
            true,
            cube::CUBE_METRIC_GHOST
            );
        advisor_services->addMetric( _met, nullptr );
    }
}

void
PerformanceTest::add_max_comp_time( cube::CubeProxy* cube ) const
{
    cube::Metric* _met = cube->getMetric( "max_comp_time" );
    if ( _met == nullptr )
    {
        _met = cube->defineMetric(
            tr( "Max Computation Time" ).toUtf8().data(),
            "max_comp_time",
            "DOUBLE",
            "sec",
            "",
            TIME_METRIC_URL "#max_comp_time",
            tr( "Maximal Computation time over all locations" ).toUtf8().data(),
            nullptr,
            cube::CUBE_METRIC_POSTDERIVED,
            "metric::comp()",
            "",
            "",
            "",
            "max(arg1, arg2)",
            true,
            cube::CUBE_METRIC_GHOST
            );
        advisor_services->addMetric( _met, nullptr );
    }
}


void
PerformanceTest::add_mpi_time( cube::CubeProxy* cube ) const
{
    cube::Metric* _met = cube->getMetric( "mpi" );
    if ( _met == nullptr )
    {
        _met = cube->defineMetric(
            tr( "MPI" ).toUtf8().data(),
            "mpi",
            "DOUBLE",
            tr( "sec" ).toUtf8().data(),
            "",
            TIME_METRIC_URL "#mpi",
            tr( "Time spent in MPI calls" ).toUtf8().data(),
            nullptr,
            cube::CUBE_METRIC_PREDERIVED_EXCLUSIVE,
            "${mpi}[${calculation::callpath::id}] * ( metric::time(e) - metric::omp_idle_threads(e))",
            "",
            "",
            "",
            "",
            true,
            cube::CUBE_METRIC_GHOST
            );
        advisor_services->addMetric( _met, nullptr );
    }
    add_parallel_mpi_time( cube );
    add_serial_mpi_time( cube );
    add_non_mpi_time( cube );
}

void
PerformanceTest::add_non_mpi_time( cube::CubeProxy* cube ) const
{
    cube::Metric* _met = cube->getMetric( "non_mpi_time" );
    if ( _met == nullptr )
    {
        _met = cube->defineMetric(
            tr( "Not MPI Time" ).toUtf8().data(),
            "non_mpi_time",
            "DOUBLE",
            tr( "sec" ).toUtf8().data(),
            "",
            TIME_METRIC_URL "#mpi",
            tr( "Time spent in Non MPI calls" ).toUtf8().data(),
            nullptr,
            cube::CUBE_METRIC_PREDERIVED_EXCLUSIVE,
            "(1-${mpi}[${calculation::callpath::id}]) * ( metric::time(e) - metric::omp_idle_threads(e))",
            "",
            "",
            "",
            "",
            true,
            cube::CUBE_METRIC_GHOST
            );
        advisor_services->addMetric( _met, nullptr );
    }
    add_max_non_mpi_time( cube );
}

void
PerformanceTest::add_max_non_mpi_time( cube::CubeProxy* cube ) const
{
    cube::Metric* _met = cube->getMetric( "max_non_mpi_time" );
    if ( _met == nullptr )
    {
        _met = cube->defineMetric(
            tr( "Maximal Not MPI Time" ).toUtf8().data(),
            "max_non_mpi_time",
            "DOUBLE",
            tr( "sec" ).toUtf8().data(),
            "",
            TIME_METRIC_URL "#mpi",
            tr( "Maximal Time spent in Non MPI calls" ).toUtf8().data(),
            nullptr,
            cube::CUBE_METRIC_POSTDERIVED,
            "metric::non_mpi_time()",
            "",
            "",
            "",
            "max(arg1,arg2)",
            true,
            cube::CUBE_METRIC_GHOST
            );
        advisor_services->addMetric( _met, nullptr );
    }
}



void
PerformanceTest::add_mpi_indicator( cube::CubeProxy* cube ) const
{
    cube::Metric* _met = cube->getMetric( "mpi_indicator" );
    if ( _met == nullptr )
    {
        _met = cube->defineMetric(
            tr( "MPI Indicator" ).toUtf8().data(),
            "mpi_indicator",
            "DOUBLE",
            "",
            "",
            TIME_METRIC_URL "#mpi_indicator",
            tr( "1 if MPI has been executed on this call path, 0 - if no" ).toUtf8().data(),
            nullptr,
            cube::CUBE_METRIC_PREDERIVED_EXCLUSIVE,
            " ${mpi}[${calculation::callpath::id}] * ( metric::mpi(e,*) /metric::mpi(e,*) )",
            "",
            "max( arg1 , arg2)",
            "max( arg1 , arg2)",
            "max( arg1 , arg2)",
            true,
            cube::CUBE_METRIC_GHOST
            );
        advisor_services->addMetric( _met, nullptr );
    }
}



void
PerformanceTest::add_mpi_io_time( cube::CubeProxy* cube ) const
{
    add_mpi_io_individual_time( cube );
    add_mpi_io_collective_time( cube );
    cube::Metric* _met = cube->getMetric( "mpi_io" );
    if ( _met == nullptr )
    {
        _met = cube->defineMetric(
            tr( "MPI IO" ).toUtf8().data(),
            "mpi_io",
            "DOUBLE",
            tr( "sec" ).toUtf8().data(),
            "",
            TIME_METRIC_URL "#mpi_io",
            tr( "Time spent in MPI file I/O calls" ).toUtf8().data(),
            nullptr,
            cube::CUBE_METRIC_POSTDERIVED,
            "metric::mpi_io_individual() + metric::mpi_io_collective()",
            "",
            "",
            "",
            "",
            true,
            cube::CUBE_METRIC_GHOST
            );
        advisor_services->addMetric( _met, nullptr );
    }
}

void
PerformanceTest::add_mpi_io_individual_time( cube::CubeProxy* cube ) const
{
    cube::Metric* _met = cube->getMetric( "mpi_io_individual" );
    if ( _met == nullptr )
    {
        _met = cube->defineMetric(
            tr( "MPI Individual" ).toUtf8().data(),
            "mpi_io_individual",
            "DOUBLE",
            tr( "sec" ).toUtf8().data(),
            "",
            TIME_METRIC_URL "#mpi_io_individual",
            tr( "Time spent in individual MPI file I/O calls" ).toUtf8().data(),
            nullptr,
            cube::CUBE_METRIC_POSTDERIVED,
            "${mpi_file_individual}[${calculation::callpath::id}] * ( metric::time(e) - metric::omp_idle_threads(e) )",
            "",
            "",
            "",
            "",
            true,
            cube::CUBE_METRIC_GHOST
            );
        advisor_services->addMetric( _met, nullptr );
    }
}

void
PerformanceTest::add_mpi_io_collective_time( cube::CubeProxy* cube ) const
{
    cube::Metric* _met = cube->getMetric( "mpi_io_collective" );
    if ( _met == nullptr )
    {
        _met = cube->defineMetric(
            tr( "MPI Collective" ).toUtf8().data(),
            "mpi_io_collective",
            "DOUBLE",
            tr( "sec" ).toUtf8().data(),
            "",
            TIME_METRIC_URL "#mpi_io_collective",
            tr( "Time spent in collective MPI file I/O calls" ).toUtf8().data(),
            nullptr,
            cube::CUBE_METRIC_POSTDERIVED,
            "${mpi_file_collective}[${calculation::callpath::id}] * ( metric::time(e) - metric::omp_idle_threads(e) )",
            "",
            "",
            "",
            "",
            true,
            cube::CUBE_METRIC_GHOST
            );
        advisor_services->addMetric( _met, nullptr );
    }
}



void
PerformanceTest::add_wait_time_mpi( cube::CubeProxy* cube ) const
{
    cube::Metric* _met = cube->getMetric( "wait_time_mpi" );
    if ( _met == nullptr )
    {
        _met = cube->defineMetric(
            tr( "Waiting time in MPI" ).toUtf8().data(),
            "wait_time_mpi",
            "DOUBLE",
            tr( "sec" ).toUtf8().data(),
            "",
            TIME_METRIC_URL "#mpi_waiting_time",
            tr( "Waiting time in MPI, (mpi_latesender + mpi_latereceiver + mpi_earlyreduce + mpi_earlyscan + mpi_latebroadcast + mpi_wait_nxn + mpi_barrier_wait + mpi_finalize_wait)" ).toUtf8().data(),
            nullptr,
            cube::CUBE_METRIC_PREDERIVED_EXCLUSIVE,
            "metric::mpi_latesender() + metric::mpi_latereceiver() + metric::mpi_earlyreduce() + metric::mpi_earlyscan() + metric::mpi_latebroadcast() + metric::mpi_wait_nxn() + metric::mpi_barrier_wait() + metric::mpi_finalize_wait()",
            "",
            "",
            "",
            "",
            true,
            cube::CUBE_METRIC_GHOST
            );
        advisor_services->addMetric( _met, nullptr );
    }
}




void
PerformanceTest::add_shmem_time( cube::CubeProxy* cube ) const
{
    cube::Metric* _met = cube->getMetric( "shmem_time" );
    if ( _met == nullptr )
    {
        _met = cube->defineMetric(
            tr( "SHMEM" ).toUtf8().data(),
            "shmem_time",
            "DOUBLE",
            tr( "sec" ).toUtf8().data(),
            "",
            TIME_METRIC_URL "#shmem",
            tr( "Time spent in SHMEM calls" ).toUtf8().data(),
            nullptr,
            cube::CUBE_METRIC_PREDERIVED_EXCLUSIVE,
            "${shmem}[${calculation::callpath::id}] * ( metric::time(e) - metric::omp_idle_threads(e))",
            "",
            "",
            "",
            "",
            true,
            cube::CUBE_METRIC_GHOST
            );
        advisor_services->addMetric( _met, nullptr );
    }
}


void
PerformanceTest::add_omp_time( cube::CubeProxy* cube ) const
{
    cube::Metric* _met = cube->getMetric( "omp_time" );
    if ( _met == nullptr )
    {
        _met = cube->defineMetric(
            tr( "OpenMP" ).toUtf8().data(),
            "omp_time",
            "DOUBLE",
            tr( "sec" ).toUtf8().data(),
            "",
            TIME_METRIC_URL "#omp_time",
            tr( "Time spent in the OpenMP run-time system and API" ).toUtf8().data(),
            nullptr,
            cube::CUBE_METRIC_PREDERIVED_EXCLUSIVE,
            " metric::omp_management() +  "
            " ${omp}[${calculation::callpath::id}] * ( metric::time(e) - metric::omp_idle_threads(e)) ",
            "",
            "",
            "",
            "",
            true,
            cube::CUBE_METRIC_GHOST
            );
        advisor_services->addMetric( _met, nullptr );
    }
}



void
PerformanceTest::add_omp_non_wait_time( cube::CubeProxy* cube, bool as_ghost ) const
{
    add_parallel_execution_time( cube, as_ghost );
    cube::Metric* _met = cube->getMetric( "omp_non_wait_time" );
    if ( _met == nullptr )
    {
        _met = cube->defineMetric(
            "OMP non-wait time",
            "omp_non_wait_time",
            "DOUBLE",
            "sec",
            "",
            TIME_METRIC_URL "#omp_comp_time",
            "Time spent on computation within OpenMP regions",
            nullptr, //cube->getMetric( "par_execution_time" ),
            cube::CUBE_METRIC_PREDERIVED_EXCLUSIVE,
            "${omp_comp}[${calculation::callpath::id}]*(metric::comp())",
            "",
            "",
            "",
            "",
            true,
            ( as_ghost ) ? cube::CUBE_METRIC_GHOST : cube::CUBE_METRIC_NORMAL
            );
        advisor_services->addMetric( _met, nullptr /* advisor_services->getMetricTreeItem( "par_execution_time" ) */ );
    }
}


void
PerformanceTest::add_parallel_execution_time( cube::CubeProxy* cube, bool ) const
{
    add_execution_time( cube );
    cube::Metric* _met = cube->getMetric( "par_execution_time" );
    if ( _met == nullptr )
    {
        _met = cube->defineMetric(
            "Parallel execution time",
            "par_execution_time",
            "DOUBLE",
            "sec",
            "",
            TIME_METRIC_URL "#serial_comp_time",
            "Time spent on computation in parallel part of execution ",
            nullptr, //cube->getMetric( "execution" ),
            cube::CUBE_METRIC_PREDERIVED_EXCLUSIVE,
            "${omp_comp}[${calculation::callpath::id}]*metric::execution()",
            "",
            "",
            "",
            "",
            true,
            cube::CUBE_METRIC_GHOST
            );

        advisor_services->addMetric( _met, nullptr /* advisor_services->getMetricTreeItem( "execution" ) */ );
    }
}


void
PerformanceTest::add_parallel_mpi_time( cube::CubeProxy* cube, bool ) const
{
    cube::Metric* _met = cube->getMetric( "par_mpi_time" );
    if ( _met == nullptr )
    {
        _met = cube->defineMetric(
            "Parallel MPI time",
            "par_mpi_time",
            "DOUBLE",
            "sec",
            "",
            TIME_METRIC_URL "#parallel_mpi_time",
            "Time spent oin MPI in parallel part of execution ",
            nullptr, //cube->getMetric( "execution" ),
            cube::CUBE_METRIC_PREDERIVED_EXCLUSIVE,
            "${omp_comp}[${calculation::callpath::id}]*metric::mpi()",
            "",
            "",
            "",
            "",
            true,
            cube::CUBE_METRIC_GHOST
            );

        advisor_services->addMetric( _met, nullptr /* advisor_services->getMetricTreeItem( "execution" ) */ );
    }
}

void
PerformanceTest::add_serial_mpi_time( cube::CubeProxy* cube, bool ) const
{
    cube::Metric* _met = cube->getMetric( "ser_mpi_time" );
    if ( _met == nullptr )
    {
        _met = cube->defineMetric(
            "Serial MPI time",
            "ser_mpi_time",
            "DOUBLE",
            "sec",
            "",
            TIME_METRIC_URL "#serial_comp_time",
            "Time spent in MPI in serial part of execution ",
            nullptr, //cube->getMetric( "execution" ),
            cube::CUBE_METRIC_PREDERIVED_EXCLUSIVE,
            "(1-${omp_comp}[${calculation::callpath::id}])*metric::mpi()",
            "",
            "",
            "",
            "",
            true,
            cube::CUBE_METRIC_GHOST
            );

        advisor_services->addMetric( _met, nullptr /* advisor_services->getMetricTreeItem( "execution" ) */ );
    }
    add_max_serial_mpi_time( cube );
}

void
PerformanceTest::add_max_serial_mpi_time( cube::CubeProxy* cube, bool ) const
{
    cube::Metric* _met = cube->getMetric( "max_ser_mpi_time" );
    if ( _met == nullptr )
    {
        _met = cube->defineMetric(
            "Max Serial MPI time",
            "max_ser_mpi_time",
            "DOUBLE",
            "sec",
            "",
            TIME_METRIC_URL "#serial_comp_time",
            "Maximal time spent in MPI in serial part of execution ",
            nullptr, //cube->getMetric( "execution" ),
            cube::CUBE_METRIC_PREDERIVED_EXCLUSIVE,
            "(1-${omp_comp}[${calculation::callpath::id}])*metric::mpi()",
            "",
            "",
            "",
            "max(arg1,arg2)",
            true,
            cube::CUBE_METRIC_GHOST
            );

        advisor_services->addMetric( _met, nullptr /* advisor_services->getMetricTreeItem( "execution" ) */ );
    }
}



void
PerformanceTest::add_omp_execution( cube::CubeProxy* cube ) const
{
    cube::Metric* _met = cube->getMetric( "omp_execution_time" );
    if ( _met == nullptr )
    {
        _met = cube->defineMetric(
            tr( "OpenMP Execution" ).toUtf8().data(),
            "omp_execution_time",
            "DOUBLE",
            "sec",
            "",
            TIME_METRIC_URL "#omp_time",
            tr( "Time spent in the OpenMP region" ).toUtf8().data(),
            nullptr,
            cube::CUBE_METRIC_PREDERIVED_EXCLUSIVE,
            "${omp_comp}[${calculation::callpath::id}] * ( metric::execution() ) ",
            "",
            "",
            "",
            "",
            true,
            cube::CUBE_METRIC_GHOST
            );
        advisor_services->addMetric( _met, nullptr );
    }
}

void
PerformanceTest::add_max_omp_and_ser_execution( cube::CubeProxy* cube ) const
{
    cube::Metric* _met = cube->getMetric( "max_omp_serial_comp_time" );
    if ( _met == nullptr )
    {
        _met = cube->defineMetric(
            tr( "Max OpenMP Execution and Serial Execution" ).toUtf8().data(),
            "max_omp_serial_comp_time",
            "DOUBLE",
            "sec",
            "",
            TIME_METRIC_URL "#omp_time",
            tr( "Maximal" ).toUtf8().data(),
            nullptr,
            cube::CUBE_METRIC_POSTDERIVED,
            " metric::ser_comp_time() + metric::omp_execution_time()",
            "",
            "",
            "",
            "max(arg1, arg2)",
            true,
            cube::CUBE_METRIC_GHOST
            );
        advisor_services->addMetric( _met, nullptr );
    }
}


void
PerformanceTest::add_max_omp_time( cube::CubeProxy* ) const
{
    cube::Metric* _met = cube->getMetric( "max_omp_time" );
    if ( _met == nullptr )
    {
        _met = cube->defineMetric(
            tr( "Maximal OMP run time" ).toUtf8().data(),
            "max_omp_time",
            "DOUBLE",
            "sec",
            "",
            POP_LB_EFF_METRIC_URL,
            tr( "Maximal OpenMP time, max( comp )" ).toUtf8().data(),
            nullptr,
            cube::CUBE_METRIC_POSTDERIVED,
            "metric::omp_execution_time()",
            "",
            "",
            "",
            "max(arg1, arg2)",
            true,
            cube::CUBE_METRIC_GHOST
            );
        if ( _met != nullptr )
        {
            _met->setConvertible( false );
        }
        advisor_services->addMetric( _met, nullptr );
    }
}
void
PerformanceTest::add_max_omp_comp_time( cube::CubeProxy* ) const
{
    cube::Metric* _met = cube->getMetric( "max_omp_comp_time" );
    if ( _met == nullptr )
    {
        _met = cube->defineMetric(
            tr( "Maximal OMP computation time" ).toUtf8().data(),
            "max_omp_comp_time",
            "DOUBLE",
            "sec",
            "",
            POP_LB_EFF_METRIC_URL,
            tr( "Maximal OpenMP Computation time, max( comp )" ).toUtf8().data(),
            nullptr,
            cube::CUBE_METRIC_POSTDERIVED,
            "metric::omp_comp_time()",
            "",
            "",
            "",
            "max(arg1, arg2)",
            true,
            cube::CUBE_METRIC_GHOST
            );
        if ( _met != nullptr )
        {
            _met->setConvertible( false );
        }
        advisor_services->addMetric( _met, nullptr );
    }
}

void
PerformanceTest::add_avg_omp_comp_time( cube::CubeProxy* ) const
{
    cube::Metric* _met = cube->getMetric( "avg_omp_comp_time" );
    if ( _met == nullptr )
    {
        _met = cube->defineMetric(
            tr( "Average OMP computation runtime" ).toUtf8().data(),
            "avg_omp_comp_time",
            "DOUBLE",
            "sec",
            "",
            POP_LB_EFF_METRIC_URL,
            tr( "Average computation OpenMP time, max( comp )" ).toUtf8().data(),
            nullptr,
            cube::CUBE_METRIC_POSTDERIVED,
            "metric::omp_comp_time()/${cube::#locations}",
            "",
            "",
            "",
            "",
            true,
            cube::CUBE_METRIC_GHOST
            );
        if ( _met != nullptr )
        {
            _met->setConvertible( false );
        }
        advisor_services->addMetric( _met, nullptr );
    }
}




void
PerformanceTest::add_pthread_time( cube::CubeProxy* cube ) const
{
    cube::Metric* _met = cube->getMetric( "pthread_time" );
    if ( _met == nullptr )
    {
        _met = cube->defineMetric(
            tr( "POSIX threads" ).toUtf8().data(),
            "pthread_time",
            "DOUBLE",
            tr( "sec" ).toUtf8().data(),
            "",
            TIME_METRIC_URL "#pthread_time",
            tr( "Time spent in the POSIX threads API" ).toUtf8().data(),
            nullptr,
            cube::CUBE_METRIC_PREDERIVED_EXCLUSIVE,
            "${pthread}[${calculation::callpath::id}] * (metric::time(e) - metric::omp_idle_threads(e))",
            "",
            "",
            "",
            "",
            true,
            cube::CUBE_METRIC_GHOST
            );
        advisor_services->addMetric( _met, nullptr );
    }
}


void
PerformanceTest::add_opencl_time( cube::CubeProxy* cube ) const
{
    add_opencl_kernel_execution_time( cube );
    cube::Metric* _opencl_kernel_execution = cube->getMetric( "opencl_time" );
    if ( _opencl_kernel_execution == nullptr )
    {
        cube::Metric* _met = cube->defineMetric(
            tr( "OpenCL" ).toUtf8().data(),
            "opencl_time",
            "DOUBLE",
            tr( "sec" ).toUtf8().data(),
            "",
            TIME_METRIC_URL "#opencl_time",
            tr( "Time spent in the OpenCL run-time system, API and on device" ).toUtf8().data(),
            nullptr,
            cube::CUBE_METRIC_PREDERIVED_EXCLUSIVE,
            "${opencl}[${calculation::callpath::id}] * (metric::time(e) - metric::opencl_kernel_executions(e) - metric::omp_idle_threads(e))",
            "",
            "",
            "",
            "",
            true,
            cube::CUBE_METRIC_GHOST
            );
        advisor_services->addMetric( _met, nullptr );
    }
}

void
PerformanceTest::add_opencl_kernel_execution_time( cube::CubeProxy* cube ) const
{
    cube::Metric* _opencl_kernel_execution = cube->getMetric( "opencl_kernel_executions" );
    if ( _opencl_kernel_execution == nullptr )
    {
        cube::Metric* _met = cube->defineMetric(
            tr( "OpenCL kernels" ).toUtf8().data(),
            "opencl_kernel_executions",
            "DOUBLE",
            tr( "sec" ).toUtf8().data(),
            "",
            TIME_METRIC_URL "#opencl_kernel_executions",
            tr( "Time spent executing OpenCL kernels" ).toUtf8().data(),
            nullptr,
            cube::CUBE_METRIC_PREDERIVED_EXCLUSIVE,
            "${opencl_kernel_executions}[${calculation::callpath::id}] * ( metric::time(e) - metric::omp_idle_threads(e) )",
            "",
            "",
            "",
            "",
            true,
            cube::CUBE_METRIC_GHOST
            );
        advisor_services->addMetric( _met, nullptr );
    }
}


void
PerformanceTest::add_cuda_time( cube::CubeProxy* cube ) const
{
    add_cuda_kernel_execution_time( cube );
    cube::Metric* _met = cube->getMetric( "cuda_time" );
    if ( _met == nullptr )
    {
        _met = cube->defineMetric(
            tr( "CUDA" ).toUtf8().data(),
            "cuda_time",
            "DOUBLE",
            tr( "sec" ).toUtf8().data(),
            "",
            TIME_METRIC_URL "#cuda_time",
            tr( "Time spent in the CUDA run-time system, API and on device" ).toUtf8().data(),
            nullptr,
            cube::CUBE_METRIC_PREDERIVED_EXCLUSIVE,
            "${cuda}[${calculation::callpath::id}]* ( metric::time(e) - metric::cuda_kernel_executions(e) - metric::omp_idle_threads(e) )",
            "",
            "",
            "",
            "",
            true,
            cube::CUBE_METRIC_GHOST
            );
        advisor_services->addMetric( _met, nullptr );
    }
}


void
PerformanceTest::add_cuda_kernel_execution_time( cube::CubeProxy* cube ) const
{
    cube::Metric* _cuda_kernel_execution = cube->getMetric( "cuda_kernel_executions" );
    if ( _cuda_kernel_execution == nullptr )
    {
        cube::Metric* _met = cube->defineMetric(
            tr( "CUDA kernels" ).toUtf8().data(),
            "cuda_kernel_executions",
            "DOUBLE",
            tr( "sec" ).toUtf8().data(),
            "",
            TIME_METRIC_URL "#cuda_kernel_executions",
            tr( "Time spent executing CUDA kernels" ).toUtf8().data(),
            nullptr,
            cube::CUBE_METRIC_PREDERIVED_EXCLUSIVE,
            "${cuda_kernel_executions}[${calculation::callpath::id}] * ( metric::time(e) - metric::omp_idle_threads(e) )",
            "",
            "",
            "",
            "",
            true,
            cube::CUBE_METRIC_GHOST
            );
        advisor_services->addMetric( _met, nullptr );
    }
}



void
PerformanceTest::add_libwrap_time( cube::CubeProxy* cube ) const
{
    cube::Metric* _met = cube->getMetric( "libwrap_time" );
    if ( _met == nullptr )
    {
        _met = cube->defineMetric(
            tr( "Wrapped libraries" ).toUtf8().data(),
            "libwrap_time",
            "DOUBLE",
            tr( "sec" ).toUtf8().data(),
            "",
            TIME_METRIC_URL "#libwrap_time",
            tr( "Time spent in wrapped libraries" ).toUtf8().data(),
            nullptr,
            cube::CUBE_METRIC_PREDERIVED_EXCLUSIVE,
            "${libwrap}[${calculation::callpath::id}] * ( metric::time(e) - metric::omp_idle_threads(e) )",
            "",
            "",
            "",
            "",
            true,
            cube::CUBE_METRIC_GHOST
            );
        advisor_services->addMetric( _met, nullptr );
    }
}
