/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2016-2020                                                **
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
 *  @file    SunburstSettingsHandler.cpp
 *  @ingroup CubeSunburst
 *  @brief   Definition of the class SunburstSettingsHandler.
 **/
/*-------------------------------------------------------------------------*/


#include "SunburstSettingsHandler.h"


void
SunburstSettingsHandler::loadGlobalSettings( QSettings& settings )
{
    frameLineColor     =        settings.value( "frameLineColor", QColor( Qt::black ) ).value<QColor>();
    selectionLineColor =    settings.value( "selectionLineColor", QColor( Qt::blue ) ).value<QColor>();
    markZero           =              settings.value( "markZero", markZeroDefault ).toBool();
    hideInfo           =              settings.value( "hideInfo", hideInfoDefault ).toBool();
    hideSmallArcFrames =    settings.value( "hideSmallArcFrames", hideSmallArcFramesDefault ).toBool();
    zoomTowardsCursor  =     settings.value( "zoomTowardsCursor", zoomTowardsCursorDefault ).toBool();
    zoomInverted       =          settings.value( "zoomInverted", zoomInvertedDefault ).toBool();
}


void
SunburstSettingsHandler::saveGlobalSettings( QSettings& settings )
{
    settings.setValue( "frameLineColor", frameLineColor );
    settings.setValue( "selectionLineColor", selectionLineColor );
    settings.setValue( "markZero", markZero );
    settings.setValue( "hideInfo", hideInfo );
    settings.setValue( "hideSmallArcFrames", hideSmallArcFrames );
    settings.setValue( "zoomTowardsCursor", zoomTowardsCursor );
    settings.setValue( "zoomInverted", zoomInverted );
}

/*
   void
   SunburstSettingsHandler::loadExperimentSettings( QSettings &settings )
   {

   }


   void
   SunburstSettingsHandler::saveExperimentSettings( QSettings &settings )
   {

   }
 */

QString
SunburstSettingsHandler::settingName()
{
    return "SunburstPluginSettings";
}


QColor
SunburstSettingsHandler::getFrameLineColor()
{
    return frameLineColor;
}


void
SunburstSettingsHandler::setFrameLineColor( const QColor& value )
{
    frameLineColor = value;
}


QColor
SunburstSettingsHandler::getSelectionLineColor()
{
    return selectionLineColor;
}


void
SunburstSettingsHandler::setSelectionLineColor( const QColor& value )
{
    selectionLineColor = value;
}


bool
SunburstSettingsHandler::getMarkZero()
{
    return markZero;
}


void
SunburstSettingsHandler::setMarkZero( bool value )
{
    markZero = value;
}


bool
SunburstSettingsHandler::getHideInfo()
{
    return hideInfo;
}


void
SunburstSettingsHandler::setHideInfo( bool value )
{
    hideInfo = value;
}


bool
SunburstSettingsHandler::getHideSmallArcFrames()
{
    return hideSmallArcFrames;
}


void
SunburstSettingsHandler::setHideSmallArcFrames( bool value )
{
    hideSmallArcFrames = value;
}


bool
SunburstSettingsHandler::getZoomTowardsCursor()
{
    return zoomTowardsCursor;
}


void
SunburstSettingsHandler::setZoomTowardsCursor( bool value )
{
    zoomTowardsCursor = value;
}


bool
SunburstSettingsHandler::getZoomInverted()
{
    return zoomInverted;
}


void
SunburstSettingsHandler::setZoomInverted( bool value )
{
    zoomInverted = value;
}
