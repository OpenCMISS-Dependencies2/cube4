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
 *  @file
 *  @ingroup CubeSunburst
 *  @brief   Declaration of the class SystemSunburst.
 **/
/*-------------------------------------------------------------------------*/


#ifndef CUBE_SYSTEM_SUNBURST_PLUGIN_H
#define CUBE_SYSTEM_SUNBURST_PLUGIN_H

// Cube includes
#include "CubePlugin.h"
#include "PluginServices.h"
#include "TabInterface.h"

// Sunburst includes
#include "DataAccessFunctions.h"
#include "SunburstSettingsHandler.h"
#include "SunburstShapeData.h"
#include "TransformationData.h"
#include "UIEventWidget.h"


// Qt forward declarations
class QAction;
class QWidget;

namespace cube_sunburst
{
class SystemSunburstPlugin
    : public QObject,
      public cubepluginapi::CubePlugin,
      cubepluginapi::TabInterface
{
    Q_OBJECT
    Q_INTERFACES( cubepluginapi::CubePlugin )

    #if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA( IID "SystemSunburstPlugin" )   // unique PluginName
    #endif


public:
    // CubePlugin implementation
    virtual bool
    cubeOpened( cubepluginapi::PluginServices* service );
    virtual void
    cubeClosed();
    virtual QString
    name() const;

    QIcon
    icon() const;

    virtual void
    version( int& major,
             int& minor,
             int& bugfix ) const;
    virtual QString
    getHelpText() const;

    virtual QString
    getHelpURL() const
    {
        return "SystemSunburstPlugin.html";
    }

    // TabInterface implementation
    virtual QString
    label() const;
    virtual QWidget*
    widget();

    // public member methods
    /// Called whenever the sunburst has to be updated.
    void
    valuesChanged();

    /// Called when the tab is set active or inactive.
    void
    setActive( bool active );

    QString
    getDeactivationMessage();

private:
    // private member methods
    /// configurations for the submenu of the 'Plugins' menu in CUBE
    void
    addSunburstMenu( QMenu* );

    /// called after the SettingHandler (SessionSettings settings) received data from the pluginServices object
    void
    applyGlobalSettings();

    /// called after the SettingHandler (SessionSettings settings) received data from the pluginServices object
    void
    applyExperimentSettings();

    /** returns true, if the number of children on each tree level is equal  */
    bool
    treeIsHomogeneous();

    // private member attributes
    cubepluginapi::PluginServices* service;
    QWidget*                       widget_;

    /// SessionSettings object of the plugin.
    SunburstSettingsHandler settings;

    /// Flag which is only true until the screenWidget got initialized.
    bool initialized;

    /// The widget which manages the user interaction and is used as the screen by SunburstPainter.
    /** has to be a pointer since it is declared as sub widget of widget_ which has to be able to delete its children */
    UIEventWidget* screenWidget;

    /// The shape data of the sunburst.
    SunburstShapeData shapeData;

    /// Information of the appearence of the sunburst.
    TransformationData transformationData;

    /// actions of the submenu of the 'Plugins' menu in CUBE
    QAction* blackFrameLinesAct, * grayFrameLinesAct, * whiteFrameLinesAct,
           * noFrameLinesAct,
           * blueSelectionLinesAct, * redSelectionLinesAct,
           * magentaSelectionLinesAct, * noSelectionLinesAct,
           * markZeroAct, * infoAct, * smallArcFramesAct, * zoomTowardsCursorAct, * zoomInvertedAct,
           * resetRotationAct, * resetSizesAct, * resetZoomAct, * resetSunburstPositionAct, * resetAllAct;

private slots:
    // set frame line coloring of the sunburst arcs
    void
    blackFrameLines();
    void
    grayFrameLines();
    void
    whiteFrameLines();
    void
    noFrameLines();

    // set selection line coloring of the sunburst arcs
    void
    blueSelectionLines();
    void
    redSelectionLines();
    void
    magentaSelectionLines();
    void
    noSelectionLines();

    /// set if an orientation line at 0 degrees should be drawn
    void
    setMarkZero( bool value );

    /// set if info should be shown when hovering over an element with the cursor
    void
    setHideInfo( bool value );

    /// set if frames of small arcs should be drawn
    void
    setHideSmallArcFrames( bool value );

    /// set if the zoom should have the cursor as a fix point instead of the sunburst center
    void
    setZoomTowardsCursor( bool value );

    /// set if the zoom should direction should be inverted
    void
    setZoomInverted( bool value );

    /// Sets degree offset variables to 0.
    void
    resetDegreeOffset();

    /// Resets any changes to arc sizes.
    void
    resetArcSizes();

    /// Resets zoom.
    void
    resetZoom();

    /// Resets position of the sunburst.
    void
    resetSunburstPosition();

    /// Resets all user interactions.
    void
    resetAll();

    // TabInterface interface
    void
    treeItemSelected( cubepluginapi::TreeItem* item );
};
}      // namespace cube_sunburst

#endif // CUBE_SYSTEM_SUNBURST_PLUGIN_H
