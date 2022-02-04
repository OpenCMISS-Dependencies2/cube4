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


#ifndef ProgressBar_H
#define ProgressBar_H

#include <QWidget>
#include <QColor>

namespace advisor
{
/**
 */
class Bar : public QWidget
{
public:
    Bar();

    void
    paintEvent( QPaintEvent* );

    /** minimum bar value, default is 0 */
    void
    setMinimum( double min );

    /** maximum bar value, default is 100 */
    void
    setMaximum( double max );

    /** @param val average value */
    void
    setValue( double val );

    /** @param min minimum value
     *  @param avg average value
     *  @param max maximum value */
    void
    setValues( double min,
               double avg,
               double max );

    /** sets fill color */
    void
    setColor( QColor color );

    double
    getMinValue() const
    {
        return min;
    }

    double
    getAvgValue() const
    {
        return value;
    }

    double
    getMaxValue() const
    {
        return max;
    }

    bool
    isSingleValue() const
    {
        return singleValue;
    }

protected:

    void
    mousePressEvent( QMouseEvent* event ) override;


private:
    double maxValue, minValue; // bar values go from minValue to maxValue
    double value, min, max;
    int    padding;            // free space above and below the bar
    QColor color;
    bool   singleValue;
};
}
#endif
