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





#ifndef _TABWIDGET2
#define _TABWIDGET2

#include <QSettings>
#include <QTabWidget>
#include <QComboBox>
#include <QHash>
#include <QMainWindow>
#include <QStackedLayout>

#include "Constants.h"
#include "InfoWidget.h"
#include "PluginServiceExports.h"

namespace cube
{
class Cube;
}

namespace cubepluginapi
{
class CubePlugin;
}

namespace cubegui
{
class TabInterface;
class ValueWidget;
class TreeView;
class Tree;
class DetachableTabWidget;
class TabContents;
class ValueModusCombo;

/**
   A TabWidget consists of three graphical elements which are vertically aligned:
   - a combo box to choose the value mode
   - a tabbar with a group of tabs, e.g. TreeViews or other classes that derive from TabInterface
   - a value widget

   Cube creates three instances of TabWidget with different types (TabWidgetType):
   - the metric tab contains only the metric tree
   - the call tab contains the call tree and a flat profile
   - the system tab contains the system tree and may contain tabs from plugins
 */
class TabWidget : public QWidget
{
    Q_OBJECT

signals:
    void
    externalValueModusSelected( TabWidget* );

public:

    TabWidget( DisplayType displayType );

    // initialize the tab widget
    void
    initialize( const QList<DisplayType>& order );

    void
    setOrder( const QList<DisplayType>& order );

    int
    createSystemTabWidget( const QString& label,
                           TabType        tabType = DEFAULT_TAB );

    void
    addPluginTab( TabInterface*,
                  const QString& pluginName,
                  int            tabWidgetID = DEFAULT_TAB );

    void
    addTabInterface( TabInterface* tc,
                     bool          detachable = false,
                     int           tabWidgetID = DEFAULT_TAB );

    void
    addTreeTab( TreeView* );

    TreeView*
    getActiveTreeView();

    /************ get methods **********/

    // return the current value modus
    ValueModus
    getValueModus();

    // return the precision widget
    ValueWidget*
    getValueWidget();

    /*********** initialization / clean up **************/

    // called when files are closed;
    // clear the tab, no data is shown
    void
    cubeClosed();

    /************** miscellaneous ****************/

    // disable/enable the given tab, e.g. required if system widget is 1st
    void
    enableTab( TabInterface* tab,
               bool          enabled );

    // return the minimal width necessary to display all information
    virtual
    QSize
    sizeHint() const;

    DisplayType
    getType() const;

    TabInterface*
    currentTabInterface() const;

    void
    updateValueWidget();

    /** detaches the given tab interface */
    void
    detachTab( TabInterface* ti );

    /** sets the given TabInterface as active tab */
    void
    toFront( TabInterface* tc );

    /** sets the given TabInterface as active tab */
    void
    toFront( const QString& label );

    /** sets the first tab of the plugin as active tab */
    void
    toFront( cubepluginapi::CubePlugin* plugin );

    void
    addToolBar( QToolBar*,
                TabInterface* );
    void
    removeToolBar( QToolBar*,
                   TabInterface* );
    TabContents*
    getTab( TabInterface* tc ) const;

    DisplayType
    getDisplayType() const;

    QString
    getDefaultPlugin() const;

public slots:
    void
    valuesChanged();

    void
    setValueModus( ValueModus modus );

    void
    removePluginTab( TabInterface* );

    void
    setDefaultPlugin( const QString& name );

private slots:

    // signal currentChanged() is emitted when the visible tab element has changed;
    // that signal gets connected to this slot
    void
    onCurrentChanged();

    void
    setActiveTab( TabInterface* tc );

    void
    systemTabChanged( int nr );

    void
    externalValueModusSelectedSlot();

private:
    /*********************/
    // the 3 components of this widget
    QTabWidget*      mainTabWidget;
    ValueModusCombo* valueCombo;

    DisplayType displayType;                       // type: metric, call or system tab, see constants.h for TabWidgetType
    cube::Cube* cube;                              // the cube database
    ValueModus  valueModus;                        // value modus, see constants.h

    QList<DetachableTabWidget*> tabWidgetList;     // index = tab widget id , value = tab widget
    QList<DisplayType>          order;
    QList<TreeView*>            treeList;          // contains only TreeWidgets

    // only for system tabs
    QHash<QWidget*, DetachableTabWidget*> systemTabs; // child of systemTabWidget
    QHash<TabInterface*, TabContents*>    tabHash;    // tab interface -> tab widget contents
    QHash<TabInterface*, bool>            changeInfo; // true, if tree values have been changed since tab has been inactive

    TreeView* currentCallView;                        // active call tree view

    QString defaultPlugin;

    /*********************/
    void
    updateValueCombo();

    bool
    isOrder( DisplayType left,
             DisplayType right );

    void
    updateToolbarStatus( TabInterface* tab );

    DetachableTabWidget*
    getParentTabWidget( TabInterface* tc ) const;

    TabContents*
    currentTab() const;
};

class TabContents : public QWidget
{
public:
    TabContents( TabInterface*        tc,
                 DetachableTabWidget* parent,
                 bool                 detachable = true );
    TabInterface*
    getTabInterface()
    {
        return tc;
    }
    ValueWidget*
    getValueWidget();
    DetachableTabWidget*
    getParentTab();
    QStackedLayout*
    getStatusLayout();
    QToolBar*
    getToolbar() const;
    void
    setToolbar( QToolBar* value );

private:
    TabInterface*        tc;
    QToolBar*            toolbar;
    ValueWidget*         valueWidget;
    QWidget*             statusPanel;  // contains valueWidget as default for system and topology tabs
    QStackedLayout*      statusLayout; // layout of statusPanel
    DetachableTabWidget* parentTab;    // parent tab widget
};

/** TabWidget whose tabs can be detached to a separate Window */
class DetachableTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    DetachableTabWidget( TabType        type = DEFAULT_TAB,
                         const QString& defaultTab = "" );
    void
    setDetachable( TabContents*   tab,
                   const QString& pluginName = QString() );
    void
    addTab( TabContents* contents );
    void
    removeAllTabs();
    void
    removeTabInterface( TabInterface* );

    void
    setToolbar( TabInterface* tc,
                QToolBar*     bar );
    void
    removeToolbar( TabInterface* tc,
                   QToolBar*     bar );

    TabInterface*
    getTabInterface( QWidget* ) const;

    void
    detachTab( int );

    static QMainWindow*
    getParentWindow( QWidget* tab );

    TabType
    getType() const;

signals:
    /** this signal is emitted, if a tab get detached which wasn't visible before */
    void
    activated( TabInterface* ti );
    void
    pluginClosed( TabInterface* ti );
    void
    defaultPluginChanged( const QString& name );

private slots:
    void
    showContextMenu( const QPoint& point );
    void
    detachTabAction();
    void
    detachedTabClosed();
    void
    defaultPluginChanged();
    void
    closePlugin();
    void
    closeTab();

private:
    QHash<TabContents*, QString>       tabPluginHash;    // tab widget -> uniq plugin name
    QHash<TabInterface*, TabContents*> tabInterfaceHash; // tabInterface -> main tab widget of type TabContents
    QList<TabContents*>                detachableTabs;   // tabs which can be detached
    QList<QMainWindow*>                detachedTabs;     // detached tabs, which should be deleted if cube is closed
    TabType                            type;
    TabContents*
    currentTab();
    TabContents*
    tabContents( int index );

    const QString& defaultTab;
};

class CloseableMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    CloseableMainWindow( QWidget* parent ) : QMainWindow( parent )
    {
    }
signals:
    void
    closed();

    // QWidget interface
protected:
    void
    closeEvent( QCloseEvent* )
    {
        emit closed();
    }
};
}
#endif
