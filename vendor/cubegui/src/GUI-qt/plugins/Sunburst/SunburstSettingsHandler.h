/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2016-2017                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2015-2016                                                **
**  RWTH Aachen University, JARA-HPC                                       **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef SUNBURST_SETTINGS_HANDLER_H
#define SUNBURST_SETTINGS_HANDLER_H

// Cube includes
#include "SettingsHandler.h"

// Qt includes
#include <QColor>

class SunburstSettingsHandler : public cubegui::SettingsHandler
{
public:
    // SettingsHandler implementation
    virtual void
    loadGlobalSettings( QSettings& settings );

    virtual void
    saveGlobalSettings( QSettings& settings );

/*
        virtual void
        loadExperimentSettings( QSettings& settings );

        virtual void
        saveExperimentSettings( QSettings& settings );
 */
    virtual QString
    settingName();

    // public member methods
    QColor
    getFrameLineColor();

    void
    setFrameLineColor( const QColor& value );

    QColor
    getSelectionLineColor();

    void
    setSelectionLineColor( const QColor& value );

    bool
    getMarkZero();

    void
    setMarkZero( bool value );

    bool
    getHideInfo();

    void
    setHideInfo( bool value );

    bool
    getHideSmallArcFrames();

    void
    setHideSmallArcFrames( bool value );

    bool
    getZoomTowardsCursor();

    void
    setZoomTowardsCursor( bool value );

    bool
    getZoomInverted();

    void
    setZoomInverted( bool value );

private:
    // private member attributes
    // the current settings
    QColor frameLineColor;
    QColor selectionLineColor;
    bool   markZero;
    bool   hideInfo;
    bool   hideSmallArcFrames;
    bool   zoomTowardsCursor;
    bool   zoomInverted;

    // Default values.
    static const bool markZeroDefault           = false;
    static const bool hideInfoDefault           = false;
    static const bool hideSmallArcFramesDefault = true;
    static const bool zoomTowardsCursorDefault  = true;
    static const bool zoomInvertedDefault       = false;
};


#endif    /* SUNBURST_SETTINGS_HANDLER_H */
