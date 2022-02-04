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


#ifndef TABWIDGETCONTAINER_H
#define TABWIDGETCONTAINER_H

#include <QSplitter>
#include <QTemporaryFile>
#include <QSet>
#include <QAbstractItemModel>
#include "TreeItem.h"
#include "Constants.h"
#include "TabWidget.h"
#include "CubeProxy.h"

namespace cube
{
class Cube;
}

namespace cubegui
{
class Tree;
class TreeItemMarker;
class Future;
class TreeModelProxy;

class MarkerTripletList : public QList<QList<TreeItem*> >
{
public:
    void
    addTriplet( TreeItem* m, TreeItem* c, TreeItem* s )
    {
        QList<TreeItem*> triplet;
        triplet.append( m );
        triplet.append( c );
        triplet.append( s );
        append( triplet );
    }
};

/** This class manages the 3 TabWidgets of type METRICTAB, CALLTAB and SYSTEMTAB.
 *  - it handles the tree selections, as this may have effect on other trees
 *
 */
class TabManager : public QSplitter
{
    Q_OBJECT
public:
    /**
     * creates an empty container for the three tabs with their trees.
     */
    TabManager( QWidget* parent = 0 );

    /** To be called after the settings have been processed. Ensures that at least one item is selected in
     *  all trees, connects signals and calculates all tree values */
    void
    initialize();

    void
    reinit();

    void
    reinit( TreeItem* item );

    /**
     * Initializes the container with the given cube data.
     * Creates the four trees and its views and initializes them.
     */
    void
    cubeOpened( cube::CubeProxy* cube );
    void
    cubeClosed();

    QList<DisplayType>
    getOrder() const;
    void
    setOrder( const QList<DisplayType>& value );

    TabWidget*
    getTab( DisplayType tabType ) const;

    Tree*
    getTree( TreeType type ) const;

    /** returns the tree in the active (visible) tab */
    Tree*
    getActiveTree( DisplayType tabType ) const;

    /** returns the view of the active (visible) tab */
    TreeView*
    getActiveView( DisplayType type ) const;

    /** returns the view that belongs to the given tree*/
    TreeView*
    getView( Tree* tree ) const;

    // return a pointer on CubeProxy object
    inline
    cube::CubeProxy*
    getCube()
    {
        return cube;
    }

    // SettingsHandler interface
    void
    loadGlobalSettings( QSettings& );
    void
    saveGlobalSettings( QSettings& );
    void
    loadExperimentSettings( QSettings& );
    void
    saveExperimentSettings( QSettings& );
    QString
    settingName();

    // end SettingsHandler interface

    QList<TabWidget*>
    getTabWidgets() const;

    void
    getNeighborTrees( QList<Tree*>& left,
                      QList<Tree*>& right,
                      Tree*         current ) const;

    /* repaints all tree views after colormap has been changed */
    void
    updateColors();

    void
    addStaticMarker( const TreeItemMarker* marker,
                     TreeItem*             metric,
                     TreeItem*             call,
                     TreeItem*             system );

    /** removes all static markers, that have items of the given tree type */
    void
    removeStaticMarker( DisplayType type );
    void
    removeStaticMarker( const TreeItemMarker* marker );

    void
    updateValueViews();

    QString
    getDefaultPluginName() const;

    QList<TreeView*>
    getViews() const
    {
        return treeViewHash.values();
    }
    QList<Tree*>
    getTrees() const
    {
        return treeViewHash.keys();
    }

public slots:
    /** redraws all tree items with their changed properties, e.g. color, precision */
    void
    updateTreeItemProperties();

    /** redraws all tree items but doesn't recalculate their properties */
    void
    updateTreeItems();

private slots:
    void
    treeItemSelected( Tree* tree );
    void
    treeItemExpanded( TreeItem* item,
                      bool      expanded );
    void
    recalculateTree( Tree* tree );

    /** only calculates the given list of tree items */
    void
    calculateTreeItems( QList<TreeItem*> items );

    void
    updateStatus();

private:
    QHash<const TreeItemMarker*, MarkerTripletList > markerHash;

    QHash<Tree*, TreeView*>    treeViewHash;
    QList<Tree*>               trees;             // = treeViewHash.keys()
    QList<QAbstractItemModel*> proxyModels;       // delete all models after cube is closed
    QMap<TreeType, Tree*>      treeHash;          // tree type -> corresponding tree

    QList<TabWidget*>  tabWidgets;                // the three TabWidget: METRICTAB, CALLTAB, SYSTEMTAB
    QList<DisplayType> order;                     // order of the tabs METRICTAB, CALLTAB, SYSTEMTAB
    bool               isInitialized;             // true, if all values have been calculated
    QString            defaultPlugin;             // name of the plugin which should be visible at startupdefault

    QHash<Tree*, cubepluginapi::Future*> futures;

    cube::CubeProxy* cube;            // own copy of cube object (used for info widget)
    // --------------------------------------------------------------

    void
    activateStaticMarker();

    void
    createTreesAndViews( cube::CubeProxy* cube );
    void
    fillTreeHash( Tree*           tree,
                  TreeView*       view,
                  TreeModelProxy* proxyModel );
};
}
#endif // TABWIDGETCONTAINER_H
