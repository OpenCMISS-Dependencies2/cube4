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
#include <QList>
#include "KnlMemoryAnalysis.h"
#include "TreeItem.h"
#include "CubeAdvice.h"
#include "PerformanceTest.h"
#include "PluginServices.h"
#include <QSlider>


using namespace cube;
using namespace cubegui;
using namespace cubepluginapi;
using namespace advisor;

extern cubepluginapi::PluginServices* advisor_services;

static
bool
LLCMissesLessThan( const QPair<TreeItem*, QPair< QPair<double, double >, double > >& a,
                   const QPair<TreeItem*, QPair< QPair<double, double >, double > >& b );

static
bool
BandwidthLessThan( const QPair<TreeItem*, QPair< QPair<double, double >, double > >& a,
                   const QPair<TreeItem*, QPair< QPair<double, double >, double > >& b );



KnlMemoryAnalysis::KnlMemoryAnalysis( cube::CubeProxy* _cube ) : PerformanceAnalysis( _cube )
{
    knl_memory_transdfer_test = new KnlMemoryTransferTest( cube );
    knl_memory_bandwidth_test = new KnlMemoryBandwidthTest( cube );
    knl_llc_miss_test         = new     KnlLLCMissTest( cube );
    MCDRAM_ISSUE              = tr( "This memory transfer should be moved to MCDRAM" );
    header.clear();
    fillAdviceHeader();
}


KnlMemoryAnalysis::~KnlMemoryAnalysis()
{
    delete knl_memory_bandwidth_test;
    delete knl_memory_transdfer_test;
    delete knl_llc_miss_test;
};



QList<AdvisorAdvice>
KnlMemoryAnalysis::getCandidates( const QList<TreeItem*>& _list )
{
    QList<AdvisorAdvice>                                               to_return;
    QList<QPair<TreeItem*, QPair< QPair<double, double >, double > > > candidate_items;
    foreach( TreeItem * item, _list )
    {
        cube::Cnode* cnode = static_cast<cube::Cnode*>( item->getCubeObject() );

        knl_memory_bandwidth_test->apply( cnode, cube::CUBE_CALCULATE_EXCLUSIVE );
        knl_memory_transdfer_test->apply( cnode, cube::CUBE_CALCULATE_EXCLUSIVE );
        knl_llc_miss_test->apply( cnode, cube::CUBE_CALCULATE_EXCLUSIVE );

        double bandwidth = knl_memory_bandwidth_test->value();
        double transfer  =  knl_memory_transdfer_test->value();
        double llcm      = knl_llc_miss_test->value();


        candidate_items.append( qMakePair( item, qMakePair( qMakePair( bandwidth, transfer ), llcm ) ) );
    }

    std::stable_sort( candidate_items.begin(), candidate_items.end(), BandwidthLessThan );

    if ( number_candidates > 0 )
    {
        while ( candidate_items.size() > number_candidates )
        {
            candidate_items.removeLast();
        }
    }
    std::stable_sort( candidate_items.begin(), candidate_items.end(), LLCMissesLessThan );




    for ( QList<QPair<TreeItem*, QPair< QPair<double, double >, double > > >::const_iterator iter = candidate_items.begin(); iter != candidate_items.end(); ++iter )
    {
        AdvisorAdvice advice;
        advice.callpath = ( *iter ).first;

        QPair<QString, QString> bdw_value_unit = advisor_services->formatNumberAndUnit( ( *iter ).second.first.first,
                                                                                        tr( "bytes/s" ),
                                                                                        cubegui::FORMAT_TREES,
                                                                                        false );

        QPair<QString, QString> transfer_value_unit = advisor_services->formatNumberAndUnit( ( *iter ).second.first.second,
                                                                                             tr( "bytes" ),
                                                                                             cubegui::FORMAT_TREES,
                                                                                             false );

        advice.comments << ( bdw_value_unit.first + " " + bdw_value_unit.second );
        advice.comments << ( transfer_value_unit.first + " " + transfer_value_unit.second );
        advice.comments << MCDRAM_ISSUE;
        to_return << advice;
    }

    return to_return;
}

QList<PerformanceTest*>
KnlMemoryAnalysis::getPerformanceTests()
{
    QList<PerformanceTest*> to_return;
    to_return << knl_memory_transdfer_test
              << knl_memory_bandwidth_test
              << knl_llc_miss_test;
    return to_return;
}

QWidget*
KnlMemoryAnalysis::getToolBar()
{
    QSlider* set_treashold = new QSlider( Qt::Horizontal );
    QObject::connect( set_treashold, SIGNAL( valueChanged( int ) ), this, SLOT( setTreashold( int ) ) );
    set_treashold->setValue( 5 );
    setTreashold( 5 );

    set_treashold->setMinimum( 0 );
    set_treashold->setMaximum( 200 );
    set_treashold->setTracking( true );
    return set_treashold;
}

void
KnlMemoryAnalysis::fillAdviceHeader()
{
    header << tr( "Callpath" ) << tr( "Bandwith" ) << tr( "Transfer" ) << tr( "Issue" );
}


// ------ overview tests ---------

bool
KnlMemoryAnalysis::isActive() const
{
    return true;
}


QString
KnlMemoryAnalysis::getAnchorHowToMeasure()
{
    return "MeasurementForKnlMemoryAnalysis";
}



void
KnlMemoryAnalysis::setTreashold( int nv )
{
    number_candidates = nv;
//    treashold->setText( QString( "%1 % runtime" ).arg( nv / 2. ) );
}


/**
 * Implements LessThan on bandwidth operator to make a double sort
 */
static
bool
BandwidthLessThan( const QPair<TreeItem*, QPair< QPair<double, double >, double > >& a,
                   const QPair<TreeItem*, QPair< QPair<double, double >, double > >& b )
{
    return a.second.first.first < b.second.first.first;
}

/**
 * Implements LessThan on bandwidth operator to make a double sort
 */
static
bool
LLCMissesLessThan( const QPair<TreeItem*, QPair< QPair<double, double >, double > >& a,
                   const QPair<TreeItem*, QPair< QPair<double, double >, double > >& b )
{
    return a.second.first.second < b.second.first.second;
}
