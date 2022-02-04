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


#ifndef PLUGINSETTINGDIALOG_H
#define PLUGINSETTINGDIALOG_H

#include <QDialog>
#include <QList>
#include <QCheckBox>
#include <QRadioButton>
#include <QGroupBox>

namespace cubegui
{
class PluginSettingDialog : public QDialog
{
    Q_OBJECT
public:

    static void
    showSettingsDialog( QWidget* parent );


private slots:
    void
    selectPlugins( bool enabled );

private:
    PluginSettingDialog( QWidget* parent = 0 );
    void
    showSettings();

    QList<QCheckBox*> pluginChecks;
    QRadioButton*     all;
    QRadioButton*     none;
    QGroupBox*        groupBox;
};
}
#endif // PLUGINSETTINGDIALOG_H
