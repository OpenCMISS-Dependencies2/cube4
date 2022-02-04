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



#ifndef VAMPIR_CONNECTION_DIALOG_H
#define VAMPIR_CONNECTION_DIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QCheckBox>
#include <QThread>
#include <QList>
#include "VampirConnecter.h"
#include "PluginServices.h"

class VampirConnectionThread;

/**
 * Creates a dialog to connect to vampir
 */
class VampirConnectionDialog : public QDialog
{
    Q_OBJECT
public:
    VampirConnectionDialog( cubepluginapi::PluginServices* service,
                            QWidget*                       par,
                            const QString&                 cubeFile,
                            QList<VampirConnecter*>*       connecterList );
    void
    setError( const QString& title,
              const QString& message );

    virtual
    ~VampirConnectionDialog();
private:
    cubepluginapi::PluginServices* service;
    QLineEdit*                     fileLine;
    QLineEdit*                     hostLine;
    QSpinBox*                      portLine;
    QPushButton*                   fileButton;
    QPushButton*                   cancelButton;
    QPushButton*                   okButton;
    QCheckBox*                     fromServerCheckbox;
    VampirConnectionThread*        connectionThread;
    QString                        errorTitle, errorMessage;
    QList<VampirConnecter*>*       connecterList;

    QString
    getDefaultVampirFileName( const QString& cubeFile ) const;

private slots:
    void
    establishVampirConnection();
    void
    getTraceFileName();
    void
    printError();
};

#endif
