/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2020                                                     **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef TREESETTINGS_H
#define TREESETTINGS_H

#include <QString>
#include <QMenu>
#include "PluginManager.h"

namespace cubegui
{
class TreeItem;
class Tree;

struct CallDisplayConfig
{
public:
    CallDisplayConfig()
    {
        hideArguments      = false;
        hideReturnValue    = false;
        hideModules        = false;
        hideClassHierarchy = -1;
        demangleFunctions  = true;
    }
    bool hideArguments;
    bool hideReturnValue;
    bool hideModules;
    int  hideClassHierarchy;
    bool demangleFunctions;
};

/**
 * @brief The TreeConfig class contains all data and actions for the Display->tree menu.
 */
class TreeConfig : public QObject
{
    Q_OBJECT
public:
    static TreeConfig*
    getInstance();

    QMenu*
    getMenu();

    QString
    createDisplayName( TreeItem* item );

    void
    saveGlobalSettings( QSettings& settings );
    void
    loadGlobalSettings( QSettings& settings );

private slots:
    void
    configureCallTreeLabel();
    void
    enableDemangling( bool enable );
    void
    updateCallTreeLabel();
    void
    updateCallDisplayConfig( const CallDisplayConfig& newConfig );
    void
    enableSystemLabelRank( bool enable );

private:
    static TreeConfig* single;
    CallDisplayConfig  callConfig;
    QAction*           demangleAction;
    QAction*           systemLabelAction;

    bool appendSystemRank;

    TreeConfig();
    QString
    createCallDisplayName( TreeItem* item );
    QString
    createSystemDisplayName( TreeItem* item );
};
}

#endif // TREESETTINGS_H
