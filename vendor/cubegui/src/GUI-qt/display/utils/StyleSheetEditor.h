/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2019                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef STYLESHEETEDITOR_H
#define STYLESHEETEDITOR_H

#include <QDialog>
#include <QTextEdit>
#include "SettingsHandler.h"

namespace cubegui
{
class StyleSheetEditor : public QDialog, public SettingsHandler
{
    Q_OBJECT
public:
    static StyleSheetEditor*
    getInstance();

public slots:
    void
    configureStyleSheet();

private slots:
    void
    apply();
    void
    cancel();
    void
    ok();
    void
    chooseColor();
    void
    help();

private:
    QTextEdit*   editor;
    QPushButton* colorBut;
    QColor       currentColor;
    QString      style;
    StyleSheetEditor( QWidget* parent = 0 );
    static StyleSheetEditor* single;

    void
    init();

    // SettingsHandler interface
public:
    virtual void
    loadGlobalStartupSettings( QSettings& );
    virtual void
    saveGlobalStartupSettings( QSettings& );
    virtual QString
    settingName();
};
}

#endif // STYLESHEETEDITOR_H
