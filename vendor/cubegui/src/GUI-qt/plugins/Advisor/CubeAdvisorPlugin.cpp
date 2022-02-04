/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2021                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include <config.h>
#include <QtPlugin>
#include <QDebug>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QRect>
#include <QFileDialog>
#include <QApplication>
#include <QComboBox>
#include <QStyle>
#include <QFrame>
#include <QLabel>
#include <QWidget>
#include "CubeAdvisorProgress.h"
#include "CubeAdvisorPlugin.h"
#include "CubeAdvisorTableWidgetItem.h"
#include "CubeAdvice.h"
#include "PluginServices.h"
#include "CubeProxy.h"
#include "CubeTypes.h"
#include "CallTree.h"
#include "CubeIdIndexMap.h"
#include "algebra4.h"
#include "Globals.h"

using namespace cube;
using namespace cubepluginapi;
using namespace advisor;
cubepluginapi::PluginServices* advisor_services;


#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2( CubeAdvisorPlugin, CubeAdvisor ) // ( PluginName, ClassName )
#endif
CubeAdvisor::CubeAdvisor()
{
    _widget                            = nullptr;
    pop_audit_analysis                 = nullptr;
    pop_hybrid_audit_analysis          = nullptr;
    pop_hybrid_audit_analysis_bs       = nullptr;
    pop_hybrid_additive_audit_analysis = nullptr;
    jsc_audit_analysis                 = nullptr;
    knl_vectorization_analysis         = nullptr;
    knl_memory_analysis                = nullptr;
    direct_calculation_state           = false;
}

CubeAdvisor::~CubeAdvisor()
{
    delete _widget;
    delete pop_audit_analysis;
    delete pop_hybrid_audit_analysis;
    delete pop_hybrid_audit_analysis_bs;
    delete pop_hybrid_additive_audit_analysis;
    delete jsc_audit_analysis;
    delete knl_vectorization_analysis;
    delete knl_memory_analysis;

    DELETE_BUSY_WAIT_PROGRESS
}



void
CubeAdvisor::version( int& major, int& minor, int& bugfix ) const
{
    major  = 2;
    minor  = 0;
    bugfix = 0;
}

QString
CubeAdvisor::name() const
{
    return "CubeAdvisor";
}

bool
CubeAdvisor::cubeOpened( PluginServices* service )
{
    INIT_BUSY_WAIT_PROGRESS

    this->service    = service;
    advisor_services = service;

    startAnalysis            = false;
    initializationIsFinished = false;

    cube      = service->getCube();
    execution = cube->getMetric( "execution" );
    if ( execution == nullptr )
    {
        execution = cube->getMetric( "time" ); // fallback in case of not remapped file. we just use time metric as an execution time
    }
    if ( execution == nullptr )
    {
        return false;
    }

    markerLabel = new DefaultMarkerLabel( tr( "Advisor Marker" ) );
    defineTreeItemMarker();

    _widget       = new QWidget();
    this->toolBar = new AdvisorToolBar( service );
    toolBar->hide();
    connect( toolBar, SIGNAL( treasholdChanged( int ) ), this, SLOT( treasholdChanged( int ) ) );

    service->addTab( SYSTEM, this, OTHER_PLUGIN_TAB );

    // start initialization in a separate thread
    future = service->createFuture();
    future->addCalculation( new InitialisationTask( this ) );
    connect( future, SIGNAL( calculationFinished() ), this, SLOT( initializationFinished() ) );
    future->startCalculation( false );

    QVBoxLayout* top_layout = new QVBoxLayout();
    _widget->setLayout( top_layout );

    createTests();

    top_layout->addWidget( advisor_progress_widget );

    recalculate_widget = new QWidget();

    QHBoxLayout* _recalculate_layout = new QHBoxLayout();
    recalculate_widget->setLayout( _recalculate_layout );


    recalculate_button      = new QPushButton( tr( "Recalculate" ) );
    automatic_recalculation = new QCheckBox( tr( "automatic" ) );
    direct_calculation      = new QCheckBox( tr( "direct calculation" ) );
    direct_calculation->setCheckState( Qt::Unchecked );
    _recalculate_layout->addWidget( recalculate_button );
    _recalculate_layout->addWidget( automatic_recalculation );
    _recalculate_layout->addWidget( direct_calculation );
    top_layout->addWidget( recalculate_widget );
    recalculate_widget->show();

    connect( recalculate_button, SIGNAL( pressed() ), this, SLOT( recalculate() ) );
    connect( automatic_recalculation, SIGNAL( stateChanged( int ) ), this, SLOT( automatic_recalculate( int ) ) );
    connect( direct_calculation, SIGNAL( stateChanged( int ) ), this, SLOT( direct_calculate( int ) ) );



    analyses = new QStackedWidget;
    top_layout->addWidget( analyses );



    QComboBox* analysisComboBox = toolBar->getComboForAnalyses();
    connect( analysisComboBox, SIGNAL( activated( int ) ),
             this, SLOT( showAnalysis( int ) ) );

    QStackedWidget* analysisWidgets = toolBar->getPlaceForAnalysesWidgets();

    // pop audit
    CubeRatingWidget* pop_rating = new CubeRatingWidget( this, tr( "POP Assessment " ), pop_audit_analysis );
    analyses->addWidget( pop_rating );
    analysisComboBox->addItem( QString::fromStdString( pop_audit_analysis->name() ) );
    analysisWidgets->addWidget( pop_audit_analysis->getToolBar() );

    if (  cubegui::Globals::optionIsSet( cubegui::ExpertMode ) )
    {
        // pop hybrid audit
        CubeRatingWidget* pop_hybrid_rating = new CubeRatingWidget( this, tr( "POP Hybrid Assessment " ), pop_hybrid_audit_analysis );
        analyses->addWidget( pop_hybrid_rating );
        analysisComboBox->addItem( QString::fromStdString( pop_hybrid_audit_analysis->name() ) );
        analysisWidgets->addWidget( pop_hybrid_audit_analysis->getToolBar() );
    }

    // pop hybrid audit
    CubeRatingWidget* pop_hybrid_additive_rating = new CubeRatingWidget( this, tr( "POP Hybrid Assessment ( Additive ) " ), pop_hybrid_additive_audit_analysis );
    analyses->addWidget( pop_hybrid_additive_rating );
    analysisComboBox->addItem( QString::fromStdString( pop_hybrid_additive_audit_analysis->name() ) );
    analysisWidgets->addWidget( pop_hybrid_additive_audit_analysis->getToolBar() );

    // pop hybrid audit ( Barcelona version )
    CubeRatingWidget* pop_hybrid_rating_bs = new CubeRatingWidget( this, tr( "POP Hybrid Assessment (BS) " ), pop_hybrid_audit_analysis_bs );
    analyses->addWidget( pop_hybrid_rating_bs );
    analysisComboBox->addItem( QString::fromStdString( pop_hybrid_audit_analysis_bs->name() ) );
    analysisWidgets->addWidget( pop_hybrid_audit_analysis_bs->getToolBar() );


    if (  cubegui::Globals::optionIsSet( cubegui::ExpertMode ) )
    {
        // jsc hybrid audit
        CubeRatingWidget* jsc_hybrid_rating = new CubeRatingWidget( this, tr( "JSC Hybrid Assessment " ), jsc_audit_analysis );
        analyses->addWidget( jsc_hybrid_rating );
        analysisComboBox->addItem( QString::fromStdString( jsc_audit_analysis->name() ) );
        analysisWidgets->addWidget( jsc_audit_analysis->getToolBar() );
    }
    // knl vectorization analysis
    CubeRatingWidget* knl_vectorization_rating = new CubeRatingWidget( this,  tr( "KNL Vectorization " ), knl_vectorization_analysis );
    analyses->addWidget( knl_vectorization_rating );
    analysisComboBox->addItem( QString::fromStdString( knl_vectorization_analysis->name() ) );
    analysisWidgets->addWidget( knl_vectorization_analysis->getToolBar() );


    // knl memory analysis
    CubeRatingWidget* knl_memory_rating = new CubeRatingWidget( this,  tr( "KNL Memory " ), knl_memory_analysis );
    analyses->addWidget( knl_memory_rating );
    analysisComboBox->addItem( QString::fromStdString( knl_memory_analysis->name() ) );
    analysisWidgets->addWidget( knl_memory_analysis->getToolBar() );

    QAction* copyClip = toolBar->getCopyAction();
    connect( copyClip, SIGNAL( triggered( bool ) ), this, SLOT( copyMetrics( bool ) ) );

    QAction* runAnalysis = toolBar->getRunAnalysisAction();
    connect( runAnalysis, SIGNAL( triggered( bool ) ), this, SLOT( runAnalysis() ) );

    connect( service, SIGNAL( contextMenuIsShown( cubepluginapi::DisplayType, cubepluginapi::TreeItem* ) ),
             this, SLOT( contextMenuIsShown( cubepluginapi::DisplayType, cubepluginapi::TreeItem* ) ) );




//     calculateOverallTests();

    service->addToolBar( toolBar, this );
    setActive( false );




    return true;
}



void
CubeAdvisor::contextMenuIsShown( cubegui::DisplayType type, cubegui::TreeItem* item )
{
    if ( ( item == nullptr ) || ( type != CALL )  )
    {
        return;
    }
    QAction* action = service->addContextMenuItem( type, "Analyse for candidates" );
    connect( action, SIGNAL( triggered( bool ) ), this, SLOT( analyseSubtree() ) );
    context_menu_item = item;
}


void
CubeAdvisor::analyseSubtree()
{
    if ( !initializationIsFinished )   // advisor->initialization_run() has to be finished before
    {
        startAnalysis = true;
        return;
    }
    START_BUSY_WAIT
    service->removeMarker();
    CubeRatingWidget* current_analysis = static_cast<CubeRatingWidget*>( analyses->currentWidget() );
    int               progress         = 0;

    std::list<cubegui::TreeItem*> cnodes;
    QList<cubegui::TreeItem*>     selected_cnodes;

    if ( context_menu_item != nullptr )
    {
        cnodes.push_back( context_menu_item );
    }

    value_container inclusive_values;
    value_container exclusive_values;
    list_of_metrics lmetrics;
    metric_pair     metric;
    metric.second = cube::CUBE_CALCULATE_INCLUSIVE;
    metric.first  = execution;
    lmetrics.push_back( metric );

    cube::list_of_sysresources lsysres;
    cube::list_of_cnodes       lcnodes;

    cube::cnode_pair pcnode = std::make_pair( dynamic_cast<cube::Cnode*>( context_menu_item->getCubeObject() ), cube::CUBE_CALCULATE_INCLUSIVE );
    lcnodes.push_back( pcnode );

    cube::Value* v = cube->calculateValue( lmetrics,
                                           lcnodes,
                                           lsysres );
    double execution_of_parent = v->getDouble();
    delete v;

    // collect all children
    std::list<TreeItem*>::const_iterator citer = cnodes.begin();
    while ( citer !=  cnodes.end()  )
    {
        TreeItem* _item = *citer;
        foreach( TreeItem * _child, _item->getChildren() )
        {
            exclusive_values.clear();
            inclusive_values.clear();
            cube::list_of_cnodes lcnodes;

            cube::cnode_pair pcnode = std::make_pair( dynamic_cast<cube::Cnode*>( _child->getCubeObject() ), cube::CUBE_CALCULATE_INCLUSIVE );
            lcnodes.push_back( pcnode );

            cube::Value* v = cube->calculateValue( lmetrics,
                                                   lcnodes,
                                                   lsysres );
            double execution_of_child = v->getDouble();
            delete v;
            if ( execution_of_child / execution_of_parent >= 0.05 ) // add if child exclusive run of a child is at least 5 % of a total execution of parent.
            {
                selected_cnodes << _child;
            }
            cnodes.push_back( _child );
        }
        ++citer;
    }

    QList<AdvisorAdvice> candidates = current_analysis->runAnalysis( selected_cnodes );

    foreach( AdvisorAdvice advice, candidates )
    {
        PROGRESS_BUSY_WAIT( tr( "Calculate %1" ).arg( advice.callpath->getName() ), ( ( int )( progress / candidates.size() * 100. ) ) );
        service->addMarker( advice.callpath,  markerList.at( 0 ) );
        QString advice_label = tr( "Advisor: \n" );
        foreach( QString partial_advice, advice.comments )
        {
            advice_label += partial_advice + "\n";
        }
        markerLabel->setLabel( advice.callpath, advice_label );
        progress++;
    }
    END_BUSY_WAIT
}


void
CubeAdvisor::initialization_run()
{
    value_container       inclusive_values;
    value_container       exclusive_values;
    IdIndexMap            metric_id_indices;
    list_of_cnodes        lcnodes;
    std::vector< Cnode* > cnodes = cube->getRootCnodes();
    for ( std::vector< Cnode* >::iterator iter = cnodes.begin(); iter != cnodes.end(); ++iter )
    {
        cnode_pair cnode;
        cnode.first  = *iter;
        cnode.second = cube::CUBE_CALCULATE_INCLUSIVE;
        lcnodes.push_back( cnode );
    }

    list_of_sysresources           lsysres;
    std::vector< SystemTreeNode* > sysress = cube->getRootSystemTreeNodes();
    for ( std::vector< SystemTreeNode* >::iterator iter = sysress.begin(); iter != sysress.end(); ++iter )
    {
        sysres_pair sres;
        sres.first  = *iter;
        sres.second = cube::CUBE_CALCULATE_INCLUSIVE;
        lsysres.push_back( sres );
    }




    root_callpaths = service->getTopLevelItems( service->getActiveTree( CALL )->getType() );

    cube->getMetricSubtreeValues(   lcnodes,
                                    lsysres,
                                    *execution,
                                    ( size_t )0,
                                    metric_id_indices,
                                    &inclusive_values,
                                    &exclusive_values );

    total_execution = inclusive_values[ 0 ]->getDouble();

    for ( value_container::iterator iter = inclusive_values.begin(); iter != inclusive_values.end(); ++iter )
    {
        delete *iter;
    }
    for ( value_container::iterator iter = exclusive_values.begin(); iter != exclusive_values.end(); ++iter )
    {
        delete *iter;
    }
}





void
CubeAdvisor::showAnalysis( int i )
{
    analyses->setCurrentIndex( i );
    toolBar->getPlaceForAnalysesWidgets()->setCurrentIndex( i );
    const QList<TreeItem*>& items =  service->getSelections( service->getActiveTree( CALL )->getType() );
    cube::list_of_cnodes    lcnodes;
    foreach( TreeItem * item, items )
    {
        cube::Cnode* cnode = static_cast<cube::Cnode*> ( item->getCubeObject() );
        if ( cnode == nullptr )
        {
            continue;
        }
        cube::cnode_pair pcnode;
        pcnode.first  = cnode;
        pcnode.second = ( item->isExpanded() ) ? cube::CUBE_CALCULATE_EXCLUSIVE : cube::CUBE_CALCULATE_INCLUSIVE;
        lcnodes.push_back( pcnode );
    }
    CubeRatingWidget* current_analysis = static_cast<CubeRatingWidget*>( analyses->currentWidget() );
    current_analysis->apply( lcnodes, direct_calculation_state );
}


void
CubeAdvisor::copyMetrics( bool f )
{
    ( void )f;
    const QList<TreeItem*>& items =  service->getSelections( service->getActiveTree( CALL )->getType() );
    /* handle special case: item is a loop in the call tree, which may be aggregated */
    cube::list_of_cnodes lcnodes;
    foreach( TreeItem * item, items )
    {
        cube::Cnode* cnode = static_cast<cube::Cnode*> ( item->getCubeObject() );
        if ( cnode == nullptr )
        {
            continue;
        }
        cube::cnode_pair pcnode;
        pcnode.first  = cnode;
        pcnode.second = ( item->isExpanded() ) ? cube::CUBE_CALCULATE_EXCLUSIVE : cube::CUBE_CALCULATE_INCLUSIVE;
        lcnodes.push_back( pcnode );
    }
    CubeRatingWidget* current_analysis = static_cast<CubeRatingWidget*>( analyses->currentWidget() );
    current_analysis->copyMetricsValues( lcnodes );
}

void
CubeAdvisor::initializationFinished()
{
    initializationIsFinished = true;
    if ( startAnalysis )
    {
        runAnalysis();
    }
}

void
CubeAdvisor::runAnalysis()
{
    if ( !initializationIsFinished )   // advisor->initialization_run() has to be finished before
    {
        startAnalysis = true;
        return;
    }

    START_BUSY_WAIT
    service->removeMarker();
    CubeRatingWidget* current_analysis = static_cast<CubeRatingWidget*>( analyses->currentWidget() );
    //double            my_treashold_value = toolBar->getTreashold();

    const QList<cubegui::TreeItem*>& cnodes     = advisor_services->getTreeItems( cubegui::CALL );
    int                              progress   = 0;
    QList<AdvisorAdvice>             candidates = current_analysis->runAnalysis( cnodes );

    foreach( AdvisorAdvice advice, candidates )
    {
        PROGRESS_BUSY_WAIT( tr( "Calculate %1" ).arg( advice.callpath->getName() ), ( ( int )( progress / candidates.size() * 100. ) ) );
        service->addMarker( advice.callpath,  markerList.at( 0 ) );
        QString advice_label = tr( "Advisor: \n" );
        foreach( QString partial_advice, advice.comments )
        {
            advice_label += partial_advice + "\n";
        }
        markerLabel->setLabel( advice.callpath, advice_label );
        progress++;
    }
    END_BUSY_WAIT
}




void
CubeAdvisor::createTests()
{
    pop_audit_analysis                 = new POPAuditPerformanceAnalysis( cube );
    pop_hybrid_audit_analysis          = new POPHybridAuditPerformanceAnalysis( cube );
    pop_hybrid_audit_analysis_bs       = new BSPOPHybridAuditPerformanceAnalysis( cube );
    pop_hybrid_additive_audit_analysis = new POPHybridAuditPerformanceAnalysisAdd( cube );
    jsc_audit_analysis                 = new JSCAuditPerformanceAnalysis( cube );
    knl_vectorization_analysis         = new KnlVectorizationAnalysis( cube );
    knl_memory_analysis                = new KnlMemoryAnalysis( cube );
}


void
CubeAdvisor::calculateOverallTests()
{
    START_BUSY_WAIT
    const QList<TreeItem*>& items =  service->getSelections( service->getActiveTree( CALL )->getType() );
    cube::list_of_cnodes    lcnodes;
    foreach( TreeItem * item, items )
    {
        cube::Cnode* cnode = static_cast<cube::Cnode*> ( item->getCubeObject() );
        if ( cnode == nullptr )
        {
            continue;
        }
        cube::cnode_pair pcnode;
        pcnode.first  = cnode;
        pcnode.second = ( item->isExpanded() ) ? cube::CUBE_CALCULATE_EXCLUSIVE : cube::CUBE_CALCULATE_INCLUSIVE;
        lcnodes.push_back( pcnode );
    }
    CubeRatingWidget* current_analysis = static_cast<CubeRatingWidget*>( analyses->currentWidget() );
    current_analysis->apply( lcnodes, direct_calculation_state );
    END_BUSY_WAIT
}


void
CubeAdvisor::treasholdChanged( int nv )
{
    START_BUSY_WAIT
    service->removeMarker();
    ( void )nv;
    CubeRatingWidget* current_analysis = static_cast<CubeRatingWidget*>( analyses->currentWidget() );
    current_analysis->invalidateAnalysis();
    END_BUSY_WAIT
}




void
CubeAdvisor::tableItemClicked( QTableWidgetItem* _item )
{
    START_BUSY_WAIT
    CubeAdvisorTableWidgetItem* item = dynamic_cast<CubeAdvisorTableWidgetItem*>( _item );
    if ( item == nullptr )
    {
        return;
    }
    cubegui::TreeItem* callpath_item = const_cast<cubegui::TreeItem*>( item->getCallPathItem() );
    service->selectItem( callpath_item, false );
    callpath_item->setExpanded( true );
    END_BUSY_WAIT
}


void
CubeAdvisor::cubeClosed()
{
    markerList.clear();
    delete pop_audit_analysis;
    delete pop_hybrid_audit_analysis;
    delete pop_hybrid_audit_analysis_bs;
    delete pop_hybrid_additive_audit_analysis;
    delete jsc_audit_analysis;
    delete knl_vectorization_analysis;
    delete knl_memory_analysis;
}

QString
CubeAdvisor::getHelpText() const
{
    return tr( "This plugin evaluates performance issues and helps to browse them one-by-one." );
}


QWidget*
CubeAdvisor::widget()
{
    return _widget;
}

QString
CubeAdvisor::label() const
{
    return tr( "Advisor" );
}



QIcon
CubeAdvisor::icon() const
{
    return QIcon( ":/images/advisor-icon.png" );
}



/**
 * @brief DemoPlugin::setActive is called when the tab gets activated or deactivated by selecting another tab
 * The tab related demo elements should only react on signals, if the tab is active. For that reason the
 * signals are disconnected if another tab becomes active.
 */
void
CubeAdvisor::setActive( bool active )
{
    if ( active )
    {
        toolBar->show();
        service->connect( service, SIGNAL( treeItemIsSelected( cubepluginapi::TreeItem* ) ),
                          this, SLOT( treeItemIsSelected( cubepluginapi::TreeItem* ) ) );
        service->connect( service, SIGNAL( contextMenuIsShown( cubepluginapi::DisplayType, cubepluginapi::TreeItem* ) ),
                          this, SLOT( contextMenuIsShown( cubepluginapi::DisplayType, cubepluginapi::TreeItem* ) ) );
        // ??? treeItemIsSelected( nullptr );
    }
    else
    {
        toolBar->hide();
        service->disconnect( service, SIGNAL( treeItemIsSelected( cubepluginapi::TreeItem* ) ),
                             this, SLOT( treeItemIsSelected( cubepluginapi::TreeItem* ) ) );
        service->disconnect( service, SIGNAL( contextMenuIsShown( cubepluginapi::DisplayType, cubepluginapi::TreeItem* ) ),
                             this, SLOT( contextMenuIsShown( cubepluginapi::DisplayType, cubepluginapi::TreeItem* ) ) );
    }
}

void
CubeAdvisor::treeItemIsSelected( cubepluginapi::TreeItem* item )
{
    if ( ( item == nullptr ) || ( item->getDisplayType() != CALL )  )
    {
        return;
    }
    if ( item->isExpanded() /*do not calculate for expanded items*/ )
    {
        analyses->setEnabled( false );
        recalculate_widget->setEnabled( false );
        return;
    }
    analyses->setEnabled( true );
    recalculate_widget->setEnabled( true );
    recalculate_widget->show();
    if ( automatic_recalculation->isChecked() )
    {
        recalculate();
    }
}


void
CubeAdvisor::recalculate()
{
    /* handle special case: item is a loop in the call tree, which may be aggregated */
    CubeRatingWidget* current_analysis = static_cast<CubeRatingWidget*>( analyses->currentWidget() );
    if ( current_analysis->isCalculating() )
    {
        advisor_services->setMessage( tr( "Calculation is in progress..." ), cubegui::Warning );
        return;
    }
    /** Returns selected items in the given active call tree */
    QList<TreeItem*> selected_items = service->getSelections( service->getActiveTree( CALL )->getType() );
    list_of_cnodes   lcnodes;
    foreach( TreeItem * item, selected_items )
    {
        cnode_pair pcnode;
        pcnode.first  = static_cast<cube::Cnode*>( item->getCubeObject() );
        pcnode.second = ( item->isExpanded() ) ? cube::CUBE_CALCULATE_EXCLUSIVE :
                        cube::CUBE_CALCULATE_INCLUSIVE;
        lcnodes.push_back( pcnode );
    }

    current_analysis->apply( lcnodes, direct_calculation_state );
}



void
CubeAdvisor::automatic_recalculate( int state )
{
    if ( state ==  Qt::Checked  )
    {
        recalculate();
    }
    recalculate_button->setEnabled( state != Qt::Checked );
}

void
CubeAdvisor::direct_calculate( int state )
{
    direct_calculation_state = ( state ==  Qt::Checked );
    recalculate();
}




/** defines a number of tree items markers, which are used to mark tree items with different background colour and
    icons */
void
CubeAdvisor::defineTreeItemMarker()
{
    QList<QPixmap> icons;
    icons.append( QPixmap( ":/images/advisor-icon.png" ) );
    markerList.append( service->getTreeItemMarker( tr( "Advisor Marker" ), icons, false, markerLabel ) );
}
