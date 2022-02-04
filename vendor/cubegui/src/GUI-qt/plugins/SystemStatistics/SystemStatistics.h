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



#ifndef SYSTEMBOXPLOT_H
#define SYSTEMBOXPLOT_H

#include <QStackedWidget>
#include <QWidget>
#include <QComboBox>
#include <QSplitter>
#include <QAbstractButton>
#include "BoxPlot.h"

#include "ViolinPlot.h"
#include "PluginServices.h"
#include "ScrollArea.h"
#include "CubePlugin.h"

namespace system_statistics
{
class SystemStatistics : public QObject, public cubepluginapi::TabInterface, public cubepluginapi::CubePlugin
{
    Q_OBJECT
    Q_INTERFACES( cubepluginapi::CubePlugin )

#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA( IID "SystemStatisticsPlugin" ) // unique PluginName
#endif

public:
    SystemStatistics();

    // CubePlugin implementation
    virtual bool
    cubeOpened( cubepluginapi::PluginServices* service );
    void
    cubeClosed();

    virtual QString
    name() const;

    virtual void
    version( int& major,
             int& minor,
             int& bugfix ) const;

    virtual QString
    getHelpText() const;

    virtual QString
    getHelpURL() const
    {
        return "SystemStatisticsPlugin.html";
    }

    // end CubePlugin implementation

    // TabInterface implementation
    virtual QWidget*
    widget();

    virtual QString
    label() const;

    QIcon
    icon() const;

    void
    valuesChanged();

    void
    setActive( bool active );

    // end TabInterface implementation

private slots:
    void
    orderHasChanged( const QList<cubepluginapi::DisplayType>& order );

    void
    subsetChanged( int idx );

    void
    showBoxPlot( bool enabled );

    void
    showViolinPlot( bool enabled );

private:
    static const QString VIOLIN_HELP;
    static const QString BOX_HELP;

    std::vector<double>
    getValues( bool absolute );

    void
    updateValueWidget();

    void
    updateSubsetCombo();

    QSplitter*                     container;
    cubegui::ScrollArea*           scrollArea;
    QComboBox*                     subsetCombo;
    QStringListModel               subsetModel;
    cubepluginapi::PluginServices* service;
    QStackedWidget*                stacker;
    QWidget*                       graph_sel;
    QSlider*                       hslider;
    QSlider*                       oslider;
    cubegui::BoxPlot*              boxplot;
    ViolinPlot*                    violinplot;
};
}
#endif // SYSTEMBOXPLOT_H
