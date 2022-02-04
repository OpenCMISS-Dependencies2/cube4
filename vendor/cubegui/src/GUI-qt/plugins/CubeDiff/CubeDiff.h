/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2018                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef CUBE_TOOLS_DIFF_H
#define CUBE_TOOLS_DIFF_H

#include <QDebug>
#include <QPushButton>
#include <QLabel>
#include <QCheckBox>
#include "ContextFreePlugin.h"
#include "ContextFreeServices.h"

class CubeDiff : public QObject, public cubepluginapi::ContextFreePlugin
{
    Q_OBJECT
    Q_INTERFACES( cubepluginapi::ContextFreePlugin )
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA( IID "CubeDiffPlugin" )
#endif


public:
    // ContextFreePlugin interface
    virtual QString
    name() const;

    virtual void
    opened( cubepluginapi::ContextFreeServices* service );

    virtual void
    closed();

    virtual void
    version( int& major,
             int& minor,
             int& bugfix ) const;

    virtual QString
    getHelpText() const;

    virtual void
    setArguments( const QStringList& args )
    {
        this->args = args;
    }

private slots:
    void
    startAction();
    void
    loadFile1();
    void
    loadFile2();
    void
    uncheckChoice();

private:
    cubepluginapi::ContextFreeServices* service;

    QPushButton* diff;
    QLabel*      fileLabel1;
    QLabel*      fileLabel2;
    QString      fileName1;
    QString      fileName2;
    QCheckBox*   reduce;
    QCheckBox*   collapse;
    QStringList  args;
};

#endif // CUBE_TOOLS_H
