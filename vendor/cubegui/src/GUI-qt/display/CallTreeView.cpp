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

#include "CallTreeView.h"
#include "DefaultCallTree.h"
#include "Globals.h"
#include "PluginManager.h"

using namespace std;
using namespace cubegui;

CallTreeView::CallTreeView( TreeModelInterface* model,
                            const QString&      tabLabel ) : TreeView( model, tabLabel )
{
    createContextMenuActions();

    selectionIsInvalidated = false;
    isActive               = true;

    setIcon( QIcon( ":images/tree-icon.png" ) );
}

/**
 * called, if user selects an item in another tree view -> select corresponding items, if this tree becomes active
 */
void
CallTreeView::invalidateSelection( Tree* tree )
{
    selectionIsInvalidated = true;
    selectedTree           = tree;
}

/* Select all entries in this tree, that have the same label and the same region as the selected entries
   in the prevous active tree.
   If nothing is clicked in the previous tree, the selection os this tree remains unchanged.
 */
void
CallTreeView::copySelection()
{
    QList<TreeItem*> selectedTreeItems;
    QStringList      selectedItems;
    QStringList      taskRoots;

    foreach( TreeItem * item, selectedTree->getSelectionList() )
    {
        // special case task tree: only the direct children of the top level task tree items (level 2) also exist in
        // call tree -> select level 2 item instead
        if ( selectedTree->getTreeType() == TASKTREE )
        {
            if ( item->getDepth() == 1 ) // top level task tree item is artifical -> use child (level 2)
            {
                item = item->getChildren().first();
            }
            while ( item->getDepth() > 2 )
            {
                item = item->getParent();
            }
        }
        selectedTreeItems.append( item );
        selectedItems.append( item->getName() );
        taskRoots.append( item->getParent()->getLabel() );
    }
    QModelIndexList found = searchItems( selectedItems );
    QModelIndexList foundRegions;

    if ( selectedTree->getTreeType() == TASKTREE )
    {
        if ( found.size() != selectedItems.size() )
        {
            // special case task tree: only select items with the same top level parent name
            for ( QModelIndex idx : found )
            {
                TreeItem* item = modelInterface->getTreeItem( idx );
                while ( item->getDepth() > 1 )
                {
                    item = item->getParent();
                }
                if ( taskRoots.contains( item->getLabel() ) )
                {
                    foundRegions.append( idx );
                }
            }
        }
        else
        {
            foundRegions = found;
        }
    }
    else // ensure that only items from the same region are selected
    {
        for ( QModelIndex idx : found )
        {
            TreeItem* item      = modelInterface->getTreeItem( idx );
            uint32_t  regionId1 = ( static_cast<cube::Cnode* > ( item->getCubeObject() ) )->get_callee()->get_id();
            for ( TreeItem* sItem : selectedTreeItems )
            {
                bool     flat      = selectedTree->getTreeType() == FLATTREE;
                uint32_t regionId2 = flat ? ( static_cast<cube::Region* > ( sItem->getCubeObject() ) )->get_id() :
                                     ( static_cast<cube::Cnode* > ( sItem->getCubeObject() ) )->get_callee()->get_id();
                if ( regionId1 == regionId2 )
                {
                    foundRegions.append( idx );
                    break;
                }
            }
        }
    }

    this->blockSignals( true );       // selected items will be handled in TreeView::setActive
    selectItems( foundRegions );
    this->blockSignals( false );
}


/** called if the call tree tab becomes active */
void
CallTreeView::setActive( bool active )
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
CallTreeView::fillContextMenu()
{
    addExpertContextMenuOptions();

    foreach( QAction * action, callContextMenuHash.values() ) // enable all context menu actions
    {
        action->setEnabled( true );
    }

    DefaultCallTree* tree       = static_cast<DefaultCallTree*> ( modelInterface->getTree() );
    bool             isLoopRoot = contextMenuItem && ( contextMenuItem == tree->getLoopRootItem() || contextMenuItem == tree->getAggregatedRootItem() );

    if ( isLoopRoot )
    {
        if ( contextMenuItem == tree->getAggregatedRootItem() )
        {
            contextMenu->addAction( callContextMenuHash.value( ShowIterations ) );
        }
        else
        {
            contextMenu->addAction( callContextMenuHash.value( HideIterations ) );
        }
        contextMenu->addSeparator();
    }

    contextMenu->addAction( contextMenuHash.value( TreeItemInfo ) );
    contextMenu->addAction( contextMenuHash.value( Documentation ) );

    if ( contextMenuItem && !isLoopRoot && !contextMenuItem->isAggregatedLoopItem() )
    {
        contextMenu->addAction( callContextMenuHash.value( SetAsLoop ) );
    }

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

    QString cutText = multipleContext ? tr( "Cut selected call tree items" ) : tr( "Cut call tree item" );
    QMenu*  cutMenu = contextMenu->addMenu( cutText );
    cutMenu->setWhatsThis( tr( "Cut selected subtree." ) );
    cutMenu->addAction( callContextMenuHash.value( Reroot ) );
    cutMenu->addAction( callContextMenuHash.value( Prune ) );
    cutMenu->addAction( callContextMenuHash.value( SetAsLeaf ) );
    cutMenu->addAction( callContextMenuHash.value( Undo ) );
    if ( commands.isEmpty() )
    {
        callContextMenuHash.value( Undo )->setEnabled( false );
    }
    contextMenu->addSeparator();

    contextMenu->addAction( contextMenuHash.value( FindItems ) );
    contextMenu->addAction( contextMenuHash.value( ClearFound ) );
    contextMenu->addAction( contextMenuHash.value( SortingMenu ) );
    contextMenu->addSeparator();
    contextMenu->addAction( contextMenuHash.value( UserDefinedMinMax ) );
    contextMenu->addSeparator();
    contextMenu->addAction( contextMenuHash.value( CopyClipboard ) );
    contextMenu->addSeparator();

    if ( isLoopRoot || ( contextMenuItem && contextMenuItem->isAggregatedLoopItem() ) )
    {
        cutMenu->setEnabled( false );
    }

    if ( !contextMenuItem || contextIsInvalid )
    {
        cutMenu->setEnabled( false );
    }
    if ( !contextMenuItem || multipleContext )
    {
        callContextMenuHash.value( HideIterations )->setEnabled( false );
        callContextMenuHash.value( ShowIterations )->setEnabled( false );
        callContextMenuHash.value( SetAsLoop )->setEnabled( false );
    }

    // HidingThreshold can only be set if hiding is enabled
    contextMenuHash.value( HidingThreshold )->setEnabled( !contextMenuHash.value( NoHiding )->isChecked() );
    if ( contextMenuItem && contextMenuItem->isTopLevelItem() )
    {
        contextMenuHash.value( HideItem )->setEnabled( false ); // disabled for root item
    }
}

void
CallTreeView::createContextMenuActions()
{
    QAction* action = new QAction( this );
    action->setIcon( QIcon( QPixmap::fromImage( QImage( ":images/iterations_small.png" ) ) ) );
    action->setIconVisibleInMenu( true );
    action->setStatusTip( tr( "Shows summary or detailed list of iterations" ) );
    action->setWhatsThis( tr( "Show summary or detailed list of iterations" ) );
    action->setText( tr( "Show iterations" ) );
    connect( action, SIGNAL( triggered() ), this, SLOT( showIterations() ) );
    callContextMenuHash.insert( ShowIterations, action );

    action = new QAction( this );
    action->setIcon( QIcon( QPixmap::fromImage( QImage( ":images/iterations_small.png" ) ) ) );
    action->setIconVisibleInMenu( true );
    action->setStatusTip( tr( "Shows summary or detailed list of iterations" ) );
    action->setWhatsThis( tr( "Shows summary or detailed list of iterations" ) );
    action->setText( tr( "Hide iterations" ) );
    connect( action, SIGNAL( triggered() ), this, SLOT( hideIterations() ) );
    callContextMenuHash.insert( HideIterations, action );

    action = new QAction( tr( "Set as loop" ), this );
    action->setStatusTip( tr( "Handle the current item as loop and its children as iterations." ) );
    action->setWhatsThis( tr( "Handle the current item as loop and its children as iterations." ) );
    connect( action, SIGNAL( triggered() ), this, SLOT( setAsLoop() ) );
    callContextMenuHash.insert( SetAsLoop, action );

    // Replaces the root of the selected callpath by its subtree. Selected callpath becomes a new root.
    action = new QAction( tr( "&Set as root" ), this );
    action->setStatusTip( tr( "Replaces the root element of the tree of the selected callpath by its subtree." ) );
    connect( action, SIGNAL( triggered() ), this, SLOT( onRerootItem() ) );
    action->setWhatsThis( tr( "For call trees. Removes all elements of the selected tree above the selected item. Selected item becomes a new root element." ) );
    callContextMenuHash.insert( Reroot, action );

    // Removes element and its subtree from the parent element. Prune
    action = new QAction( tr( "&Prune element" ), this );
    action->setStatusTip( tr( "Removes selected element and its subtree." ) );
    connect( action, SIGNAL( triggered() ), this, SLOT( onPruneItem() ) );
    action->setWhatsThis( tr( "For call tree. Removes selected element from the list of the children ot its parent. Pruned element contributes to exclusive value of its parent." ) );
    callContextMenuHash.insert( Prune, action );

    // Removes all children of its element, but element stays.
    action = new QAction( tr( "&Set as leaf" ), this );
    action->setStatusTip( tr( "Removes all children and their subtrees from the element." ) );
    connect( action, SIGNAL( triggered() ), this, SLOT( onSetAsLeaf() ) );
    action->setWhatsThis( tr( "For call trees. Removes all children and their subtrees from the element. Removed children contribute to the exclusive calue it the selected item." ) );
    callContextMenuHash.insert( SetAsLeaf, action );

    // Undo-Action
    action = new QAction( tr( "Undo" ), this );
    action->setShortcut( QKeySequence( Qt::CTRL | Qt::Key_Z ) );
    connect( action, SIGNAL( triggered() ), this, SLOT( undo() ) );
    action->setShortcutContext( Qt::ApplicationShortcut );
    callContextMenuHash.insert( Undo, action );
    // make undo shortcut also work, if context menu isn't active
    action = new QAction( tr( "Undo" ), this );
    action->setShortcut( QKeySequence( Qt::CTRL | Qt::Key_Z ) );
    connect( action, SIGNAL( triggered() ), this, SLOT( undo() ) );
    action->setShortcutContext( Qt::ApplicationShortcut );
    this->addAction( action ); // to activate shortcut
}

void
CallTreeView::showIterations()
{
    DefaultCallTree* tree = static_cast<DefaultCallTree*> ( modelInterface->getTree() );
    tree->showIterations();
    emit recalculateRequest( tree );
}

void
CallTreeView::hideIterations()
{
    DefaultCallTree* tree = static_cast<DefaultCallTree*> ( modelInterface->getTree() );
    tree->hideIterations();
    emit recalculateRequest( tree );
}

void
CallTreeView::rebuild()
{
    setUpdatesEnabled( false );
    DefaultCallTree* tree  = static_cast<DefaultCallTree*> ( getTree() );
    TreeModel*       model = tree->getModel();

    TreeItem* loop   = tree->getLoopRootItem();
    bool      hidden = tree->iterationsAreHidden();

    tree->unsetLoop();
    model->removeItem( tree->getRootItem() );
    tree->initialize(); // rebuild call tree from original cube
    tree->activeNodes = cube::CubeProxy::ALL_CNODES;
    model->addItem( tree->getRootItem() );

    if ( loop ) // recover previous loop state
    {
        tree->setAsLoop( loop );
        if ( hidden )
        {
            tree->hideIterations();
        }
    }

    this->selectItem( tree->getRootItem()->getChildren().first(), false );
    for ( TreeItem* item : tree->getItems() )
    {
        if ( item->isExpanded() )
        {
            tree->expandItem( item, true );
        }
    }
    setUpdatesEnabled( true );
}

void
CallTreeView::setAsLoop()
{
    DefaultCallTree* tree = static_cast<DefaultCallTree*> ( modelInterface->getTree() );
    tree->setAsLoop( contextMenuItem );

    PluginManager::getInstance()->genericUserAction( LoopMarkAction );
    updateValues();
}


void
CallTreeView::onRerootItem()
{
    assert( !contextIsInvalid );
    QList<TreeItem*> items = ( multipleContext ) ? modelInterface->getTree()->getSelectionList() : QList<TreeItem*>() << contextMenuItem;
    executeUndoableCommand( std::bind( &CallTreeView::rerootItems, this, items ) );
}

void
CallTreeView::onPruneItem()
{
    assert( !contextIsInvalid );
    QList<TreeItem*> items = ( multipleContext ) ? modelInterface->getTree()->getSelectionList() : QList<TreeItem*>() << contextMenuItem;
    executeUndoableCommand( std::bind( &CallTreeView::pruneItems, this, items ) );
}

void
CallTreeView::onSetAsLeaf()
{
    assert( !contextIsInvalid );
    QList<TreeItem*> items = ( multipleContext ) ? modelInterface->getTree()->getSelectionList() : QList<TreeItem*>() << contextMenuItem;
    executeUndoableCommand( std::bind( &CallTreeView::setAsLeaf, this, items ) );
}

void
CallTreeView::rerootItems( QList<TreeItem*> items )
{
    DefaultCallTree* tree  = static_cast<DefaultCallTree*> ( modelInterface->getTree() );
    TreeModel*       model = tree->getModel();

    // remove all top level elements from model
    model->removeItem( tree->getRootItem() );

    // add new roots
    for ( TreeItem* item : items )
    {
        model->addItem( item );
    }
    tree->setAsRoot( items );
}

// prune element and add its value to the parent
void
CallTreeView::pruneItems( QList<TreeItem*> items )
{
    DefaultCallTree* tree  = static_cast<DefaultCallTree*> ( modelInterface->getTree() );
    TreeModel*       model = tree->getModel();

    for ( TreeItem* item : items )
    {
        if ( item->isTopLevelItem() )
        {
            Globals::setStatusMessage( tr( "Action is not allowed for call tree root" ), Warning );
            return;
        }

        model->removeItem( item );
        tree->pruneItem( item );
    }

    // select 1st element, if nothing is selected
    if ( selectionModel()->selectedRows().size() == 0 )
    {
        QModelIndex idx  = modelInterface->getModel()->index( 0, 0 );
        TreeItem*   item = modelInterface->getTreeItem( idx );

        selectItem( item, QItemSelectionModel::Select == QItemSelectionModel::Select );
    }
}

// set as leaf : hide all children of the given items
void
CallTreeView::setAsLeaf( QList<TreeItem*> items )
{
    DefaultCallTree* tree  = static_cast<DefaultCallTree*> ( modelInterface->getTree() );
    TreeModel*       model = tree->getModel();

    foreach( TreeItem * item, items )
    {
        item->setExpanded( false ); // collapse item, no changes in tree required
        foreach( TreeItem * child, item->getChildren() )
        {
            model->removeItem( child );
        }
    }

    // select this leaf element, if nothing is selected
    if ( selectionModel()->selectedRows().size() == 0 )
    {
        selectItem( contextMenuItem, QItemSelectionModel::Select == QItemSelectionModel::Select );
    }
}

void
CallTreeView::executeUndoableCommand( std::function<void()> command )
{
    command();
    commands.append( command );
}

void
CallTreeView::undo()
{
    if ( commands.isEmpty() )
    {
        Globals::setStatusMessage( "No further undo operation possible", Warning );
        return;
    }

    // return to initial state
    rebuild();

    // apply all changed but last one
    commands.removeLast();
    for ( auto function : commands )
    {
        function();
    }

    // invalidate all metric tree items
    Tree* metric = Globals::getTabManager()->getActiveTree( METRIC );
    metric->invalidateItems();

    getTree()->invalidateItems();

    // invalidate left trees
    TabManager*  tab = Globals::getTabManager();
    QList<Tree*> left, right;
    tab->getNeighborTrees( left, right, getTree() );
    foreach( Tree * tree, left )
    {
        tree->invalidateItems();
    }

    Globals::getTabManager()->reinit();
}

// this slot displays the location of the clicked item's callee
//
void
CallTreeView::onLocation()
{
    TreeItem* item = contextMenuItem;
    location( item, false );
}

void
CallTreeView::onLocationCallee()
{
    TreeItem* item = contextMenuItem;
    location( item, true );
}

// display the location of a call or region item
//
void
CallTreeView::location( TreeItem* item, bool takeCallee )
{
    QString mod;
    int     line  = -1;
    int     line2 = -1;
    QString lineStr, line2Str;

    if ( !item->getCubeObject() )
    {
        return;
    }

    if ( item->getType() == CALLITEM )
    {
        if ( !takeCallee )
        {
            cube::Cnode* cnode = static_cast<cube::Cnode*>( item->getCubeObject() );
            mod  = QString::fromStdString( cnode->get_mod() );
            line = cnode->get_line();
        }
        else
        {
            cube::Cnode*  cnode  = static_cast<cube::Cnode*>( item->getCubeObject() );
            cube::Region* region = cnode->get_callee();
            mod   = QString::fromStdString( region->get_mod() );
            line  = region->get_begn_ln();
            line2 = region->get_end_ln();
        }
    }
    else if ( item->getType() == REGIONITEM )
    {
        cube::Region* region = static_cast<cube::Region*>( item->getCubeObject() );
        mod   = QString::fromStdString( region->get_mod() );
        line  = region->get_begn_ln();
        line2 = region->get_end_ln();
    }

    if ( mod.isEmpty() )
    {
        mod = tr( "Undefined" );
    }
    if ( line == -1 )
    {
        lineStr = tr( "Undefined" );
    }
    else
    {
        lineStr = QString::number( line );
    }
    if ( line2 == -1 )
    {
        line2Str = tr( "Undefined" );
    }
    else
    {
        line2Str = QString::number( line2 );
    }


    QString text;
    text.append( tr( "\nModule:          " ) );
    text.append( mod );
    text.append( tr( "\n\nStarts at line:  " ) );
    text.append( lineStr );
    CallTree* call       = dynamic_cast<CallTree*> ( modelInterface->getTree() );
    bool      isFlatTree = call && ( call->getRootItem()->getType() == REGIONITEM );
    if ( isFlatTree || takeCallee )
    {
        text.append( tr( "\n\nEnds at line:    " ) );
        text.append( line2Str );
    }
    text.append( "\n" );

    Globals::setStatusMessage( text, Information );
}
// end of location()

QString
CallTreeView::getContextDescription( TreeItem* item )
{
    cube::Cnode* cnode = static_cast<cube::Cnode* >( item->getCubeObject() );
    if ( !cnode )
    {
        return "";
    }
    cube::Region* region = cnode->get_callee();

    std::string descr = tr( "Region name:   " ).toUtf8().data() + region->get_name() + "\n";
    descr = descr + tr( "Mangled name:   " ).toUtf8().data() + region->get_mangled_name() + "\n\n";
    descr = descr + tr( "Region description:   " ).toUtf8().data() + region->get_descr() + "\n";

    stringstream sstr;
    sstr << cnode->get_id();
    string _str_cnode_id;
    sstr >> _str_cnode_id;

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
    descr = descr + tr( "Call path ID:   " ).toUtf8().data() + _str_cnode_id + "\n";
    descr = descr + tr( "Beginning line:   " ).toUtf8().data() + _str_beg_ln + "\n";
    descr = descr + tr( "Ending line:   " ).toUtf8().data() + _str_end_ln + "\n";
    descr = descr + tr( "Paradigm:   " ).toUtf8().data() + region->get_paradigm() + "\n";
    descr = descr + tr( "Role:   " ).toUtf8().data() + region->get_role() + "\n";
    descr = descr + tr( "Source file:   " ).toUtf8().data() + region->get_mod() + "\n";
    descr = descr + tr( "Url:   " ).toUtf8().data() + region->get_url() + "\n";
    // descr = descr + "Description:   " + _region->get_descr();

    QString callerMod = QString::fromStdString( cnode->get_mod() );
    if ( callerMod.isEmpty() )
    {
        descr = descr + tr( "Caller info:   not available" ).toUtf8().data();
    }
    // todo write caller info

    const std::map<std::string, std::string>& attrs = cnode->get_attrs();
    descr = descr + "\n" +  tr( "Attributes :" ).toUtf8().data();
    for ( std::map<std::string, std::string>::const_iterator iter = attrs.begin(); iter != attrs.end(); ++iter )
    {
        descr = descr + "\n" +  iter->first + " : " + iter->second;
    }



    vector<pair<string, double> > num_params = cnode->get_num_parameters();
    string                        sep        = tr( "\nNumeric parameters\n  " ).toUtf8().data();
    for ( unsigned i = 0; i < num_params.size(); i++ )
    {
        char buffer[ 100 ];
        sprintf( buffer, "%g", num_params[ i ].second );
        descr += sep + num_params[ i ].first + ":  " + string( buffer );
        sep    = "\n  ";
    }

    vector<pair<string, string> > str_params = cnode->get_str_parameters();
    sep = tr( "\nString parameters\n  " ).toUtf8().data();
    for ( unsigned i = 0; i < str_params.size(); i++ )
    {
        descr += sep + str_params[ i ].first + ":  " + str_params[ i ].second;
        sep    = "\n  ";
    }

    return QString( descr.c_str() );
}

//=============== settings ========================================================================================================================
void
CallTreeView::loadStatus( QSettings& settings )
{
    if ( !isVisible() )
    {
        return;
    }

    QModelIndex elem = modelInterface->findPath( settings.value( "loopRootItem" ).toString() );

    if ( elem.isValid() )
    {
        TreeItem* loop = modelInterface->getTreeItem( elem );
        if ( loop )
        {
            DefaultCallTree* tree = static_cast<DefaultCallTree*> ( getTree() );
            tree->setAsLoop( loop );
            bool hidden = settings.value( "iterationsAreHidden" ).toBool();
            if ( hidden )
            {
                tree->hideIterations();
            }
        }
    }

    TreeView::loadStatus( settings );
}

void
CallTreeView::saveStatus( QSettings& settings )
{
    if ( !isVisible() )
    {
        return;
    }

    DefaultCallTree* tree   = static_cast<DefaultCallTree*> ( getTree() );
    bool             hidden = tree->iterationsAreHidden();
    TreeItem*        loop   = hidden ? tree->getAggregatedRootItem() : tree->getLoopRootItem();
    if ( loop )
    {
        QModelIndex loopIndex = modelInterface->find( loop );
        settings.setValue( "loopRootItem", QVariant( modelIndexToPath( loopIndex ) ) );
        settings.setValue( "iterationsAreHidden", hidden );
    }

    TreeView::saveStatus( settings );
}

void
CallTreeView::loadExperimentSettings( QSettings& settings )
{
    QModelIndex elem = convertQVariantToIndex( settings.value( "loopRootItem" ).toList() );
    if ( elem.isValid() )
    {
        TreeItem* loop = modelInterface->getTreeItem( elem );
        if ( loop )
        {
            DefaultCallTree* tree = static_cast<DefaultCallTree*> ( getTree() );
            tree->setAsLoop( loop );
            bool hidden = settings.value( "iterationsAreHidden" ).toBool();
            if ( hidden )
            {
                tree->hideIterations();
            }
        }
    }
    TreeView::loadExperimentSettings( settings );
}

void
CallTreeView::saveExperimentSettings( QSettings& settings )
{
    DefaultCallTree* tree   = static_cast<DefaultCallTree*> ( getTree() );
    bool             hidden = tree->iterationsAreHidden();
    TreeItem*        loop   = hidden ? tree->getAggregatedRootItem() : tree->getLoopRootItem();
    if ( loop )
    {
        QModelIndex loopIndex = modelInterface->find( loop );
        settings.setValue( "loopRootItem", QVariant( convertIndexToQVariant( loopIndex ) ) );
        settings.setValue( "iterationsAreHidden", hidden );
    }
    TreeView::saveExperimentSettings( settings );
}

//=============== end settings ====================================================================================================================
