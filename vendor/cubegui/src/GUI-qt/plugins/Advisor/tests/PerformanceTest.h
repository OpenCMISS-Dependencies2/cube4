/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2015-2019                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef PERFORMANCE_TEST_H
#define PERFORMANCE_TEST_H

#include <string>
#include <QObject>
#include <QList>
#include "CubeProxy.h"
#include "CubeTypes.h"
#include "CubeIdIndexMap.h"
#include "PluginServices.h"
#include "PerformanceTest_incl.h"

extern cubepluginapi::PluginServices* advisor_services;

namespace advisor
{
class PerformanceTest : public QObject
{
protected:

    cube::CubeProxy*      cube;
    cube::Cnode*          root_cnode;
    std::string           test_name;
    std::string           test_comment;
    double                test_value;
    double                test_min_value;
    double                test_max_value;
    double                test_weight;
    static std::string    no_comment;
    cube::value_container inclusive_values;
    cube::value_container exclusive_values;
    cube::list_of_metrics lmetrics;
    bool                  single_value;

    virtual
    void
    applyCnode( const cube::list_of_cnodes& cnodes,
                const bool                  direct_calculation = false ) = 0;

    virtual
    void
    applyCnode( const cube::Cnode*             cnode,
                const cube::CalculationFlavour cnf = cube::CUBE_CALCULATE_INCLUSIVE,
                const bool                     direct_calculation = false ) = 0;

    virtual
    const std::string&
    getCommentText() const = 0; // every test should deliver comment to its value and some suggestion of improvements.


    void
    setWeight( const double& _w )
    {
        test_weight = _w;
    }

    void
    setValue( const double& _v )
    {
        test_value     = _v;
        test_min_value = _v;
        test_max_value = _v;
    }

    void
    setValues( const double& _v, const double& _min_v, const double& _max_v )
    {
        test_value     = _v;
        test_min_value = _min_v;
        test_max_value = _max_v;
        single_value   = false;
    }

    void
    setComment( const std::string& _c )
    {
        test_comment = _c;
    }

    void
    setName( const std::string& _n )
    {
        test_name = _n;
    }

    void
    findRoot()
    {
        const std::vector<cube::Cnode*>& cnodes = cube->getRootCnodes();
        if ( cnodes.size() == 1 )
        {
            root_cnode = cnodes[ 0 ];
            return;
        }
        for ( std::vector<cube::Cnode*>::const_iterator iter = cnodes.begin(); iter != cnodes.end(); ++iter )
        {
            if ( ( ( *iter )->get_callee()->get_name().compare( "main" ) == 0 ) ||
                 ( ( *iter )->get_callee()->get_name().compare( "MAIN" ) == 0 ) )
            {
                root_cnode = *iter;
                return;
            }
        }
        root_cnode = nullptr;
    }

    cube::list_of_sysresources
    getRootsOfSystemTree()
    {
        cube::list_of_sysresources           lsysres;
        std::vector< cube::SystemTreeNode* > sysress = cube->getRootSystemTreeNodes();
        for ( std::vector< cube::SystemTreeNode* >::iterator iter = sysress.begin(); iter != sysress.end(); ++iter )
        {
            cube::sysres_pair sres;
            sres.first  = *iter;
            sres.second = cube::CUBE_CALCULATE_INCLUSIVE;
            lsysres.push_back( sres );
        }
        return lsysres;
    }


    virtual
    void
    adjustForTest( cube::CubeProxy* cube ) const;

    bool
    scout_metrics_available( cube::CubeProxy* cube ) const;

// --- calls to build Time hierarhy. Used by derivative classes

    void
    add_comp_time( cube::CubeProxy* cube ) const;

    void
    add_mpi_comp_time( cube::CubeProxy* cube ) const;

    void
    add_omp_comp_time( cube::CubeProxy* cube,
                       bool             as_ghot = true  ) const;

    void
    add_omp_non_wait_time( cube::CubeProxy* cube,
                           bool             as_ghost = true ) const;

    void
    add_parallel_execution_time( cube::CubeProxy* cube,
                                 bool             as_ghost = true ) const;

    void
    add_parallel_mpi_time( cube::CubeProxy* cube,
                           bool             as_ghost = true ) const;
    void
    add_serial_mpi_time( cube::CubeProxy* cube,
                         bool             as_ghost = true ) const;
    void
    add_max_serial_mpi_time( cube::CubeProxy* cube,
                             bool             as_ghost = true ) const;

    void
    add_ser_comp_time( cube::CubeProxy* cube ) const;

    void
    add_max_comp_time( cube::CubeProxy* cube ) const;

    void
    add_max_omp_comp_time( cube::CubeProxy* cube ) const;

    void
    add_execution_time( cube::CubeProxy* cube ) const;

    void
    add_max_omp_and_ser_execution( cube::CubeProxy* cube ) const;

    void
    add_max_time( cube::CubeProxy* cube ) const;

    void
    add_mpi_time( cube::CubeProxy* cube ) const;

    void
    add_non_mpi_time( cube::CubeProxy* cube ) const;

    void
    add_max_non_mpi_time( cube::CubeProxy* cube ) const;

    void
    add_mpi_indicator( cube::CubeProxy* cube ) const;

    void
    add_mpi_io_time( cube::CubeProxy* cube ) const;

    void
    add_mpi_io_individual_time( cube::CubeProxy* cube ) const;

    void
    add_mpi_io_collective_time( cube::CubeProxy* cube ) const;

    void
    add_wait_time_mpi( cube::CubeProxy* cube ) const;

    void
    add_shmem_time( cube::CubeProxy* cube ) const;

    void
    add_omp_time( cube::CubeProxy* cube ) const;

    void
    add_omp_execution( cube::CubeProxy* cube ) const;

    void
    add_max_omp_time( cube::CubeProxy* cube ) const;

    void
    add_avg_omp_comp_time( cube::CubeProxy* cube ) const;

    void
    add_pthread_time( cube::CubeProxy* cube ) const;

    void
    add_opencl_time( cube::CubeProxy* cube ) const;

    void
    add_opencl_kernel_execution_time( cube::CubeProxy* cube ) const;

    void
    add_cuda_time( cube::CubeProxy* cube ) const;

    void
    add_cuda_kernel_execution_time( cube::CubeProxy* cube ) const;

    void
    add_libwrap_time( cube::CubeProxy* cube ) const;

public:
    PerformanceTest( cube::CubeProxy* _cube ) : QObject()
    {
        cube = _cube;
        if ( cube != nullptr )
        {
            findRoot();
            adjustForTest( _cube );
        }
        lmetrics.clear();
        setValue( 0 );
        setWeight( 0 );
        single_value = true;
    }

    virtual
    ~PerformanceTest()
    {
    };

    void
    apply( const cube::list_of_cnodes& cnodes, const bool direct_calculation = false )
    {
        applyCnode( cnodes, direct_calculation );
        setComment( getCommentText() );
    }

    void
    apply( const cube::Cnode*             cnode,
           const cube::CalculationFlavour cnf = cube::CUBE_CALCULATE_INCLUSIVE,
           const bool                     direct_calculation = false  )
    {
        applyCnode( cnode, cnf, direct_calculation );
        setComment( getCommentText() );
    }


    virtual
    double
    analyze( const cube::list_of_cnodes& cnodes,
             cube::LocationGroup*        _lg = nullptr ) const
    {
        ( void )cnodes;
        ( void )_lg;
        return 0;
    }

    virtual
    QList<PerformanceTest*>
    getPrereqs()
    {
        return QList<PerformanceTest*>();
    }

    bool
    isSingleValue() const
    {
        return single_value;
    }

    inline
    double
    value() const
    {
        return test_value;
    }


    inline
    double
    min_value() const
    {
        return test_min_value;
    }

    inline
    double
    max_value() const
    {
        return test_max_value;
    }



    inline
    double
    weight() const
    {
        return test_weight;
    }

    inline
    const
    std::string&
    comment() const
    {
        return test_comment;
    }

    inline
    const
    std::string&
    name() const
    {
        return test_name;
    }


    virtual
    inline
    double
    getMinimum() const
    {
        return 0.;
    }

    virtual
    inline
    double
    getMaximum() const
    {
        return 1.;
    }

    virtual
    inline
    bool
    isPercent() const
    {
        return false;
    }

    virtual
    inline
    bool
    isRegular() const // returns true, if 0 to 1 indicates improvement, false - otherwise
    {
        return true;
    }

    virtual
    inline
    bool
    isNormalized() const // returns true, if range goes from 0 to 1. false -> otherwise
    {
        return true;
    }



    inline
    virtual
    const
    QString
    getHelpUrl()
    {
        return ( isActive() ) ?
               QString::fromStdString( "PerformanceTest" ) :
               QString::fromStdString( "MissingPerformanceTest" );
    }

    inline
    virtual
    const
    QString
    units()
    {
        return "";
    }



// ------ overview tests ---------

    virtual
    bool
    isActive() const = 0;

    virtual
    bool
    isIssue() const = 0;
};
};

#endif // ADVISER_RATING_WIDGET_H
