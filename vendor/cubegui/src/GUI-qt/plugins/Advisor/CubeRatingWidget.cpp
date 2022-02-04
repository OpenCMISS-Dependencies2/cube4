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
#include <QFrame>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QApplication>
#include <QClipboard>
#include <QListIterator>
#include <QHeaderView>
#include <QFontMetrics>

#include "CubeAdvisorProgress.h"
#include "CubeRatingWidget.h"
#include "CubeHelpButton.h"
#include "PerformanceAnalysis.h"
#include "CubeTestWidget.h"
#include "PluginServices.h"
#include "HelpBrowser.h"
#include "CubeAdvice.h"
#include "CubeAdvisorTableWidgetItem.h"
#include "TreeItem.h"
#include "Future.h"
#include "Globals.h"

using namespace advisor;

extern cubepluginapi::PluginServices* advisor_services;


CubeRatingWidget::CubeRatingWidget( cubepluginapi::TabInterface* _tab,
                                    QString                      _title,
                                    PerformanceAnalysis*         _analysis,
                                    QWidget*                     parent ) : QWidget( parent ), analysis( _analysis ), tab( _tab )
{
    calculating_in_progress = false;
    analysis_possible       = false;
    title                   = _title;
    QVBoxLayout* top_layout = new QVBoxLayout();
    callpath_frame = new QGroupBox( title );
    main_grid      = new QGridLayout();
    callpath_frame->setLayout( main_grid );
    future = advisor_services->createFuture( _tab );

    QList<PerformanceTest*> _tests = analysis->getPerformanceTests();


    foreach( PerformanceTest * test, _tests )
    {
        addPerformanceTest( test );
        analysis_possible |=  ( test->isActive() );
    }

    top_layout->addWidget( callpath_frame );



/*    QScrollArea* scroll = new QScrollArea();

    scroll->setWidgetResizable( true );
    scroll->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
    scroll->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
    scroll->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) ); */
    if ( !analysis_possible )
    {
        top_layout->addWidget( new QLabel( tr( "<b> Analysis is not possible: </b>" ) ) );
        QString              path = cubegui::Globals::getOption( cubegui::DocPath ) + "cubegui/guide/html/";
        cubegui::HtmlWidget* html = cubegui::HtmlWidget::createHtmlWidget();
        html->showUrl( path + analysis->getAnchorHowToMeasure() + ".html" );
        top_layout->addWidget( html );
    }
    else
    {
        top_layout->addWidget( new QLabel( tr( "<b> Candidates </b>" ) ) );
        QStringList header = analysis->getAdviceHeader();
        table = new QTableWidget( 0, header.size() );
//        scroll->setWidget( table );
        top_layout->addWidget( table );
        table->horizontalHeader()->setStretchLastSection( true );
#if QT_VERSION < 0x050000
        table->horizontalHeader()->setResizeMode( QHeaderView::Stretch );
        table->verticalHeader()->setResizeMode( QHeaderView::ResizeToContents );
#else
        table->horizontalHeader()->setSectionResizeMode( QHeaderView::Stretch );
        table->verticalHeader()->setSectionResizeMode( QHeaderView::ResizeToContents );
#endif

        table->setVerticalHeaderLabels( header );
        table->setHorizontalHeaderLabels( header );
        table->setSelectionBehavior( QAbstractItemView::SelectRows );
        table->setSelectionMode( QAbstractItemView::SingleSelection );
    }

    setLayout( top_layout );
    updatetimer = new QTimer( this );
    connect( updatetimer, SIGNAL( timeout() ), this, SLOT( calculationProgress() ) );
}

CubeRatingWidget::~CubeRatingWidget()
{
    QListIterator<CubeTestWidget*> i( list_of_tests );
    while ( i.hasNext() )
    {
        delete ( i.next() );
    }
}

void
CubeRatingWidget::copyMetricsValues(  const cube::list_of_cnodes& lcnodes )
{
    for ( cube::list_of_cnodes::const_iterator iter = lcnodes.begin(); iter != lcnodes.end(); ++iter )
    {
        cube::Cnode*                   cnode     = iter->first;
        const std::string&             name      = ( cnode != NULL ) ? cnode->get_callee()->get_name() : "";
        QClipboard*                    clipboard = QApplication::clipboard();
        QListIterator<CubeTestWidget*> i( list_of_tests );
        i.toFront();
        QString text_to_copy = title + "\t" + QString::fromStdString( name ) + "\n---------------\n" + tr( "POP Metrics" ) + "\t" + tr( "Min\tAvg\tMax" ) + "\n";
        while ( i.hasNext() )
        {
            CubeTestWidget* _t = i.next();
            if ( !_t->isActive() )
            {
                continue;
            }

            if ( _t->getProgressBar() != nullptr )
            {
                if ( _t->getProgressBar()->isSingleValue() )
                {
                    text_to_copy += QString( "%1 \t %2\t %3 \t %4 \n" ).arg( _t->getName()->text().trimmed() ).arg( "" ).arg( _t->getProgressBar()->getAvgValue() ).arg( "" );
                }
                else
                {
                    text_to_copy += QString( "%1 \t %2\t %3 \t %4 \n" ).arg( _t->getName()->text().trimmed() ).arg( _t->getProgressBar()->getMinValue() ).arg( _t->getProgressBar()->getAvgValue() ).arg( _t->getProgressBar()->getMaxValue() );
                }
            }
            else
            {
                text_to_copy += QString( "%1 \t %2\t %3 \t %4 \n" ).arg( _t->getName()->text().trimmed() ).arg( "" ).arg( _t->getValue()->text().trimmed() ).arg( "" );
            }
        }

        clipboard->setText( text_to_copy );
        advisor_services->setMessage( tr( "Copied values of analysis \"" ) + title + tr( "\" into clipboard." ) );
    }
}


void
CubeRatingWidget::addPerformanceTest( PerformanceTest* t )
{
    CubeTestWidget* test_widget = new CubeTestWidget( t );
    int             n           = list_of_tests.size();
    main_grid->addWidget( test_widget->getName(), n, 0 );
    main_grid->addWidget( test_widget->getValue(), n, 1 );
    Bar* bar = test_widget->getProgressBar();
    if ( bar != NULL )
    {
        bar->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum );
        main_grid->addWidget( bar, n, 2 );
    }
    main_grid->addWidget( test_widget->getValueText(), n, 3 );
    HelpButton* help = new HelpButton( t->getHelpUrl(), t->isActive() );
    main_grid->addWidget( help, n, 4 );
    list_of_tests << test_widget;
}


static inline
bool
find_cnode_in_list_of_cnodes( const cube::list_of_cnodes& cnodes, const cube::Cnode* c )
{
    for ( cube::list_of_cnodes::const_iterator iter = cnodes.begin(); iter != cnodes.end(); ++iter )
    {
        if ( iter->first == c )
        {
            return true;
        }
    }
    return false;
}

void
CubeRatingWidget::apply( const cube::list_of_cnodes& cnodes, const bool direct_calculation )
{
    calculating_in_progress = true;
    if ( cnodes.size() != 0 )
    {
        const std::string& name = ( cnodes.size() == 1 ) ? cnodes[ 0 ].first->get_callee()->get_name() :
                                  tr( "...cnodes..." ).toUtf8().data();
        QFontMetrics fm            = this->fontMetrics();
        int          width         = callpath_frame->width() - 2 * fm.boundingRect( "..." ).width();
        QString      correctedName = fm.elidedText( title + QString( ": %1 " ).arg( QString::fromStdString( name ) ), Qt::ElideMiddle, width );
        callpath_frame->setTitle( correctedName  );
    }
    elapsedTimer.start();
    QListIterator<CubeTestWidget*> i( list_of_tests );
    calculations.clear();
    QHash<PerformanceTest*, ParallelCalculation*> tests;
    i.toBack();
    while ( i.hasPrevious() )
    {
        CubeTestWidget*      tt   = i.previous();
        ParallelCalculation* pcal =  new ParallelCalculation( &calculations, tt->getPerformanceTest(), cnodes, direct_calculation );
        tests[ tt->getPerformanceTest() ] = pcal;
        calculations[ pcal ]              = tt;
        tt->disable();
    }

    QHashIterator<PerformanceTest*, ParallelCalculation*> ii( tests );
    while ( ii.hasNext() )
    {
        ii.next();
        QList<PerformanceTest*> _tests = ii.key()->getPrereqs();
        foreach( PerformanceTest * t, _tests )
        {
            ii.value()->addPrereq( tests[ t ] );
        }
    }
    advisor_services->debug() << "[Advisor] The calculation operation took" << elapsedTimer.elapsed() << "milliseconds";

    for ( ParallelCalculation* tt : tests.values() )
    {
        future->addCalculation( tt );  // calculation data
    }

    if ( analysis_possible )
    {
        AnalysisUpdateTask* _analysis_task = new AnalysisUpdateTask( table, cnodes );
        future->addCalculation( _analysis_task );
    }

    connect( future, SIGNAL( calculationFinished() ), this, SLOT( calculationFinished() ) );
    connect( future, SIGNAL( calculationStepFinished() ), this, SLOT( calculationStepFinished() ) );
    elapsedTimer.start();
    elapsedStageTimer.start();
    future->startCalculation();
    updatetimer->start( 1000 );
}

QList<AdvisorAdvice>
CubeRatingWidget::runAnalysis( const QList<cubegui::TreeItem*>& cnodes )
{
    QList<AdvisorAdvice> candidates;
    if ( !analysis_possible )
    {
        return candidates;
    }
    table->clearContents();
    table->setRowCount( 0 );

    candidates = analysis->getCandidates( cnodes );
    int row = 0;
    foreach( AdvisorAdvice advice, candidates )
    {
        table->setRowCount( row + 1 );
        cubegui::TreeItem*          c_item  = advice.callpath;
        CubeAdvisorTableWidgetItem* varItem = new CubeAdvisorTableWidgetItem( c_item,  c_item->getName() );
        table->setItem( row, 0, varItem );
        varItem->setFlags( varItem->flags() &  ~( Qt::ItemIsEditable ) );
        int i = 1;
        foreach( QString partial_advice, advice.comments )
        {
            CubeAdvisorTableWidgetItem* valItem = new CubeAdvisorTableWidgetItem( c_item,  partial_advice );
            table->setItem( row, i, valItem );
            valItem->setFlags( valItem->flags() &  ~( Qt::ItemIsEditable ) );
            i++;
        }
        row++;
        connect( table, SIGNAL( itemClicked( QTableWidgetItem* ) ), this, SLOT( tableItemClicked( QTableWidgetItem* ) ) );
    }
    return candidates;
}

void
CubeRatingWidget::invalidateAnalysis()
{
    if ( !analysis_possible )
    {
        return;
    }
    table->clearContents();
    table->setRowCount( 0 );
}


void
CubeRatingWidget::tableItemClicked( QTableWidgetItem* _item )
{
    if ( !analysis_possible )
    {
        return;
    }
    CubeAdvisorTableWidgetItem* item = dynamic_cast<CubeAdvisorTableWidgetItem*>( _item );
    if ( item == NULL )
    {
        return;
    }
    cubegui::TreeItem* callpath_item = const_cast<cubegui::TreeItem*>( item->getCallPathItem() );
    callpath_item->setExpanded( false );
    advisor_services->selectItem( callpath_item, false );
}


void
CubeRatingWidget::calculationStepFinished()
{
    calculationProgress();
    advisor_services->debug() << tr( "[Advisor] The calculation stage operation took" ) << elapsedStageTimer.restart() << tr( "milliseconds" );
}


void
CubeRatingWidget::calculationFinished()
{
    calculating_in_progress = false;
    calculationProgress();
    updatetimer->stop();
    advisor_services->setMessage( tr( "Calculation is finished." ) );
    advisor_services->debug() << tr( "[Advisor] The calculation operation took" ) << elapsedTimer.elapsed() << tr( "milliseconds" );
    disconnect( future, SIGNAL( calculationFinished() ), this, SLOT( calculationFinished() ) );
    disconnect( future, SIGNAL( calculationStepFinished() ), this, SLOT( calculationStepFinished() ) );
}

void
CubeRatingWidget::calculationProgress()
{
    if ( !guard.tryLock() )
    {
        return;
    }
    QListIterator<CubeTestWidget*> i( list_of_tests );
    i.toBack();
    while ( i.hasPrevious() )
    {
        CubeTestWidget* tt = i.previous();
        tt->updateCalculation();
    }
    guard.unlock();
}


void
AnalysisUpdateTask::calculate()
{
    // proceed only if analysis is possible
    int rows = table->rowCount();
    int cols = table->columnCount();

    for ( int row = 0; row < rows; ++row )
    {
        CubeAdvisorTableWidgetItem* tableItem      = dynamic_cast<CubeAdvisorTableWidgetItem*>( table->item( row, 0 ) );
        cube::Cnode*                _table_element = static_cast<cube::Cnode*>( tableItem->getCallPathItem()->getCubeObject() );
        bool                        parent         = false;
        while ( _table_element != NULL )
        {
            if ( find_cnode_in_list_of_cnodes( cnodes, _table_element ) )
            {
                parent = true;
                break;
            }
            _table_element = _table_element->get_parent();
        }
        QTableWidgetSelectionRange selection( row, 0, row, cols - 1 );
        table->setRangeSelected( selection, false );


        for ( int col = 0; col < cols; ++col )
        {
            CubeAdvisorTableWidgetItem* _tableItem = dynamic_cast<CubeAdvisorTableWidgetItem*>( table->item( row, col ) );
            Qt::ItemFlags               flags      = _tableItem->flags();
            if ( parent )
            {
                flags |= Qt::ItemIsEnabled;
            }
            else
            {
                flags &= ~Qt::ItemIsEnabled;
            }
            _tableItem->setFlags( flags );
        }
    }
}


void
ParallelCalculation::calculate()
{
    QElapsedTimer t;
    qDebug() << QObject::tr( "[Advisor] Start calculation " ) << test->name().c_str();
    t.start();
    guard.lock();
    ( *calculations )[ this ]->setCalculating( true );
    guard.unlock();
    test->apply( cnodes, direct_calculation );
    guard.lock();
    ( *calculations )[ this ]->setCalculating( false );
    guard.unlock();
    foreach( ParallelCalculation * _pr, toNotify )
    {
        _pr->notifiedBy( this );
    }
    qDebug() << QObject::tr( "[Advisor] Finished calculation " ) << test->name().c_str() << QObject::tr( "in " ) << t.elapsed() << QObject::tr( "milliseconds" );
}
