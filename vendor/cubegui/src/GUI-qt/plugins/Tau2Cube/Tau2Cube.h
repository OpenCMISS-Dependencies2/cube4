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



#ifndef CUBE_TOOLS_TAU2CUBE_H
#define CUBE_TOOLS_TAU2CUBE_H

#include <QPushButton>
#include <QLabel>
#include <QCheckBox>
#include <QListWidget>
#include "ContextFreePlugin.h"
#include "ContextFreeServices.h"

namespace cube
{
class CubeProxy;
}

class Tau2Cube : public QObject, public cubepluginapi::ContextFreePlugin
{
    Q_OBJECT
    Q_INTERFACES( cubepluginapi::ContextFreePlugin )
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA( IID "Tau2CubePlugin" )
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
    version(        int& major,
                    int& minor,
                    int& bugfix ) const;

    virtual QString
    getHelpText() const;

private slots:
    void
    selectProfilesCubes();
    void
    selectCubes(  const QStringList& args  );
    void
    clearCubes();
    void
    startAction();


private:
    cubepluginapi::ContextFreeServices* service;
    QStringList                         args;

    QPushButton* addCube;
    QPushButton* merge;
    QCheckBox*   reduce;
    QCheckBox*   collapse;
    QListWidget* cubes;

    QWidget* widget;
//     cubepluginapi::ContextFreeServices* service;
//     cube::CubeProxy*                    cube;
//     QPushButton*                        selectTauProfile;
};

#endif // CUBE_TOOLS_H
