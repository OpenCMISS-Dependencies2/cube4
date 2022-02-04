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

#include <assert.h>
#include <math.h>
#include <QDebug>
#include <ctype.h>

#include "CubeRegion.h"
#include "CubeVertex.h"
#include "CubeValues.h"
#include "CubeCnode.h"

#include "PluginManager.h"
#include "Globals.h"
#include "Tree.h"
#include "TreeView.h"
#include "TreeItem.h"
#include "AggregatedTreeItem.h"
#include "MetricTree.h"
#include "TreeItemMarker.h"
#include "ValueView.h"
#include "DefaultValueView.h"
#include "TreeConfig.h"

using namespace cubegui;

TreeItem::TreeItem(   Tree*          tree,
                      const QString& name,
                      TreeItemType   type,
                      cube::Vertex*  cube )
{
    this->name        = name;
    this->label       = name;
    this->type        = type;
    this->cubeObject  = cube;
    this->tree        = tree;
    this->displayName = "";
    ownValue_adv      = nullptr;
    totalValue_adv    = nullptr;

    rootItem   = NULL;
    parentItem = 0;
    depth      = 0;

    singleValue = true;
    expanded    = false;
    hidden      = false;
    selected    = false;
    toCalculate = !tree->isManualCalculation();
    invalidate();
}

TreeItem::~TreeItem()
{
    if ( ownValue_adv != NULL )
    {
        delete ownValue_adv;
    }
    if ( totalValue_adv != NULL )
    {
        delete totalValue_adv;
    }
}

int
TreeItem::getDepth() const
{
    return depth;
}

void
TreeItem::invalidate()
{
    statusExclusive = INVALID;
    statusInclusive = INVALID;
    currentValue    = false;

    delete ownValue_adv;
    ownValue_adv = nullptr;
    delete totalValue_adv;
    totalValue_adv = nullptr;

    totalValue = 0.0;
    ownValue   = 0.0;
    minValue   = 0.0;
    maxValue   = 0.0;

    textExpanded   = "";
    textCollapsed  = "";
    colorExpanded  = Qt::white;
    colorCollapsed = Qt::white;

    // the minimal and maximal peer values for system tree items
    minValue = 0.0;
    maxValue = 0.0;

    if ( displayName.isEmpty() )
    {
        displayName = TreeConfig::getInstance()->createDisplayName( this );
    }
    displayValue = nan( "" );
    label        = QString( " - " ).append( displayName );
}

void
TreeItem::invalidateLabel()
{
    if ( getCalculationStatus() == READY )
    {
        setCalculationStatus( CALCULATED );
    }
    if ( displayName.isEmpty() )
    {
        displayName = TreeConfig::getInstance()->createDisplayName( this );
    }
    label = QString( " - " ).append( displayName );

    displayValue   = nan( "" );
    currentValue   = nan( "" );
    colorExpanded  = Qt::white;
    colorCollapsed = Qt::white;
}

Tree*
TreeItem::getTree() const
{
    return tree;
}

cube::Vertex*
TreeItem::getCubeObject() const
{
    return cubeObject;
}


TreeItem*
TreeItem::child( int row ) const
{
    return children.value( row );
}

void
TreeItem::addChild( TreeItem* item )
{
    children.append( item );

    item->parentItem = this;
    item->setDepth( this->getDepth() + 1 );
}

bool
TreeItem::isHidden() const
{
    return this->hidden;
}

void
TreeItem::setHidden( bool hidden )
{
    this->hidden = hidden;
}

bool
TreeItem::isSelected() const
{
    return this->selected;
}


const QColor&
TreeItem::getColor() const
{
    return expanded ? colorExpanded : colorCollapsed;
}


bool
TreeItem::isIntegerType() const
{
    if ( tree->getValueModus() != ABSOLUTE_VALUES )
    {
        return false;
    }
    if ( tree->getType() == METRIC )
    {   // each item has different type
        return ( static_cast<cube::Metric*> ( cubeObject ) )->isIntegerType();
    }
    else
    {   //  all tree items have the same type
        return tree->hasIntegerType();
    }
}

bool
TreeItem::isTopLevelItem() const
{
    return !( getParent() && getParent()->getParent() );
}

bool
TreeItem::isDerivedMetric() const
{
    bool isDerived = false;

    cube::Metric* metric = dynamic_cast<cube::Metric*> ( this->getCubeObject() );
    if ( metric )
    {
        cube::TypeOfMetric metricType = metric->get_type_of_metric();
        isDerived = metricType == cube::CUBE_METRIC_POSTDERIVED || metricType == cube::CUBE_METRIC_PREDERIVED_EXCLUSIVE || metricType == cube::CUBE_METRIC_PREDERIVED_INCLUSIVE;
    }

    return isDerived;
}

const TreeItem*
TreeItem::getTopLevelItem() const
{
    const TreeItem* item = this;
    while ( !item->isTopLevelItem() ) // get top level parent item
    {
        item = item->getParent();
    }
    return item;
}


void
TreeItem::getSourceInfo( QString& filename, int& startLine, int& endLine ) const
{
    cube::Region* region = 0;
    startLine = -1;
    endLine   = -1;
    filename.clear();

    if ( !getCubeObject() )
    {
        return;
    }

    if ( type == CALLITEM )
    {
        cube::Cnode* cnode = static_cast<cube::Cnode*>( getCubeObject() );
        region = static_cast<cube::Region*>( cnode->get_callee() );
    }
    else if ( type == REGIONITEM )
    {
        region = static_cast<cube::Region*>( getCubeObject() );
    }

    if ( region )
    {
        std::string str = region->get_mod();
        filename  = QString::fromStdString( region->get_mod() ).trimmed();
        startLine = region->get_begn_ln();
        endLine   = region->get_end_ln();
    }
    if ( ( filename == "MPI" ) || ( filename == "INTERNAL" ) || ( filename ==  "OMP" )  || ( filename == "PTHREAD" ) ||
         ( filename == "CUDA" ) || ( filename ==  "OPENCL" ) )
    {
        filename = "";
    }
}

void
TreeItem::setMarker( const TreeItemMarker* marker, bool isDependencyMarker, bool markParents )
{
    if ( isDependencyMarker )
    {
        if ( !dependencyMarkerList.contains( marker ) )
        {
            dependencyMarkerList.append( marker );
        }
    }
    else
    {
        markerList.append( marker );
    }

    if ( markParents )
    {
        TreeItem* parent = this->getParent();
        while ( parent )
        {
            if ( !parent->parentMarkerList.contains( marker ) )
            {
                parent->parentMarkerList.append( marker );
            }
            parent = parent->getParent();
        }
    }
}

void
TreeItem::removeMarker( const TreeItemMarker* marker )
{
    bool found = markerList.removeOne( marker );
    if ( found )
    {
        TreeItem* parent = this->getParent();
        while ( parent )
        {
            found  = parent->parentMarkerList.removeOne( marker );
            parent = found ? parent->getParent() : 0;
        }
    }

    dependencyMarkerList.removeOne( marker );
}

const QList<const TreeItemMarker*>&
TreeItem::getMarkerList() const
{
    return markerList;
}

double
TreeItem::getAbsoluteValue() const
{
    return expanded ? ownValue : totalValue;
}

double
TreeItem::getOwnValue() const
{
    return ownValue;
}

const cube::Value*
TreeItem::getOwnValueObject() const
{
    const std::lock_guard<std::mutex> lockGuard( cmutex );
    return ownValue_adv;
}

double
TreeItem::getTotalValue() const
{
    return totalValue;
}

const cube::Value*
TreeItem::getTotalValueObject() const
{
    const std::lock_guard<std::mutex> lockGuard( cmutex );
    return totalValue_adv;
}

cube::Value*
TreeItem::getValueObject() const
{
    const std::lock_guard<std::mutex> lockGuard( cmutex );
    return expanded ? this->ownValue_adv : this->totalValue_adv;
}

/* functions for use in the corresponding model TreeModel */

TreeItem*
TreeItem::getParent() const
{
    return parentItem;
}

int
TreeItem::row() const
{
    if ( parentItem )
    {
        return parentItem->children.indexOf( const_cast<TreeItem*>( this ) );
    }

    return 0;
}

TreeItemType
TreeItem::getType() const
{
    return type;
}

// private function for use in trees after new values have been calculated
void
TreeItem::setValues( cube::Value* total, cube::Value* own )
{
    setInclusiveValue( total );
    setExclusiveValue( own );
}

void
TreeItem::setValue( cube::Value* value )
{
    if ( isExpanded() )
    {
        setExclusiveValue( value );
    }
    else
    {
        setInclusiveValue( value );
    }
}

void
TreeItem::setInclusiveValue( cube::Value* value )
{
    const std::lock_guard<std::mutex> lockGuard( cmutex );
    if ( totalValue_adv != value && totalValue_adv != NULL )
    {
        delete totalValue_adv;
    }
    totalValue_adv  = value;
    totalValue      = ( totalValue_adv != NULL && !( totalValue_adv->isZero() ) ) ? Globals::getValueView( totalValue_adv->myDataType() )->getDoubleValue( totalValue_adv ) : 0.;
    statusInclusive = CALCULATED;
    singleValue     = ( totalValue_adv != NULL ) ? totalValue_adv->singleValue() : true;
}

void
TreeItem::setExclusiveValue( cube::Value* value )
{
    const std::lock_guard<std::mutex> lockGuard( cmutex );
    if ( ownValue_adv != value && ownValue_adv != NULL )
    {
        delete ownValue_adv;
    }
    ownValue_adv    = value;
    ownValue        = ( ownValue_adv != NULL && !( ownValue_adv->isZero() ) ) ? Globals::getValueView( ownValue_adv->myDataType() )->getDoubleValue( ownValue_adv ) : 0.;
    statusExclusive = CALCULATED;
    singleValue     = ( ownValue_adv != NULL ) ? ownValue_adv->singleValue() : true;
}

bool
TreeItem::isLeaf() const
{
    return getChildren().count() == 0;
}

bool
TreeItem::isAggregatedLoopItem() const
{
    return this->cubeObject == 0;
}

bool
TreeItem::isAggregatedRootItem() const
{
    return false;
}

QList<TreeItem*>
TreeItem::getLeafs() const
{
    QList<TreeItem*> leafs;

    QList<TreeItem*> children = getChildren();
    while ( !children.isEmpty() )
    {
        TreeItem* item = children.last();
        children.removeLast();
        if ( item->getChildren().size() == 0 )
        {
            leafs.append( item );
        }
        else
        {
            foreach( TreeItem * child, item->getChildren() )
            {
                children.append( child );
            }
        }
    }

    return leafs;
}

// ======== private ===================================================================
//

void
TreeItem::removeChild( TreeItem* item )
{
    assert( item->getParent() == this );

    children.removeOne( item );
    item->parentItem = NULL;
}

/**
 * @brief returns the value of this item adapted to the given value modus
 */
double
TreeItem::calculateValue( ValueModus valueModus, bool& ok, bool exclusive ) const
{
    ok = true;
    cube::Value* valueObj = ( exclusive ? this->ownValue_adv : this->totalValue_adv );

    if ( !valueObj )
    {
        ok = false;
        return 0.;
    }

    // if the metric tab is on the right, then the value is undefined
    DisplayType        currentDisplay = getTree()->getType();
    QList<DisplayType> order          = Globals::getTabManager()->getOrder();
    foreach( DisplayType display, order )
    {
        if ( display == METRIC ) // metric is on the left or current is metric
        {
            break;
        }
        else if ( display == currentDisplay ) // metric is on the right of current one
        {
            ok = false;
            return 0;
        }
    }

    // get the absolute value
    double value = Globals::getValueView( getValueObject()->myDataType() )->getDoubleValue( getValueObject() );

    if ( value != 0. && valueObj->isZero() )
    {
        /* For some metrics (e.g. metrics which are building a minimum) no valid exclusive value exists.
         * The zero value for a minimum metrics is the maximum negative value. In this case, the value
         * should not be displayed and be marked as invalid.
         */
        ok = false;
        return 0.;
    }

    double roundThreshold = Globals::getRoundThreshold( FORMAT_TREES );

    if ( tree->getType() == SYSTEM && exclusive && children.size() > 0 )
    {
        ok = false; // no exclusive values for system tree
        return 0;
    }

    // compute the value in the current modus
    if ( valueModus != ABSOLUTE_VALUES )
    {
        double referenceValue = 0.0;

        if ( valueModus == OWNROOT_VALUES )
        {
            referenceValue = this->rootItem->totalValue;
        }
        else if ( valueModus == PEER_VALUES )
        {
            referenceValue = this->maxValue;
        }
        else if ( type == METRICITEM && valueModus == EXTERNAL_VALUES )
        {
            MetricTree*     metricTree = static_cast<MetricTree*> ( tree );
            const TreeItem* item       = getTopLevelItem();
            cube::Metric*   metric     = static_cast<cube::Metric* > ( item->getCubeObject() );
            QString         metricName = QString::fromStdString( metric->get_uniq_name() );

            referenceValue = metricTree->getExternalReferenceValue( metricName );
        }
        else // only one reference value for the complete tree
        {
            referenceValue = tree->getValueModusReferenceValue();
        }

        if ( value <= roundThreshold && value >= -roundThreshold ) // don't calculate percentage for values near zero
        {
            value = 0.0;
        }

        if ( ok )
        {
            if ( valueModus != PEERDIST_VALUES )
            {
                if ( referenceValue == 0.0 )
                {
                    if ( value != 0.0 )
                    {
                        ok = false;
                    }
                }
                else
                {
                    value = 100.0 * value / referenceValue;
                }
            }
            else // peer distribution
            {
                double referenceValue1 = this->maxValue;
                double referenceValue2 = this->minValue;
                double diff            = ( referenceValue1 - referenceValue2 );
                if ( diff == 0.0 )
                {
                    if ( value - referenceValue2 != 0 )
                    {
                        ok = false;
                    }
                    else
                    {
                        value = 0.0;
                    }
                }
                else
                {
                    if ( referenceValue2 <= roundThreshold && referenceValue2 >= -roundThreshold )
                    {   // don't calculate percentage for values near zero
                        referenceValue2 = 0.0;
                    }
                    value = 100.0 * ( value - referenceValue2 ) / diff;
                }
            }
        }
    }

    // round to 0.0 if very close to it
    if ( value <= roundThreshold && value >= -roundThreshold )
    {
        value = 0.0;
    }

    return value;
}

/*
 * @param valueOk true, if the calculation of the value succeeded
 * @param value the tree items's value in the current value modus. If expanded, the values of all hidden children are added to the item's value
 * @param hiddenValueOk true, if the calculation of the value and the hidden values succeeded
 * @param hiddenChildrenValue the sum of the values of all hidden children
 * @param hiddenChildren the number of hidden child items
 */
void
TreeItem::calculateValuePlusHidden( bool& valueOk, double& value,
                                    bool& hiddenValueOk, double& hiddenChildrenValue,
                                    int& hiddenChildren, ValueModus modus )
{
    valueOk        = false;
    hiddenValueOk  = false;
    hiddenChildren = 0;

    value = calculateValue( modus, valueOk, isExpanded() );

    if ( tree->isHidingEnabled() )
    {
        if ( tree->getType() == SYSTEM && isExpanded() )
        {
            // Expanded system tree items have invalid (empty) values. If these items get hidden, set valueOk to true to
            // enable calculation of its hidden children
            valueOk = true;
        }

        hiddenChildrenValue = 0.0;
        double childValue = 0.0;

        if ( valueOk && expanded )
        {
            hiddenValueOk = true;
            // add up hidden children's value to the own value
            for ( int i = 0; i < this->getChildren().size() && hiddenValueOk; i++ )
            {
                if ( this->child( i )->isHidden() )
                {
                    hiddenChildren++;
                    if ( hiddenValueOk )
                    {
                        childValue = child( i )->calculateValue( tree->getValueModus(), hiddenValueOk, child( i )->isExpanded() );
                    }
                    if ( hiddenValueOk )
                    {
                        hiddenChildrenValue += childValue;
                    }
                }
            }
            if ( hiddenValueOk )
            {
                value += hiddenChildrenValue;
            }
        }
    }
}

static const QColor unfinishedColor( 220, 220, 220 );

/*
 * @brief updateItem has to be called if the value of an tree item has been changed.
 * It sets the item label and its color.
 * @param calculationFinished true, if all tree values and maximum/minimum have been calculated
 */
void
TreeItem::updateItem( bool maximumIsCalculated )
{
    if ( !maximumIsCalculated && tree->getValueModus() != ABSOLUTE_VALUES )
    {
        return; // only show progress in absolute value modus
    }

    // cubelib has finished calculation -> update item label and value
    if ( getCalculationStatus() == CALCULATED )
    {
        colorExpanded  = unfinishedColor;
        colorCollapsed = unfinishedColor;

        if ( tree->isHidingEnabled() && isExpanded() ) // children of expanded items may be hidden -> wait for children to be calculated
        {
            foreach( TreeItem * child, children )
            {
                if ( !child->isCalculated() )
                {
                    return;
                }
            }
        }
        setCalculationStatus( READY );

        double value;                // value including values of hidden children
        double hiddenChildrenValue;  // sum of the values of all hidden children
        int    hiddenChildren;
        bool   valueOk, hiddenValueOk;

        calculateValuePlusHidden( valueOk, value, hiddenValueOk, hiddenChildrenValue, hiddenChildren, tree->getValueModus() );
        this->currentValue = value;
        this->currentValid = valueOk;

        // ---- sets the label of the item which will be displayed in the tree view
        QString text = "";
        if ( !valueOk )
        {
            text.append( "-" );
        }
        else // value exists
        {
            if ( singleValue )
            {
                text.append( Globals::formatNumber( value, isIntegerType(), FORMAT_TREES, this ) );
            }
            else
            {
                text.append( Globals::getValueView( getValueObject()->myDataType() )->toString( this ) );
            }
        }
        text.append( " " );

        text.append( displayName );

        // add units to the labels of the top level items of the metric tree
        if ( isTopLevelItem() && type == METRICITEM && tree->getValueModus() == ABSOLUTE_VALUES )
        {
            cube::Metric* met = static_cast<cube::Metric*> ( this->cubeObject );
            QString       unit( met->get_uom().c_str() );

            if ( met->get_parent() == NULL )
            {
                if ( unit.size() > 0 )
                {
                    text.append( " (" );
                    text.append( unit ); // add the unit of measurement
                    text.append( ")" );
                }
            }
        }

        // ----- write info about hidden children's value
        if ( valueOk && hiddenChildren > 0 )
        {
            text.append( " (" );
            if ( hiddenValueOk )
            {
                double eps     = Globals::getRoundThreshold( FORMAT_TREES );
                double percent = value > eps ? 100.0 * hiddenChildrenValue / value : 0;
                text.append( Globals::formatNumber( percent, false, FORMAT_TREES, this ) );
                text.append( "%" );
            }
            else
            {
                text.append( "-" );
            }
            text.append( ")" );
        }

        this->label        = text;
        this->displayValue = value;

        if ( !valueOk )
        {
            displayValue = nan( "" );
        }
    } // end if CALCULATED

    if ( maximumIsCalculated )
    {
        int  hiddenChildren = 0;
        bool hiddenValueOk  = true;  // todo

        // set the colors for the item
        QColor color = Globals::getColor( 0, 0, 0 );
        if ( std::isnan( displayValue ) || ( hiddenChildren > 0 && !hiddenValueOk ) )
        {
            color = Globals::getColor( 0, 0, 0 );
        }
        else
        {
            color = calculateColor( displayValue );
        }

        if ( expanded )
        {
            colorExpanded = color;
        }
        else
        {
            colorCollapsed = color;
        }
    }
}
// end of displayItem()

QColor
TreeItem::calculateColor( double value ) const
{
    QColor color;
    if ( !tree->hasUserDefinedMinMaxValues() )
    {
        double min = 0.0;
        double max = 0.0;
        if ( tree->getValueModus() == ABSOLUTE_VALUES )
        {
            max = tree->getMaxValue( this );
        }
        else
        {
            max = 100.0;
        }
        // in the case, calculated min is greater than max in its ablut value, we swap them here
        if ( fabs( min ) > fabs( max ) )
        {
            double tmp = min;
            min = max;
            max = tmp;
        }

        // coloring is only according to the absolut values of the min and max.
        color = Globals::getColor( fabs( value ), fabs( min ), fabs( max ) );
    }
    else // userDefinedMinMaxValues
    {
        // coloring is only according to the absolut values of the min and max.
        color = Globals::getColor( fabs( value ),
                                   fabs( tree->getUserDefinedMinValue() ),
                                   fabs( tree->getUserDefinedMaxValue() ) );
    }

    return color;
}

void
TreeItem::setExpanded( bool expand )
{
    tree->expandItem( this, expand );
}

void
TreeItem::select( bool addToSelection )
{
    tree->selectItem( this, addToSelection );
}

void
TreeItem::deselect()
{
    tree->deselectItem( this );
}

// ---- methods which are called by the view, if user has made selections

void
TreeItem::setExpandedStatus( bool expanded, bool recursive )
{
    if ( !isLeaf() )
    {
        this->expanded  = expanded;
        statusExclusive = statusExclusive == READY ? CALCULATED : statusExclusive;
        statusInclusive = statusInclusive == READY ? CALCULATED : statusInclusive;
    }
    if ( recursive )
    {
        foreach( TreeItem * item, this->getChildren() )
        {
            if ( !item->isLeaf() )
            {
                item->setExpandedStatus( expanded, true );
            }
        }
    }
}

void
TreeItem::setSelectionStatus( bool selected )
{
    this->selected = selected;
}


// sets the depth of this items and all its children recursively
void
TreeItem::setDepth( int value )
{
    this->depth = value;

    if ( children.size() > 0 )
    {
        QList<TreeItem*> list;
        list.append( this );
        while ( !list.isEmpty() )
        {
            TreeItem* item = list.takeLast();
            foreach( TreeItem * child, item->getChildren() )
            {
                child->depth = item->depth + 1;
                list.append( child );
            }
        }
    }
}

DisplayType
TreeItem::getDisplayType() const
{
    return tree->getType();
}

TreeType
TreeItem::getTreeType() const
{
    return tree->getTreeType();
}

QVariant
TreeItem::convertToQVariant()
{
    QList<QVariant> list;

    TreeItem* p = this;
    while ( p->parentItem )
    {
        list.insert( 0, p->parentItem->children.indexOf( const_cast<TreeItem*>( p ) ) );
        p = p->parentItem;
    }

    list.insert( 0, tree->getTreeType() );
    return QVariant( list );
}

void
TreeItem::setCalculationEnabled( bool enabled )
{
    toCalculate = enabled;
}

TreeItem*
TreeItem::convertQVariantToTreeItem( QVariant entry )
{
    QList<QVariant> list = entry.toList();

    TreeType type = ( TreeType )list.takeFirst().toInt();
    Tree*    tree = Globals::getTabManager()->getTree( type );

    TreeItem* item = tree->getRootItem();
    foreach( QVariant pos, list )
    {
        if ( pos.toInt() < item->children.size() )
        {
            item = item->children.at( pos.toInt() );
        }
        else
        {
            return NULL; // position not valid
        }
    }
    return item;
}
