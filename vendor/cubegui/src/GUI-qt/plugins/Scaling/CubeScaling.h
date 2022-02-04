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


#ifndef CUBE_SCALING_H
#define CUBE_SCALING_H

#include <QDebug>
#include <QPushButton>
#include <QLabel>
#include <QRadioButton>
#include <QListWidget>
#include "ContextFreePlugin.h"
#include "ContextFreeServices.h"

/**
 * A general structure of the mapping of one Cube on another.
 */

namespace scaling
{
class CubeScaling : public QObject, public cubepluginapi::ContextFreePlugin
{
    Q_OBJECT
    Q_INTERFACES( cubepluginapi::ContextFreePlugin )
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA( IID "CubeScalingPlugin" )
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


private slots:
    void
    startAction();
    void
    selectCubes();
    void
    selectCubes( const QString& path );
    void
    clearCubes();

private:
    cubepluginapi::ContextFreeServices* service;

    cube::CubeProxy* cube;
    QListWidget*     cubes;
    QPushButton*     addCube;
    QPushButton*     scaling;
    QRadioButton*    aggregation;
    QRadioButton*    maximum;

    QRadioButton* profile_cubex;
    QRadioButton* summary_cubex;
    QRadioButton* scout_cubex;
    QRadioButton* trace_cubex;
    QRadioButton* custom_cubex;

    QLineEdit* custom_name;
};
}

#endif // CUBE_TOOLS_H
