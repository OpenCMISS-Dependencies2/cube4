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
#include "CallTree.h"

using namespace cube;
using namespace cubegui;


CallTree::~CallTree()
{
    // delete cached values
    for ( QPair<cube::Value*, cube::Value*>& values : metricValueHash.values() )
    {
        delete values.first;
        delete values.second;
    }
}

void
CallTree::saveMetricValues( Tree* metricTree )
{
    for ( TreeItem* item : metricTree->getItems() )
    {
        Metric* metric = static_cast<Metric*> ( item->getCubeObject() );
        if ( metric->isCacheable() )
        {
            cube::Value* total = item->totalValue_adv ? item->totalValue_adv->copy() : nullptr;
            cube::Value* own   = item->ownValue_adv ? item->ownValue_adv->copy() : nullptr;
            metricValueHash.insert( item, qMakePair( total, own ) );
        }
    }
}

void
CallTree::restoreMetricValues( Tree* metric )
{
    for ( TreeItem* item : metric->getItems() )
    {
        item->invalidate();
        if ( metricValueHash.contains( item ) )
        {
            const QPair<cube::Value*, cube::Value*>& values = metricValueHash.value( item );
            if ( values.first )
            {
                item->setInclusiveValue( values.first );
                item->setCalculationStatus( item->CALCULATED );
            }
            if ( values.second )
            {
                item->setExclusiveValue( values.second );
                item->setCalculationStatus( item->CALCULATED );
            }
        }
    }
    metricValueHash.clear(); // don't delete cached values in destructor because they will be deleted by metric tree destructor
}
