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


#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <QToolButton>
#include <QSpinBox>

#include "SystemTopologyWidget.h"

class SystemTopologyToolBar : public QToolBar
{
    Q_OBJECT
public:
    SystemTopologyToolBar( cubepluginapi::PluginServices* service );

    void
    setTopologyWidget( SystemTopologyWidget* parent );
    void
    enableTopologyButtons();
    void
    disableTopologyButtons();
    cubepluginapi::TreeItem*
    getRecentSelection();
    void
    setRecentSelection( cubepluginapi::TreeItem* item );
    void
    reinit();

private slots:
    void
    moveDown();
    void
    moveUp();
    void
    moveRight();
    void
    moveLeft();
    void
    increasePlaneDistance();
    void
    decreasePlaneDistance();
    void
    zoomIn();
    void
    zoomOut();
    void
    reset();
    void
    scale();
    void
    coloring();
    void
    setXAngle( int angle );
    void
    setYAngle( int angle );

private:
    cubepluginapi::PluginServices* service;
    SystemTopologyWidget*          parent;
    QAction*                       leftAct, * rightAct, * upAct, * downAct;
    QAction*                       dist1Act, * dist2Act, * zoom1Act, * zoom2Act;
    QAction*                       resetAct, * scaleAct, * userAct;
    QSpinBox*                      xSpin, * ySpin;
    QLabel*                        xLabel, * yLabel;
    cubepluginapi::TreeItem*       recentSelection;
};
#endif
