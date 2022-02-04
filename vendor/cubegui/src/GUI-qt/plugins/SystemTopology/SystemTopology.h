/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2020                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef SYSTEMTOPOLOGY_H
#define SYSTEMTOPOLOGY_H

#include <QList>
#include "CubePlugin.h"
#include "PluginServices.h"

class SystemTopologyWidget;

/**
 * @brief The SystemTopology class organizes all topology tabs
 */
class SystemTopology : public QObject, public cubepluginapi::CubePlugin, public cubepluginapi::SettingsHandler
{
    Q_OBJECT
    Q_INTERFACES( cubepluginapi::CubePlugin )

    #if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA( IID "SystemTopologyPlugin" )
    #endif

public:
    virtual void
    version( int& major,
             int& minor,
             int& bugfix ) const;

    virtual QString
    name() const;

    virtual QString
    getHelpText() const;

    virtual QString
    getHelpURL() const
    {
        return "SystemTopologyPlugin.html";
    }

    virtual bool
    cubeOpened( cubepluginapi::PluginServices* service );

    virtual void
    cubeClosed();

    cubepluginapi::PluginServices*
    getService();

    void
    addTopologyMenu( QMenu* topologyMenu );

    // SettingsHandler interface
    void
    loadGlobalSettings( QSettings& );
    void
    saveGlobalSettings( QSettings& );
    void
    loadExperimentSettings( QSettings& );
    void
    saveExperimentSettings( QSettings& );
    QString
    settingName();

    // return if unused topology planes should be displayed
    bool
    getShowUnusedTopologyPlanes();

    // return if lines in topology display should be antialiased
    bool
    getAntialiasing();

    // return the line style for topologies
    // (black, gray, white or no lines, see constants.h for the LineType type)
    cubepluginapi::LineType
    getLineType();

    // tell how zero values in topologies are colored
    // (white or minimal color)
    bool
    getWhiteForZero();

    // set how zero values in topologies should be colored
    // (white or minimal color)
    void
    setWhiteForZero( bool whiteForZero );

private slots:
    void
    enableFocus( bool );

    void
    setIconToolBar();
    void
    setTextToolBar();
    void
    hideToolBar();

    // defines if zero values in topologies should be represented by the
    // color white or by the minimal color;
    // this slot is connected to the actions in the menu
    // Display/Coloring/Topology coloring
    void
    whiteOff();
    void
    whiteOn();

    // set line coloring in topology widgets;
    // connected to the actions in the menu Display/Coloring/Topology line coloring
    void
    blackLines();
    void
    grayLines();
    void
    whiteLines();
    void
    noLines();

    // sets line rendering in topology;
    // connected to the menu Display/Topology/Topology antialiasing
    void
    toggleAntialiasing();

    // slot for handling requests if unused planes in topologies should be painted;
    // connected to menu Display/Topology/Show also unused...
    void
    toggleUnused();

    void
    setAntialiasing( bool value );
    void
    setLineType( cubepluginapi::LineType lineType );

    void
    showTopologyToolbar( bool showAlways );

private:
    cubepluginapi::PluginServices* service;
    QList<SystemTopologyWidget*>   widgetList;

    QAction* white1Act, * white2Act;
    QAction* blackLinesAct, * grayLinesAct, * whiteLinesAct, * noLinesAct;
    QAction* unusedAct, * antialiasingAct, * focusPlaneAct, * showTop;
    // actions modifying the tool bar outlook
    QAction* textAct;
    QAction* iconAct;
    QAction* hideToolbarAct;

    // line type and line antialiasing for line drawing in topologies
    cubepluginapi::LineType lineType;
    bool                    antialiasing;
    // should unsused planes be displayed in topologies?
    bool showUnusedTopologyPlanes;
    // in topologies: should zero values be colored by white or by the minimal color?
    bool whiteForZero;

    int  tabInfoCount;
    int  tabInfoVisible;
    bool toolbarIsHidden;
    void
    setToolBarStyle( Qt::ToolButtonStyle style );
    void
    saveMenuSettings( QSettings& settings );
    void
    loadMenuSettings( QSettings& settings );
};

#endif // SYSTEMTOPOLOGY_H
