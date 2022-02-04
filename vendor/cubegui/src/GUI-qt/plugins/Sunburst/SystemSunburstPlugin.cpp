/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2016-2021                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2015-2016                                                **
**  RWTH Aachen University, JARA-HPC                                       **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


/*-------------------------------------------------------------------------*/
/**
 *  @file    SystemSunburstPlugin
 *  @ingroup CubeSunburst
 *  @brief   Definition of the class SystemSunburstPlugin.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include "SystemSunburstPlugin.h"

#include <cassert>

// Qt includes
#include <QVBoxLayout>
#include <QtPlugin>

#include <PluginServices.h>

// introduce Cube namespaces
using namespace cubegui;
using namespace cubepluginapi;

using namespace cube_sunburst;

/*
 * If the plugin doesn't load, the most likely reason is an undefined reference.
 * To check this, call "make check-libexample-plugin" in the build-backend directory.
 */

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2( SystemSunburstPlugin, SystemSunburstPlugin );   // ( PluginName, ClassName )
#endif

bool
SystemSunburstPlugin::cubeOpened( PluginServices* service )
{
    this->service = service;

    if ( !treeIsHomogeneous() )
    {
        return false; // inhomogeneous trees are not yet supported
    }

    widget_ = new QWidget();
    widget_->setAutoFillBackground( true );

    // management of the subwidgets
    QGridLayout* layout = new QGridLayout();
    widget_->setLayout( layout );

    screenWidget = new UIEventWidget();
    layout->addWidget( screenWidget, 0, 0, 0, 0 );

    // add the plugin to cube
    service->addTab( SYSTEM, this );
    service->addSettingsHandler( &settings );

    initialized = false;

    return true;    // initialisation is ok => plugin should be shown
}

QString
SystemSunburstPlugin::getDeactivationMessage()
{
    return "Sunburst doesn't yet support inhomogeneous trees";
}

bool
SystemSunburstPlugin::treeIsHomogeneous()
{
    QList<TreeItem*> items = service->getTopLevelItems( SYSTEM );
    QList<int>       levelCount; // level (TreeItem::getDepth())-> number of children in this level
    int              count;
    int              level;
    while ( !items.isEmpty() )
    {
        TreeItem* current = items.takeFirst();
        level = current->getDepth() - 1; // tree level beginning with 0
        count = current->getChildren().size();
        if ( level >= levelCount.size() )
        {
            levelCount.insert( level, count );
        }
        else if ( count != levelCount[ level ] )
        {
            return false;
        }
        items.append( current->getChildren() );
    }
    return true;
}

void
SystemSunburstPlugin::cubeClosed()
{
    delete screenWidget;
    delete widget_;
}

void
SystemSunburstPlugin::treeItemSelected( cubepluginapi::TreeItem* item )
{
    if ( item->getDisplayType()  == SYSTEM )
    {
        screenWidget->update();
    }
}

QIcon
SystemSunburstPlugin::icon() const
{
    return QIcon( ":/images/sunbursticon.png" );
}


/** set a version number, the plugin with the highest version number will be loaded */
void
SystemSunburstPlugin::version( int& major,
                               int& minor,
                               int& bugfix ) const
{
    major  = 0;
    minor  = 1;
    bugfix = 0;
}


/** unique plugin name */
QString
SystemSunburstPlugin::name() const
{
    return "System Sunburst";
}


QString
SystemSunburstPlugin::getHelpText() const
{
    return tr( "Visualize the system tree as a sunburst chart." );
}


/** widget that will be placed into the tab */
QWidget*
SystemSunburstPlugin::widget()
{
    return widget_;
}


void
SystemSunburstPlugin::valuesChanged()
{
    // ONLY FOR MAC: for some reason the Qt event loop calls this before values got initialized
    if ( !initialized )
    {
        return;
    }
    // adjust SunburstShapeData to the tree
    for ( int i = 0; i < shapeData.getNumberOfLevels(); i++ )
    {
        QList< cubegui::TreeItem* > levelList = detail::getElementsOfLevel( *service->getTopLevelItems( SYSTEM ).first(), i );
        assert( levelList.count() == shapeData.getNumberOfElements( i ) );
        bool levelVisible = false;
        for ( int j = 0; j < shapeData.getNumberOfElements( i ); j++ )
        {
            bool treeItemExpanded = levelList.at( j )->isExpanded();
            // apply changes to expanded
            if ( treeItemExpanded != shapeData.getExpanded( i, j ) )
            {
                shapeData.setExpanded( i, j, treeItemExpanded );
            }
            levelVisible = ( levelVisible || treeItemExpanded );
        }
        if ( !levelVisible )
        {
            break;
        }
    }
    // update the level sizes with regard to the new expanded data (manually called once after many changes for efficiency)
    shapeData.updateLevelSizes();

    screenWidget->update();
}


void
SystemSunburstPlugin::setActive( bool active )
{
    if ( active )
    {
        if ( !initialized )
        {
            /* we initialize the data objects shapeData and TransformationData here
             * because here we can be sure that the tree which is the basis for
             * shapeData was initialized as well as the settings*/
            initialized = true;

            // initialize shapeData
            TreeItem*        item  = service->getTopLevelItems( SYSTEM ).first();
            int              depth = detail::getTreeDepth( *item );
            QVector< qreal > elementsPerLevel( depth );
            for ( int i = 0; i < depth; i++ )
            {
                elementsPerLevel[ i ] = detail::getQuantityOfLevel( *item, i );
            }
            shapeData.reset( depth, elementsPerLevel );
            shapeData.setTopLevelItem( item );
            shapeData.setService( service );
            shapeData.setInnerRadius( 0, 0 );
            screenWidget->useShapeData( &shapeData );

            // initialize transformationData
            transformationData.initialize();

            // no initial zoom
            transformationData.setZoomLevel( 0 );
            screenWidget->useTransformationData( &transformationData );

            // initialize sunburst position
            screenWidget->resetSunburstPosition();

            // Enable mouse tracking for the mouse move interaction.
            screenWidget->setMouseTracking( true );

            // add a submenu to the 'Plugins' menu
            QMenu* menu = service->enablePluginMenu();
            addSunburstMenu( menu );
            screenWidget->setContextMenu( menu );

            // apply settings
            applyGlobalSettings();
            applyExperimentSettings();
        }
        service->connect( service, SIGNAL( treeItemIsSelected( cubepluginapi::TreeItem* ) ),
                          this, SLOT( treeItemSelected( cubepluginapi::TreeItem* ) ) );
        valuesChanged();
    }
    else
    {
        service->disconnect( service, SIGNAL( treeItemIsSelected( cubepluginapi::TreeItem* ) ),
                             this, SLOT( treeItemSelected( cubepluginapi::TreeItem* ) ) );
    }
}


/** tab label */
QString
SystemSunburstPlugin::label() const
{
    return "Sunburst";
}


void
SystemSunburstPlugin::addSunburstMenu( QMenu* sunburstMenu )
{
    sunburstMenu->setStatusTip( tr( "Ready" ) );

    QMenu* frameLineColorMenu = sunburstMenu->addMenu( tr( "Frame coloring" ) );
    frameLineColorMenu->setStatusTip( tr( "Ready" ) );
    frameLineColorMenu->setWhatsThis(
        tr( "Allows one to define the color of the framing lines of sunburst arcs. Available colors are black, gray, white, or no lines." ) );

    blackFrameLinesAct = new QAction( tr( "Black" ), this );
    blackFrameLinesAct->setStatusTip( tr(
                                          "Framing lines of sunburst arcs should be painted black." ) );
    connect( blackFrameLinesAct, SIGNAL( triggered() ), this,
             SLOT( blackFrameLines() ) );
    blackFrameLinesAct->setCheckable( true );
    blackFrameLinesAct->setWhatsThis(
        tr( "Sets the color of the framing lines of sunburst arcs to black." ) );
    frameLineColorMenu->addAction( blackFrameLinesAct );

    grayFrameLinesAct = new QAction( tr( "Gray" ), this );
    grayFrameLinesAct->setStatusTip( tr(
                                         "Framing lines of sunburst arcs should be painted gray." ) );
    connect( grayFrameLinesAct, SIGNAL( triggered() ), this,
             SLOT( grayFrameLines() ) );
    grayFrameLinesAct->setCheckable( true );
    grayFrameLinesAct->setWhatsThis(
        tr( "Sets the color of the framing lines of sunburst arcs to gray." ) );
    frameLineColorMenu->addAction( grayFrameLinesAct );

    whiteFrameLinesAct = new QAction( tr( "White" ), this );
    whiteFrameLinesAct->setStatusTip( tr(
                                          "Framing lines of sunburst arcs be painted white." ) );
    connect( whiteFrameLinesAct, SIGNAL( triggered() ), this,
             SLOT( whiteFrameLines() ) );
    whiteFrameLinesAct->setCheckable( true );
    whiteFrameLinesAct->setWhatsThis(
        tr( "Sets the color of the  framing lines of sunburst arcs to white." ) );
    frameLineColorMenu->addAction( whiteFrameLinesAct );

    noFrameLinesAct = new QAction( tr( "No lines" ), this );
    noFrameLinesAct->setStatusTip( tr(
                                       "Framing lines of sunburst arcs should not be painted." ) );
    connect( noFrameLinesAct, SIGNAL( triggered() ), this, SLOT( noFrameLines() ) );
    noFrameLinesAct->setCheckable( true );
    noFrameLinesAct->setWhatsThis(
        tr( "Defines that there should be no framing lines for the sunburst arcs." ) );
    frameLineColorMenu->addAction( noFrameLinesAct );

    QActionGroup* frameLinesActionGroup = new QActionGroup( this );

    // only one element in this group can be checked
    frameLinesActionGroup->setExclusive( true );
    frameLinesActionGroup->addAction( blackFrameLinesAct );
    frameLinesActionGroup->addAction( grayFrameLinesAct );
    frameLinesActionGroup->addAction( whiteFrameLinesAct );
    frameLinesActionGroup->addAction( noFrameLinesAct );

    QMenu* selectionLineColorMenu =
        sunburstMenu->addMenu( tr( "Selection coloring" ) );
    selectionLineColorMenu->setStatusTip( tr( "Ready" ) );
    selectionLineColorMenu->setWhatsThis(
        tr( "Allows one to define the color of the selection lines of selected sunburst arcs." ) );

    blueSelectionLinesAct = new QAction( tr( "Blue" ), this );
    blueSelectionLinesAct->setStatusTip( tr(
                                             "Selection lines of selected sunburst arcs should be painted blue." ) );
    connect( blueSelectionLinesAct, SIGNAL( triggered() ), this,
             SLOT( blueSelectionLines() ) );
    blueSelectionLinesAct->setCheckable( true );
    blueSelectionLinesAct->setWhatsThis(
        tr( "Sets the color of the selection lines of selected sunburst arcs to blue." ) );
    selectionLineColorMenu->addAction( blueSelectionLinesAct );

    redSelectionLinesAct = new QAction( tr( "Red" ), this );
    redSelectionLinesAct->setStatusTip( tr(
                                            "Selection lines of selected sunburst arcs should be painted red." ) );
    connect( redSelectionLinesAct, SIGNAL( triggered() ), this,
             SLOT( redSelectionLines() ) );
    redSelectionLinesAct->setCheckable( true );
    redSelectionLinesAct->setWhatsThis( tr( "Sets the color of the selection lines of selected sunburst arcs to red." ) );
    selectionLineColorMenu->addAction( redSelectionLinesAct );

    magentaSelectionLinesAct = new QAction( tr( "Magenta" ), this );
    magentaSelectionLinesAct->setStatusTip( tr(
                                                "Selection lines of selected sunburst arcs should be painted magenta." ) );
    connect( magentaSelectionLinesAct, SIGNAL( triggered() ), this,
             SLOT( magentaSelectionLines() ) );
    magentaSelectionLinesAct->setCheckable( true );
    magentaSelectionLinesAct->setWhatsThis( tr( "Sets the color of the selection lines of selected sunburst arcs to magenta." ) );
    selectionLineColorMenu->addAction( magentaSelectionLinesAct );

    noSelectionLinesAct = new QAction( tr( "No lines" ), this );
    noSelectionLinesAct->setStatusTip( tr( "Framing lines of sunburst arcs should not be painted." ) );
    connect( noSelectionLinesAct, SIGNAL( triggered() ), this,
             SLOT( noSelectionLines() ) );
    noSelectionLinesAct->setCheckable( true );
    noSelectionLinesAct->setWhatsThis(
        tr( "Defines that there should be no framing lines for the sunburst arcs." ) );
    selectionLineColorMenu->addAction( noSelectionLinesAct );

    QActionGroup* selectionLinesActionGroup = new QActionGroup( this );

    // only one element in this group can be checked
    selectionLinesActionGroup->setExclusive( true );
    selectionLinesActionGroup->addAction( blueSelectionLinesAct );
    selectionLinesActionGroup->addAction( redSelectionLinesAct );
    selectionLinesActionGroup->addAction( magentaSelectionLinesAct );
    selectionLinesActionGroup->addAction( noSelectionLinesAct );

    sunburstMenu->addSeparator();

    markZeroAct = sunburstMenu->addAction( tr( "Mark 0 degrees" ) );
    markZeroAct->setCheckable( true );
    markZeroAct->setStatusTip( tr( "Marks 0 degrees with a line" ) );
    markZeroAct->setWhatsThis( tr( "If checked, 0 degrees are marked with a line." ) );
    connect( markZeroAct, SIGNAL( toggled( bool ) ), this,
             SLOT( setMarkZero( bool ) ) );

    infoAct = sunburstMenu->addAction( tr( "Hide info tooltip" ) );
    infoAct->setCheckable( true );
    infoAct->setStatusTip( tr( "Do not show info about the item touched by the cursor" ) );
    infoAct->setWhatsThis( tr( "If checked, the info tooltip for the item touched by the cursor is hidden." ) );
    connect( infoAct, SIGNAL( toggled( bool ) ), this,
             SLOT( setHideInfo( bool ) ) );

    smallArcFramesAct = sunburstMenu->addAction( tr( "Hide frames of small arcs" ) );
    smallArcFramesAct->setCheckable( true );
    smallArcFramesAct->setStatusTip( tr( "Hide frames of small arcs" ) );
    smallArcFramesAct->setWhatsThis( tr( "If checked, small arcs are drawn without a frame." ) );
    connect( smallArcFramesAct, SIGNAL( toggled( bool ) ), this,
             SLOT( setHideSmallArcFrames( bool ) ) );

    zoomTowardsCursorAct = sunburstMenu->addAction( tr( "Zoom towards the cursor" ) );
    zoomTowardsCursorAct->setCheckable( true );
    zoomTowardsCursorAct->setStatusTip( tr( "Zoom towards the cursor instead of the sunburst center" ) );
    zoomTowardsCursorAct->setWhatsThis( tr( "If checked, a zoom has the cursor as a fix point instead of the sunburst center." ) );
    connect( zoomTowardsCursorAct, SIGNAL( toggled( bool ) ), this,
             SLOT( setZoomTowardsCursor( bool ) ) );

    zoomInvertedAct = sunburstMenu->addAction( tr( "Invert zoom" ) );
    zoomInvertedAct->setCheckable( true );
    zoomInvertedAct->setStatusTip( tr( "Invert the zoom direction" ) );
    zoomInvertedAct->setWhatsThis( tr( "If checked, the zoom direction is inverted." ) );
    connect( zoomInvertedAct, SIGNAL( toggled( bool ) ), this,
             SLOT( setZoomInverted( bool ) ) );

    sunburstMenu->addSeparator();

    QMenu* resetMenu = sunburstMenu->addMenu( tr( "Reset..." ) );
    resetMenu->setStatusTip( tr( "Ready" ) );
    resetMenu->setWhatsThis( tr( "Allows one to reset specific or all user interactions." ) );

    resetRotationAct = new QAction( tr( "Rotations" ), this );
    resetRotationAct->setStatusTip( tr( "Resets the rotation user-interaction" ) );
    resetRotationAct->setWhatsThis( tr( "Resets the rotation to have the first item back at 0 degrees." ) );
    connect( resetRotationAct, SIGNAL( triggered() ), this,
             SLOT( resetDegreeOffset() ) );
    resetMenu->addAction( resetRotationAct );

    resetSizesAct = new QAction( tr( "Arc sizes" ), this );
    resetSizesAct->setStatusTip( tr( "Resets the resize user interaction" ) );
    resetSizesAct->setWhatsThis( tr( "Resets the arc sizes to have them evenly distributed per ring." ) );
    connect( resetSizesAct, SIGNAL( triggered() ), this,
             SLOT( resetArcSizes() ) );
    resetMenu->addAction( resetSizesAct );

    resetZoomAct = new QAction( tr( "Zoom" ), this );
    resetZoomAct->setStatusTip( tr( "Resets the zoom user interaction" ) );
    resetZoomAct->setWhatsThis( tr( "Resets the zoom user interaction." ) );
    connect( resetZoomAct, SIGNAL( triggered() ), this,
             SLOT( resetZoom() ) );
    resetMenu->addAction( resetZoomAct );

    resetSunburstPositionAct = new QAction( tr( "Sunburst Position" ), this );
    resetSunburstPositionAct->setStatusTip( tr( "Resets the shift user interaction" ) );
    resetSunburstPositionAct->setWhatsThis( tr( "Resets the shift user interaction." ) );
    connect( resetSunburstPositionAct, SIGNAL( triggered() ), this,
             SLOT( resetSunburstPosition() ) );
    resetMenu->addAction( resetSunburstPositionAct );

    resetAllAct = new QAction( tr( "All interactions" ), this );
    resetAllAct->setStatusTip( tr( "Resets all user interactions" ) );
    resetAllAct->setWhatsThis( tr( "Resets all user interactions." ) );
    connect( resetAllAct, SIGNAL( triggered() ), this,
             SLOT( resetAll() ) );
    resetMenu->addAction( resetAllAct );
}


void
SystemSunburstPlugin::applyGlobalSettings()
{
    infoAct->setChecked( settings.getHideInfo() );
    setHideInfo( settings.getHideInfo() );

    markZeroAct->setChecked( settings.getMarkZero() );
    setMarkZero( settings.getMarkZero() );

    smallArcFramesAct->setChecked( settings.getHideSmallArcFrames() );
    setHideSmallArcFrames( settings.getHideSmallArcFrames() );

    zoomTowardsCursorAct->setChecked( settings.getZoomTowardsCursor() );
    setZoomTowardsCursor( settings.getZoomTowardsCursor() );

    zoomInvertedAct->setChecked( settings.getZoomInverted() );
    setZoomInverted( settings.getZoomInverted() );

    QColor color = settings.getFrameLineColor();
    transformationData.setFrameLineColor( color );
    if ( color == Qt::black )
    {
        blackFrameLinesAct->setChecked( true );
    }
    else if ( color == Qt::gray )
    {
        grayFrameLinesAct->setChecked( true );
    }
    else if ( color.alpha() == 0 )
    {
        noFrameLinesAct->setChecked( true );
    }
    else
    {
        whiteFrameLinesAct->setChecked( true );
    }

    color = settings.getSelectionLineColor();
    transformationData.setSelectionLineColor( color );
    if ( color == Qt::blue )
    {
        blueSelectionLinesAct->setChecked( true );
    }
    else if ( color == Qt::red )
    {
        redSelectionLinesAct->setChecked( true );
    }
    else if ( color == Qt::magenta )
    {
        magentaSelectionLinesAct->setChecked( true );
    }
    else
    {
        noSelectionLinesAct->setChecked( true );
    }
}


void
SystemSunburstPlugin::applyExperimentSettings()
{
    return;
}


void
SystemSunburstPlugin::blackFrameLines()
{
    settings.setFrameLineColor( Qt::black );
    transformationData.setFrameLineColor( Qt::black );
    screenWidget->update();
}


void
SystemSunburstPlugin::grayFrameLines()
{
    settings.setFrameLineColor( Qt::gray );
    transformationData.setFrameLineColor( Qt::gray );
    screenWidget->update();
}


void
SystemSunburstPlugin::whiteFrameLines()
{
    settings.setFrameLineColor( Qt::white );
    transformationData.setFrameLineColor( Qt::white );
    screenWidget->update();
}


void
SystemSunburstPlugin::noFrameLines()
{
    QColor color = Qt::white;
    color.setAlpha( 0 );
    settings.setFrameLineColor( color );
    transformationData.setFrameLineColor( color );
    screenWidget->update();
}


void
SystemSunburstPlugin::blueSelectionLines()
{
    settings.setSelectionLineColor( Qt::blue );
    transformationData.setSelectionLineColor( Qt::blue );
    screenWidget->update();
}


void
SystemSunburstPlugin::redSelectionLines()
{
    settings.setSelectionLineColor( Qt::red );
    transformationData.setSelectionLineColor( Qt::red );
    screenWidget->update();
}


void
SystemSunburstPlugin::magentaSelectionLines()
{
    settings.setSelectionLineColor( Qt::magenta );
    transformationData.setSelectionLineColor( Qt::magenta );
    screenWidget->update();
}


void
SystemSunburstPlugin::noSelectionLines()
{
    QColor color = Qt::white;
    color.setAlpha( 0 );
    settings.setSelectionLineColor( color );
    transformationData.setSelectionLineColor( color );
    screenWidget->update();
}


void
SystemSunburstPlugin::setMarkZero( bool value )
{
    settings.setMarkZero( value );
    screenWidget->setMarkZero( value );
    screenWidget->update();
}


void
SystemSunburstPlugin::setHideInfo( bool value )
{
    settings.setHideInfo( value );
    screenWidget->setShowToolTip( !value );
    screenWidget->update();
}


void
SystemSunburstPlugin::setHideSmallArcFrames( bool value )
{
    settings.setHideSmallArcFrames( value );
    if ( value )
    {
        transformationData.setFrameSizeThreshold( 5 );
    }
    else
    {
        transformationData.setFrameSizeThreshold( 0 );
    }
    screenWidget->update();
}


void
SystemSunburstPlugin::setZoomTowardsCursor( bool value )
{
    settings.setZoomTowardsCursor( value );
    screenWidget->setZoomTowardsCursor( value );
}


void
SystemSunburstPlugin::setZoomInverted( bool value )
{
    settings.setZoomInverted( value );
    screenWidget->setZoomInverted( value );
}


void
SystemSunburstPlugin::resetDegreeOffset()
{
    screenWidget->resetDegreeOffset();
    screenWidget->update();
}


void
SystemSunburstPlugin::resetArcSizes()
{
    screenWidget->resetArcSizes();
    screenWidget->update();
}


void
SystemSunburstPlugin::resetZoom()
{
    screenWidget->resetZoom();
    screenWidget->update();
}


void
SystemSunburstPlugin::resetSunburstPosition()
{
    screenWidget->resetSunburstPosition();
    screenWidget->update();
}


void
SystemSunburstPlugin::resetAll()
{
    screenWidget->resetAll();
    screenWidget->update();
}
