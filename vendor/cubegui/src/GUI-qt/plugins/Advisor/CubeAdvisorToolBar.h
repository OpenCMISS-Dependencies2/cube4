/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2015-2020                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef KNL_ADVISOR_TOOLBAR_H
#define KNL_ADVISOR_TOOLBAR_H

#include <QToolButton>
#include <QLineEdit>
#include <QComboBox>
#include <QStackedWidget>
#include <QToolBar>
#include <QToolButton>
#include <QLabel>
#include <QSlider>
#include "PluginServices.h"

namespace advisor
{
class AdvisorToolBar : public QToolBar
{
    Q_OBJECT
public:
    AdvisorToolBar( cubepluginapi::PluginServices* service );

    double
    getTreashold() const
    {
        return treashold_value / 200.;
    }

    inline
    QComboBox*
    getComboForAnalyses()
    {
        return analysisComboBox;
    }

    inline
    QAction*
    getCopyAction()
    {
        return copyAction;
    }

    inline
    QAction*
    getRunAnalysisAction()
    {
        return runAnalysis;
    }


    inline
    QStackedWidget*
    getPlaceForAnalysesWidgets()
    {
        return analysisWidgets;
    }



signals:
    void
    treasholdChanged( int );

private slots:

    void
    setTreashold( int );


private:
    cubepluginapi::PluginServices* service;
    QComboBox*                     analysisComboBox;
    QStackedWidget*                analysisWidgets;
    QAction*                       copyAction;
    QAction*                       runAnalysis;
    QLabel*                        treashold;
    QSlider*                       set_treashold;
    int                            treashold_value;
    bool                           active;
};
};

#endif
