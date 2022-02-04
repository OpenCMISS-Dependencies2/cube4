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

#include "SystemTopologyWidget.h"
#include "SystemTopologyToolBar.h"
#include "ScrollArea.h"

#include "CubeProxy.h"
#include "CubeCartesian.h"

#include <QScrollBar>
#include <QSettings>
#include <QDebug>
#include <assert.h>

#include "SystemTopology.h"
#include "SystemTopologyView.h"
#include "SystemTopologyDrawing.h"
// #include "SystemTopologyGL.h"
#include "SystemTopologyToolBar.h"
#include "ScrollArea.h"

using namespace std;
using namespace cube;
using namespace cubepluginapi;

/**
   controls several widgets:
   TopologyDimensionBar which allows to reorder the dimensions
   SystemTopologyView which draws the topology
 */

/*************** constructor / destructor *************************/

SystemTopologyWidget::SystemTopologyWidget( SystemTopology* sys,
                                            unsigned        topologyId ) : QSplitter( Qt::Vertical )

{
    this->sys        = sys;
    this->topologyId = topologyId;
    service          = sys->getService();
    firstVisible     = true;
    selectMode       = FOLD;
    cube             = service->getCube();

    // initialize topologyName, which is used as tab label
    topologyName = cube->getCartesian( topologyId ).get_name().c_str();
    if ( topologyName == "" )
    {
        topologyName.append( tr( "Topology " ) );
        topologyName.append( QString::number( topologyId ) );
    }

    data            = new SystemTopologyData( sys, topologyId );
    topologyToolBar = new SystemTopologyToolBar( service );

    transform = 0;
}

void
SystemTopologyWidget::initialize()
{
    service->addToolBar( topologyToolBar, this );

    dimensionBar = createDimensionSelectionBar( cube, data );
    data->setFoldingDimensions( dimensionBar->getFoldingVector() );
    data->initialize( cube );

    transform = new SystemTopologyViewTransform( data );
    view      = new SystemTopologyDrawing( data, transform, topologyToolBar );
    // view = new SystemTopologyGL(data);

    topologyToolBar->setTopologyWidget( this );

    // put topology drawing inside a scroll area
    scroll = new cubegui::ScrollArea( this );
    scroll->setWidget( view );
    connect( scroll, SIGNAL( resized( QSize ) ), this, SLOT( setSize( QSize ) ) );
    QScrollBar* sbh = scroll->horizontalScrollBar();
    QScrollBar* sbv = scroll->verticalScrollBar();
    connect( sbh, SIGNAL( valueChanged( int ) ), this, SLOT( hscroll( int ) ) );
    connect( sbv, SIGNAL( valueChanged( int ) ), this, SLOT( vscroll( int ) ) );
    this->addWidget( scroll );
    this->addWidget( dimensionBar );
    showDimensionSelectionBar( false ); // hide bar, if preferable

    connect( view, SIGNAL( selectItem( cubepluginapi::TreeItem*, bool ) ), this, SLOT( selectItem( cubepluginapi::TreeItem*, bool ) ) );
    connect( view, SIGNAL( scrollTo( int, int ) ), scroll, SLOT( scrollTo( int, int ) ) );

    connect( data, SIGNAL( dataChanged() ), view, SLOT( updateDrawing() ) );
    connect( data, SIGNAL( viewChanged() ), view, SLOT( updateDrawing() ) );
    connect( data, SIGNAL( rescaleRequest() ), transform, SLOT( rescale() ) );

    connect( transform, SIGNAL( rescaleRequest() ), view, SLOT( rescaleDrawing() ) );
    connect( transform, SIGNAL( viewChanged() ), view, SLOT( updateDrawing() ) );
    connect( transform, SIGNAL( zoomChanged( double ) ), view, SLOT( changeZoom( double ) ) );
    connect( transform, SIGNAL( xAngleChanged( int ) ), view, SLOT( setXAngle( int ) ) );
    connect( transform, SIGNAL( yAngleChanged( int ) ), view, SLOT( setYAngle( int ) ) );
    connect( transform, SIGNAL( positionChanged( int, int ) ), view, SLOT( move( int, int ) ) );

    connect( service, SIGNAL( treeItemIsSelected( cubepluginapi::TreeItem* ) ), this, SLOT( handleSelection( cubepluginapi::TreeItem* ) ) );
}

void
SystemTopologyWidget::showDimensionSelectionBar( bool show )
{
    QList<int> sizeList;
    long       ndims = cube->getCartesian( topologyId ).get_ndims();
    if ( !show && ( ndims <= 3 ) )
    {
        sizeList << 1 << 0; // minimize dimension selection bar
    }
    else
    {
        sizeList << 1 << 1; // show dimension selection bar
    }
    this->setSizes( sizeList );
}

SystemTopologyWidget::~SystemTopologyWidget()
{
    delete data;
    delete view;
    delete transform;
    delete scroll;
}

/**
 * @brief returns the widget that will be placed in SystemTabWidget
 */
QWidget*
SystemTopologyWidget::widget()
{
    return this;
}

void
SystemTopologyWidget::vscroll( int val )
{
    assert( view != 0 );
    view->scrolledTo( scroll->horizontalScrollBar()->value(), val );
}

void
SystemTopologyWidget::hscroll( int val )
{
    assert( view != 0 );
    view->scrolledTo( val, scroll->verticalScrollBar()->value() );
}

QIcon
SystemTopologyWidget::icon() const
{
    return QIcon( ":images/topology-icon.png" );
}

QString
SystemTopologyWidget::label() const
{
    return topologyName;
}

/**************************** miscellaneous **********************/


/** create toolbar to select dimensions to display
 */
TopologyDimensionBar*
SystemTopologyWidget::createDimensionSelectionBar( cube::CubeProxy* cube, SystemTopologyData* data )
{
    const std::vector<long>&        dims  = cube->getCartesian( topologyId ).get_dimv();
    const std::vector<std::string>& names = cube->getCartesian( topologyId ).get_namedims();

    TopologyDimensionBar* dimBar = new TopologyDimensionBar( dims, names );

    connect( dimBar, SIGNAL( selectedDimensionsChanged( std::vector<long>) ), data, SLOT( selectedDimensionsChanged( std::vector<long>) ) );
    connect( dimBar, SIGNAL( foldingDimensionsChanged( std::vector<std::vector<int> >) ), data, SLOT( foldingDimensionsChanged( std::vector<std::vector<int> >) ) );
    connect( dimBar, SIGNAL( splitLengthChanged( int ) ), data, SLOT( splitLengthChanged( int ) ) );
    connect( dimBar, SIGNAL( toggleAxis() ), data, SLOT( toggleAxis() ) );

    return dimBar;
}

SystemTopologyToolBar*
SystemTopologyWidget::getTopologyToolBar() const
{
    return topologyToolBar;
}

void
SystemTopologyWidget::handleSelection( TreeItem* item )
{
    if ( item->getDisplayType() == SYSTEM )
    {
        topologyToolBar->setRecentSelection( item );
        view->update();
        updateValueWidget();
    }
}

/**
   marks the given item as selected in System tree
   @param add if true, adds the item to the previously selected items, otherwise deselects previous selections
 */
void
SystemTopologyWidget::selectItem( cubepluginapi::TreeItem* item, bool add )
{
    service->selectItem( item, add );

    data->updateSelection();
    updateValueWidget();
}


// -----------------------------------------------------------------------

/* Updates the value widget below the tab widget. Only uses the leaf values of a system tree. */
void
SystemTopologyWidget::updateValueWidget()
{
    // in case the topology is empty , just clear the value widgt
    if ( data->getDim( 0 ) == 0 || data->getDim( 1 ) == 0 || data->getDim( 2 ) == 0 )
    {
        service->clearValueWidget();
    }
    else
    {
        double _sigma    = 0.0;
        double _variance = 0.0;
        double _mean     = 0.0;

        double tmp = 0.;

        unsigned N = 0;
        foreach( TreeItem * item, service->getTreeItems( SYSTEM ) )
        {
            if ( item->isLeaf() )
            {
                tmp    = item->getAbsoluteValue();
                _mean += tmp;
                N++;
            }
        }

        _mean = ( N != 0 ) ? _mean / N : 0.;

        foreach( TreeItem * item, service->getTreeItems( SYSTEM ) )
        {
            if ( item->isLeaf() )
            {
                tmp        = item->getAbsoluteValue();
                _variance += ( _mean - tmp ) * ( _mean - tmp );
            }
        }
        _sigma = ( N != 0 ) ? sqrt( _variance / N ) : 0.;

        bool   nothingSelected = true;
        double value           = 0.0;
        double absoluteValue   = 0.0;
        foreach( TreeItem * item, service->getSelections( SYSTEM ) )
        {
            if ( item->isLeaf() )
            {
                nothingSelected = false;
                absoluteValue  +=  item->getAbsoluteValue();
                if ( service->getValueModus() != ABSOLUTE_VALUES )
                {
                    value += item->getValue();
                }
            }
        }
        if ( nothingSelected )
        {
            value = nan( "" );
        }

        bool   userDefinedMinMaxValues;
        double userMinValue, userMaxValue;
        userDefinedMinMaxValues = service->getUserDefinedMinValues( SYSTEM, userMinValue, userMaxValue );

        // compute value for minimal color
        double min;
        if ( userDefinedMinMaxValues )
        {
            min = userMinValue;
        }
        else if ( service->getValueModus() == ABSOLUTE_VALUES )
        {
            min = data->getMinValue();
        }
        else
        {
            min = 0.0;
        }

        // compute value for minimal color
        double max;
        if ( userDefinedMinMaxValues )
        {
            max = userMaxValue;
        }
        else if ( service->getValueModus() == ABSOLUTE_VALUES )
        {
            max = data->getMaxValue();
        }
        else
        {
            max = 100.0;
        }

        if ( nothingSelected )
        {
            value         = nan( "" );
            absoluteValue = nan( "" );
        }
        if ( service->getValueModus() == ABSOLUTE_VALUES )
        {
            // in the absolute value mode display the absolute value (with
            // the min/max values)
            service->updateValueWidget( this, min, max, absoluteValue );
        }
        else
        {
            // in value modes other that the absolute value mode display the
            // current minvalue/value/maxvalue plus absolute
            // minvalue/value/maxvalue
            service->updateValueWidget( this, 0.0, 100.0, value, data->getMinAbsValue(), data->getMaxAbsValue(), absoluteValue, _mean, _sigma );
        }
    }
}


/****************** slots for menuitems / TabWidget ***************************/

void
SystemTopologyWidget::rescale()
{
    transform->rescale( false );
}

// set the line type for topologies: black, gray, white, or no lines
void
SystemTopologyWidget::setLineType( LineType lineType )
{
    data->setLineType( lineType );
}

// toggle the state if unused planes should be displayed or not
void
SystemTopologyWidget::toggleUnused()
{
    data->toggleUnused();
}

void
SystemTopologyWidget::valuesChanged()
{
    assert( view != 0 );
    data->updateColors();
    view->updateDrawing();
    updateValueWidget();
}

void
SystemTopologyWidget::updateColors()
{
    assert( view != 0 );
    data->updateColors();
    view->updateDrawing();
    updateValueWidget();
}

/************************ settings ***************************/

// save topology settings
//
void
SystemTopologyWidget::saveExperimentSettings( QSettings& settings )
{
    QList<int>      sizes = this->sizes(); // splitter sizes
    QList<QVariant> qsizes;
    foreach( int size, sizes )
    {
        qsizes.append( size );
    }
    settings.setValue( "splitter", qsizes );

    transform->saveSettings( settings, topologyId );
    dimensionBar->saveSettings( settings, topologyId );
}

bool
SystemTopologyWidget::loadExperimentSettings( QSettings& settings )
{
    QList<int>      sizes; // splitter sizes
    QList<QVariant> qsizes;
    qsizes = settings.value( "splitter" ).toList();
    if ( qsizes.size() == 2 )
    {
        foreach( QVariant qsize, qsizes )
        {
            sizes.append( qsize.toInt() );
        }
        this->setSizes( sizes );
    }

    dimensionBar->loadSettings( settings, topologyId );
    transform->loadSettings( settings, topologyId );
    return true;
}

void
SystemTopologyWidget::setActive( bool active )
{
    if ( active )
    {
        valuesChanged();

        // if tab becomes visible for the 1st time, rescale the drawing to fit into the widget
        if ( firstVisible )
        {
            transform->rescale( true );
            firstVisible = false;
        }

        connect(  service, SIGNAL( treeItemIsSelected( cubepluginapi::TreeItem* ) ), this, SLOT( handleSelection( cubepluginapi::TreeItem* ) ) );
    }
    else
    {
        disconnect(  service, SIGNAL( treeItemIsSelected( cubepluginapi::TreeItem* ) ), this, SLOT( handleSelection( cubepluginapi::TreeItem* ) ) );
    }
}

void
SystemTopologyWidget::setSize( const QSize& size )
{
    view->setSize( size );
}
