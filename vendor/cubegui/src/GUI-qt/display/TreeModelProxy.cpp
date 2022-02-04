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


#include "config.h"

#include "TreeModelProxy.h"
#include "TreeItem.h"
#include "Globals.h"

using namespace cubegui;

TreeModelProxy::TreeModelProxy( TreeModel* smodel ) : TreeModelInterface( this ), tree( smodel->getTree() ), sourceModel( smodel )
{
    // setDynamicSortFilter(true);
    setSourceModel( sourceModel );
    filterThreshold = 0;
    comparator      = 0;
}

Tree*
TreeModelProxy::getTree() const
{
    return tree;
}

void
TreeModelProxy::setFoundItems( const QModelIndexList& indexList )
{
    QModelIndexList sourceList;

    foreach( QModelIndex idx, indexList )
    {
        idx = mapToSource( idx );
        sourceList.append( idx );
    }
    sourceModel->setFoundItems( sourceList );
}

QModelIndexList
TreeModelProxy::getFoundItems()
{
    QModelIndexList sourceList;

    foreach( QModelIndex idx, sourceModel->getFoundItems() )
    {
        idx = mapFromSource( idx );
        sourceList.append( idx );
    }
    return sourceList;
}

void
TreeModelProxy::markSelectedParents( const QModelIndexList& indexList )
{
    QModelIndexList parents;

    foreach( QModelIndex idx, indexList )
    {
        idx = mapToSource( idx );
        parents.append( idx );
    }
    sourceModel->markSelectedParents( parents );
}

void
TreeModelProxy::clearFoundItems()
{
    sourceModel->clearFoundItems();
}

QModelIndex
TreeModelProxy::find( TreeItem* searchItem ) const
{
    QModelIndex idx = sourceModel->find( searchItem );

    return mapFromSource( idx );
}

QModelIndexList
TreeModelProxy::find( const QString& name ) const
{
    QModelIndexList srcList = sourceModel->find( name );
    QModelIndexList found;

    foreach( QModelIndex idx, srcList )
    {
        idx = mapFromSource( idx );
        found.append( idx );
    }

    return found;
}

QModelIndex
TreeModelProxy::findPath( const QString& path ) const
{
    QModelIndex idx = sourceModel->findPath( path );
    return mapFromSource( idx );
}

// -------------------------------------------------------------------------------------------

/**
 * @brief TreeModelProxy::filterAcceptsRow
 * returns if the item at souceRow of the sourceParent item of the source model should be visible
 */
bool
TreeModelProxy::filterAcceptsRow( int sourceRow, const QModelIndex& sourceParent ) const
{
    if ( tree->getFilter() == Tree::FILTER_NONE )
    {
        return true;
    }
    else
    {
        QModelIndex idx  = sourceModel->index( sourceRow, DATA_COL, sourceParent );
        TreeItem*   item = sourceModel->getTreeItem( idx );
        return !item->isHidden();
    }
    return true; // 1. call has invalid indices for all tree elements
}

/**
 * allows sorting of the items of the model
 */
bool
TreeModelProxy::lessThan( const QModelIndex& left, const QModelIndex& right ) const
{
    if ( !comparator )
    {
        return true;
    }
    TreeItem* item1 = static_cast<TreeItem*>( left.internalPointer() );
    TreeItem* item2 = static_cast<TreeItem*>( right.internalPointer() );
    return comparator->compare( item1, item2 );
}


void
TreeModelProxy::setComparator( Comparator* c )
{
    comparator = c;
}

/**
 * @brief TreeModelProxy::setFilter
 * @param threshold percentual threshold, all values below are hidden
 * @param dynamic if true, the threshold is evaluated for new item values
 * @param recalculate if true, the hidden values for the given threshold are recalculated, otherwise
 * the filter only changes from dynamic to static or vice versa.
 */
void
TreeModelProxy::setFilter( Tree::FilterType filter, double threshold )
{
    filterThreshold = threshold;

    tree->setFilter( filter, threshold );

    this->invalidateFilter();                            // required to restart filtering after values have been changed
    emit filteringEnded();
    emit( dataChanged( QModelIndex(), QModelIndex() ) ); // all values have been changed
}

void
TreeModelProxy::updateValues()
{
    if ( tree->getFilter() == Tree::FILTER_DYNAMIC )
    {
        tree->setFilter( Tree::FILTER_DYNAMIC, filterThreshold );
        this->invalidateFilter();   // required to restart filtering after values have been changed
        emit filteringEnded();
    }
    if ( comparator && autoApplySortFilter )
    {
#if QT_VERSION >= 0x050000
        this->sort( -1 ); // following sort dosn't work for qt5, if this line is missing
#endif
        this->sort( DATA_COL );
    }
    emit( dataChanged( QModelIndex(), QModelIndex() ) ); // all values have been changed
}
