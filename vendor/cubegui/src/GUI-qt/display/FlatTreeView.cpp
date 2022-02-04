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



#include "config.h"


#include "CubeRegion.h"


#include "FlatTreeView.h"
#include "FlatTree.h"
#include "Globals.h"
#include "TabManager.h"

using namespace cubegui;
using namespace std;

FlatTreeView::FlatTreeView( TreeModelInterface* modelInterface,
                            const QString&      tabLabel ) : TreeView( modelInterface, tabLabel )
{
    isActive               = false;
    selectionIsInvalidated = false;
    selectedTree           = nullptr;
    setIcon( QIcon( ":images/list-icon.png" ) );
}


/**
 * called, if user selects an item in the call tree view -> select corresponding items, if flat tree becomes active
 */
void
FlatTreeView::invalidateSelection( Tree* tree )
{
    selectionIsInvalidated = true;
    selectedTree           = tree;
}

/* select all entries in the flat tree, that have the same label and same region as the selected entries
   in the call tree
 */
void
FlatTreeView::copySelection()
{
    QStringList selectedItems;
    foreach( TreeItem * item, selectedTree->getSelectionList() )
    {
        if ( !selectedItems.contains( item->getName() ) )
        {
            foreach( TreeItem * item, selectedTree->getSelectionList() )
            {
                selectedItems.append( item->getName() );
            }
            selectedItems.append( item->getName() );
        }
    }
    QModelIndexList found = searchItems( selectedItems );
    QModelIndexList foundRegions;

    // only select items from the same region
    for ( QModelIndex idx : found )
    {
        TreeItem* item      = modelInterface->getTreeItem( idx );
        uint32_t  regionId1 = ( static_cast<cube::Region* > ( item->getCubeObject() ) )->get_id();
        for ( TreeItem* sItem : selectedTree->getSelectionList() ) // previous tree ( call tree )
        {
            uint32_t regionId2 = ( static_cast<cube::Cnode* > ( sItem->getCubeObject() ) )->get_callee()->get_id();
            if ( regionId1 == regionId2 )
            {
                foundRegions.append( idx );
                break;
            }
        }
    }

    this->blockSignals( true );  // selected items will be handled in TreeView::setActive
    selectItems( foundRegions );
    this->blockSignals( false );
}

void
FlatTreeView::setActive( bool active )
{
    isActive = active;

    if ( isActive && selectionIsInvalidated )
    {
        copySelection();
        selectionIsInvalidated = false;
    }

    TreeView::setActive( active );
}

/**
 * creates context menu items for call trees
 */
void
FlatTreeView::fillContextMenu()
{
    if ( getTree()->isManualCalculation() )
    {
        QAction* calculateAct = new QAction( tr( "Calculate flat tree" ), this );
        connect( calculateAct, SIGNAL( triggered() ), this, SLOT( calculateTree() ) );
        contextMenu->addAction( calculateAct );
        contextMenu->addSeparator();
    }

    contextMenu->addAction( contextMenuHash.value( TreeItemInfo ) );
    contextMenu->addSeparator();
    contextMenu->addAction( contextMenuHash.value( ExpandMenu ) );
    contextMenu->addSeparator();
    contextMenu->addAction( contextMenuHash.value( ExpandMenu ) );

    QMenu* hidingMenu = contextMenu->addMenu( tr( "Hiding" ) );
    hidingMenu->setWhatsThis( tr( "Hide subtrees." ) );
    hidingMenu->addAction( contextMenuHash.value( HidingThreshold ) );
    hidingMenu->addSeparator();
    hidingMenu->addAction( contextMenuHash.value( DynamicHiding ) );
    hidingMenu->addAction( contextMenuHash.value( StaticHiding ) );
    hidingMenu->addAction( contextMenuHash.value( HideItem ) );
    hidingMenu->addAction( contextMenuHash.value( UnhideItem ) );
    hidingMenu->addAction( contextMenuHash.value( NoHiding ) );

    contextMenu->addAction( contextMenuHash.value( FindItems ) );
    contextMenu->addAction( contextMenuHash.value( ClearFound ) );
    contextMenu->addAction( contextMenuHash.value( SortingMenu ) );
    contextMenu->addSeparator();
    contextMenu->addAction( contextMenuHash.value( UserDefinedMinMax ) );
    contextMenu->addSeparator();
    contextMenu->addAction( contextMenuHash.value( CopyClipboard ) );
    contextMenu->addSeparator();

    // HidingThreshold can only be set if hiding is enabled
    contextMenuHash.value( HidingThreshold )->setEnabled( !contextMenuHash.value( NoHiding )->isChecked() );
    if ( contextMenuItem && contextMenuItem->isTopLevelItem() )
    {
        contextMenuHash.value( HideItem )->setEnabled( false ); // disabled for root item
    }
}

QString
FlatTreeView::getContextDescription( TreeItem* item )
{
    if ( item->getCubeObject() && item->getType() == REGIONITEM )
    {
        cube::Region* region = static_cast<cube::Region* >( item->getCubeObject() );
        std::string   descr  = tr( "Region name:    " ).toUtf8().data() + region->get_name() + "\n";
        descr = descr + tr( "Region description:    " ).toUtf8().data() + region->get_descr() + "\n";

        FlatTree*       flat         = static_cast<FlatTree*> ( getTree() );
        TreeView*       callTreeView = Globals::getTabManager()->getView( flat->callTree );
        QModelIndexList found        = callTreeView->searchItems( QStringList() << item->getName() );
        descr = descr + tr( "References in call tree: " ).toUtf8().data() + QString::number( found.size() ).toStdString() + "\n";

        stringstream sstr1;
        string       _str_beg_ln;
        if ( region->get_begn_ln() != -1 )
        {
            sstr1 << region->get_begn_ln();
            sstr1 >> _str_beg_ln;
        }
        else
        {
            _str_beg_ln = tr( "undefined" ).toUtf8().data();
        }

        stringstream sstr2;
        string       _str_end_ln;
        if ( region->get_end_ln() != -1 )
        {
            sstr2 << region->get_end_ln();
            sstr2 >> _str_end_ln;
        }
        else
        {
            _str_end_ln = tr( "undefined" ).toUtf8().data();
        }
        descr = descr + tr( "Beginning line:   " ).toUtf8().data() + _str_beg_ln + "\n";
        descr = descr + tr( "Ending line:   " ).toUtf8().data() + _str_end_ln + "\n";
        descr = descr + tr( "Paradigm:   " ).toUtf8().data() + region->get_paradigm() + "\n";
        descr = descr + tr( "Role:   " ).toUtf8().data() + region->get_role() + "\n";
        descr = descr + tr( "Source file:   " ).toUtf8().data() + region->get_mod() + "\n";
        descr = descr + tr( "Url:   " ).toUtf8().data() + region->get_url() + "\n";


        const std::map<std::string, std::string>& attrs = region->get_attrs();
        descr = descr + "\n" +  tr( "Attributes :" ).toUtf8().data();
        for ( std::map<std::string, std::string>::const_iterator iter = attrs.begin(); iter != attrs.end(); ++iter )
        {
            descr = descr + "\n" +  iter->first + " : " + iter->second;
        }



        return QString( descr.c_str() );
    }
    return "";
}

// ManualCalculation mode
void
FlatTreeView::calculateTree()
{
    // only mark first visible item because flat tree is calculated in one chunk
    QList<TreeItem*> items;
    items.append( getTree()->getRootItem()->getChildren().first() );
    emit calculateRequest( items );
}
