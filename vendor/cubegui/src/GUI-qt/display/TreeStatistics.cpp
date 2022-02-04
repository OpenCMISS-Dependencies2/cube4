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

#include "TreeStatistics.h"
#include "Tree.h"

using namespace cubegui;

TreeStatistics::TreeStatistics( Tree* tree )
{
    this->tree = tree;

    valid              = true;
    absoluteMode       = true;
    userDefinedMinMax_ = false;
    isInteger_         = false;

    update();
}

void
TreeStatistics::update()
{
    minAbsValue_   = 0;
    maxAbsValue_   = 0;
    minValue_      = 0;
    maxValue_      = 0;
    absValue_      = 0;
    mean_          = 0;
    varianceSqrt_  = 0;
    selectedValue_ = 0;
    absValue_      = 0;

    absoluteMode = tree->getValueModus() == ABSOLUTE_VALUES;

    bool                    selValid      = false;
    const QList<TreeItem*>& selectionList = tree->getSelectionList();
    if ( selectionList.size() > 0 )
    {
        double value;

        TreeItem* selection =  tree->getLastSelection();

        foreach( TreeItem * selectedItem, selectionList )
        {
            if ( selectedItem->isHidden() )
            {
                continue; // value already included in calculateValuePlusHidden
            }

            bool   valueOk, hiddenValueOk;
            int    hiddenChildren;
            double hiddenChildrenValue;
            {
                selectedItem->calculateValuePlusHidden( valueOk, value, hiddenValueOk, hiddenChildrenValue,
                                                        hiddenChildren, tree->getValueModus() );
                if ( valueOk )
                {
                    selectedValue_ += value;
                    selValid        = true;
                }
                else
                {
                    selectedValue_ = nan( "" );
                    break;
                }
            }
            if ( valueOk && !absoluteMode ) // additionally calculate absolute values
            {
                selectedItem->calculateValuePlusHidden( valueOk, value, hiddenValueOk, hiddenChildrenValue,
                                                        hiddenChildren, ABSOLUTE_VALUES );
                if ( valueOk )
                {
                    absValue_ += value;
                    selValid   = true;
                }
                else
                {
                    absValue_ = nan( "" );
                    break;
                }
            }
        }

        double maxTreeValue = tree->getMaxValue( selection );
        if ( tree->hasUserDefinedMinMaxValues() )
        {
            minValue_ = tree->getUserDefinedMinValue();
            maxValue_ = tree->getUserDefinedMaxValue();
        }
        else
        {
            minValue_ = 0.0;
            maxValue_ = absoluteMode ? maxTreeValue : 100.0;
        }

        // absolute values
        minAbsValue_ = 0;
        maxAbsValue_ = maxTreeValue;

        isInteger_ = selection->isIntegerType();
    } // end selection exist

    userDefinedMinMax_ = tree->hasUserDefinedMinMaxValues();

    // mean and variance are only displayed in system tree, if no thread or process is selected in
    // percent value modus
    if ( !selValid )
    {
        double   tmp = 0.;
        bool     ok  = false;
        unsigned N   = 0;
        foreach( TreeItem * item, tree->getItems() )
        {
            if ( item->getChildren().size() == 0 ) // get sum of all leafs
            {
                tmp    = item->calculateValue( ABSOLUTE_VALUES, ok, false );
                mean_ += tmp;
                N++;
            }
        }
        mean_ = ( N != 0 ) ? mean_ / N : 0.;

        double variance = 0.;
        foreach( TreeItem * item, tree->getItems() )
        {
            if ( item->getChildren().size() == 0 )
            {
                tmp       = item->calculateValue( ABSOLUTE_VALUES, ok, false );
                variance += ( mean_ - tmp ) * ( mean_ - tmp );
                N++;
            }
        }
        varianceSqrt_ = ( N != 0 ) ? sqrt( variance / N ) : 0.;
    }
    else
    {
        mean_         = nan( "" );
        varianceSqrt_ = nan( "" );
    }
    valid = true;
}

void
TreeStatistics::invalidate()
{
    valid = false;
}

bool
TreeStatistics::isValid() const
{
    return valid;
}

bool
TreeStatistics::isAbsoluteValueModus() const
{
    return absoluteMode;
}

bool
TreeStatistics::hasUserDefinedMinMaxValues() const
{
    return userDefinedMinMax_;
}

bool
TreeStatistics::isInteger() const
{
    return isInteger_;
}  // end TreeStatistics::update()

double
TreeStatistics::mean() const
{
    return mean_;
}

double
TreeStatistics::varianceSqrt() const
{
    return varianceSqrt_;
}

double
TreeStatistics::minValue() const
{
    return minValue_;
}

double
TreeStatistics::maxValue() const
{
    return maxValue_;
}

double
TreeStatistics::selectedValue() const
{
    return selectedValue_;
}

double
TreeStatistics::minAbsValue() const
{
    return minAbsValue_;
}

double
TreeStatistics::maxAbsValue() const
{
    return maxAbsValue_;
}

double
TreeStatistics::absValue() const
{
    return absValue_;
}
