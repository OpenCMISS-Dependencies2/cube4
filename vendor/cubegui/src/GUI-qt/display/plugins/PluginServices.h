/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2021                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/




#ifndef PLUGINSERVICES_H
#define PLUGINSERVICES_H
#include <stdint.h>
#include <QtGui>
#include <QToolBar>
#include <QMenu>

#include "Globals.h"
#include "TreeItem.h"
#include "AggregatedTreeItem.h"
#include "TabInterface.h"
#include "SettingsHandler.h"
#include "PluginServiceExports.h"
#include "CubeProxy.h"
#include "Task.h"

/**
 * The class PluginServices provides the interface for all plugins to the cube GUI and cube
 * data. If a function is added or the signature of a function has changed, the version number
 * of the identifier string in PluginInterface.h (PLUGIN_VERSION) has to be increased
 */

namespace cube
{
class Cube;
class CubeProxy;
class Vertex;
}

namespace cubegui
{
class PluginManager;
class Tree;
class TreeItem;
class TabWidget;
class TreeItemMarker;
class ColorMap;
class TabInterface;
class SettingsHandler;
class MarkerLabel;
class DefaultMarkerLabel;
class Future;
}

namespace cubepluginapi
{
using cubegui::DisplayType;
using cubegui::TreeType;
using cubegui::Tree;
using cubegui::TreeItem;
using cubegui::TreeItemMarker;
using cubegui::MarkerLabel;
using cubegui::DefaultMarkerLabel;
using cubegui::TabInterface;
using cubegui::Task;

class PluginServices : public QObject
{
    Q_OBJECT
signals:
    /** This signal is emitted if the user selects one ore more tree items. To get all
     * selected items @see Pluginservices::getSelections( TreeType type )
     * The calculation of the corresponding tree values may not finished, when this signal is emitted. Use
     * TabInterface::valuesChanged if the plugin depends on tree values.
     * @param item the recently selected item
     */
    void
    treeItemIsSelected( cubepluginapi::TreeItem* item );

    /** This signal is emitted if the order of the tabs has changed.
     *  @param order list which contains METRICWIDGET, CALLWIDGET, SYSTEMWIDGET in the order the user
     *  has selected
     */
    void
    orderHasChanged( const QList<cubepluginapi::DisplayType>& order );

    /** This signal is emitted if the context menu in a tree widget type will be shown.
     *  Additional menu items may be added now.
     * @param type METRIC, CALL or SYSTEM
     * @param item the item, the user has clicked with the right mouse or nullptr if clicked on the background
     */
    void
    contextMenuIsShown( cubepluginapi::DisplayType type,
                        cubepluginapi::TreeItem*   item );

    /** This signal is emitted for special user actions, eg. if a tree item is marked as loop.  */
    void
    genericUserAction( cubepluginapi::UserAction action );

    /**
       This signal is emitted if a global value has changed.
       @param name the name of the global value
       @see PluginServices::getGlobalValue
     */
    void
    globalValueChanged( const QString& name );

    /**
       This signal is emitted if the selected tree has changed
     */
    void
    tabActivated( cubepluginapi::DisplayType type );

public:
    // ------------------------------------
    /** Returns a pointer to the cube data. */
    cube::CubeProxy*
    getCube() const;

    /** Returns the name of the loaded cube file including the path. */
    const QString&
    getCubeFileName() const;

    /** Returns the basename of the currently loaded cube file including the path. */
    QString
    getCubeBaseName() const;

    /** If a statistics file exists, the filename of that file is returned, otherwise an empty string. */
    QString
    getStatName() const;

    /** Returns a widget which can be used e.g. as parent widget of dialogs, if the plugins doesn't define tabs. */
    QWidget*
    getParentWidget() const;

    // -------- functions to access tree items ---------------
    /** Returns a list with all tree items of the active tree. */
    const QList<TreeItem*>&
    getTreeItems( DisplayType type ) const;

    /** Returns a list with all tree items of the given tree. */
    const QList<TreeItem*>&
    getTreeItems( TreeType type ) const;

    /** Returns a list with the top level tree items of the given tree.
     * For the calltree and systemtree, the top level tree items refer to the visible root nodes in CUBE.*/
    const QList<TreeItem*>
    getTopLevelItems( DisplayType type ) const;

    /** Returns the system tree item with the given id. */
    TreeItem*
    getSystemTreeItem( uint32_t sysId ) const;

    /** Returns the call tree item with the given id. */
    TreeItem*
    getCallTreeItem( uint32_t cnodeId ) const;

    /** Returns the metric tree item with the given id. */
    TreeItem*
    getMetricTreeItem( std::string metricId ) const;

    // -------- end functions to access tree items ---------------

    /** Marks the given tree item item combination with a marker. The items of the left
     * and the middle tree are marked as dependency, the item of the rightmost tree is marked with the
     * given marker.
     * If an item with NULL value is given, the rightmost item left to this item is marked.
     * Examples: DemoPlugin, StatisticPlugin, LaunchPlugin
     * @param marker see @ref getTreeItemMarker
     * @see Pluginservices::getTreeItemMarker
     * */
    void
    addMarker( const TreeItemMarker* marker,
               TreeItem*             metric,
               TreeItem*             call,
               TreeItem*             system );

    /** Marks the given tree item with a marker, use @ref updateTreeView to activate it.
     * The plugin has to manage the dependencies, e.g. to add or remove the markers if the tree order
     * changes.
     * This function should only be used, if the marker depends on runtime values.
     * @param item the item to which the marker is added
     * @param marker see @ref getTreeItemMarker
     * @param isDependency if true, the item is marked as a dependency to another marked item (usually
     * of another tree)
     */
    void
    addMarker( TreeItem*             item,
               const TreeItemMarker* marker,
               bool                  isDependency = false );

    /** Removes the given marker from the given tree item. */
    void
    removeMarker( TreeItem*             item,
                  const TreeItemMarker* marker );

    /** Removes all plugin managed marker which have been set by this plugin. */
    void
    removeMarker();

    /** Removes all plugin managed marker from the active tree which have been set by this plugin. */
    void
    removeMarker( DisplayType type );

    /** returns a new tree item marker, which can be used to marks tree items with
     * different background color or other attributes, which can be chosen by the user
     * @param label text for the color legend
     * @param icons list of pixmaps of ascending size. The best fitting icon will be shrunk to tree item height
     * and displayed for each marked item.
     * @param isInsignificant marks item as insignificant, if true. Insignificant items will be grayed out.
     * @param markerLabel contains a function, that returns a label which depends on the choosen tree item; if
     * markerLabel is not set, the first parameter (label) is always used. Example: TreeItemMarker plugin
     */
    const TreeItemMarker*
    getTreeItemMarker( const QString&        label,
                       const QList<QPixmap>& icons = QList<QPixmap>( ),
                       bool                  isInsignificant = false,
                       MarkerLabel*          markerLabel = 0 );

    /** Returns order of tabs, default order is METRIC, CALL, SYSTEM */
    const QList<cubepluginapi::DisplayType>&
    getOrder() const;

    /** Returns the currenly selected value modus. */
    ValueModus
    getValueModus() const;

    // --- functions to handle selections  ---------------------------------

    /** Returns last selected item in the active tree.
     *  @param type METRIC, CALL, SYSTEM */
    TreeItem*
    getSelection( DisplayType type ) const;

    /** Returns selected items in the active tree.
     *   @param type METRIC, CALL, SYSTEM */
    const QList<TreeItem*>&
    getSelections( DisplayType type ) const;

    /** Returns true, if the currenly selected metric item uses integer values. */
    bool
    intMetricSelected() const;

    /** Selects the given item.
     *  @param item The tree item to select
     *  @param add If add is false, all previously selected items are deselected. If
     *  add is true and the item already is selected, it becomes deselected.
     */
    void
    selectItem( TreeItem* item,
                bool      add );

    // --- functions for menus, statusbar, toolbar  ---------------------------------

    /** Inserts a menu item to the context menu of the treeWidget of the given type and returns the corresponding action.
     *  The action is automatically deleted after the context menu is closed.
     * @param type METRIC, CALL, SYSTEM
     * @param menuItemText the label of the created context menu item
     */
    QAction*
    addContextMenuItem( DisplayType    type,
                        const QString& menuItemText );

    /** Adds a submenu with the plugin label to the plugin menu and returns a pointer to the created QMenu.
     */
    QMenu*
    enablePluginMenu();

    /** Adds the given toolbar below the menu bar.
     * @param tab If tab is 0, the toolbar is always visible. Otherwise the toolbar is initially hidden and gets only visible, if the
     * corresponding tab becomes visible. If the tab is detached, the toolbar is moved from the main panel to the new window. */
    void
    addToolBar( QToolBar*     toolbar,
                TabInterface* tab = 0 );

    /** Removes the previously added toolbar. */
    void
    removeToolBar( QToolBar*     toolbar,
                   TabInterface* tab = 0 );

    /** Writes the given message to the status line at the bottom of the cube window. */
    void
    setMessage( const QString&             str,
                cubepluginapi::MessageType type = cubepluginapi::Information );

    // --- system tree related function ---------------------------------

    /** Returns the index of the currenly selected subset of the system tree. */
    int
    getActiveSubsetIndex() const;

    /** Returns the contents of the currenly selected subset of the system tree. */
    const QList<TreeItem*>&
    getActiveSubset() const;

    /** Sets the currenly selected subset of the system tree to the given index. */
    void
    setActiveSubset( int index );

    /** Returns a list of all names of the defined subsets for the system tree. */
    QStringList
    getSubsetLabelList() const;

    // --- end system tree related function ---------------------------------

    // --- functions which are only available for TabInterface subclasses -------------------------

    /**
     * Adds an additional sub-tabwidget to the SYSTEMTAB with the given label. The following tabs already exist:
     * DEFAULT_TAB ("System view") and OTHER_PLUGIN_TAB ("General")
     * @return the id of the tabWidget to be used in PluginServices::addTab
     */
    int
    defineSystemTab( const QString& label,
                     TabType        tabType = DEFAULT_TAB );

    /**
     * Adds a tab to METRICTAB, CALLTAB or SYSTEMTAB
     * Adds a the given tab. Currently only SYSTEMTAB is supported.
     * @param type cubepluginapi::SYSTEMTAB
     * @param addAtIndex the desired position of the tab
     * @param tabWidgetID use OTHER_PLUGIN_TAB, if the plugin isn't related to the system tree. In that case, the value mode combo
     * box and the value view aren't shown, use defineSystemTab to create a new system tab category
     */
    void
    addTab( cubepluginapi::DisplayType type,
            TabInterface*              tab,
            int                        tabWidgetID = DEFAULT_TAB );

    /** Removes the previously added tab. @see PluginServices::addTab */
    void
    removeTab( TabInterface* tab );

    /** Enables or disables the given tab. If disabled, the tab gets inactive and greyed out. */
    void
    enableTab( TabInterface* tab,
               bool          enabled );

    /** Sets the given tab selected or brings the detached tab to front. */
    void
    toFront( TabInterface* tab );

    /** Clears the value widget, the widget only shows an empty area.
     */
    void
    clearValueWidget();

    /** Updates the value widget at the bottom of a plugin tab. This method has to be called, if the default behaviour
     * (see value widget below the tree) doesn't fit.
     * The value widget consists of two rows:
     * - The first row shows:      minValue | value (% of max-min) | maxValue. Set value to NaN to hide it.
     * - The second row shows:  minAbsValue | absValue             | maxAbsValue
     * If absValue is NaN but mean and varianceSqrt exist, the value field is replaced with mean +/- varianceSqrt
     *
     * @param minValue the minimum value
     * @param maxValue the maximum value
     * @param value the value to display in the center
     * @param minAbsValue the minimum absolute value
     * @param maxAbsValue the maximum absolute value
     * @param absValue the absolute value of the parameter "value"
     * @param mean the mean of all values
     * @param varianceSqrt the standard deviation of all values
     */
    void
    updateValueWidget( TabInterface* tc,
                       double        minValue,
                       double        maxValue,
                       double        value = std::nan( "" ),
                       double        minAbsValue = std::nan( "" ),
                       double        maxAbsValue = std::nan( "" ),
                       double        absValue = std::nan( "" ),
                       double        mean = std::nan( "" ),
                       double        varianceSqrt = std::nan( "" ) );

    // --- end tab related functions ----

    /** Allows the plugin to save and load settings. @see cubepluginapi::SettingsHandler */
    void
    addSettingsHandler( SettingsHandler* s );

    /** Adds a colormap to the list of available colormaps from which the user can choose the active one. */
    void
    addColorMap( ColorMap* map );

    /** Adds a value view to the list of available views from which the user can choose the active one. */
    void
    addValueView( ValueView* view );

    // --- access global settings ----

    /** Sets a global value:
    * @param name identifier of the value
    * @param value the value which can be of any type
    * @param notifyMyself if true, the signal globalValueChanged is also sent to the caller
    * of this method. Causes infinite recursion, if called inside the slot connected to
    * globalValueChanged. */
    void
    setGlobalValue( const QString&  name,
                    const QVariant& value,
                    bool            notifyMyself = false );

    /**
     * Retreives the global value of the given identifier name.
     */
    QVariant
    getGlobalValue( const QString& name ) const;


    /** Calculates a color corresponding to the value parameter having minValue at color
     * position 0.0 and maxValue at color position 1.0 on the color scale.
     * If whiteForZero is set to true, the zero value is assigned the color white.
     */
    QColor
    getColor( double value,
              double minValue,
              double maxValue,
              bool   whiteForZero = true ) const;


    /**
     * formatNumberAndUnit writes the given value into a string using the given format. If
     * integerType is true, the value is formatted without decimal places.
     * @param format FORMAT_TREES for treeWidgets and FORMAT_DEFAULT for all other widgets
     */
    QPair<QString, QString>
    formatNumberAndUnit( double          value,
                         const QString&  unit,
                         PrecisionFormat format,
                         bool            integerType = false ) const;

    /**
     * formatNumber writes the given value into a string using the given format. If
     * integerType is true, the value is formatted without decimal places.
     * @param format FORMAT_TREES for treeWidgets and FORMAT_DEFAULT for all other widgets
     */
    QString
    formatNumber( double          value,
                  PrecisionFormat format,
                  bool            integerType = false ) const;

    /**
     * Convenience function, equivalent to formatNumber(double value, PrecisionFormat format, bool integerType = false)
     * with PrecisionFormat = FORMAT_DEFAULT
     */
    QString
    formatNumber( double value,
                  bool   integerType = false ) const;

    // --- end access global settings ----

    // --- update the trees after plugin has changed data  ----

    /** Recalculates the values of the items of all trees and updates the views.
     */
    void
    recalculateTreeItems();

    /** Recalculates the value of the given item and its dependent items and updates the corresponding view.
     */
    void
    recalculateTreeItem( cubegui::TreeItem* item );

    /** Updates the items of all trees, if their values (cube::Values) have been changed by a plugin and
        notifies all tabs that values have been changed. */
    void
    updateTreeItems();

    /** Updates the properties (label, color, font...) of the active tree. This method has to be called if
        tree items have been marked */
    void
    updateTreeView( DisplayType treeType );

    /** Updates the properties (label, color, font...) of the given tree. This method has to be called if
        tree items have been marked */
    void
    updateTreeView( TreeType type );

    // --- end update the trees after plugin has been changed data  ----

    /** Returns a stream to write debug messages; only active if -verbose command line option is set. */
    QTextStream&
    debug();

    /** Returns a stream to write debug messages; only active if -verbose command line option is set.
       @param sender: a short identifier of the sender of the debug message */
    static QTextStream&
    debug( const QString& sender );

    /** Shows a dialog to let the user set the minimum and maximum value for coloring. */
    void
    setUserDefinedMinMaxValues( DisplayType type );

    /** Returns true, if user defined values are set. */
    bool
    getUserDefinedMinValues( DisplayType type,
                             double&     min,
                             double&     max ) const;

    /** adds a new metric to the metric tree
     * @param metric the new metric to add
     * @param parent the parent item or NULL for top level metrics */
    void
    addMetric( cube::Metric* metric,
               TreeItem*     parent = 0 );

    /** Removes the given metric item from the metric tree. */
    void
    removeMetric( TreeItem* metricItem );

    /** Recalculates the values of the given metric item. */
    void
    updateMetric( TreeItem* metricItem );

    /** Returns the active tree in the tab of the given type */
    Tree*
    getActiveTree( cubegui::DisplayType type ) const;

    /** true, if iterations are defined in calltree */
    bool
    hasIterations();

    /** Creates a Future object to execute tasks in parallel. The object is deleted after the plugin has been closed.
     * @param tab if exists, a progress bar will be displayed in the given tab while the tasks are running */
    Future*
    createFuture( TabInterface* tab = 0 );

    /** returns a list with the names of the saved states of a cube  */
    QStringList
    getBookmarks();

    /** loads the saved state of a cube and applies it
     * @param loadGlobals also load global settings like colormap, fonts ...
     */
    void
    loadBookmark( const QString& name,
                  bool           loadGlobals = false );

    /** returns the description for the given bookmark name */
    QString
    getBookmarkDescription( const QString& name );

    /** returns list of tabs, which the plugin has created */
    QList<cubegui::TabInterface*>
    getTabList();

private:
    friend class cubegui::PluginManager; // to allow PluginManager to emit signals of PluginServices directly

    PluginServices( cubegui::PluginManager* pm,
                    const QString&          pluginName );
    ~PluginServices();
    void
    resizeTreeItemMarker( int iconHeight );

    /** Closes all tabs of the current plugin. */
    void
    closeTabs();

    /** Returns the tree of the given type */
    Tree*
    getTree( cubegui::TreeType type ) const;

    cubegui::TabWidget*   currentTabWidget;  // TabWidget, in which the plugin has placed its tabs
    QList<TabInterface* > currentTabs;       // all tabs of the current plugin/service

    cubegui::PluginManager* pm;
    QString                 pluginName;      // unique plugin name
    SettingsHandler*        settingsHandler; // required if the plugin wants to  save and load settings

    QList<TreeItem*>       markedItemList;   // items that are currenly marked by this plugin
    QList<TreeItemMarker*> markerList;       // all marker that the current plugin has requested
    QList<Future*>         futureList;
};
}
#endif // PLUGINSERVICES_H
