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


#ifndef TAUVALUEVIEW_H
#define TAUVALUEVIEW_H
#include <QWidget>
#include <QLabel>
#include <QSpinBox>
#include <QLineEdit>
#include <QFormLayout>
#include "PluginServices.h"
#include "CubePlugin.h"
#include "ValueView.h"

namespace tauvalueplugin
{

/** TauValueView implements a value view for tau tuplets (data type CUBE_DATA_TYPE_TAU_ATOMIC)
 **/
class TauValueView : public cubepluginapi::ValueView, public cubepluginapi::SettingsHandler
{
public:
    TauValueView();
    // ValueView implementation
    double
    getDoubleValue( cube::Value* ) const;
    QString
    toString( const cubegui::TreeItem* ) const;
    QPixmap
    getIcon( const cubegui::TreeItem*,
             bool grayOut ) const;
    QSize
    getIconSize( const cubegui::TreeItem* ) const;
    bool
    isAvailable( cube::DataType type ) const;
    QString
    getName() const;
    QWidget*
    getConfigWidget( cube::DataType type );
    void
    applyConfig();
    void
    cancelConfig();

    // SettingHandler implementation
    virtual QString
    settingName()
    {
        return "TauValueViewPlugin";
    }
    virtual void
    loadGlobalSettings( QSettings& );
    virtual void
    saveGlobalSettings( QSettings& );

private:
    QSpinBox*  wbox, * hbox;
    QLineEdit* tauInput;
    QWidget*   configWidget;
    QSize      iconSize, iconSizeOld;
};

/**
 * @brief The TauValueViewPlugin class offers a new value view for tau tuplets using TauValueView
 */
class TauValueViewPlugin : public QObject, public cubepluginapi::CubePlugin
{
    Q_OBJECT
    Q_INTERFACES( cubepluginapi::CubePlugin )

#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA( IID "TauValueViewPlugin" ) // unique plugin name
#endif

public:
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

private:
    cubepluginapi::PluginServices* service;
    TauValueView*                  view;
};
}

#endif // TAUVALUEVIEW_H
