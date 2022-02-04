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


#ifndef TREESTATISTICS_H
#define TREESTATISTICS_H
#include <QDebug>

namespace cubegui
{
class Tree;

class TreeStatistics
{
public:
    TreeStatistics( Tree* tree );

    double
    mean() const;
    double
    varianceSqrt() const;
    double
    minValue() const;
    double
    maxValue() const;
    double
    selectedValue() const;
    double
    minAbsValue() const;
    double
    maxAbsValue() const;
    double
    absValue() const;

    bool
    isValid() const;
    bool
    isAbsoluteValueModus() const;
    bool
    hasUserDefinedMinMaxValues() const;
    bool
    isInteger() const;

    void
    update();
    void
    invalidate();

private:
    Tree* tree;

    double mean_;
    double varianceSqrt_;
    // the currently displayed minimal/maximal values and the selected value
    double minValue_;
    double maxValue_;
    double selectedValue_;
    // the currently displayed absolute minimal/maximal values and the selected absolute value
    double minAbsValue_;
    double maxAbsValue_;
    double absValue_;

    bool valid;
    bool absoluteMode;
    bool userDefinedMinMax_;
    bool isInteger_;
};
}

#endif // TREESTATISTICS_H
