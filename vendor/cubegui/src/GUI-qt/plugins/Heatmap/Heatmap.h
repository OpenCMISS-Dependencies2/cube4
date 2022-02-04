/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2018                                                **
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


#ifndef HEATMAP_H
#define HEATMAP_H

#include <QtGui>
#include <QMenu>
#include <QToolTip>

#include "CubePlugin.h"
#include "TabInterface.h"
#include "PluginServices.h"

#include "HeatmapPlotArea.h"
#include "CubeRead.h"
#include "DataProvider.h"
#include "HeatmapController.h"
#include "HorizontalRulerController.h"
#include "VerticalRulerController.h"
#include "HeatmapSettingsWidget.h"
#include "ImageSaverController.h"

class Heatmap : public QObject, public cubepluginapi::CubePlugin, cubepluginapi::TabInterface, cubepluginapi::SettingsHandler
{
    Q_OBJECT
    Q_INTERFACES( cubepluginapi::CubePlugin )
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA( IID "SystemHeatmapPlugin" )
#endif


public:
    Heatmap();

    // CubePlugin implementation
    virtual bool
    cubeOpened( cubepluginapi::PluginServices* service );
    virtual void
    cubeClosed();
    virtual QString
    name() const;
    virtual void
    version( int& major,
             int& minor,
             int& bugfix ) const;
    virtual QString
    getHelpText() const;

    virtual QString
    getHelpURL() const
    {
        return "SystemHeatmapPlugin.html";
    }

    // TabInterface implementation
    virtual QString
    label() const;
    virtual QIcon
    icon() const;
    virtual QWidget*
    widget();
    virtual void
    valuesChanged();
    virtual void
    setActive( bool active );
    virtual QSize
    sizeHint() const;

    // SettingsHandler implementation
    virtual void
    loadGlobalSettings( QSettings& settings );
    virtual void
    saveGlobalSettings( QSettings& settings );
    virtual QString
    settingName();

    // specified fo heatmap plugin
    void
    setWidgetNorth( QWidget* WidgetNorth );
    void
    setWidgetWest( QWidget* WidgetWest );
    void
    setWidgetCenter( QWidget* WidgetCenter );
    void
    addTabToGUI();

private slots:
    void
    orderHasChanged( const QList<cubepluginapi::DisplayType>& order );
    void
    heatmapAreaMouseCoordEventHandler( QMouseEvent* mouseEvent );
    void
    ShowContextMenu( const QPoint& pos );    // for save image
    void
    markItemAsLoop( cubepluginapi::UserAction type );

private:
    void
    initialize();

    QWidget*                       widget_;
    cubepluginapi::PluginServices* service;

    QGridLayout*                HeatmapWidgetLayout;
    CubeRead*                   cubeReader;
    DataProvider*               dataProvider_H;
    HorizontalRulerController*  heatMapHorizontalRulerController;
    VerticalRulerController*    heatMapVerticalRulerController;
    HeatMapPlotAreaController*  heatMapPlotAreaController;
    HeatmapCustomizationzation* heatMapCustomizationDialog;
    ImageSaverController*       heatMapSaveController;
    QMenu*                      contextMenu;
    QAction*                    saveImageContextMenu;
    QAction*                    menuAction;
    bool                        tabNotPresent;
};

#endif // HEATMAP_H
