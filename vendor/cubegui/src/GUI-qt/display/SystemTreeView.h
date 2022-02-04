/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2019                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef SYSTEMTREEVIEW_H
#define SYSTEMTREEVIEW_H

#include <QSplitter>
#include <QComboBox>
#include <QStringListModel>
#include "TreeView.h"

namespace cubegui
{
/** a SystemTreeView is a TreeView which displays an additional combo box
 *  which allows to define subsets of tree items
 */
class SystemTreeView : public TreeView
{
    Q_OBJECT
public:
    SystemTreeView( TreeModelInterface* modelInterface,
                    const QString&      tabLabel );

    // implementation of TabInterface
    virtual QWidget*
    widget();

    virtual void
    setActive( bool active );
    virtual void
    valuesChanged();

    /** returns a list with the items of the currenly selected subset */
    const QList<TreeItem*>&
    getActiveSubset();

    /** returns all available subsets labels */
    QStringList
    getSubsetLabelList() const;

    /** returns the index of the currently selected subset */
    int
    getActiveSubsetIndex() const;

    /** selects the subset with the given index */
    void
    setActiveSubset( int index );

signals:
    void
    definedSubsetsChanged( const QString& );

protected:
    virtual QString
    getContextDescription( TreeItem* item );

    virtual void
    fillContextMenu();

private slots:
    void
    resetSubsetCombo();
    void
    updateSubsetModel();
    void
    updateSubsetMarks();
    void
    defineSubset();
    void
    fillSubsetCombo( const QString& name = "" );
    void
    info();
    void
    calculateTree();
    void
    enableTreeCalculation();
    void
    disableTreeCalculation();


private:
    QSplitter* splitter;
    QComboBox* subsetCombo;

    QStringListModel                  subsetModel;
    QHash<QString, QList<TreeItem*> > userDefinedSubsets;
    QHash<QString, QList<TreeItem*> > predefinedSubsets; // predefined subsets
    QList<TreeItem*>                  visitedSubset;

    QAction* defineSubsetAct;     // context menu action
    QAction* calculateAct;        // context menu action
    QAction* calculateEnableAct;  // context menu action
    QAction* calculateDisableAct; // context menu action

    int numberOfLeafs;
    int
    getSubsetItemCount( const QString& name );
    void
    updateVisitedSubset();
    void
    initializeCombo();
    QString
    getActiveSubsetLabel() const;
    void
    createContextMenuActions();
    void
    getPredefinedSubsets();
};
}
#endif // SYSTEMTREEVIEW_H
