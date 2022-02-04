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

#include <QInputDialog>
#include <QLineEdit>
#include "Compatibility.h"
#ifdef HAS_QREGULAR_EXPRESSION
#include <QRegularExpression>
#else
#include <QRegExp>
#endif
#include "SystemTreeView.h"
#include "SystemTree.h"
#include "Globals.h"
#include "InfoWidget.h"
#include "TabManager.h"

using namespace cubegui;

SystemTreeView::SystemTreeView( TreeModelInterface* model,
                                const QString&      tabLabel ) : TreeView( model, tabLabel )
{
    setIcon( QIcon( ":images/tree-icon.png" ) );
    subsetCombo = new QComboBox();
    subsetCombo->setModel( &subsetModel );

    subsetCombo->setWhatsThis(
        tr( "The Boxplot uses the currently selected subset of threads when determining its statistics."
            " Other defined subsets can be chosen from the combobox menu, such as \"All\" threads or"
            " \"Visited\" threads for only threads that visited the currently selected callpath."
            " Additional subsets can be defined from the System Tree with the \"Define subset\" context menu"
            " using the currently slected threads via multiple selection (control + left mouseclick)"
            " or with the \"Find items\" context menu selection option." ) );

    splitter = new QSplitter( Qt::Vertical );
    splitter->addWidget( this );
    splitter->addWidget( subsetCombo );

    QList<int> sizeList;
    sizeList << splitter->size().height() << 1;
    splitter->setSizes( sizeList );

    initializeCombo();
    createContextMenuActions();

    connect( this, SIGNAL( definedSubsetsChanged( const QString & ) ),
             this, SLOT( fillSubsetCombo( const QString & ) ) );
}

void
SystemTreeView::setActive( bool active )
{
    if ( active )
    {
        connect( subsetCombo, SIGNAL( currentIndexChanged( int ) ), this, SLOT( updateSubsetMarks() ) );
        connect( subsetCombo, SIGNAL( currentIndexChanged( int ) ), this, SLOT( updateSubsetModel() ) );
    }
    else
    {
        subsetCombo->disconnect();
    }
}

void
SystemTreeView::valuesChanged()
{
    updateSubsetModel();
    updateSubsetMarks();
}

/**
   mark the items of the subset in the tree
 */
void
SystemTreeView::updateSubsetMarks()
{
    QModelIndexList list;
    if ( subsetCombo->currentIndex() != 0 ) // do not mark if "All" is selected
    {
        list = convertToModelIndexList( getActiveSubset() );
    }

    modelInterface->setFoundItems( list );
    this->viewport()->update();
}

QWidget*
SystemTreeView::widget()
{
    if ( !containerWidget )
    {
        // create a container widget which consists of the tree view and the search widget
        containerWidget = new QWidget;
        QVBoxLayout* vlayout = new QVBoxLayout();
        vlayout->setContentsMargins( 0, 0, 0, 0 );
        containerWidget->setLayout( vlayout );
        containerWidget->layout()->addWidget( splitter );
    }
    return containerWidget;
}

void
SystemTreeView::initializeCombo()
{
    numberOfLeafs = 0;

    foreach( TreeItem * item, modelInterface->getTree()->getItems() )
    {
        if ( item->getChildren().size() == 0 )
        {
            numberOfLeafs++;
        }
    }
    fillSubsetCombo();
    resetSubsetCombo();
    subsetCombo->setMaximumHeight( subsetCombo->sizeHint().height() );
}


void
SystemTreeView::getPredefinedSubsets()
{
    foreach( TreeItem * item, getTree()->getItems() )
    {
        cube::Sysres*     vertex = dynamic_cast<cube::Sysres*> ( item->getCubeObject() );
        cube::sysres_kind kind   = vertex->get_kind();
        QString           type;

        if ( item->isLeaf() )
        {
            QList<TreeItem*>& list = predefinedSubsets[ "All" ];
            list.append( item );
        }

        switch ( kind )
        {
            case cube::CUBE_SYSTEM_TREE_NODE:
                type = "tree node";
                break;
            case cube::CUBE_LOCATION_GROUP:
            {
                cube::LocationGroup* group = dynamic_cast<cube::LocationGroup*> ( item->getCubeObject() );
                type = QString::fromStdString( group->get_type_as_string() );
                break;
            }
            case cube::CUBE_LOCATION:
            {
                cube::Location* location = dynamic_cast<cube::Location*> ( item->getCubeObject() );
                type = QString::fromStdString( location->get_type_as_string() );

                if ( location->get_type() == cube::CUBE_LOCATION_TYPE_CPU_THREAD )
                {
                    if ( location->get_rank() == 0 )
                    {
                        QList<TreeItem*>& list = predefinedSubsets[ "Thread 0" ];
                        list.append( item );
                    }
                    else
                    {
                        QList<TreeItem*>& list = predefinedSubsets[ "Thread > 0" ];
                        list.append( item );
                    }
                }
                break;
            }
            default:
                type = tr( "undefined" );
                break;
        }
        *type.begin() = ( *type.begin() ).toUpper(); // start with uppercase letter

        QList<TreeItem*>& list = predefinedSubsets[ type ];
        list.append( item );
    }
}

/**
 * @brief SystemTreeView::fillSubsetCombo
 * @param name, if not empty, the corresponding item will be selected
 */
void
SystemTreeView::fillSubsetCombo( const QString& name )
{
    QStringList list;

    getPredefinedSubsets(); // node types
    QStringList preList = predefinedSubsets.keys();
    preList.sort();
    foreach( QString type, preList )
    {
        QString label = type + " (" + QString::number( predefinedSubsets[ type ].size() ) + " elements)";
        list << label;
    }
    list.insert( 1, tr( "Visited" ) ); // 2nd position after "All"

    int         idx  = -1;
    QStringList keys = userDefinedSubsets.keys();
    keys.sort();
    for ( int i = 0; i < keys.size(); ++i )
    {
        uint size = userDefinedSubsets.value( keys.at( i ) ).size();
        list << keys.at( i ) + " (" + QString::number( size ) + " elements)";
        if ( !name.isEmpty() && keys.at( i ) == name )   // select new item
        {
            idx = list.size() - 1;
        }
    }

    subsetModel.setStringList( list );
    if ( idx >= 0 )
    {
        subsetCombo->setCurrentIndex( idx );
    }
}


/**
   updates the number of processes/threads for the visited subset, if visited is active
 */
void
SystemTreeView::updateSubsetModel()
{
    QString name = tr( "Visited" );
    int     idx  = subsetCombo->findText( name, Qt::MatchContains );
    if ( idx == subsetCombo->currentIndex() ) // "Visited" is visible
    {
        visitedSubset = ( static_cast<SystemTree*>( modelInterface->getTree() ) )->getVisitedItems();
        QString txt = name + " (" + QString::number( visitedSubset.size() ) + " elements)";
        subsetModel.setData( subsetModel.index( idx ), txt );
    }
}


/**
 * @brief TreeWidget::getActiveSubset
 * @return the TreeWidgetItems which belong the the currently selected subset
 */
const QList<TreeItem*>&
SystemTreeView::getActiveSubset()
{
    updateSubsetModel(); // update visited subset because TreeView may be not visible/active

    QString name = getActiveSubsetLabel();
    if ( name == tr( "Visited" ) )
    {
        return visitedSubset;
    }
    else if ( predefinedSubsets.contains( name ) )
    {
        return predefinedSubsets[ name ];
    }
    else
    {
        return userDefinedSubsets[ name ];
    }
}

void
SystemTreeView::setActiveSubset( int index )
{
    if ( index != subsetCombo->currentIndex() )
    {
        subsetCombo->setCurrentIndex( index );
    }
}

QStringList
SystemTreeView::getSubsetLabelList() const
{
    return subsetModel.stringList();
}

int
SystemTreeView::getActiveSubsetIndex() const
{
    return subsetCombo->currentIndex();
}

/**
 * @brief return current subset label, removes additional description
 */
QString
SystemTreeView::getActiveSubsetLabel() const
{
    QString name = subsetCombo->currentText();
#ifdef HAS_QREGULAR_EXPRESSION
    name.remove( QRegularExpression( "\\s*\\([0-9]* elements\\)$" ) );
#else
    name.remove( QRegExp( "\\s*\\([0-9]* elements\\)$" ) );
#endif
    return name;
}

void
SystemTreeView::resetSubsetCombo()
{
    subsetCombo->setCurrentIndex( 0 );
}

QString
SystemTreeView::getContextDescription( TreeItem* item )
{
    if ( !item )
    {
        return QString( "" );
    }
    cube::Sysres* sys = static_cast<cube::Sysres* >( item->getCubeObject() );

    std::string descr = tr( "Name: " ).toUtf8().data() + sys->get_name() + "\n\n";

    if ( item->getType() == SYSTEMTREENODEITEM )
    {
        descr = descr + ( static_cast<cube::SystemTreeNode*>( sys ) )->get_desc() + "\n";
    }
    std::stringstream id_str;
    id_str << sys->get_id();
    descr = descr + tr( "System tree ID: " ).toUtf8().data() + id_str.str() + "\n";

    if ( item->getType() == SYSTEMTREENODEITEM )
    {
        descr = descr + tr( "Class: " ).toUtf8().data() + ( static_cast<cube::SystemTreeNode*>( sys ) )->get_class() + "\n";
    }
    else
    if ( item->getType() == LOCATIONGROUPITEM )
    {
        descr = descr + tr( "Type: " ).toUtf8().data() + ( static_cast<cube::LocationGroup*>( sys ) )->get_type_as_string() + "\n";
    }
    else
    if ( item->getType() == LOCATIONITEM )
    {
        descr = descr + tr( "Type: " ).toUtf8().data() + ( static_cast<cube::Location*>( sys ) )->get_type_as_string() + "\n";
    }
    const std::map<std::string, std::string>& attrs = sys->get_attrs();
    std::string                               sep   = tr( "\nAttributes\n  " ).toUtf8().data();
    for ( std::map<std::string, std::string>::const_iterator it = attrs.begin(); it != attrs.end(); ++it )
    {
        descr += sep + it->first + ": " + it->second;
        sep    = "\n  ";
    }
    return QString( descr.c_str() );
}

void
SystemTreeView::createContextMenuActions()
{
    defineSubsetAct = new QAction( tr( "Define subset" ), this );
    defineSubsetAct->setStatusTip( tr( "Define a named subset with all selected items" ) );
    connect( defineSubsetAct, SIGNAL( triggered() ), this, SLOT( defineSubset() ) );

    calculateAct = new QAction( tr( "Calculate system tree" ), this );
    connect( calculateAct, SIGNAL( triggered() ), this, SLOT( calculateTree() ) );

    calculateEnableAct = new QAction( tr( "Enable calculation of system tree" ), this );
    connect( calculateEnableAct, SIGNAL( triggered() ), this, SLOT( enableTreeCalculation() ) );

    calculateDisableAct = new QAction( tr( "Disable calculation of system tree" ), this );
    connect( calculateDisableAct, SIGNAL( triggered() ), this, SLOT( disableTreeCalculation() ) );
}

void
SystemTreeView::fillContextMenu()
{
    if ( getTree()->isManualCalculation() )
    {
        contextMenu->addAction( calculateAct );
        //contextMenu->addAction( calculateEnableAct );
        //contextMenu->addAction( calculateDisableAct );
        contextMenu->addSeparator();
    }

    QAction* infoAction = new QAction( tr( "Info" ), this );
    infoAction->setStatusTip( tr( "Shows a short description of the clicked item" ) );
    infoAction->setWhatsThis( tr( "Gives some short information about the clicked node. Disabled if you did not click over a node or if no information is available for the clicked node." ) );
    connect( infoAction, SIGNAL( triggered() ), this, SLOT( info() ) );
    contextMenu->addAction( infoAction );

    contextMenu->addSeparator();
    QMenu* expandMenu = contextMenuHash.value( ExpandMenu )->menu();
    expandMenu->insertAction( contextMenuHash.value( ExpandAll ), contextMenuHash.value( ExpandLevel ) );
    contextMenu->addAction( expandMenu->menuAction() );

    QMenu* hidingMenu = contextMenu->addMenu( tr( "Hiding" ) );
    hidingMenu->setWhatsThis( tr( "Hide subtrees." ) );
    hidingMenu->addAction( contextMenuHash.value( HidingThreshold ) );
    hidingMenu->addSeparator();
    hidingMenu->addAction( contextMenuHash.value( DynamicHiding ) );
    hidingMenu->addAction( contextMenuHash.value( StaticHiding ) );
    hidingMenu->addAction( contextMenuHash.value( HideItem ) );
    hidingMenu->addAction( contextMenuHash.value( UnhideItem ) );
    hidingMenu->addAction( contextMenuHash.value( NoHiding ) );

    contextMenu->addAction( contextMenuHash.value( HideTreeLevel ) );
    contextMenu->addAction( contextMenuHash.value( UnhideTreeLevel ) );
    contextMenu->addSeparator();
    contextMenu->addAction( contextMenuHash.value( FindItems ) );
    contextMenu->addAction( contextMenuHash.value( ClearFound ) );
    contextMenu->addAction( contextMenuHash.value( SortingMenu ) );
    contextMenu->addSeparator();
    contextMenu->addAction( defineSubsetAct );
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

/** additional slot for TreeItemInfo context menu */
void
SystemTreeView::info()
{
    InfoWidget::getInstance()->activate();
    TabWidget* tab = Globals::getTabManager()->getTab( SYSTEM );
    tab->detachTab( InfoWidget::getInstance() );
    InfoWidget::getInstance()->toFront();
    InfoWidget::getInstance()->showSystemInfo();
}

// ManualCalculation mode
void
SystemTreeView::calculateTree()
{
    // only mark first visible item because system tree is calculated in one chunk
    QList<TreeItem*> items;
    items.append( getTree()->getRootItem()->getChildren().first() );
    emit calculateRequest( items );
}

void
SystemTreeView::enableTreeCalculation()
{
    // only mark first visible item because system tree is calculated in one chunk
    getTree()->getRootItem()->getChildren().first()->setCalculationEnabled( true );
    emit recalculateRequest( getTree() );
}

void
SystemTreeView::disableTreeCalculation()
{
    // only mark first visible item because system tree is calculated in one chunk
    getTree()->getRootItem()->getChildren().first()->setCalculationEnabled( false );
}

void
SystemTreeView::defineSubset()
{
    QModelIndexList list = this->selectedIndexes();

    int              level = -1;
    QList<TreeItem*> selectedList;
    foreach( QModelIndex listIndex, list )
    {
        TreeItem* item = modelInterface->getTreeItem( listIndex );
        selectedList.append( item );
        if ( ( level != -1 ) && ( item->getDepth() != level ) )
        {
            Globals::setStatusMessage( tr( "Only items of the same level can build a subset" ), Warning );
            return;
        }
        level = item->getDepth();
    }
    if ( selectedList.size() < 3 )
    {
        Globals::setStatusMessage( tr( "At least 3 selected items are necessary to define a subset" ), Warning );
        return;
    }

    // ok gets false if the user canceled the input
    bool    ok;
    QString name = QInputDialog::getText( this, tr( "Subset" ),
                                          tr( "Define named subset" ), QLineEdit::Normal,
                                          "", &ok );

    if ( name.length() > 0 )
    {
        userDefinedSubsets.insert( name, selectedList );
        emit definedSubsetsChanged( name );
    }
}
