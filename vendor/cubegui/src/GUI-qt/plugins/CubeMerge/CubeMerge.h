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


#ifndef CUBE_TOOLS_MERGE_H
#define CUBE_TOOLS_MERGE_H

#include <QPushButton>
#include <QLabel>
#include <QCheckBox>
#include <QRadioButton>
#include <QListWidget>
#include "ContextFreePlugin.h"
#include "ContextFreeServices.h"




class CubeMerge : public QObject, public cubepluginapi::ContextFreePlugin
{
    Q_OBJECT
    Q_INTERFACES( cubepluginapi::ContextFreePlugin )
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA( IID "CubeMergePlugin" )
#endif


public:

    CubeMerge();

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
    selectCubes();
    void
    selectCubes( const QString& path );
    void
    clearCubes();
    void
    startAction();
    void
    uncheckChoice();

private:
    cubepluginapi::ContextFreeServices* service;
    QStringList                         args;

    QPushButton* addCube;
    QPushButton* merge;
    QCheckBox*   reduce;
    QCheckBox*   collapse;
    QListWidget* cubes;

    QRadioButton* profile_cubex;
    QRadioButton* summary_cubex;
    QRadioButton* scout_cubex;
    QRadioButton* trace_cubex;
    QRadioButton* custom_cubex;

    QLineEdit* custom_name;
};

#endif // CUBE_TOOLS_H
