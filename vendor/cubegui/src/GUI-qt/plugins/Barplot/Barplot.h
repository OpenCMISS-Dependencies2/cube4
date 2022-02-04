/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2021                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2009-2015                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef BARPLOT_H
#define BARPLOT_H

#include <QtGui>
#include <QMenu>
#include "CubePlugin.h"
#include "PluginServices.h"
#include "TabInterface.h"


#include "BarplotArea.h"
#include "CubeRead.h"
#include "DataProvider.h"
#include "BarplotController.h"
#include "HorizontalRulerController.h"
#include "VerticalRulerController.h"
#include "PlotsListController.h"
#include "SettingsWidget.h"
#include "ImageSaverController.h"
#include <QToolTip>

class Barplot : public QObject, public cubepluginapi::CubePlugin, cubepluginapi::TabInterface, cubepluginapi::SettingsHandler
{
    Q_OBJECT
    Q_INTERFACES( cubepluginapi::CubePlugin )
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA( IID "SystemBarplotPlugin" )
#endif

public:
    Barplot();

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

    virtual QString
    getHelpURL() const
    {
        return "SystemBarplotPlugin.html";
    }

    // TabInterface implementation
    virtual QString
    label() const;
    virtual QIcon
    icon() const;
    virtual QWidget*
    widget();
    virtual void
    setActive( bool active );
    virtual QSize
    sizeHint() const;
    void
    valueModusChanged( cubepluginapi::ValueModus modus );

    // SettingsHandler implementation
    virtual void
    loadGlobalSettings( QSettings& settings );
    virtual void
    saveGlobalSettings( QSettings& settings );
    virtual QString
    settingName();

    // specific for Barplot
    void
    setWidgetNorth( QWidget* WidgetNorth );
    void
    setWidgetWest( QWidget* WidgetWest );
    void
    setWidgetCenter( QWidget* WidgetCenter );
    void
    setValues( QStringList mathOps,
               QList<int>  colorIndices );
    QComboBox*
    getColorList() const;
    QComboBox*
    getMathOpList() const;
    void
    hideColorList();
    void
    showColorList();
    void
    addTabToGUI();

private slots:
    void
    treeItemIsSelected( cubepluginapi::TreeItem* item );
    void
    orderHasChanged( const QList<cubepluginapi::DisplayType>& order );

    void
    globalValueChanged( const QString& name );

    //  void
    // menuItemIsSelected();

    void
    barsPlotAreaMouseCoordEventHandler( QMouseEvent* mouseEvent );
    void
    mathOpListCurrentIndexChangedHandler( int );
    void
    mathOpIndexChangeHandler( int );
    void
    opComboHandler( int newIndex );
    void
    colorComboHandler();
    void
    changeComboboxColor( QColor c );
    void
    stackButtonHasClicked();
    void
    ShowContextMenu( const QPoint& pos ); // for save image
    void
    markItemAsLoop( cubepluginapi::UserAction type );

signals:
    void
    requestToAddNewPlot();
    void
    requestToChangePlotColor();

private:
    void
    initialize();

    QWidget*                       widget_;
    cubepluginapi::PluginServices* service;

    QComboBox*   mathOpList;
    QComboBox*   colorList;
    QLabel*      noColorListLabel;
    QList<int>   colorindices;
    QPushButton* stackButton;
    QPushButton* removeAll;

    QFormLayout* layout1;
    QFormLayout* layout2;
    QHBoxLayout* OperationalBoxH;
    QVBoxLayout* WidgetLayout;
    QGridLayout* PlotLayout;
    QAction*     menuAction;

    CubeRead*                  cubeReader;
    DataProvider*              dataProvider_H;
    HorizontalRulerController* plotAreaHorizontalRulerController;
    VerticalRulerController*   plotAreaVerticalRulerController;
    BarsPlotAreaController*    barsPlotAreaController;
    PlotsListController*       plotsListController;
    ImageSaverController*      plotImageSaveController;
    SettingWidget*             settingWidget;
    bool                       changeColorAutomatically;
    QMenu*                     contextMenu;
    QAction*                   saveImageContextMenu;
    /**
     * True if currently selected item in tree is an aggregated loop item.
     */
    bool currentTreeItemLoopItem;
    /**
     * True if tab has NOT been added to GUI.
     */
    bool tabNotPresent;
};

class BarplotSlot : public QObject
{
    Q_OBJECT
public:
    BarplotSlot( const QString& txt ) : txt_( txt )
    {
    }
private:
    QString txt_;
};
#endif // BARPLOT_H
