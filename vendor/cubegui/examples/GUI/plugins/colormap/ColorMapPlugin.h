/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2016                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef ColorMapPlugin_H
#define ColorMapPlugin_H
#include <QtGui>
#include "CubePlugin.h"
#include "PluginServices.h"
#include "ColorMap.h"

namespace colormapplugin
{

class ColorMapPlugin : public cubepluginapi::ColorMap, public cubepluginapi::CubePlugin
{
    Q_OBJECT
    Q_INTERFACES( cubepluginapi::CubePlugin )
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA( IID "ColorMapPlugin" )
#endif


public:
    // CubePlugin interface
    virtual QString
    name() const;

    virtual bool
    cubeOpened( cubepluginapi::PluginServices* service );

    virtual void
    cubeClosed();

    virtual void
    version( int& major,
             int& minor,
             int& bugfix ) const;

    virtual QString
    getHelpText() const;

    // ColorMap interface
    QColor
    getColor( double value,
              double minValue,
              double maxValue,
              bool   whiteForZero ) const;

    QString
    getMapName() const;
};

}

#endif // ColorMapPlugin_H

