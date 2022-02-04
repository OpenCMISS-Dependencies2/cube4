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


#ifndef CALLTREEVIEW_H
#define CALLTREEVIEW_H

#include "TreeView.h"

namespace cubegui
{
class CallTreeView : public TreeView
{
    Q_OBJECT
public:
    CallTreeView( TreeModelInterface* modelInterface,
                  const QString&      tabLabel );

    virtual void
    setActive( bool );

    // settings
    void
    loadStatus( QSettings& settings );
    void
    saveStatus( QSettings& settings );
    void
    loadExperimentSettings( QSettings& settings );
    void
    saveExperimentSettings( QSettings& settings );

public slots:
    void
    invalidateSelection( Tree* selectedTree );

protected:
    virtual void
    fillContextMenu();

    virtual QString
    getContextDescription( TreeItem* item );

private:
    enum CallTreeContextMenuAction { ShowIterations, HideIterations, SetAsLoop, Reroot, Prune, SetAsLeaf, Undo };
    QHash <CallTreeContextMenuAction, QAction*> callContextMenuHash; // contains all call tree specific context menu actions
    QList< std::function<void()> >              commands;

    bool  isActive;
    bool  selectionIsInvalidated;
    Tree* selectedTree; // other tree whose selections should be selected here

    void
    createContextMenuActions();
    void
    location( TreeItem* item,
              bool      takeCallee );
    void
    executeUndoableCommand( std::function<void()> command );

    void
    rerootItems( QList<TreeItem*> items );
    void
    pruneItems( QList<TreeItem*> items );
    void
    setAsLeaf( QList<TreeItem*> items );

    void
    copySelection();

private slots:
    void
    rebuild();
    void
    setAsLoop();
    void
    showIterations();
    void
    hideIterations();
    void
    onRerootItem();
    void
    onPruneItem();
    void
    onSetAsLeaf();
    void
    undo();
    void
    onLocation();
    void
    onLocationCallee();
};
}
#endif // CALLTREEVIEW_H
