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


#ifndef VALUEVIEWCONFIG_H
#define VALUEVIEWCONFIG_H

#include <QDialog>
#include <QComboBox>
#include <QLineEdit>
#include <QHash>

namespace editor_plugin
{
class EditorConfig : public QDialog
{
public:
    Q_OBJECT
public:
    EditorConfig( QWidget* parent,
                  QHash<QString, QStringList>& externalEditors,
                  QString& externalEditor );
public slots:
    void
    close();

private slots:
    void
    accept();
    void
    apply();
    void
    reject();

    void
    editorChanged( int index );

private:
    QComboBox*                   editorCombo;
    QLineEdit*                   editorNameInput;
    QLineEdit*                   editorCommandInput1;
    QLineEdit*                   editorCommandInput2;
    QWidget*                     configWidget;
    QHash<QString, QStringList>& editorHash;
    QString&                     editorName;
    QHash<QString, QStringList>  tmpEditorHash;
};
}

#endif // VALUEVIEWCONFIG_H
