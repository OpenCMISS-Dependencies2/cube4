/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2021                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2009-2015                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include "config.h"

#include "Barplot.h"
#include "PluginServices.h"
#include "TreeItem.h"
#include "CallTree.h"
#include <QVBoxLayout>
#include <QtPlugin>

#include <QString>
#include <iostream>
#include <string.h>
using namespace std;
using namespace cubepluginapi;

/**  If the plugin doesn't load, the most likely reason is an undefined reference.
 * - go into the build-backend/ directory of the cube sources
 * - export CHECK_PLUGIN=$PREFIX/plugins/libDemoPlugin.so
 * - call "make check-plugin"
 */

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2( SystemBarplotPlugin, Barplot );
#endif

Barplot::Barplot() : currentTreeItemLoopItem( false ), tabNotPresent( true )
{
}


/******************************************************************************************************
* start of CubePlugin implementation
******************************************************************************************************/
/** set a version number, the plugin with the highest version number will be loaded */
void
Barplot::version( int& major, int& minor, int& bugfix ) const
{
    major  = 1;
    minor  = 0;
    bugfix = 0;
}

QString
Barplot::getHelpText() const
{
    return "";
}

/**
 * returns the unique plugin name
 */
QString
Barplot::name() const
{
    return "System Barplot";
}

/**
 * starting point of the plugin
 */
bool
Barplot::cubeOpened( PluginServices* service )
{
    this->service = service;

    dataProvider_H = 0;

    tabNotPresent    = true;
    widget_          = new QWidget();
    mathOpList       = new QComboBox();
    colorList        = new QComboBox();
    noColorListLabel = new QLabel( tr( "Automatic" ) );
    noColorListLabel->hide();
    stackButton = new QPushButton( tr( "Keep on Stack" ) );
    removeAll   = new QPushButton( tr( "Clean Stack" ) );

    PlotLayout      = new QGridLayout();
    OperationalBoxH = new QHBoxLayout();
    WidgetLayout    = new QVBoxLayout( widget_ );


    layout1 = new QFormLayout();
    layout1->setRowWrapPolicy( QFormLayout::WrapLongRows );
    layout1->addRow( tr( "Operation:" ), mathOpList );

    layout2 = new QFormLayout();
    layout2->setRowWrapPolicy( QFormLayout::WrapLongRows );
    layout2->addRow( tr( "Color:" ), colorList );

    // QStringList colorNames = QColor::colorNames();
    QStringList colorNames = dataProvider_H->createColorPalettesBarplot();

    short        size = colorList->style()->pixelMetric( QStyle::PM_SmallIconSize );
    QPixmap      icon( size, size );
    unsigned int i = 0;
    foreach( QString colorName, colorNames )
    {
        colorList->addItem( colorName, QColor( i ) );
        icon.fill( QColor( colorName ) );
        colorList->setItemData( i, icon, Qt::DecorationRole );
        i++;
    }
    colorList->setMaximumWidth( 150 );
    mathOpList->setMaximumWidth( 150 );

    OperationalBoxH->addLayout( layout1 );
    OperationalBoxH->addLayout( layout2 );

    // OperationalBoxH->addWidget( stackButton );
    // OperationalBoxH->addWidget( removeAll );

    QHBoxLayout* buttonLine = new QHBoxLayout();
    buttonLine->addWidget( stackButton );
    buttonLine->addWidget( removeAll );

    WidgetLayout->addLayout( OperationalBoxH );
    WidgetLayout->addLayout( buttonLine );
    WidgetLayout->addLayout( PlotLayout );
    widget_->setLayout( WidgetLayout );
    widget_->layout()->setSpacing( 0 );

    changeColorAutomatically = false;

    // add menu item and submenu to the Plugin Menu
    QMenu* menu = service->enablePluginMenu();
    menuAction = menu->addAction( tr( "BarPlot Settings" ) );
    // connect( menuAction, SIGNAL( triggered() ), this, SLOT( menuItemIsSelected() ) );
    //  service->addPluginMenu("BarPlot Submenu (without elements)");

    // registered plugins may load and save settings
    service->addSettingsHandler( this );

    // react on signals emitted by PluginServices
    connect( service, SIGNAL( globalValueChanged( QString ) ),
             this, SLOT( globalValueChanged( QString ) ) );

    connect( service, SIGNAL( orderHasChanged( const QList<cubepluginapi::DisplayType> & ) ),
             this, SLOT( orderHasChanged( const QList<cubepluginapi::DisplayType> & ) ) );

    connect( service, SIGNAL( genericUserAction( cubepluginapi::UserAction ) ),
             this, SLOT( markItemAsLoop( cubepluginapi::UserAction ) ) );


    if ( service->hasIterations() )
    {
        addTabToGUI();
    }
    return true; // initialisation is ok => plugin should be shown
}

void
Barplot::initialize()
{
    service->setMessage( tr( "Initializing barplot plugin..." ) );

    cubeReader     = new CubeRead( service->getCube() );
    dataProvider_H = new DataProvider( cubeReader );

    // /create controllers
    barsPlotAreaController            = new BarsPlotAreaController( widget() );
    plotAreaHorizontalRulerController = new HorizontalRulerController( widget() );
    plotAreaVerticalRulerController   = new VerticalRulerController( widget() );
    plotsListController               = new PlotsListController( dataProvider_H, widget() );

    // / Setting  ruler limits
    plotAreaVerticalRulerController->setLimitsF( 0, 2 );
    plotAreaHorizontalRulerController->setLimits( 0, dataProvider_H->getIterationsCount() );
    // / Setting  ruler widgets default notches count
    plotAreaHorizontalRulerController->setMinorNotchesCount( plotWidget::DEFAULT_PLOT_WIDGET_MINOR_NOTCHES_COUNT );
    plotAreaHorizontalRulerController->setMajorNotchesCount( plotWidget::DEFAULT_PLOT_WIDGET_MAJOR_NOTCHES_COUNT );
    plotAreaVerticalRulerController->setMinorNotchesCount( plotWidget::DEFAULT_PLOT_WIDGET_MINOR_NOTCHES_COUNT );
    plotAreaVerticalRulerController->setMajorNotchesCount( plotWidget::DEFAULT_PLOT_WIDGET_MAJOR_NOTCHES_COUNT );
    // / define setting menu
    settingWidget = new SettingWidget( plotWidget::DEFAULT_PLOT_WIDGET_MINOR_NOTCHES_COUNT, plotWidget::DEFAULT_PLOT_WIDGET_MINOR_NOTCHES_COUNT,
                                       plotWidget::DEFAULT_PLOT_WIDGET_MAJOR_NOTCHES_COUNT, plotWidget::DEFAULT_PLOT_WIDGET_MINOR_NOTCHES_COUNT, widget() );


    // /Adding controllers view to the main view
    // /
    setWidgetNorth( plotAreaHorizontalRulerController->getView() );
    setWidgetWest( plotAreaVerticalRulerController->getView() );
    setWidgetCenter( barsPlotAreaController->getView() );

    // / Setting mouse lines alignment for heat map plot controller
    barsPlotAreaController->updateMouseAxisHorizontalAlignment( plotWidget::Left );
    barsPlotAreaController->updateMouseAxisVerticalAlignment( plotWidget::Bottom );

    // / save image controller
    plotImageSaveController = new ImageSaverController( barsPlotAreaController, plotAreaHorizontalRulerController, plotAreaVerticalRulerController );

    contextMenu          = new QMenu( barsPlotAreaController->getView() );
    saveImageContextMenu = contextMenu->addAction( tr( "Save Image ..." ) );

    // init color and mathop combobox _______________________
    QList<int> colorIndices;
    int        i = 0;
    foreach( QColor color, plotWidget::CALC_COLORS )
    {
        i = 0;
        QStringList colorNames = dataProvider_H->createColorPalettesBarplot();
        foreach( QColor colorName, colorNames )
        {
            if ( color == QColor( colorName ) )
            {
                colorIndices.append( i );
                break;
            }
            i++;
        }
    }

    setValues( plotWidget::CALC_NAMES, colorIndices );
    // _________________________________________________________

    // / Connect heat map area with updates to the mesh lines positions coming from noth vertical and horizontal rulers
    QObject::connect( plotAreaVerticalRulerController, SIGNAL( majorNotchesLocationsChanged( QList<int>) ), barsPlotAreaController, SLOT( updateHorizontalMajorMeshLines( QList<int>) ) );
    QObject::connect( plotAreaVerticalRulerController, SIGNAL( minorNotchesLocationsChanged( QList<int>) ), barsPlotAreaController, SLOT( updateHorizontalMinorMeshLines( QList<int>) ) );
    QObject::connect( plotAreaHorizontalRulerController, SIGNAL( majorNotchesLocationsChanged( QList<int>) ), barsPlotAreaController, SLOT( updateVerticalMajorMeshLines( QList<int>) ) );
    QObject::connect( plotAreaHorizontalRulerController, SIGNAL( minorNotchesLocationsChanged( QList<int>) ), barsPlotAreaController, SLOT( updateVerticalMinorMeshLines( QList<int>) ) );

    QObject::connect( dataProvider_H, SIGNAL( dataUpdated( QList<CubeDataItem>& ) ), barsPlotAreaController, SLOT( updateData( QList<CubeDataItem>& ) ) );
    QObject::connect( dataProvider_H, SIGNAL( barPlotDisable() ), barsPlotAreaController, SLOT( reset() ) );

    QObject::connect( dataProvider_H, SIGNAL( changeCalibrateStatus( bool ) ), plotAreaVerticalRulerController, SLOT( setCalibrateStatus( bool ) ) );
    QObject::connect( dataProvider_H, SIGNAL( changeCalibrateStatus( bool ) ), plotAreaHorizontalRulerController, SLOT( setCalibrateStatus( bool ) ) );

    //   QObject::connect( dataProvider_H, SIGNAL( changeDisplayStatus(bool) ), barsPlotAreaController, SLOT( setDisplayStatus(bool) ));


    QObject::connect( dataProvider_H, SIGNAL( limitsHaveChanged( double, double ) ), plotAreaVerticalRulerController, SLOT( setLimitsF( double, double ) ) );
    QObject::connect( dataProvider_H, SIGNAL( hLimitsHaveChangedHeatmap( int, int ) ), plotAreaHorizontalRulerController, SLOT( setLimits( int, int ) ) );

    QObject::connect( dataProvider_H, SIGNAL( dataUpdated( QList<CubeDataItem>& ) ), plotsListController, SLOT( setData( QList<CubeDataItem>& ) ) );

    QObject::connect( barsPlotAreaController, SIGNAL( mouseIsClicked( QMouseEvent* ) ), this, SLOT( barsPlotAreaMouseCoordEventHandler( QMouseEvent* ) ) );
    QObject::connect( barsPlotAreaController->getView(), SIGNAL( customContextMenuRequested( const QPoint & ) ), this, SLOT( ShowContextMenu( const QPoint & ) ) );
    QObject::connect( saveImageContextMenu, SIGNAL( triggered() ), plotImageSaveController, SLOT( saveImage() ) );


    QObject::connect( dataProvider_H, SIGNAL( updateColorCombobox( QColor ) ), this, SLOT( changeComboboxColor( QColor ) ) );
    QObject::connect( this, SIGNAL( requestToChangePlotColor() ), dataProvider_H, SLOT( changePlotColor() ) );
    QObject::connect( this, SIGNAL( requestToAddNewPlot() ), dataProvider_H, SLOT( setAddMetricSettings() ) );

    QObject::connect( getColorList(), SIGNAL( currentIndexChanged( int ) ), this, SLOT( colorComboHandler() ) );
    QObject::connect( getMathOpList(), SIGNAL( currentIndexChanged( int ) ), this, SLOT( opComboHandler( int ) ) );
    QObject::connect( menuAction, SIGNAL( triggered() ), settingWidget, SLOT( show() ) );

    QObject::connect( dataProvider_H, SIGNAL( maxValueIsCalculatedAutomatically() ), settingWidget, SLOT( setTopNotchToAutomatic() ) );
    QObject::connect( dataProvider_H, SIGNAL( minValueIsCalculatedAutomatically() ), settingWidget, SLOT( setBottomNotchToAutomatic() ) );

    QObject::connect( settingWidget, SIGNAL( resetMeasurementTopNotchValue() ), dataProvider_H, SLOT( resetLimitMaxValue() ) );
    QObject::connect( settingWidget, SIGNAL( resetMeasurementBottomNotchValue() ), dataProvider_H, SLOT( resetLimitMinValue() ) );
    QObject::connect( settingWidget, SIGNAL( setMeasurementTopNotchValue( double ) ), dataProvider_H, SLOT( setCurrentLimitMaxValue( double ) ) );
    QObject::connect( settingWidget, SIGNAL( setMeasurementBottomNotchValue( double ) ), dataProvider_H, SLOT( setCurrentLimitMinValue( double ) ) );
    QObject::connect( settingWidget, SIGNAL( setProcessesRulerMajorNotchesCount( int ) ), plotAreaHorizontalRulerController, SLOT( setMajorNotchesCount( int ) ) );
    QObject::connect( settingWidget, SIGNAL( setProcessesRulerMajorNotchesInterval( int ) ), plotAreaHorizontalRulerController, SLOT( setMajorNotchesInterval( int ) ) );
    QObject::connect( settingWidget, SIGNAL( setProcessesRulerMinorNotchesCount( int ) ), plotAreaHorizontalRulerController, SLOT( setMinorNotchesCount( int ) ) );
    QObject::connect( settingWidget, SIGNAL( setMeasurementRulerMajorNotchesCount( int ) ), plotAreaVerticalRulerController, SLOT( setMajorNotchesCount( int ) ) );
    QObject::connect( settingWidget, SIGNAL( setMeasurementRulerMajorNotchesInterval( double ) ), plotAreaVerticalRulerController, SLOT( setMajorNotchesIntervalF( double ) ) );
    QObject::connect( settingWidget, SIGNAL( setMeasurementRulerMinorNotchesCount( int ) ), plotAreaVerticalRulerController, SLOT( setMinorNotchesCount( int ) ) );
    QObject::connect( settingWidget, SIGNAL( setMeasurementRulerMinorNotchesCount( int ) ), plotAreaVerticalRulerController, SLOT( setMinorNotchesCount( int ) ) );

    QObject::connect( removeAll, SIGNAL( clicked() ), plotsListController, SLOT( deleteItemHandler() ) );
    QObject::connect( removeAll, SIGNAL( clicked() ), dataProvider_H, SLOT( DisableBarPlot() ) );

    QObject::connect( stackButton, SIGNAL( clicked() ), this, SLOT( stackButtonHasClicked() ) );

    service->setMessage( tr( "Finished initialization." ) );
}

void
Barplot::cubeClosed()
{
    delete widget_;
}

/******************************************************************************************************
* start of TabInterface implementation
******************************************************************************************************/

/**
 * returns the tab label of the created system tab
 */

QString
Barplot::label() const
{
    return tr( "Barplot" );
}

/** returns the icon which will be placed left to the tab label */
QIcon
Barplot::icon() const
{
    return QIcon( ":/images/barplot.png" );
}

/**
 * returns widget that will be placed into the tab
 */
QWidget*
Barplot::widget()
{
    return widget_;
}

/**
 * @brief BarplotPlugin::setActive is called when the tab gets activated or deactivated by selecting another tab
 * The tab related Barplot elements should only react on signals, if the tab is active. For that reason the
 * signals are disconnected if another tab becomes active.
 */
void
Barplot::setActive( bool active )
{
    if ( active )
    {
        if ( !dataProvider_H )
        {
            initialize();
        }

        service->connect( service, SIGNAL( treeItemIsSelected( cubepluginapi::TreeItem* ) ),
                          this, SLOT( treeItemIsSelected( cubepluginapi::TreeItem* ) ) );

        QString txt = service->getSelection( METRIC )->getName();

        dataProvider_H->getMetricNames(); // to fill metricnames array of dataprovider

        TreeItem* calltreeItem = service->getSelection( CALL );
        if ( !calltreeItem->isAggregatedLoopItem() && !calltreeItem->isAggregatedRootItem() )
        {
            dataProvider_H->DisableBarPlot();
            currentTreeItemLoopItem = false;
            return;
        }
        currentTreeItemLoopItem = true;

        AggregatedTreeItem*        aggregatedItem = static_cast<AggregatedTreeItem*> ( calltreeItem );
        const QList<cube::Cnode*>& clist          = aggregatedItem->getIterations();

        dataProvider_H->setMetricName( txt );
        dataProvider_H->setIterationsFunc( clist );

        if ( calltreeItem->isExpanded() == true )
        {
            dataProvider_H->setCalcType( cube::CUBE_CALCULATE_EXCLUSIVE );
            dataProvider_H->setAddMetricSettings();
        }
        else
        {
            dataProvider_H->setCalcType( cube::CUBE_CALCULATE_INCLUSIVE  );
            dataProvider_H->setAddMetricSettings();
        }
    }
    else
    {
        service->disconnect( service, SIGNAL( treeItemIsSelected( cubepluginapi::TreeItem* ) ),
                             this, SLOT( treeItemIsSelected( cubepluginapi::TreeItem* ) ) );
    }
}

/** Returns the minimal width that is necessary to display all information.
    This value is used in the menu item "Display->Optimize width".
 */
QSize
Barplot::sizeHint() const
{
    return QSize( 100, 100 );
}

void
Barplot::valueModusChanged( ValueModus modus )
{
    ( void )modus;
}

// ******************************************************************************************************


/******************************************************************************************************
* implementation of slots to handle signals of PluginServices
******************************************************************************************************/

/**
 * @brief Example::loadSettings loads settings from global setting file
 * the plugin should create a group with its name as group label*/

void
Barplot::loadGlobalSettings( QSettings& settings )
{
    ( void )settings; // to suppress warning... why this call exists at all?
}

/**
   Saves settings system specific file, for UNIX systems to $HOME/.config/FZJ/Cube.conf
 */
void
Barplot::saveGlobalSettings( QSettings& settings )
{
    int numCalls = settings.value( "BarplotPluginCalls", 0 ).toInt(); // 0 is default, if no value exists
    settings.setValue( "BarplotPluginCalls", ++numCalls );
}
QString
Barplot::settingName()
{
    return "barplotPlugin";
}

void
Barplot::treeItemIsSelected( TreeItem* item )
{
    DisplayType type         = item->getDisplayType();
    TreeItem*   calltreeItem = ( type == CALL ? item : service->getSelection( CALL ) );

    if ( !calltreeItem->isAggregatedLoopItem() && !calltreeItem->isAggregatedRootItem() )
    {
        currentTreeItemLoopItem = false;
        dataProvider_H->DisableBarPlot();
        return;
    }
    currentTreeItemLoopItem = true;

    AggregatedTreeItem*        aggregatedItem = static_cast<AggregatedTreeItem*> ( calltreeItem );
    const QList<cube::Cnode*>& clist          = aggregatedItem->getIterations();

    QString metric = ( type == METRIC ? item->getName() : service->getSelection( METRIC )->getName() );

    dataProvider_H->setMetricName( metric );
    dataProvider_H->setIterationsFunc( clist );


    if ( item->isExpanded() == true )
    {
        dataProvider_H->setCalcType( cube::CUBE_CALCULATE_EXCLUSIVE );
        dataProvider_H->setAddMetricSettings();
    }
    else
    {
        dataProvider_H->setCalcType( cube::CUBE_CALCULATE_INCLUSIVE  );
        dataProvider_H->setAddMetricSettings();
    }
}

/**
 * called, if the user has selected the menu item "Display->Dimension Order"
 */
void
Barplot::orderHasChanged( const QList<cubepluginapi::DisplayType>& order )
{
    // example: disable Barplot plugin, if system widget is first
    bool enabled = order.at( 0 ) != SYSTEM;
    service->enableTab( this, enabled );
}

/**
 * @brief BarplotPlugin::globalValueChanged is called if the value of a global variable has changed. In this
 * example, the value of "testVal" is incremented if the plugin menu item is selected.
 */
void
Barplot::globalValueChanged( const QString& name )
{
    ( void )name;
}

// ___________ specific for Barplot
void
Barplot::setWidgetNorth( QWidget* WidgetNorth )
{
    WidgetNorth->setFixedHeight( plotWidget::DEFAULT_RULER_THICKNESS_NORTH );
    WidgetNorth->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
    PlotLayout->addWidget( WidgetNorth, 0, 1 );
}
void
Barplot::setWidgetWest( QWidget* WidgetWest )
{
    WidgetWest->setFixedWidth( plotWidget::DEFAULT_RULER_THICKNESS_WEST );
    PlotLayout->addWidget( WidgetWest, 1, 0 );
}
void
Barplot::setWidgetCenter( QWidget* WidgetCenter )
{
    WidgetCenter->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    PlotLayout->addWidget( WidgetCenter, 1, 1 );
}
void
Barplot::barsPlotAreaMouseCoordEventHandler( QMouseEvent* mouseEvent )
{
    {
        int    iterationId = plotAreaHorizontalRulerController->pixelCoordToRulerCoord( mouseEvent->pos().x() );
        double valueF      = plotAreaVerticalRulerController->pixelCoordToRulerCoordF( mouseEvent->pos().y() );
        if ( iterationId == -1 || valueF == -1.0 )
        {
            return;
        }
        // / 1. get the x-coordinate from horizontal ruler (integer)
        QString status = tr( "Iteration: " ) + QString::number( iterationId );
        // / 2. get the y-coordinate from vertical ruler (double)
        status += tr( " Value: " ) + QString::number( valueF, 'g', 4 );
        // / 3. send a message to the permanent status-bar of our view
        QToolTip::showText( mouseEvent->globalPos(), status, widget_ );
    }
}
void
Barplot::setValues( QStringList mathOps, QList<int> colorIndices )
{
    mathOpList->addItems( mathOps );
    this->colorindices.append( colorIndices );

    // set default
    dataProvider_H->setOpNameBarPlot( plotWidget::Minimum );
    dataProvider_H->setColorBarPlot( plotWidget::CALC_COLORS[ ( int )plotWidget::Minimum ] );

    mathOpList->setCurrentIndex( 0 );
    mathOpListCurrentIndexChangedHandler( 0 );
}

void
Barplot::mathOpListCurrentIndexChangedHandler( int newIndex )
{
    if ( newIndex >= colorindices.size() || newIndex < 0 )
    {
        return;
    }
    colorList->setCurrentIndex( colorindices.at( newIndex ) );
}

QComboBox*
Barplot::getColorList() const
{
    return this->colorList;
}
QComboBox*
Barplot::getMathOpList() const
{
    return this->mathOpList;
}
void
Barplot::hideColorList()
{
    int                   widgetIndex = 0;
    QFormLayout::ItemRole widgetRole;
    layout2->getWidgetPosition( getColorList(), &widgetIndex, &widgetRole );

    if ( -1 != widgetIndex )
    {
        layout2->removeWidget( getColorList() );
        getColorList()->hide();
        layout2->setWidget( widgetIndex, widgetRole, noColorListLabel );
        noColorListLabel->show();
    }
}
void
Barplot::showColorList()
{
    int                   widgetIndex = 0;
    QFormLayout::ItemRole widgetRole;
    layout2->getWidgetPosition( noColorListLabel, &widgetIndex, &widgetRole );
    if ( -1 != widgetIndex )
    {
        layout2->removeWidget( noColorListLabel );
        noColorListLabel->hide();
        layout2->setWidget( widgetIndex, widgetRole, getColorList() );
        getColorList()->show();
    }
}
void
Barplot::mathOpIndexChangeHandler( int )
{
    if ( qStringToMathOp( getMathOpList()->currentText() ) == plotWidget::MathOpUnknown )
    {
        hideColorList();
    }
    else
    {
        showColorList();
    }
}
void
Barplot::opComboHandler( int newIndex )
{
    // don't do anything for items which are not an aggregated loop
    if ( !currentTreeItemLoopItem )
    {
        return;
    }
    plotWidget::MathOp mathOp = qStringToMathOp( getMathOpList()->currentText() );

    if ( mathOp != plotWidget::MathOpUnknown )
    {
        showColorList();
        //     QColor color(getColorList()->currentText());
        if ( newIndex >= colorindices.size() || newIndex < 0 )
        {
            return;
        }
        changeColorAutomatically = true;
        colorList->setCurrentIndex( colorindices.at( newIndex ) );
        QColor color( getColorList()->currentText() );
        dataProvider_H->setOpNameBarPlot( mathOp );
        dataProvider_H->setColorBarPlot( color );
        emit requestToAddNewPlot();
    }
    else
    {
        hideColorList();
        dataProvider_H->setOpNameBarPlot( plotWidget::Maximum );
        dataProvider_H->setColorBarPlot( plotWidget::CALC_COLORS[ ( int )plotWidget::Maximum ] );
        dataProvider_H->setTopStack( true );
        emit requestToAddNewPlot();
        dataProvider_H->setOpNameBarPlot( plotWidget::Average );
        dataProvider_H->setColorBarPlot( plotWidget::CALC_COLORS[ ( int )plotWidget::Average ] );
        dataProvider_H->setTopStack( true );
        emit requestToAddNewPlot();
        dataProvider_H->setOpNameBarPlot( plotWidget::Minimum );
        dataProvider_H->setColorBarPlot( plotWidget::CALC_COLORS[ ( int )plotWidget::Minimum ] );
        dataProvider_H->setTopStack( true );
        emit requestToAddNewPlot();
        dataProvider_H->setTopStack( false );
    }
}
void
Barplot::colorComboHandler()
{
    if ( changeColorAutomatically == false )
    {
        QColor color( getColorList()->currentText() );
        dataProvider_H->setColorBarPlot( color );
        emit requestToChangePlotColor();
    }
    else
    {
        changeColorAutomatically = false;
    }
}
void
Barplot::changeComboboxColor( QColor c )
{
    int index = -1;

    for ( int g = 0; g < colorList->count(); g++ )
    {
        QString s = ( getColorList()->itemText( g ) );
        if ( QColor( s ) == c )
        {
            index = g;
            break;
        }
    }

    if ( index != -1 )
    {
        changeColorAutomatically = true;
        dataProvider_H->setColorBarPlot( c );
        colorList->setCurrentIndex( index );
    }
}
void
Barplot::stackButtonHasClicked()
{
    dataProvider_H->setTopStack( true );
}
void
Barplot::ShowContextMenu( const QPoint& pos )
{
    if ( barsPlotAreaController->getView()->getDataToDisplay() == true )
    {
        contextMenu->popup( barsPlotAreaController->getView()->mapToGlobal( pos ) );
    }
}

void
Barplot::markItemAsLoop( UserAction type )
{
    if ( type == cubegui::LoopMarkAction )
    {
        addTabToGUI();
    }
}

void
Barplot::addTabToGUI()
{
    if ( tabNotPresent )
    {
        service->addTab( SYSTEM, this );
        tabNotPresent = false;
    }
}
