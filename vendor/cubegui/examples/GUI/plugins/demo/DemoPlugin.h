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


#ifndef TESTPLUGIN1_H
#define TESTPLUGIN1_H
#include <QtGui>
#include <QLabel>
#include <QLineEdit>
#include "PluginServices.h"
#include "CubePlugin.h"
#include "TabInterface.h"
#include "CubeCnode.h"

namespace demoplugin
{
class DemoPlugin : public QObject, public cubepluginapi::CubePlugin, cubepluginapi::TabInterface, cubepluginapi::SettingsHandler
{
    Q_OBJECT
    Q_INTERFACES( cubepluginapi::CubePlugin )

#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA( IID "DemoPlugin" )
#endif

public:
    DemoPlugin();

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
    void
    valueModusChanged( cubepluginapi::ValueModus modus );

    // SettingsHandler implementation
    virtual void
    loadExperimentSettings( QSettings& settings );
    virtual void
    saveExperimentSettings( QSettings& settings );
    virtual void
    loadGlobalSettings( QSettings& settings );
    virtual void
    saveGlobalSettings( QSettings& settings );
    virtual QString
    settingName();

private slots:
    void
    contextMenuIsShown( cubepluginapi::DisplayType,
                        cubepluginapi::TreeItem* item );
    void
    treeItemIsSelected( cubepluginapi::TreeItem* item );
    void
    orderHasChanged( const QList<cubepluginapi::DisplayType>& order );

    void
    globalValueChanged( const QString& name );

    void
    menuItemIsSelected();

    void
    checkLoop( cubepluginapi::UserAction type );

    void
    setMarkerWithDependencies();
    void
    setMarker();

private:
    void
    defineTreeItemMarker();

    QWidget*   widget_;
    QLineEdit* lineEdit_;
    QLabel*    qlabel_;

    QList<const cubepluginapi::TreeItemMarker*> markerList;

    cubepluginapi::PluginServices* service;
    cubepluginapi::TreeItem*       contextItem;
    cubepluginapi::DisplayType     contextType;
};

class DemoSlot : public QObject
{
    Q_OBJECT
public:
    DemoSlot( const QString& txt ) : txt_( txt )
    {
    }
public slots:
    void
    print()
    {
        qDebug() << "DemoPlugin::DemoSlot:: " << txt_ << Qt::endl;
    }
private:
    QString txt_;
};
}

#endif // TESTPLUGIN1_H
