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


#ifndef TESTPLUGIN1_H
#define TESTPLUGIN1_H
#include <QWidget>
#include <QTextEdit>
#include "PluginServices.h"
#include "CubePlugin.h"
#include "TabInterface.h"


/**
 * Minimal example for a plugin which excecutes calculations in parallel
 */

namespace simpleexampleplugin
{
#define N 10
class ParallelPlugin : public QObject, public cubepluginapi::CubePlugin, cubepluginapi::TabInterface
{
    Q_OBJECT
    Q_INTERFACES( cubepluginapi::CubePlugin )

#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA( IID "ParallelExamplePlugin" ) // unique plugin name
#endif

public:
    ParallelPlugin();

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

    // TabInterface implementation
    virtual QString
    label() const;
    virtual QWidget*
    widget();

private slots:
    void
    valuesChanged();

    void
    updateView();

    void
    calculationFinished();

private:
    QWidget*   widget_;
    QTextEdit* text;
    QTimer     updateTimer;                      // used for parallel execution
    int        data[ N ][ N ];

    cubepluginapi::PluginServices* service;
    cubepluginapi::Future*         future;
    std::mutex                     updateMutex;
};

/*
 * The class ParallelCalculation has to provide all data that is used for one calculation task, which is triggerd with
 * calculate(). It calculates the product of x_ and y_ and writes the result to result_.
 */
class ParallelCalculation : public cubepluginapi::Task
{
public:
    ParallelCalculation( int x, int y, int* result ) : x_( x ), y_( y ), result_( result )
    {
    };
    virtual bool
    isReady();
    virtual void
    calculate();

private:
    int  x_, y_;
    int* result_;
    bool first = true;
};
}

#endif // TESTPLUGIN1_H
