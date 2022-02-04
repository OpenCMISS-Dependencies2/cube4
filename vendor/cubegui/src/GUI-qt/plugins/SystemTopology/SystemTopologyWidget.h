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


// Controller

#ifndef _SYSTEMTOPOLOGYWIDGET_H
#define _SYSTEMTOPOLOGYWIDGET_H

#include <QScrollArea>
#include <QSplitter>
#include <vector>

#include "SystemTopologyData.h"
#include "SystemTopologyView.h"
#include "SystemTopologyViewTransform.h"
#include "TopologyDimensionBar.h"
#include "ScrollArea.h"

class QSettings;
class SystemTopology;
class SystemTopologyToolBar;

namespace cubegui
{
class ScrollArea;
class CubeProxy;
}

class SystemTopologyWidget : public QSplitter, public cubepluginapi::TabInterface
{
    Q_OBJECT

signals:
    void
    xAngleChanged( int angle );
    void
    yAngleChanged( int angle );

private slots:
    void
    vscroll( int val );
    void
    hscroll( int val );
    void
    handleSelection( cubepluginapi::TreeItem* item );

public slots:
    void
    selectItem( cubepluginapi::TreeItem* item,
                bool                     add );
    void
    showDimensionSelectionBar( bool show );

public:
    SystemTopologyWidget( SystemTopology* sys,
                          unsigned        topologyId );
    ~SystemTopologyWidget();

    void
    initialize();

    QWidget*
    widget();

    //re-compute the colors
    void
    updateColors();

    //remove all data
    void
    cleanUp();

    //rescale the topology sizes such that the topology fits into the scroll widget's window
    void
    rescale();

    //save and load topology settings
    void
    saveExperimentSettings( QSettings& settings );
    bool
    loadExperimentSettings( QSettings& settings );

    //update the value widget
    void
    updateValueWidget();

    //set the color for the lines framing processes/threads in the topology
    void
    setLineType( cubepluginapi::LineType lineType );

    //toggle the flag if unused planes should be painted or not
    void
    toggleUnused();

    SystemTopologyViewTransform*
    getTransform()
    {
        return transform;
    }
    SystemTopologyData*
    getData()
    {
        return data;
    }
    // TabInterface interface
    QIcon
    icon() const;

    // TabInterface implementation
    QString
    label() const;

    //update the colors and repaint
    void
    valuesChanged();
    void
    setActive( bool active );

    SystemTopologyToolBar*
    getTopologyToolBar() const;

private slots:
    void
    setSize( const QSize& size );

private:
    typedef enum { SELECT, FOLD } Mode;
    Mode selectMode;                         // if > 2 dimensions: call multiDimFold or multiDimSelect

    cube::CubeProxy*             cube;
    SystemTopologyData*          data;
    SystemTopologyView*          view;
    SystemTopologyViewTransform* transform;

    // widgets created in this class
    TopologyDimensionBar* dimensionBar;
    cubegui::ScrollArea*  scroll;

    // information from caller
    cubepluginapi::PluginServices* service;
    SystemTopologyToolBar*         topologyToolBar;
    SystemTopology*                sys;

    unsigned topologyId;
    QString  topologyName;

    bool firstVisible;

    TopologyDimensionBar*
    createDimensionSelectionBar( cube::CubeProxy*    cube,
                                 SystemTopologyData* data );
};

#endif
