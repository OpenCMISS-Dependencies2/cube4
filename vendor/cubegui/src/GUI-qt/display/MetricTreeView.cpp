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


#include "config.h"

#include "MetricTreeView.h"
#include "CallTree.h"
#include "Globals.h"
#include "TabManager.h"
using namespace cubegui;

MetricTreeView::MetricTreeView( TreeModelInterface* model,
                                const QString&      tabLabel ) : TreeView( model, tabLabel )
{
    setIcon( QIcon( ":images/tree-icon.png" ) );
    this->setUniformRowHeights( false ); // items of metric tree may have different size, if special value view is used
}

/**
 * @brief MetricTreeView::selectionIsValid checks if newItem can be added to the current selection
 * @param oldItems previously selected tree items
 * @param newItem item that should become selected
 * @return true, if the item can be added to the current selection
 */
bool
MetricTreeView::selectionIsValid( QList<TreeItem*>& oldItems, TreeItem* newItem )
{
    if ( oldItems.size() > 0 )
    {
        if ( newItem->isDerivedMetric() || oldItems.at( 0 )->isDerivedMetric() )
        {
            QString message( tr( "Multiple selection of derived metrics is not possible." ) );
            Globals::setStatusMessage( message, Error );
            return false;
        }
    }

    bool    sameUnit = true;
    QString first_uom, second_uom;
    for ( int i = 0; i < oldItems.size() && sameUnit; i++ )
    {
        const TreeItem* oldItem = oldItems.at( i )->getTopLevelItem();
        first_uom  = QString::fromStdString( ( ( cube::Metric* )( oldItem->getCubeObject() ) )->get_uom() );
        second_uom = QString::fromStdString( ( ( cube::Metric* )( newItem->getCubeObject() ) )->get_uom() );
        if ( first_uom != second_uom )
        {
            sameUnit = false;
            break;
        }
    }
    if ( !sameUnit )
    {
        QString message( tr( "Multiple metric selection is possible only if the unit of measurement is compatible. " ) );
        message += QString( tr( "In this case \"%1\" is incompatible with \"%2\"" ) ).arg( first_uom, second_uom );
        Globals::setStatusMessage( message, Error );
        return false;
    }

    bool sameRoot = true;
    for ( int i = 0; i < oldItems.size() && sameRoot; i++ )
    {
        const TreeItem* first  = oldItems.at( i )->getTopLevelItem();
        const TreeItem* second = newItem->getTopLevelItem();
        if ( first != second )
        {
            sameRoot = false;
            break;
        }
    }

    if ( !sameRoot )
    {
        Globals::setStatusMessage( tr( "Be careful. Metrics with different roots might be incompatible for operation \"+\"." ), Warning );
    }

    return true;
}

void
MetricTreeView::selectionChanged( const QItemSelection& selected,
                                  const QItemSelection& deselected )
{
    Globals::getTabManager()->updateValueViews(); // selection of new metric may change the icon size of the tree items in all trees
    TreeView::selectionChanged( selected, deselected );
}

/**
 * creates context menu items for call trees
 */
void
MetricTreeView::fillContextMenu()
{
    addExpertContextMenuOptions();

    foreach( QAction * action, metricContextMenuHash.values() )     // enable all metric context menu actions
    {
        action->setEnabled( true );
    }

    contextMenu->addAction( contextMenuHash.value( TreeItemInfo ) );
    contextMenu->addAction( contextMenuHash.value( Documentation ) );

    contextMenu->addSeparator();
    contextMenu->addAction( contextMenuHash.value( ExpandMenu ) );

    contextMenu->addAction( contextMenuHash.value( FindItems ) );
    contextMenu->addAction( contextMenuHash.value( ClearFound ) );
    contextMenu->addAction( contextMenuHash.value( SortingMenu ) );
    contextMenu->addSeparator();
    contextMenu->addAction( contextMenuHash.value( CopyClipboard ) );
    contextMenu->addSeparator();
}

QString
MetricTreeView::getContextDescription( TreeItem* item )
{
    cube::Metric* _met    = static_cast<cube::Metric* >( item->getCubeObject() );
    std::string   content = tr( "Metric :  " ).toUtf8().data() + _met->get_descr() + "\n";

    // store the description text in the string "descr";
    // we get the description from the cube object "cubeObject" of the clicked item,
    // where we have to distinguish on its type
    std::string title;

    std::string uniq_name;
    std::string disp_name;
    std::string dtype;
    std::string uom;
    std::string val;
    std::string url;
    std::string kind;
    std::string descr;
    std::string cubepl_expression;
    std::string cubepl_init_expression;
    std::string cubepl_plus_expression;
    std::string cubepl_minus_expression;
    std::string cubepl_aggr_expression;
    title = tr( "No information" ).toUtf8().data();

    uniq_name               = _met->get_uniq_name();
    disp_name               = _met->get_disp_name();
    dtype                   = _met->get_dtype();
    uom                     = _met->get_uom();
    val                     = _met->get_val();
    url                     = _met->get_url();
    descr                   = _met->get_descr();
    kind                    = _met->get_metric_kind();
    cubepl_expression       = _met->get_expression();
    cubepl_init_expression  = _met->get_init_expression();
    cubepl_plus_expression  = _met->get_aggr_plus_expression();
    cubepl_minus_expression = _met->get_aggr_minus_expression();
    cubepl_aggr_expression  = _met->get_aggr_aggr_expression();


    content = content +
              tr( "Display name :  " ).toUtf8().data() + disp_name + "\n" +
              tr( "Unique name :  " ).toUtf8().data() + uniq_name + "\n" +
              tr( "Data type :  " ).toUtf8().data() + dtype + "\n" +
              tr( "Unit of measurement :  " ).toUtf8().data() + uom + "\n" +
              tr( "Value :  " ).toUtf8().data() + val + "\n" +
              tr( "URL :  " ).toUtf8().data() + url + "\n" +
              tr( "Kind of values :  " ).toUtf8().data() + kind;

    if ( !( cubepl_expression.compare( "" ) == 0 ) )
    {
        content = content + "\n" +  tr( "CubePL expression :  " ).toUtf8().data() +  cubepl_expression;
    }
    if ( !( cubepl_init_expression.compare( "" ) == 0 ) )
    {
        content = content + "\n" +  tr( "CubePL Init expression :  " ).toUtf8().data() +  cubepl_init_expression;
    }
    if ( !( cubepl_plus_expression.compare( "" ) == 0 ) )
    {
        content = content + "\n" +  tr( "CubePL Plus expression :  " ).toUtf8().data() +  cubepl_plus_expression;
    }
    if ( !( cubepl_minus_expression.compare( "" ) == 0 ) )
    {
        content = content + "\n" +  tr( "CubePL Minus expression :  " ).toUtf8().data() +  cubepl_minus_expression;
    }
    if ( !( cubepl_aggr_expression.compare( "" ) == 0 ) )
    {
        content = content + "\n" +  tr( "CubePL Aggr expression :  " ).toUtf8().data() +  cubepl_aggr_expression;
    }

    content = content + "\n\n" + ( ( _met->isConvertible() ) ? tr( "Convertible to data" ).toUtf8().data() : tr( "Non convertible to data" ).toUtf8().data() );
    content = content + "\n" + ( ( _met->isCacheable() ) ? tr( "Cacheable" ).toUtf8().data() : tr( "Non cacheable" ).toUtf8().data() );
    content = content + "\n" + ( ( _met->isGhost() ) ? tr( "Ghost metric" ).toUtf8().data() : tr( "Normal metric" ).toUtf8().data() );
    content = content + "\n" + ( ( _met->isRowWise() ) ? tr( "Rowwise metric" ).toUtf8().data() : tr( "Elementswise metric" ).toUtf8().data() );

    const std::map<std::string, std::string>& attrs = _met->get_attrs();
    content = content + "\n" +  tr( "Attributes :" ).toUtf8().data();
    for ( std::map<std::string, std::string>::const_iterator iter = attrs.begin(); iter != attrs.end(); ++iter )
    {
        content = content + "\n" +  iter->first + " : " + iter->second;
    }


    return QString( content.c_str() );
}
