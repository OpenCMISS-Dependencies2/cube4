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





#ifndef _VALUEWIDGET_H
#define _VALUEWIDGET_H

#include <QWidget>
#include <cmath>
#include "TreeStatistics.h"

namespace cubegui
{
class ColorWidget;

// the ValueWidget class is used to display more precise values for the
// selected items;
// currently three instances are created and displayed
// below the metric, call, and system tabs
//
// the value widget displays in a first row the minimal and maximal
// values for coloring, and in between the current selected value;
// if several items are selected, the value is the sum, i.e, the value
// that is used for value computations on the right hand side
//
// the second row contains the same information but with absolute values;
// displayed if the current value modus is not the absolute value modus
//
// the last row contains a color gradient fill upto the color of the
// selected value; undefined values are represented by a gray filling
// (no data loaded or the selected value is undefined)

class ValueWidget : public QWidget
{
    Q_OBJECT

public:

    ValueWidget( QWidget* parent = NULL );
    ~ValueWidget();

    void
    clear();

    void
    setStatistics( TreeStatistics* s );

    /** update the value widget with the default static values */
    void
    update();

    /** update the value widget with the given parameters */
    void
    update( double value,
            double minValue,
            double maxValue,
            double absValue = std::nan( "" ),
            double minAbsValue = std::nan( "" ),
            double maxAbsValue = std::nan( "" ),
            double mean = std::nan( "" ),
            double varianceSqrt = std::nan( "" ) );

protected:
    void
    paintEvent( QPaintEvent* );

private:
    TreeStatistics* statistics;

    QString minValueString;
    QString maxValueString;
    QString selectedValueString;
    QString minAbsValueString;
    QString maxAbsValueString;
    QString absValueString;
    double  percent; // value of the color gradient
    bool    useDefaultStatistics;

    void
    createLabel( double value,
                 double minValue,
                 double maxValue,
                 double absValue = std::nan( "" ),
                 double minAbsValue = std::nan( "" ),
                 double maxAbsValue = std::nan( "" ),
                 double mean = std::nan( "" ),
                 double varianceSqrt = std::nan( "" ) );
};
}
#endif
