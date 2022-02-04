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


#ifndef FLATTREEVIEW_H
#define FLATTREEVIEW_H

#include "TreeView.h"
namespace cubegui
{
class FlatTreeView : public TreeView
{
    Q_OBJECT
public:
    FlatTreeView( TreeModelInterface* modelInterface,
                  const QString&      tabLabel );

    virtual void
    setActive( bool );

public slots:
    /** corresponding call tree has changed the selection -> select new items if tree becomes active */
    void
    invalidateSelection( Tree* tree );

protected:
    virtual void
    fillContextMenu();

protected:
    virtual QString
    getContextDescription( TreeItem* item );

private slots:
    void
    calculateTree();

private:
    bool  isActive;
    bool  selectionIsInvalidated;
    Tree* selectedTree;
    void
    copySelection();
};
}
#endif // FLATTREEVIEW_H
