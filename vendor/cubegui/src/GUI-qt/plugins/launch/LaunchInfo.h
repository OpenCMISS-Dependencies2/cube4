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


#ifndef LAUNCHINFO_H
#define LAUNCHINFO_H

#include <QtCore>
#include <QHash>
#include <QSet>

#include <inttypes.h>
#include <fstream>

#include "CubeMetric.h"


#include "TreeItem.h"
#include "Launch.h"
#include "PluginServices.h"

struct LaunchKey
{
    QString  menuItem;
    QString  metricId;
    bool     hasCnode;
    uint32_t cnodeId;
    bool
    operator==( const LaunchKey& k ) const
    {
        return this->hasCnode == k.hasCnode &&
               ( this->hasCnode ? this->cnodeId == k.cnodeId : 1 ) &&
               this->menuItem == k.menuItem &&
               this->metricId == k.metricId;
    }
    QString
    toString() const
    {
        QString str;
        QString cnode = "-";
        if ( hasCnode )
        {
            cnode = QString().number( cnodeId );
        }
        str += menuItem + metricId + cnode;
        return str;
    }
};
uint
qHash( const LaunchKey& k );

class LaunchInfo : public QObject
{
    Q_OBJECT
public:
    LaunchInfo( cubepluginapi::PluginServices* service );

    void
    launch( const QString&           command,
            cubepluginapi::TreeItem* metricItem,
            cubepluginapi::TreeItem* callItem );
    void
    launch( const QString&           command,
            cubepluginapi::TreeItem* metricItem );

    bool
    existsLaunch( cube::Metric const* metric,
                  cube::Cnode const*  cnode = 0 ) const;
    QStringList
    getMenuEntries( const cube::Metric* metric,
                    const cube::Cnode*  cnode = 0 ) const;

    QString
    findLaunchCommand( const QString&      menuText,
                       cube::Metric const* metric,
                       cube::Cnode const*  cnode = 0 ) const;

    bool
    readLaunchFile( const QString& fileName );

    const QString&
    getLaunchInitMenu() const
    {
        return launchInitMenu;
    }
    bool
    isInitialized()
    {
        return isInitialized_;
    }
    const QList<uint32_t>&
    getCnodes()
    {
        return cnodes;
    }

    const QList<QString>
    getMetricsToMark()
    {
        return cnodeMetrics.values();
    }

public slots:
    void
    receivedLaunchVar( QPair<QString, QString>);
    void
    launchInit();
    void
    launchFinalize();

private:
    bool    isInitialized_;
    QString baseFileName;
    QString launchInitMenu, launchInitCommand;
    QString launchFinalizeCommand;

    cubepluginapi::PluginServices* service;

    typedef QHash<LaunchKey, QString> LaunchMap;
    LaunchMap launchMap;                            // LaunchKey -> launch command
    typedef QMap<QString, QString>    LaunchVarMap; // variable -> value
    LaunchVarMap launchVarMap;                      // variables read from launched program are saved in this map

    QHash<QString, QStringList> menuMap;            // (metric, cnode) -> list of menu entries
    QList<uint32_t>             cnodes;
    QSet<QString>               cnodeMetrics;       // contains all metrics which have cnodes with launch info

    static void
    replaceVar( QString&       str,
                const QString& var,
                const QString& value );
    void
    insertMenuMap( const QString& key,
                   const QString& value );
};

#endif // LAUNCHINFO_H
