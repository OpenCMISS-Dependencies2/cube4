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


#include "config.h"

#include <sstream>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QWhatsThis>
#include <QToolTip>
#include <QUrl>
#include <QFileDialog>
#include <QTextStream>
#include <QClipboard>
#include <QApplication>
#include <QMimeData>
#include <QDebug>
#include <QComboBox>
#include <QMessageBox>
#include <QToolButton>
#include "CubeProxy.h"
#include "CubeMetric.h"
#include "CubePL1Driver.h"

#include "NewDerivatedMetricWidget.h"
#include "CubePLSyntaxHighlighter.h"
#include "DerivedMetricsCollection.h"
#include "Globals.h"
#include "HelpBrowser.h"
#include "HtmlHelpCollection.h"
#include "StatusBar.h"
#include "PluginServices.h"
#include "MetricData.h"
#include "PluginServices.h"

using namespace std;
using namespace cubegui;
using namespace metric_editor;

// if the separator "::" is part of a derived metric name, it will be internally replaced by the following string
const QString NewDerivatedMetricWidget::separator = "__SEP__";

/*--- Constructors & destructor -------------------------------------------*/

/**
 * @brief Constructs a widget for construction of a new derivated metric
 *
 * This constructor creates a new help browser dialog with the given window
 * @p title and @p parent widget.
 *
 * @param  title   Window title
 * @param  parent  Parent widget
 */
NewDerivatedMetricWidget::NewDerivatedMetricWidget( cubepluginapi::PluginServices* _service,
                                                    cube::Metric*                  _working_metric,
                                                    cube::Metric*                  _met_parent,
                                                    QWidget*                       parent,
                                                    QList<MetricData*>&            _userMetrics )
    : QDialog( parent ), service( _service ), working_metric( _working_metric ), userMetrics( _userMetrics )
{
    isEdited                 = _working_metric != NULL;
    metricData               = new MetricData( service->getCube(), _met_parent );
    cube                     = service->getCube();
    metric_type_selection    = NULL;
    metric_library_selection = NULL;
    display_name_input       = NULL;
    unique_name_input        = NULL;
    uom_input                = NULL;
    url_input                = NULL;
    description_input        = NULL;

    calculations_container = NULL;

    calculation_input      = NULL;
    calculation_init_input = NULL;
    calculation_aggr_plus  = NULL;
    calculation_aggr_minus = NULL;
    calculation_aggr_aggr  = NULL;


    create_metric         = NULL;
    _whatis_help          = NULL;
    _metric_kinds_help    = NULL;
    _metric_expt_help     = NULL;
    _metric_lib_help      = NULL;
    metric_from_file      = NULL;
    metric_to_file        = NULL;
    metric_from_clipboard = NULL;

    isUnique = true;

    share_metric = NULL;

    error_highlighter            = NULL;
    error_init_highlighter       = NULL;
    error_aggr_plus_highlighter  = NULL;
    error_aggr_minus_highlighter = NULL;
    error_aggr_aggr_highlighter  = NULL;

    statusBar = new StatusBar();

    setAcceptDrops( true );
    if ( _met_parent != NULL )
    {
        metricData->setUniq_name( QString::fromStdString( _met_parent->get_uniq_name() ) );
        metricData->setDisp_name( QString::fromStdString( _met_parent->get_disp_name() ) );
        metricData->setUom( QString::fromStdString( _met_parent->get_uom() ) );
        metricData->setVal( QString::fromStdString( _met_parent->get_val() ) );
        metricData->setUrl( QString::fromStdString( _met_parent->get_url() ) );
        metricData->setDescr( QString::fromStdString( _met_parent->get_descr() ) );
    }

    if ( working_metric != NULL )
    {
        setWindowTitle( tr( "Edit metric " ).append( QString::fromStdString( working_metric->get_disp_name() ) ) );
    }
    else
    {
        setWindowTitle( tr( "Create new metric" ) );
    }
    QVBoxLayout* top_layout = new QVBoxLayout();
    top_layout->setSpacing( 5 );
    QHBoxLayout* layout1 = new QHBoxLayout();

    layout1->addWidget( new QLabel( tr( "<b>Select metric from collection :</b>" ) ) );
    metric_library_selection = new QComboBox();
    metric_library_selection->addItems( DerivedMetricsCollection::getDerivedMetricsNames() );
    metric_library_selection->setCurrentIndex( 0 );
    metric_library_selection->setWhatsThis( HelpCollection::getHelpText( cubegui::WHAT_IS_DERIVED_METRICS_LIBRARY ) );
    layout1->addWidget( metric_library_selection );
    connect( metric_library_selection, SIGNAL( currentIndexChanged( int  ) ), this, SLOT( selectMetricFromLibrary( int  ) ) );
    metric_library_selection->insertSeparator( DerivedMetricsCollection::size() );
    foreach( MetricData * data, userMetrics )
    {
        metric_library_selection->addItem( data->getDisp_name() );
        userMetricHash.insert( data->getUniq_name(), data );
    }

    layout1->addSpacing( 15 );

    QToolButton* addMetric = new QToolButton();
    addMetric->setText( " + " );
    QString doc = tr( "Adds current metric to collection." );
    addMetric->setWhatsThis( doc );
    addMetric->setToolTip( doc );
    layout1->addWidget( addMetric );
    connect( addMetric, SIGNAL( pressed() ), this, SLOT( addUserMetric() ) );

    QToolButton* removeMetric = new QToolButton();
    removeMetric->setText( " - " );
    doc = tr( "Removes current metric from collection." );
    removeMetric->setWhatsThis( doc );
    removeMetric->setToolTip( doc );
    layout1->addWidget( removeMetric );
    connect( removeMetric, SIGNAL( pressed() ), this, SLOT( removeUserMetric() ) );

    if ( working_metric != NULL )
    {
        MetricData* found = userMetricHash.value( QString::fromStdString( working_metric->get_uniq_name() ) );
        if ( found )
        {
            int index = userMetrics.indexOf( found );
            metric_library_selection->blockSignals( true );
            metric_library_selection->setCurrentIndex( index + DerivedMetricsCollection::size() + 1 );
            metric_library_selection->blockSignals( false );
        }

        removeMetric->setEnabled( found );
        metric_library_selection->setEnabled( false );
    }


    metric_to_file = new QPushButton( QIcon( ":images/savetofile.png" ), "" );
    metric_to_file->setToolTip( tr( "Stores current definition of a metric." ) );
    metric_to_file->setWhatsThis( HelpCollection::getHelpText( WHAT_IS_TO_FILE ) );
    layout1->addWidget( metric_to_file );
    connect( metric_to_file, SIGNAL( pressed() ), this, SLOT( saveMetricToFile() ) );
    layout1->addSpacing( 3 );

    metric_from_file = new QPushButton( QIcon( ":images/fromtextfile.png" ), "" );
    metric_from_file->setToolTip( tr( "Creates a metric from file" ) );
    metric_from_file->setWhatsThis( HelpCollection::getHelpText( WHAT_IS_FROM_FILE ) );
    layout1->addWidget( metric_from_file );
    connect( metric_from_file, SIGNAL( pressed() ), this, SLOT( createMetricFromFile() ) );

    metric_from_clipboard = new QPushButton( QIcon( ":images/paste-icon.png" ), "" );
    metric_from_clipboard->setToolTip( tr( "Creates a metric from clipboard contents" ) );
    metric_from_clipboard->setWhatsThis( HelpCollection::getHelpText( WHAT_IS_CLIPBOARD ) );
    layout1->addWidget( metric_from_clipboard );
    connect( metric_from_clipboard, SIGNAL( pressed() ), this, SLOT( createMetricFromClipboard() ) );


    layout1->addStretch();

    top_layout->addLayout( layout1 );
    QWidget* horizontalLineWidget = new QWidget;
    horizontalLineWidget->setFixedHeight( 2 );
    horizontalLineWidget->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
    horizontalLineWidget->setStyleSheet( QString( "background-color: #c0c0c0;" ) );
    top_layout->addWidget( horizontalLineWidget  );

    QGridLayout* grid_layout2 = new QGridLayout();

    // ------- grid --------------------------------------------------------------------
    int row = 0;
    grid_layout2->addWidget( new QLabel( tr( "<b>Parent metric :</b>" ) ), row, 0 );
    metric_parent_selection = new QComboBox();
    metric_parent_selection->setEnabled( working_metric == NULL );
    grid_layout2->addWidget( metric_parent_selection, row++, 1 );
    metric_parent_selection->addItem( tr( "None (create root metric)" ), QString() );
    addMetricItem( _met_parent );
    connect( metric_parent_selection, SIGNAL( currentIndexChanged( int ) ), this, SLOT( evaluateMetricParentCombo( int  ) ) );


    grid_layout2->addWidget( new QLabel( tr( "<b>Derived metric type :</b>" ) ), row, 0 );
    metric_type_selection = new QComboBox();
    metric_type_selection->addItem( tr( " --- please select type of derived metric ---- " ) );
    metric_type_selection->addItem( tr( "Postderived metric" ) );
    metric_type_selection->addItem( tr( "Prederived inclusive metric" ) );
    metric_type_selection->addItem( tr( "Prederived exclusive metric" ) );
    grid_layout2->addWidget( metric_type_selection, row, 1 );
    connect( metric_type_selection, SIGNAL( currentIndexChanged( int  ) ), this, SLOT( setMetricType( int  ) ) );
    _metric_kinds_help = new QPushButton( QIcon( ":images/whatishelp.png" ), "" );
    grid_layout2->addWidget( _metric_kinds_help, row++, 2 );
    connect( _metric_kinds_help, SIGNAL( pressed() ), this, SLOT( showKindsOfDerivedMetricsHelp() ) );


    grid_layout2->addWidget( new QLabel( tr( "<b>Display name :</b>" ) ), row, 0 );
    display_name_input = new QLineEdit();
    grid_layout2->addWidget( display_name_input, row++, 1 );
    if ( working_metric != NULL )
    {
        display_name_input->setText( working_metric->get_disp_name().c_str() );
        setDisplayName( working_metric->get_disp_name().c_str() );
        display_name_input->setEnabled( false );
    }
    connect( display_name_input, SIGNAL( textChanged( const QString & ) ), this, SLOT( setDisplayName( const QString & ) ) );


    grid_layout2->addWidget( new QLabel( tr( "<b>Unique name :</b>" ) ), row, 0 );
    unique_name_input =  new QLineEdit();
    grid_layout2->addWidget( unique_name_input, row++, 1 );
    if ( working_metric != NULL )
    {
        unique_name_input->setText( working_metric->get_uniq_name().c_str() );
        setUniqName( working_metric->get_uniq_name().c_str() );
        unique_name_input->setEnabled( false );
    }
    connect( unique_name_input, SIGNAL( textChanged( const QString & ) ), this, SLOT( setUniqName( const QString & ) ) );


    grid_layout2->addWidget( new QLabel( tr( "<b>Data type :</b>" ) ), row, 0 );
    metricDataType = new QLabel();
    evaluateMetricParentCombo( metric_parent_selection->currentIndex() );
    metricDataType->setEnabled( working_metric == NULL );
    grid_layout2->addWidget( metricDataType, row++, 1 );


    grid_layout2->addWidget( new QLabel( tr( "<b>Unit of measurement :</b>" ) ), row, 0 );
    uom_input = new QLineEdit();
    grid_layout2->addWidget( uom_input, row++, 1 );
    if ( working_metric != NULL )
    {
        uom_input->setText( working_metric->get_uom().c_str() );
        metricData->setUom( working_metric->get_uom().c_str() );
        uom_input->setEnabled( false );
    }
    connect( uom_input, SIGNAL( textChanged( const QString & ) ), this, SLOT( setUom( const QString & ) ) );


    grid_layout2->addWidget( new QLabel( tr( "<b>URL :</b>" ) ), row, 0 );
    url_input = new QLineEdit();
    grid_layout2->addWidget( url_input, row++, 1 );
    if ( working_metric != NULL )
    {
        url_input->setText( working_metric->get_url().c_str() );
        metricData->setUrl( working_metric->get_url().c_str() );
        url_input->setEnabled( false );
    }
    connect( url_input, SIGNAL( textChanged( const QString & ) ), this, SLOT( setUrl( const QString & ) ) );

    top_layout->addLayout( grid_layout2 );
    QVBoxLayout* layout3 = new QVBoxLayout();
    layout3->addWidget( new QLabel( tr( "<b>Description :</b>" ) ) );
    description_input = new QTextEdit();
    layout3->addWidget( description_input );
    if ( working_metric != NULL )
    {
        metricData->setDescr( working_metric->get_descr().c_str() );
        description_input->setText( metricData->getDescr() );
        description_input->setEnabled( false );
    }
    connect( description_input, SIGNAL( textChanged() ), this, SLOT( setDescription() ) );

    isGhostCheck = new QCheckBox( "ghost metric" );
    isGhostCheck->setToolTip( tr( "ghost metrics are only visible in expert mode" ) );
    isGhostCheck->setEnabled( working_metric == NULL );
    grid_layout2->addWidget( isGhostCheck, row++, 1 );
    isGhostCheck->setChecked( _met_parent && _met_parent->isGhost() );
    connect( isGhostCheck, SIGNAL( toggled( bool ) ), this, SLOT( setGhost( bool ) ) );

    isRowWiseCheck = new QCheckBox( "rowwise metric" );
    isRowWiseCheck->setToolTip( tr( "values of rowwise metrics are calculated using whole system tree rows. If CubePL expression depends on system resource id this shoudl be unchecked" ) );
    isRowWiseCheck->setEnabled( working_metric == NULL );
    grid_layout2->addWidget( isRowWiseCheck, row++, 1 );
    isRowWiseCheck->setChecked( true );
    connect( isRowWiseCheck, SIGNAL( toggled( bool ) ), this, SLOT( setRowWise( bool ) ) );



    // ------- end grid ----------------------------------------------------------------

    top_layout->addLayout( layout3 );

    QGridLayout* exp_layout = new QGridLayout();

    calculations_container = new QTabWidget();

    _metric_expt_help = new QPushButton( QIcon( ":images/whatishelp.png" ), "" );
    calculations_container->setCornerWidget( _metric_expt_help );
    connect( _metric_expt_help, SIGNAL( pressed() ), this, SLOT( showMetricExpressionHelp() ) );


    exp_layout->addWidget( calculations_container );

    calculation_input = new DerivedMetricEditor();
    calculations_container->addTab( calculation_input, tr( "&Calculation" ) );
    calculations_container->setTabIcon( 0, QIcon( ":images/source.png" ) );


    calculation_init_input = new DerivedMetricEditor();
    calculations_container->addTab( calculation_init_input, tr( "Calculation &Init" ) );
    calculations_container->setTabIcon( 1, QIcon( ":images/source.png" ) );
    calculation_aggr_plus = new DerivedMetricEditor();
    calculations_container->addTab( calculation_aggr_plus, tr( "Aggregation \"&+\"" ) );
    calculations_container->setTabIcon( 2, QIcon( ":images/source.png" ) );
    calculation_aggr_minus = new DerivedMetricEditor();
    calculations_container->addTab( calculation_aggr_minus, tr( "Aggregation \"&-\"" ) );
    calculations_container->setTabIcon( 3, QIcon( ":images/source.png" ) );
    calculation_aggr_aggr = new DerivedMetricEditor();
    calculations_container->addTab( calculation_aggr_aggr, tr( "Aggregation \"&aggr\"" ) );
    calculations_container->setTabIcon( 4, QIcon( ":images/source.png" ) );




    QStringList list = getCompletionList();
    connect( calculation_input, SIGNAL( textChanged() ), this, SLOT( setCubePLExpression() ) );
    calculation_input->setKeywords( list, metricLabelHash );
    error_highlighter = new CubePLSyntaxHighlighter( calculation_input->document() );
    error_highlighter->rehighlight();

    connect( calculation_init_input, SIGNAL( textChanged() ), this, SLOT( setCubePLInitExpression() ) );
    calculation_init_input->setKeywords( list, metricLabelHash );
    error_init_highlighter = new CubePLSyntaxHighlighter( calculation_init_input->document() );
    error_init_highlighter->rehighlight();

    connect( calculation_aggr_plus, SIGNAL( textChanged() ), this, SLOT( setCubePLAggrPlusExpression() ) );
    calculation_aggr_plus->setKeywords( list, metricLabelHash );
    error_aggr_plus_highlighter = new CubePLSyntaxHighlighter( calculation_aggr_plus->document() );
    error_aggr_plus_highlighter->rehighlight();

    connect( calculation_aggr_minus, SIGNAL( textChanged() ), this, SLOT( setCubePLAggrMinusExpression() ) );
    calculation_aggr_minus->setKeywords( list, metricLabelHash );
    error_aggr_minus_highlighter = new CubePLSyntaxHighlighter( calculation_aggr_minus->document() );
    error_aggr_minus_highlighter->rehighlight();

    connect( calculation_aggr_aggr, SIGNAL( textChanged() ), this, SLOT( setCubePLAggrAggrExpression() ) );
    calculation_aggr_aggr->setKeywords( list, metricLabelHash );
    error_aggr_aggr_highlighter = new CubePLSyntaxHighlighter( calculation_aggr_aggr->document() );
    error_aggr_aggr_highlighter->rehighlight();

    if ( working_metric != NULL )
    {
        if ( working_metric->get_type_of_metric() == cube::CUBE_METRIC_POSTDERIVED )
        {
            setMetricType( 1 );
        }
        else
        if ( working_metric->get_type_of_metric() == cube::CUBE_METRIC_PREDERIVED_INCLUSIVE )
        {
            setMetricType( 2 );
        }
        else
        if ( working_metric->get_type_of_metric() == cube::CUBE_METRIC_PREDERIVED_EXCLUSIVE )
        {
            setMetricType( 3 );
        }
        else
        {
            setMetricType( 0 );
        }
        metric_type_selection->setEnabled( false );

        calculation_input->setText( working_metric->get_expression().c_str() );
        calculation_init_input->setText( working_metric->get_init_expression().c_str() );
        calculation_aggr_plus->setText( working_metric->get_aggr_plus_expression().c_str() );
        calculation_aggr_minus->setText( working_metric->get_aggr_minus_expression().c_str() );
        calculation_aggr_aggr->setText( working_metric->get_aggr_aggr_expression().c_str() );

        setCubePLExpression();
        setCubePLInitExpression();
        setCubePLAggrPlusExpression();
        setCubePLAggrMinusExpression();
        setCubePLAggrAggrExpression();
    }






    top_layout->addLayout( exp_layout );

    QHBoxLayout* button_layout = new  QHBoxLayout();

    create_metric = new QPushButton( isEdited ? tr( "&Apply changes" ) : tr( "Create &metric" ) );
    connect( create_metric, SIGNAL( pressed() ), this, SLOT( createMetric() ) );
    button_layout->addWidget( create_metric );
    create_metric->setEnabled( unique_name_input->text() != "" );

    QPushButton* _close = new QPushButton( tr( "&Cancel" ) );
    connect( _close, SIGNAL( pressed() ), this, SLOT( close() ) );
    button_layout->addWidget( _close );



    top_layout->addLayout( button_layout );
    QWidget* horizontalLineWidget2 = new QWidget;
    horizontalLineWidget2->setFixedHeight( 2 );
    horizontalLineWidget2->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
    horizontalLineWidget2->setStyleSheet( QString( tr( "background-color: #c0c0c0;" ) ) );
    top_layout->addWidget( horizontalLineWidget2  );

    share_metric = new QLabel( ""  );
    enableShareLink( false );
    top_layout->addWidget( share_metric );

    top_layout->addWidget( statusBar );

    setLayout( top_layout );

    // initialize preDefinedMetrics and availableMetrics
    for ( int i = 1; i < DerivedMetricsCollection::size(); i++ )      // first predefined metric is empty
    {
        QString     def  = DerivedMetricsCollection::getDerivedMetricText( i );
        MetricData* data = new MetricData( cube, NULL );
        data->setCubePL( def );
        predefinedMetricHash.insert( data->getUniq_name(), data );
    }
    vector<cube::Metric*> _metrics = cube->getMetrics();
    for ( vector<cube::Metric*>::iterator m_iter = _metrics.begin(); m_iter != _metrics.end(); ++m_iter )
    {
        if ( ( *m_iter ) == NULL )
        {
            continue;
        }
        availableMetricNames.append( QString::fromStdString( ( *m_iter )->get_uniq_name() ) );
    }
    _metrics = cube->getGhostMetrics();
    for ( vector<cube::Metric*>::iterator m_iter = _metrics.begin(); m_iter != _metrics.end(); ++m_iter )
    {
        if ( ( *m_iter ) == NULL )
        {
            continue;
        }
        availableMetricNames.append( QString::fromStdString( ( *m_iter )->get_uniq_name() ) );
    }
}

void
NewDerivatedMetricWidget::addMetricItem( const cube::Metric* metric )
{
    if ( metric )
    {
        QString label = QString() + metric->get_disp_name().c_str() + " (" + metric->get_uniq_name().c_str() + ")";
        metric_parent_selection->addItem( label, QString( metric->get_uniq_name().c_str() ) );
        metric_parent_selection->setCurrentIndex( metric_parent_selection->count() - 1 );
    }
}


NewDerivatedMetricWidget::~NewDerivatedMetricWidget()
{
    calculation_input->disconnect();
    calculation_init_input->disconnect();
    calculation_aggr_plus->disconnect();
    calculation_aggr_minus->disconnect();
    calculation_aggr_aggr->disconnect();

    delete error_highlighter;
    delete metricData;
    foreach( MetricData * data, predefinedMetricHash.values() )
    delete data;
}


void
NewDerivatedMetricWidget::addUserMetric()
{
    QString uniq = metricData->getUniq_name();

    if ( !userMetricHash.contains( uniq ) ) // add metric to collection
    {
        bool alreadyExists = (   predefinedMetricHash.contains( uniq )
                                 || userMetricHash.contains( uniq ) );
        if ( !alreadyExists )
        {
            MetricData* metric = new MetricData( *metricData );
            userMetrics.append( metric );
            userMetricHash.insert( metric->getUniq_name(), metric );

            statusBar->addLine( tr( "Added user metric" ), Information );
            metric_library_selection->addItem( metricData->getDisp_name() );
            metric_library_selection->setCurrentIndex( metric_library_selection->count() - 1 );
        }
        else
        {
            statusBar->addLine( tr( "Cannot insert into collection: metric name is not unique" ), Error );
        }
    }
    else // replace
    {
        int separator = 1; // one index is used for separator
        int index     = metric_library_selection->currentIndex() - DerivedMetricsCollection::size() - separator;

        MetricData* toReplace = userMetrics.at( index );
        if ( uniq != toReplace->getUniq_name() && userMetricHash.contains( uniq )  ) // unique name has changed to existing one
        {
            statusBar->addLine( tr( "Metric name is alreay used by another user defined metric." ), Error );
            return;
        }
        metric_library_selection->setItemText( metric_library_selection->currentIndex(), metricData->getDisp_name() );
        toReplace->setCubePL( metricData->toString() );
        statusBar->addLine( tr( "Replaced user metric" ), Information );
    }
}

void
NewDerivatedMetricWidget::removeUserMetric()
{
    int found = -1;
    for ( int i = 0; i < userMetrics.size(); i++ )
    {
        MetricData* data = userMetrics.at( i );
        if ( data->getUniq_name() == metricData->getUniq_name() )
        {
            found = i;
            break;
        }
    }
    if ( found == -1 )
    {
        statusBar->addLine( tr( "Metric " ) + metricData->getUniq_name() + tr( " is not a user metric" ), Error );
    }
    else
    {
        int     separator = 1; // one index is used for separator
        QString name      = metricData->getUniq_name();
        userMetrics.removeAt( found );
        userMetricHash.remove( name );
        metric_library_selection->blockSignals( true );
        metric_library_selection->removeItem( DerivedMetricsCollection::size() + found + separator );
        metric_library_selection->setCurrentIndex( 0 );
        metric_library_selection->blockSignals( false );
        statusBar->addLine( tr( "Removed user metric " ) + name, Information );
    }
}

void
NewDerivatedMetricWidget::evaluateMetricParentCombo( const int idx )
{
    // set parent metric
    QString name = metric_parent_selection->itemData( idx ).toString();
    metricData->setParentMetric( name );

    // set child metric data type
    cube::Metric* parent = metricData->getParentMetric();
    metricDataType->setText( parent ? QString::fromStdString( parent->get_dtype() ) : QString::fromStdString( "DOUBLE" ) );
}

void
NewDerivatedMetricWidget::setGhost( bool isGhost )
{
    metricData->setGhost( isGhost );
}

void
NewDerivatedMetricWidget::setRowWise( bool isRowWise )
{
    metricData->setRowWise( isRowWise );
}



void
NewDerivatedMetricWidget::setMetricType( int type )
{
    metric_type_selection->setCurrentIndex( type );
    calculations_container->setTabEnabled( 0, true );
    calculations_container->setTabEnabled( 1, true );
    calculations_container->setTabEnabled( 2, true );
    calculations_container->setTabEnabled( 3, true );
    switch ( type )
    {
        case 1:
            metricData->setMetric_type( cube::CUBE_METRIC_POSTDERIVED );
            calculations_container->setTabEnabled( 2, false );
            calculations_container->setTabEnabled( 3, false );
            break;
        case 2:
            metricData->setMetric_type( cube::CUBE_METRIC_PREDERIVED_INCLUSIVE );
            break;
        case 3:
            metricData->setMetric_type( cube::CUBE_METRIC_PREDERIVED_EXCLUSIVE );
            calculations_container->setTabEnabled( 3, false );
            break;
        default:
            metricData->setMetric_type( cube::CUBE_METRIC_SIMPLE );     // just value to set the widget in unenabled state
            break;
    }
    if ( create_metric == NULL )
    {
        return;
    }
    create_metric->setEnabled( metricData->isValid() && isUnique );
}

void
NewDerivatedMetricWidget::setDisplayName(  const QString& new_disp_name )
{
    metricData->setDisp_name( new_disp_name );
    if ( create_metric == NULL )
    {
        return;
    }
    create_metric->setEnabled( metricData->isValid() && isUnique );
}

void
NewDerivatedMetricWidget::setUniqName( const QString& _new_uniq_name )
{
    QString new_uniq_name = _new_uniq_name.trimmed();
    metricData->setUniq_name( new_uniq_name );
    isUnique = true;

    if ( !isEdited )
    {
        vector<cube::Metric*> metrics = cube->getMetrics();
        vector<cube::Metric*> ghost   = cube->getGhostMetrics();
        metrics.insert( metrics.end(), ghost.begin(), ghost.end() );

        for ( vector<cube::Metric*>::iterator m_iter = metrics.begin(); m_iter != metrics.end(); ++m_iter )
        {
            if ( *m_iter == NULL )
            {
                continue;
            }
            if ( new_uniq_name == QString::fromStdString( ( *m_iter )->get_uniq_name() ) )
            {
                statusBar->addLine( tr( "Metric name is not unique" ), Error );
                if ( create_metric )
                {
                    create_metric->setEnabled( false );
                }
                isUnique = false;
                return;
            }
        }
        statusBar->addLine( tr( "Ok" ), Information, false );
    }
    if ( create_metric )
    {
        create_metric->setEnabled( metricData->isValid() && isUnique );
    }
}

void
NewDerivatedMetricWidget::setDescription()
{
    metricData->setDescr( description_input->toPlainText() );
}

void
NewDerivatedMetricWidget::setCubePLExpression()
{
    QString error_string = metricData->setCubePLExpression( calculation_input->toPlainText() );

    if ( error_string.isEmpty() )
    {
        calculations_container->setTabIcon( 0, ( metricData->getCalculation().isEmpty() ) ? QIcon( ":images/source.png" ) : QIcon( ":images/syntax_ok.png" ) );
        error_highlighter->setErrorPlace( -1 );
        statusBar->addLine( tr( "Ok" ), Information, false );
    }
    else
    {
        calculations_container->setTabIcon( 0, QIcon( ":images/syntax_error.png" ) );

        statusBar->addLine(  error_string, Error );

        QStringList list1        = error_string.split( ":" );
        QString     locations    = list1.at( 0 );
        QStringList list2        = locations.split( "." );
        QString     row          = list2.at( 0 );
        QString     columns      = ( list2.size() > 1 ) ? list2.at( 1 ) : row;
        QStringList list3        = columns.split( "-" );
        QString     column_start = list3.at( 0 );

        QString column_end = ( list3.size() > 1 ) ? list3.at( 1 ) : column_start;

//         int errorplace    = column_start.toInt();
        int errorplace_to = column_end.toInt();
        error_highlighter->setErrorPlace( errorplace_to );
    }

    if ( create_metric == NULL )
    {
        return;
    }

    create_metric->setEnabled( metricData->isValid() && isUnique );
    enableShareLink( create_metric->isEnabled() );
}


void
NewDerivatedMetricWidget::setCubePLInitExpression()
{
    QString error_string = metricData->setCubePLInitExpression( calculation_init_input->toPlainText() );

    if ( error_string.isEmpty() )
    {
        calculations_container->setTabIcon( 1, ( metricData->getInit_calculation().isEmpty() ) ? QIcon( ":images/source.png" ) : QIcon( ":images/syntax_ok.png" ) );
        error_init_highlighter->setErrorPlace( -1 );
        statusBar->addLine( tr( "Ok" ), Information, false );
    }
    else
    {
        calculations_container->setTabIcon( 1, QIcon( ":images/syntax_error.png" ) );
        statusBar->addLine(  error_string, Error );

        QStringList list1        = error_string.split( ":" );
        QString     locations    = list1.at( 0 );
        QStringList list2        = locations.split( "." );
        QString     row          = list2.at( 0 );
        QString     columns      = ( list2.size() > 1 ) ? list2.at( 1 ) : row;
        QStringList list3        = columns.split( "-" );
        QString     column_start = list3.at( 0 );

        QString column_end = ( list3.size() > 1 ) ? list3.at( 1 ) : column_start;



//         int errorplace    = column_start.toInt();
        int errorplace_to = column_end.toInt();
        error_init_highlighter->setErrorPlace( errorplace_to );
    }

    if ( create_metric == NULL )
    {
        return;
    }

    create_metric->setEnabled( metricData->isValid() && isUnique );
    enableShareLink( create_metric->isEnabled() );
}


void
NewDerivatedMetricWidget::setCubePLAggrPlusExpression( void )
{
    QString error_string = metricData->setCubePLAggrPlusExpression( calculation_aggr_plus->toPlainText() );
    if ( error_string.isEmpty() )
    {
        calculations_container->setTabIcon( 2, ( metricData->getCalculation_plus().isEmpty() ) ? QIcon( ":images/source.png" ) : QIcon( ":images/syntax_ok.png" ) );
        error_highlighter->setErrorPlace( -1 );
        statusBar->addLine( tr( "Ok" ), Information, false );
    }
    else
    {
        calculations_container->setTabIcon( 2, QIcon( ":images/syntax_error.png" ) );
        statusBar->addLine(  error_string, Error );

        QStringList list1        = error_string.split( ":" );
        QString     locations    = list1.at( 0 );
        QStringList list2        = locations.split( "." );
        QString     row          = list2.at( 0 );
        QString     columns      = ( list2.size() > 1 ) ? list2.at( 1 ) : row;
        QStringList list3        = columns.split( "-" );
        QString     column_start = list3.at( 0 );

        QString column_end = ( list3.size() > 1 ) ? list3.at( 1 ) : column_start;
//         int errorplace    = column_start.toInt();
        int errorplace_to = column_end.toInt();
        error_highlighter->setErrorPlace( errorplace_to );
    }
    if ( create_metric == NULL )
    {
        return;
    }

    create_metric->setEnabled( metricData->isValid() && isUnique );
    enableShareLink( create_metric->isEnabled() );
}


void
NewDerivatedMetricWidget::setCubePLAggrMinusExpression( void )
{
    QString error_string =     metricData->setCubePLAggrMinusExpression( calculation_aggr_minus->toPlainText() );
    if ( error_string.isEmpty() )
    {
        calculations_container->setTabIcon( 3, ( metricData->getCalculation_minus().isEmpty() ) ? QIcon( ":images/source.png" ) : QIcon( ":images/syntax_ok.png" ) );
        error_highlighter->setErrorPlace( -1 );
        statusBar->addLine( tr( "Ok" ), Information, false );
    }
    else
    {
        calculations_container->setTabIcon( 3, QIcon( ":images/syntax_error.png" ) );

        statusBar->addLine(  error_string, Error );

        QStringList list1        = error_string.split( ":" );
        QString     locations    = list1.at( 0 );
        QStringList list2        = locations.split( "." );
        QString     row          = list2.at( 0 );
        QString     columns      = ( list2.size() > 1 ) ? list2.at( 1 ) : row;
        QStringList list3        = columns.split( "-" );
        QString     column_start = list3.at( 0 );

        QString column_end = ( list3.size() > 1 ) ? list3.at( 1 ) : column_start;
//         int errorplace    = column_start.toInt();
        int errorplace_to = column_end.toInt();
        error_highlighter->setErrorPlace( errorplace_to );
    }
    if ( create_metric == NULL )
    {
        return;
    }

    create_metric->setEnabled( metricData->isValid() && isUnique );
    enableShareLink( create_metric->isEnabled() );
}

void
NewDerivatedMetricWidget::setCubePLAggrAggrExpression( void )
{
    QString error_string =     metricData->setCubePLAggrAggrExpression( calculation_aggr_aggr->toPlainText() );
    if ( error_string.isEmpty() )
    {
        calculations_container->setTabIcon( 4, ( metricData->getCalculation_aggr().isEmpty() ) ? QIcon( ":images/source.png" ) : QIcon( ":images/syntax_ok.png" ) );
        error_highlighter->setErrorPlace( -1 );
        statusBar->addLine( tr( "Ok" ), Information, false );
    }
    else
    {
        calculations_container->setTabIcon( 4, QIcon( ":images/syntax_error.png" ) );

        statusBar->addLine(  error_string, Error );

        QStringList list1        = error_string.split( ":" );
        QString     locations    = list1.at( 0 );
        QStringList list2        = locations.split( "." );
        QString     row          = list2.at( 0 );
        QString     columns      = ( list2.size() > 1 ) ? list2.at( 1 ) : row;
        QStringList list3        = columns.split( "-" );
        QString     column_start = list3.at( 0 );

        QString column_end = ( list3.size() > 1 ) ? list3.at( 1 ) : column_start;
//         int errorplace    = column_start.toInt();
        int errorplace_to = column_end.toInt();
        error_highlighter->setErrorPlace( errorplace_to );
    }
    if ( create_metric == NULL )
    {
        return;
    }

    create_metric->setEnabled( metricData->isValid() && isUnique );
    enableShareLink( create_metric->isEnabled() );
}

void
NewDerivatedMetricWidget::createMetric( void )
{
    if ( !prepareMetric() )
    {
        return;
    }

    if ( working_metric != NULL )     // editing an existing metric
    {
        cubeplparser::CubePLDriver* driver = &cube->getCubePlDriver();
        std::string                 expression;
        std::string                 cubepl_program;
        cube::GeneralEvaluation*    formula            = NULL;
        cube::GeneralEvaluation*    init_formula       = NULL;
        cube::GeneralEvaluation*    aggr_plus_formula  = NULL;
        cube::GeneralEvaluation*    aggr_minus_formula = NULL;
        cube::GeneralEvaluation*    aggr_aggr_formula  = NULL;

        if ( !metricData->getCalculation().isEmpty() )
        {
            expression     = metricData->getCalculation().toStdString();
            cubepl_program = string( "<cubepl>" ) + expression + string( "</cubepl>" );

            stringstream strin( cubepl_program );
            formula = driver->compile( &strin, &cerr );

            if ( formula != NULL )
            {
                working_metric->setEvaluation( formula );
                working_metric->set_expression( expression );
            }
        }
        if ( !metricData->getInit_calculation().isEmpty() )
        {
            expression     = metricData->getInit_calculation().toStdString();
            cubepl_program = string( "<cubepl>" ) + expression + string( "</cubepl>" );

            stringstream strin( cubepl_program );
            init_formula = driver->compile( &strin, &cerr );

            if ( init_formula != NULL )
            {
                working_metric->setInitEvaluation( init_formula );
                working_metric->set_init_expression( expression );
            }
        }
        if ( !metricData->getCalculation_aggr().isEmpty() )
        {
            expression     = metricData->getCalculation_aggr().toStdString();
            cubepl_program = string( "<cubepl>" ) + expression + string( "</cubepl>" );

            stringstream strin( cubepl_program );
            aggr_aggr_formula = driver->compile( &strin, &cerr );
            if ( aggr_plus_formula != NULL )
            {
                working_metric->setAggrAggrEvaluation( aggr_aggr_formula );
                working_metric->set_aggr_aggr_expression( expression );
            }
        }
        if ( working_metric->get_type_of_metric() == cube::CUBE_METRIC_PREDERIVED_EXCLUSIVE || working_metric->get_type_of_metric() == cube::CUBE_METRIC_PREDERIVED_INCLUSIVE )
        {
            if ( !metricData->getCalculation_plus().isEmpty() )
            {
                expression     = metricData->getCalculation_plus().toStdString();
                cubepl_program = string( "<cubepl>" ) + expression + string( "</cubepl>" );

                stringstream strin( cubepl_program );
                aggr_plus_formula = driver->compile( &strin, &cerr );
                if ( aggr_plus_formula != NULL )
                {
                    working_metric->setAggrPlusEvaluation( aggr_plus_formula );
                    working_metric->set_aggr_plus_expression( expression );
                }
            }
            if ( working_metric->get_type_of_metric() == cube::CUBE_METRIC_PREDERIVED_INCLUSIVE )
            {
                if ( !metricData->getCalculation_minus().isEmpty() )
                {
                    expression     = metricData->getCalculation_minus().toStdString();
                    cubepl_program = string( "<cubepl>" ) + expression + string( "</cubepl>" );

                    stringstream strin( cubepl_program );
                    aggr_minus_formula = driver->compile( &strin, &cerr );

                    if ( aggr_minus_formula != NULL )
                    {
                        working_metric->setAggrMinusEvaluation( aggr_minus_formula );
                        working_metric->set_aggr_minus_expression( expression );
                    }
                }
            }
        }
    }
    else
    {
        working_metric = metricData->createMetric();
        if ( working_metric == NULL )
        {
            statusBar->addLine(  tr( "Cannot create metric. Please check the type of parent metric." ), Error );
            return;
        }
        statusBar->addLine( tr( "Ok" ), Information, false );
    }
    accept();
}

void
NewDerivatedMetricWidget::showCubePLHelp( void )
{
    HelpBrowser* help_browser = HelpBrowser::getInstance( tr( "Help" ) );
    help_browser->showHtml( HelpCollection::getHelpText( CUBEPL_HELP ) );
    QApplication::restoreOverrideCursor();
}

void
NewDerivatedMetricWidget::showKindsOfDerivedMetricsHelp( void )
{
    HelpBrowser* help_browser = HelpBrowser::getInstance( tr( "Help" ) );
    help_browser->showHtml( HelpCollection::getHelpText( DERIVED_METRIC_HELP ) );
    QApplication::restoreOverrideCursor();
}


void
NewDerivatedMetricWidget::showMetricExpressionHelp( void )
{
    HelpBrowser* help_browser = HelpBrowser::getInstance( tr( "Help" ) );

    int top_tab = calculations_container->currentIndex();
    switch ( top_tab )
    {
        default:
        case 0:     // expression
            help_browser->showHtml( HelpCollection::getHelpText( DERIVED_METRIC_EXPRESSION_HELP ) );
            break;
        case 1:     // init expression
            help_browser->showHtml( HelpCollection::getHelpText( DERIVED_METRIC_INIT_EXPRESSION_HELP ) );
            break;
        case 2:     // plus expression
            help_browser->showHtml( HelpCollection::getHelpText( DERIVED_METRIC_AGGR_PLUS_EXPRESSION_HELP ) );
            break;
        case 3:     // minus expression
            help_browser->showHtml( HelpCollection::getHelpText( DERIVED_METRIC_AGGR_MINUS_EXPRESSION_HELP ) );
            break;
        case 4:     // aggr expression
            help_browser->showHtml( HelpCollection::getHelpText( DERIVED_METRIC_AGGR_AGGR_EXPRESSION_HELP ) );
            break;
    }
    QApplication::restoreOverrideCursor();
}

void
NewDerivatedMetricWidget::dragEnterEvent( QDragEnterEvent* event )
{
    if ( event->mimeData()->hasUrls() )
    {
        event->acceptProposedAction();
    }
}


void
NewDerivatedMetricWidget::dropEvent( QDropEvent* event )
{
    QList<QUrl>           _urls = event->mimeData()->urls();
    QList<QUrl>::iterator _iter = _urls.begin();
    QUrl                  _file = *_iter;
    event->acceptProposedAction();
    fillTheFormFromUrl( _file );
}



void
NewDerivatedMetricWidget::selectFileForFillingForm()
{
    QString _file = QFileDialog::getOpenFileName( this, tr( "Open File with definition of a derived metric" ),
                                                  QDir::currentPath(),
                                                  tr( "CubePL files (*.dm);;Text files (*.txt)" ) );
    if ( _file != "" )
    {
        QUrl _url( _file );
        fillTheFormFromUrl( _url );
    }
}

void
NewDerivatedMetricWidget::setUom( const QString& value )
{
    metricData->setUom( value );
}

void
NewDerivatedMetricWidget::setUrl( const QString& value )
{
    metricData->setUrl( value );
}

void
NewDerivatedMetricWidget::fillTheFormFromUrl( QUrl& _url )
{
    QFile _file( _url.path() );
    if ( !_file.open( QIODevice::ReadOnly | QIODevice::Text ) )
    {
        return;
    }

    QTextStream in( &_file );
    fillTheForm( in.readAll() );
    _file.close();
}


void
NewDerivatedMetricWidget::fillTheFormFromClipboard( void )
{
    const QClipboard* clipboard = QApplication::clipboard();
    const QMimeData*  mimeData  = clipboard->mimeData();

    if ( mimeData->hasText() )
    {
        fillTheForm( mimeData->text() );
    }
}

void
NewDerivatedMetricWidget::fillTheForm( const QString& cubePL )
{
    metricData->setCubePL( cubePL );

    if ( working_metric == NULL )
    {
        if ( metricData->getParentMetric() )
        {
            QString name = metricData->getParentMetric()->get_uniq_name().c_str();
            int     idx  = metric_parent_selection->findData( name );
            if ( idx >= 0 )
            {
                metric_parent_selection->setCurrentIndex( idx );
            }
            else
            {
                addMetricItem( metricData->getParentMetric() );
            }
        }
        else
        {
            metric_parent_selection->setCurrentIndex( 0 ); // root metric
        }
        if ( metricData->getMetric_type() == cube::CUBE_METRIC_POSTDERIVED )
        {
            metric_type_selection->setCurrentIndex( 1 );
        }
        else if ( metricData->getMetric_type() == cube::CUBE_METRIC_PREDERIVED_INCLUSIVE )
        {
            metric_type_selection->setCurrentIndex( 2 );
        }
        else if ( metricData->getMetric_type() == cube::CUBE_METRIC_PREDERIVED_EXCLUSIVE )
        {
            metric_type_selection->setCurrentIndex( 3 );
        }
        else
        {
            metric_type_selection->setCurrentIndex( 0 );
        }
        display_name_input->setText( metricData->getDisp_name() );
        unique_name_input->setText( metricData->getUniq_name() );
        uom_input->setText( metricData->getUom() );
        url_input->setText( metricData->getUrl() );
        description_input->setText( metricData->getDescr() );
        isGhostCheck->setChecked( metricData->isGhost() );
    }
    calculation_input->setText( metricData->getCalculation() );
    calculation_init_input->setText( metricData->getInit_calculation() );
    calculation_aggr_plus->setText( metricData->getCalculation_plus() );
    calculation_aggr_minus->setText( metricData->getCalculation_minus() );
    calculation_aggr_aggr->setText( metricData->getCalculation_aggr() );
}

void
NewDerivatedMetricWidget::createMetricFromFile( void )
{
    selectFileForFillingForm();
}

void
NewDerivatedMetricWidget::saveMetricToFile( void )
{
    QString _file = QFileDialog::getSaveFileName( this, tr( "Save Metric Definition into a file..." ),
                                                  QDir::currentPath(),
                                                  tr( "CubePL files(*.dm );; Text Files (*.txt)" ) );
    if ( _file != "" )
    {
        if ( !_file.endsWith( ".dm" ) )
        {
            _file.append( ".dm" );
        }
        QFile file( _file );
        if ( file.open( QIODevice::WriteOnly ) )
        {
            QTextStream _out( &file );
            switch ( metric_type_selection->currentIndex() )
            {
                default:
                case 0:
                    break;
                case 1:
                    _out << tr( "metric type: postderived" ) << Qt::endl << Qt::endl;
                    break;
                case 2:
                    _out << tr( "metric type: prederived_inclusive" ) << Qt::endl << Qt::endl;
                    break;
                case 3:
                    _out << tr( "metric type: prederived_exclusive" ) << Qt::endl << Qt::endl;
                    break;
            }
            _out << tr( "display name: " ) << display_name_input->text().toUtf8().data() << Qt::endl << Qt::endl;
            _out << tr( "unique name: " ) << unique_name_input->text().toUtf8().data() << Qt::endl << Qt::endl;
            _out << tr( "uom: " ) << uom_input->text().toUtf8().data() << Qt::endl << Qt::endl;
            _out << tr( "url: " ) << url_input->text().toUtf8().data() << Qt::endl << Qt::endl;
            _out << tr( "description: " ) << description_input->toPlainText().toUtf8().data() << Qt::endl << Qt::endl;
            _out << tr( "cubepl expression: " ) << calculation_input->toPlainText().toUtf8().data() << Qt::endl << Qt::endl;
            _out << tr( "cubepl init expression: " ) << calculation_init_input->toPlainText().toUtf8().data() << Qt::endl << Qt::endl;
            if ( metric_type_selection->currentIndex() > 1 )
            {
                _out << tr( "cubepl aggr expression: " ) << calculation_aggr_aggr->toPlainText().toUtf8().data() << Qt::endl << Qt::endl;
                _out << tr( "cubepl plus expression: " ) << calculation_aggr_plus->toPlainText().toUtf8().data() << Qt::endl << Qt::endl;
            }
            if ( metric_type_selection->currentIndex() == 2 )
            {
                _out << tr( "cubepl minus expression: " ) << calculation_aggr_minus->toPlainText().toUtf8().data() << Qt::endl << Qt::endl;
            }
        }
    }
}

void
NewDerivatedMetricWidget::createMetricFromClipboard( void )
{
    fillTheFormFromClipboard();
}


void
NewDerivatedMetricWidget::selectMetricFromLibrary( int type )
{
    if ( type < DerivedMetricsCollection::size() )
    {
        fillTheForm( DerivedMetricsCollection::getDerivedMetricText( type ) );
    }
    else
    {
        type -= DerivedMetricsCollection::size(); // start of user defined metrics
        type -= 1;                                // separator
        fillTheForm( userMetrics.at( type )->toString() );
    }
    if ( !isUnique )
    {
        statusBar->addLine( tr( "Metric has already been inserted." ), Error );
    }
}



void
NewDerivatedMetricWidget::enableShareLink( bool flag )
{
    QString _caption( tr( "Share this metric with SCALASCA group" ) );
    if ( !flag )
    {
        share_metric->setText( _caption );
        share_metric->setOpenExternalLinks( false  );
    }
    else
    {
        QString _text_intro( tr( "Dear Scalasca Team, \n\n"
                                 "this metric is very useful in my performance analysis and I would like to suggest to include it into the library of Cube:\n\n" ) );
        QString _metric = packDataToString();
        QString _text_closure( tr( "\n\n\n\n\n Sincerely," ) );
        share_metric->setText( tr( "<a href=\"mailto:scalasca@fz-juelich.de?subject=Please add this derived metric to your library&body=" ).
                               append( _text_intro ).
                               append( _metric ).
                               append( _text_closure ).
                               append( "\">" ).
                               append( _caption ).
                               append( "</a> " ) );
        share_metric->setOpenExternalLinks( true );
    }
}


QString
NewDerivatedMetricWidget::packDataToString( void )
{
    QString _to_return( tr( "Metric type:" ).append( metric_type_selection->currentText().append( "\n\n" ).append(
                                                         tr( "Display name:" ).append( display_name_input->text().append( "\n\n" ).append(
                                                                                           tr( "Unique name:" ).append( unique_name_input->text().append( "\n\n" ).append(
                                                                                                                            tr( "UoM:" ).append( uom_input->text().append( "\n\n" ).append(
                                                                                                                                                     tr( "URL:" ).append( url_input->text().append( "\n\n" ).append(
                                                                                                                                                                              tr( "Description:" ).append( description_input->toPlainText().append( "\n\n" ).append(
                                                                                                                                                                                                               tr( "CubePL Expression:" ).append( calculation_input->toPlainText().append( "\n\n" ).append(
                                                                                                                                                                                                                                                      tr( "CubePl Init Expression:" ).append( calculation_init_input->toPlainText().append( "\n\n" ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) );
    _to_return.replace( "\"", "\\\"" );
    return _to_return;
}

QStringList
NewDerivatedMetricWidget::getCompletionList()
{
    metricLabelHash.clear();
    QStringList           list;
    vector<cube::Metric*> _metrics       = cube->getMetrics();
    vector<cube::Metric*> _ghost_metrics = cube->getGhostMetrics();
    _metrics.insert( _metrics.end(), _ghost_metrics.begin(), _ghost_metrics.end() );
    for ( vector<cube::Metric*>::iterator m_iter = _metrics.begin(); m_iter != _metrics.end(); ++m_iter )
    {
        if ( *m_iter == NULL )
        {
            continue;
        }
        QString name    = QString::fromStdString( ( *m_iter )->get_uniq_name() );
        QString display = QString::fromStdString( ( *m_iter )->get_disp_name() );
        metricLabelHash.insert( name, display );
        name.replace( "::", separator );
        list.append( "metric::" + name );
        list.append( "metric::context::" + name );
        list.append( "metric::fixed::" + name );
        list.append( "metric::call::" + name );
        list.append( "metric::set::" + name );
        list.append( "metric::get::" + name );
    }

    metricLabelHash.insert( "context", "..." );
    metricLabelHash.insert( "fixed", "..." );
    metricLabelHash.insert( "call", "..." );
    metricLabelHash.insert( "set", "..." );
    metricLabelHash.insert( "get", "..." );

    list.append( "${cube::#mirrors" );
    list.append( "${cube::#metrics" );
    list.append( "${cube::#root::metrics" );
    list.append( "${cube::#regions" );
    list.append( "${cube::#callpaths" );
    list.append( "${cube::#root::callpaths" );
    list.append( "${cube::#locations" );
    list.append( "${cube::#locationgroups" );
    list.append( "${cube::#stns" );
    list.append( "${cube::#rootstns" );
    list.append( "${cube::filename" );
    list.append( "${cube::metric::uniq::name" );
    list.append( "${cube::metric::disp::name" );
    list.append( "${cube::metric::url" );
    list.append( "${cube::metric::description" );
    list.append( "${cube::metric::dtype" );
    list.append( "${cube::metric::uom" );
    list.append( "${cube::metric::expression" );
    list.append( "${cube::metric::initexpression" );
    list.append( "${cube::metric::#children" );
    list.append( "${cube::metric::parent::id" );
    list.append( "${cube::metric::children" );
    list.append( "${cube::metric::enumeration" );
    list.append( "${cube::callpath::mod" );
    list.append( "${cube::callpath::line" );
    list.append( "${cube::callpath::#children" );
    list.append( "${cube::callpath::children" );
    list.append( "${cube::callpath::calleeid" );
    list.append( "${cube::callpath::parent::id" );
    list.append( "${cube::callpath::enumeration" );
    list.append( "${cube::region::name" );
    list.append( "${cube::region::mangled::name" );
    list.append( "${cube::region::paradigm" );
    list.append( "${cube::region::role" );
    list.append( "${cube::region::url" );
    list.append( "${cube::region::description" );
    list.append( "${cube::region::mod" );
    list.append( "${cube::region::begin::line" );
    list.append( "${cube::region::end::line" );
    list.append( "${cube::stn::name" );
    list.append( "${cube::stn::description" );
    list.append( "${cube::stn::class" );
    list.append( "${cube::stn::#children" );
    list.append( "${cube::stn::children" );
    list.append( "${cube::stn::#locationgroups" );
    list.append( "${cube::stn::locationgroups" );
    list.append( "${cube::stn::parent::id" );
    list.append( "${cube::stn::parent::sysid" );
    list.append( "${cube::locationgroup::name" );
    list.append( "${cube::locationgroup::parent::id" );
    list.append( "${cube::locationgroup::parent::sysid" );
    list.append( "${cube::locationgroup::rank" );
    list.append( "${cube::locationgroup::type" );
    list.append( "${cube::locationgroup::void" );
    list.append( "${cube::locationgroup::#locations" );
    list.append( "${cube::locationgroup::locations" );
    list.append( "${cube::location::name" );
    list.append( "${cube::location::type" );
    list.append( "${cube::location::parent::id" );
    list.append( "${cube::location::parent::sysid" );
    list.append( "${cube::location::rank" );
    list.append( "${cube::location::void" );
    list.append( "${cube::#locations::void" );
    list.append( "${cube::#locations::nonvoid" );
    list.append( "${cube::#locationgroups::void" );
    list.append( "${cube::#locationgroups::nonvoid" );
    list.append( "${calculation::metric::id" );
    list.append( "${calculation::callpath::id" );
    list.append( "${calculation::callpath::state" );
    list.append( "${calculation::callpath::#elements" );
    list.append( "${calculation::region::id" );
    list.append( "${calculation::region::#elements" );
    list.append( "${calculation::sysres::id" );
    list.append( "${calculation::sysres::kind" );
    list.append( "${calculation::sysres::sysid" );
    list.append( "${calculation::sysres::state" );
    list.append( "${calculation::sysres::#elements" );
    return list;
}

/**
 * @brief NewDerivatedMetricWidget::getReferredMetrics
 * @param metricStr contents of the metric to search for referred metrics
 * @return list of metrics that are directly referred in metricStr
 */
QList<QString>
NewDerivatedMetricWidget::getNextReferredMetrics( const QString& metricStr )
{
    QList<QString> usedMetrics;
    int            start              = 0;
    const QString  METRIC_STR         = "metric::";
    const QString  METRIC_CONTEXT_STR = "metric::context::::";
    const QString  METRIC_FIXED_STR   = "metric::fixed::";
    const QString  METRIC_CALL_STR    = "metric::call::";
    const QString  METRIC_SET_STR     = "metric::set::";
    const QString  METRIC_GET_STR     = "metric::get::";

    QList<QString> metricKeywords;
    metricKeywords << METRIC_GET_STR << METRIC_SET_STR << METRIC_CALL_STR << METRIC_FIXED_STR << METRIC_CONTEXT_STR << METRIC_STR;

    foreach( QString prefix, metricKeywords )
    {
        start = 0;
        while ( ( start = metricStr.indexOf( prefix, start ) ) != -1 )
        {
            int end = metricStr.indexOf( '(', start );
            start = metricStr.lastIndexOf( "::", end ) + 2;
            QString metric = metricStr.mid( start, end - start ).trimmed();
            if ( !usedMetrics.contains( metric ) )
            {
                usedMetrics.append( metric );
            }
        }
    }
    return usedMetrics;
}

/** returns all uniq names of predefined metrics which are referred in metricStr. Recursively searches in all
   found metrics for further references */
QList<QString>
NewDerivatedMetricWidget::getReferredMetrics( MetricData* data )
{
    QString refStr = data->getCalculation() + data->getInit_calculation() + data->getCalculation_minus() + \
                     data->getCalculation_plus() + data->getCalculation_aggr();
    QList<QString> ret = getNextReferredMetrics( refStr );

    // search recursively for all referred metrics
    QList<QString> list = ret;
    while ( !list.isEmpty() )
    {
        QString met = list.takeFirst();

        // insert referred metric at the top of the return list
        int idx = ret.indexOf( met );
        if ( idx == -1 )
        {
            ret.insert( 0, met );
        }
        else
        {
            ret.move( idx, 0 );
        }

        // search for referred metrics inside the current one
        MetricData* data = predefinedMetricHash.value( met );
        if ( !data )
        {
            data = userMetricHash.value( met );
        }
        if ( data )
        {
            QString refStr = data->getCalculation() + data->getInit_calculation() + data->getCalculation_minus() + \
                             data->getCalculation_plus() + data->getCalculation_aggr();

            foreach( QString nextMet, getNextReferredMetrics( refStr ) )
            {
                if ( !ret.contains( nextMet ) && !list.contains( nextMet ) )
                {
                    list.append( nextMet );
                }
            }
        }
    }
    return ret;
}

/**
 * @brief NewDerivatedMetricWidget::prepareMetric
 * inserts all predefined metrics that are directly or indirectly referred by this metric
 */
bool
NewDerivatedMetricWidget::prepareMetric()
{
    // insert all predefined metrics that are referred in the new one
    QList<QString> referredMetrics = getReferredMetrics( metricData );

    foreach( QString referred, referredMetrics )
    {
        if ( ( !availableMetricNames.contains( referred ) ) && ( !predefinedMetricHash.contains( referred ) )
             && ( !userMetricHash.contains( referred ) ) )
        {
            QMessageBox::critical( this, tr( "Error" ), tr( "Referred metric " ) + referred + tr( " is not available" ) );
            return false;
        }
    }

    QStringList inserted;
    foreach( QString referred, referredMetrics )
    {
        if ( !availableMetricNames.contains( referred ) ) // not yet part of the cube data
        {
            MetricData* data = predefinedMetricHash.value( referred );
            if ( !data )
            {
                data = userMetricHash.value( referred );
            }
            if ( data )
            {
                cube::Metric* metric = data->createMetric();
                inserted.append( data->getUniq_name() );
                statusBar->addLine( QString( tr( "Inserting required metric: " ) ) + metric->get_uniq_name().c_str(), Information );

                // insert dependent metric into tree (only in Expert Mode, otherwise addMetric ignores ghost metrics)
                cube::Metric* parent = data->getParentMetric();
                if ( parent == NULL ) // create top level metric
                {
                    service->addMetric( metric );
                }
                else // create child metric
                {
                    TreeItem* parentItem = service->getMetricTreeItem( parent->get_uniq_name().c_str() );
                    service->addMetric( metric, parentItem );
                }
                service->setMessage( tr( "Added referred metrics as hidden: " ) + data->getUniq_name() );
            }
        }
    }

    if ( !Globals::optionIsSet( ExpertMode ) && !inserted.isEmpty() )
    {
        QMessageBox::information( this, tr( "added metrics" ), tr( "Added referred metrics as hidden: \n" ) + inserted.join( "," ) );
    }
    return true;
}
