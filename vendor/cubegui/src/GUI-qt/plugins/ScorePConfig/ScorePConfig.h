/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2017                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef CUBE_SCOREP_CONFIG_H
#define CUBE_SCOREP_CONFIG_H

#include <QWidget>
#include "CubePlugin.h"
#include "PluginServices.h"
#include "CubeServices.h"
#include "ScorePConfigVariables.h"

namespace scorepconfigplugin
{
class ScorePConfigPlugin : public QObject, public cubepluginapi::TabInterface, public cubepluginapi::CubePlugin
{
    Q_OBJECT
    Q_INTERFACES( cubepluginapi::CubePlugin )
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA( IID "ScorePConfigPlugin" )
#endif


public:

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

    virtual QString
    getHelpURL() const
    {
        return "ScorepCfgPlugin.html";
    }
    // TabInterface implementation
    virtual QWidget*
    widget();

    virtual QString
    label() const;

    QIcon
    icon() const;

private slots:

private:
    cubepluginapi::PluginServices* service;
    QWidget*                       widget_;

    ScorePConfigVariables help;
};
};
#endif // CUBE_TOOLS_H
