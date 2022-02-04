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

#include "KnlMemoryTransferTest.h"

using namespace advisor;

KnlMemoryTransferTest::KnlMemoryTransferTest( cube::CubeProxy* cube ) : PerformanceTest( cube )
{
    setName( tr( "KNL Memory Transfer" ).toUtf8().data() );
    setWeight( 1 );   // need to be adjusted
    maxValue            = 1.;
    knl_memory_transfer = cube->getMetric( "knl_memory_transfer" );
    if ( knl_memory_transfer == NULL )
    {
        adjustForTest( cube );
    }
    knl_memory_transfer = cube->getMetric( "knl_memory_transfer" );
    if ( knl_memory_transfer == NULL )
    {
        setWeight( 0.2 );
        setValue( 0. );
        return;
    }

    cube::metric_pair metric;
    metric.first  = knl_memory_transfer;
    metric.second = cube::CUBE_CALCULATE_INCLUSIVE;
    lmetrics.push_back( metric );
}


void
KnlMemoryTransferTest::applyCnode( const cube::Cnode* cnode, const cube::CalculationFlavour cnf,
                                   const bool direct_calculation  )
{
    ( void )direct_calculation; // not used here
    if ( knl_memory_transfer == NULL )
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
    double knl_memory_transfer_value = v->getDouble();
    delete v;
    setValue( knl_memory_transfer_value );
}

void
KnlMemoryTransferTest::applyCnode( const cube::list_of_cnodes& lcnodes,
                                   const bool                  direct_calculation  )
{
    ( void )direct_calculation; // not used here
    if ( knl_memory_transfer == NULL )
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
        double knl_memory_transfer_value = v->getDouble();
        delete v;
        setValue( knl_memory_transfer_value );
    }
}


const std::string&
KnlMemoryTransferTest::getCommentText() const
{
    if ( knl_memory_transfer == NULL )
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
KnlMemoryTransferTest::isActive() const
{
    return knl_memory_transfer != NULL;
};

bool
KnlMemoryTransferTest::isIssue() const
{
    return false;
};


void
KnlMemoryTransferTest::adjustForTest( cube::CubeProxy* cube ) const
{
    cube::Metric* imc0 = cube->getMetric( "knl_unc_imc0::UNC_M_CAS_COUNT:ALL:cpu=0" );
    cube::Metric* imc1 = cube->getMetric( "knl_unc_imc1::UNC_M_CAS_COUNT:ALL:cpu=0" );
    cube::Metric* imc2 = cube->getMetric( "knl_unc_imc2::UNC_M_CAS_COUNT:ALL:cpu=0" );
    cube::Metric* imc3 = cube->getMetric( "knl_unc_imc3::UNC_M_CAS_COUNT:ALL:cpu=0" );
    cube::Metric* imc4 = cube->getMetric( "knl_unc_imc4::UNC_M_CAS_COUNT:ALL:cpu=0" );
    cube::Metric* imc5 = cube->getMetric( "knl_unc_imc5::UNC_M_CAS_COUNT:ALL:cpu=0" );

    if ( imc0 != NULL ||
         imc1 != NULL ||
         imc2 != NULL ||
         imc3 != NULL ||
         imc4 != NULL ||
         imc5 != NULL
         )
    {
        add_knl_memory_transfer( cube );
    }
}



void
KnlMemoryTransferTest::add_knl_memory_transfer( cube::CubeProxy* ) const
{
    cube::Metric* _met = cube->getMetric( "knl_memory_transfer" );
    if ( _met == NULL )
    {
        cube::Metric* _met = cube->defineMetric(
            tr( "MemoryTransfer " ).toUtf8().data(),
            "knl_memory_transfer",
            "DOUBLE",
            tr( "bytes" ).toUtf8().data(),
            "",
            KNL_MEMORY_TRANSFER_URL,
            tr( "Counts total number of DRAM CAS commands issued on this channel:CPU to program." ).toUtf8().data(),
            NULL,
            cube::CUBE_METRIC_PREDERIVED_EXCLUSIVE,
            "${without_wait_state}[${calculation::callpath::id}] *  64 * ("
            "metric::knl_unc_imc0::UNC_M_CAS_COUNT:ALL:cpu=0() + "
            "metric::knl_unc_imc1::UNC_M_CAS_COUNT:ALL:cpu=0() + "
            "metric::knl_unc_imc2::UNC_M_CAS_COUNT:ALL:cpu=0() + "
            "metric::knl_unc_imc3::UNC_M_CAS_COUNT:ALL:cpu=0() + "
            "metric::knl_unc_imc4::UNC_M_CAS_COUNT:ALL:cpu=0() + "
            "metric::knl_unc_imc5::UNC_M_CAS_COUNT:ALL:cpu=0() ) ",
            "",
            "",
            "",
            "",
            true,
            cube::CUBE_METRIC_GHOST
            );
        advisor_services->addMetric( _met, NULL );
    }
}
