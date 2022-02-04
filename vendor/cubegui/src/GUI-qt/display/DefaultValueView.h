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


#ifndef DEFAULTVALUEVIEW_H
#define DEFAULTVALUEVIEW_H

#include <QDialog>
#include <QList>
#include <QComboBox>
#include <QSpinBox>
#include <QLineEdit>
#include "ValueView.h"
#include "Settings.h"

enum TauAtomicValuePivot;

namespace cubegui
{
class TreeItem;

class DefaultValueView : public cubepluginapi::ValueView, public InternalSettingsHandler
{
    Q_OBJECT
public:
    DefaultValueView();

    double
    getDoubleValue( cube::Value* value ) const;
    QString
    toString( const TreeItem* item ) const;
    QPixmap
    getIcon( const TreeItem* item,
             bool            grayOut ) const;
    QSize
    getIconSize( const TreeItem* item ) const;

    bool
    isAvailable( cube::DataType type ) const;

    QString
    getName() const
    {
        return "Default Value View";
    }

    QWidget*
    getConfigWidget( cube::DataType type );

    QString
    getExtendedInfo( const TreeItem* item );

    // SettingsHandler interface
    void
    loadGlobalSettings( QSettings& );
    void
    saveGlobalSettings( QSettings& );

    QString
    settingName()
    {
        return "DefaultValueView";
    }

public slots:
    void
    applyConfig();
    void
    cancelConfig();

private:
    QPixmap
    getColorIcon( const TreeItem* item,
                  bool            grayOut ) const;
    QPixmap
    getTauIcon( const TreeItem* item,
                bool            grayOut ) const;
    QPixmap
    getHistogramIcon( const TreeItem* item,
                      bool            grayOut ) const;
    QString
    getTauString( const TreeItem* item ) const;

    QList<TauAtomicValuePivot> tauIconParts;
    QList<TauAtomicValuePivot> tauValueOptions;
    cube::DataType             configType;

    // dialog elements for preferences
    QComboBox* tauValueBox;
    QSpinBox*  wbox, * hbox;
    QLineEdit* tauInput;
    QWidget*   configWidget;

    // preferences that can be set by the user in the config dialog
    int     tauValueIndex, tauValueIndexOld;
    QSize   histogramSize, histogramSizeOld;
    QString tauVarString, tauVarStringOld;
};
}
#endif // DEFAULTVALUEVIEW_H
