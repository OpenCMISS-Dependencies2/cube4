/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2016                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2009-2015                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef CUBEDATAITEM_H
#define CUBEDATAITEM_H


#include <QVector>
#include <QColor>
#include "GraphWidgetGlobals.h"

#include <Cube.h>

class CubeDataItem
{
public:
    CubeDataItem();
    void setColor(QColor getColor);
    void setStyle(plotWidget::Style getStyle);
    void setScaled(bool isScaled);
    void setMetricIndex(int index);
    void setScaleMinValue(double minScaleValue);
    void setScaleMaxValue(double maxScaleValue);
    void setData(QVector<double> getData);
    void setVisible(bool isVisible);
    void setZOrder(int getZOrder);
    void setMathOp(plotWidget::MathOp mathOp);
    void setName(QString name);
    void setMaxValue(double maxValue);
    void setMinValue(double minValue);
    void setfirstItrID(unsigned int tempfirstItrID);
    void setcalcType( cube::CalculationFlavour tempcalcType );
    static bool cubeDataItemOrderComparator(const CubeDataItem& firstElement, const CubeDataItem&  secondElement);


    QColor getColor() const;
    plotWidget::Style getStyle() const;
    bool isScaled() const;
    int getMetricIndex() const;
    double getScaleMinValue() const;
    double getScaleMaxValue() const;
    QVector<double>& getData();
    const QVector<double> &getData() const;
    bool isVisible() const;
    int getZOrder() const;
    plotWidget::MathOp getMathOp() const;
    QString getName() const;
    QString toString() const;
    double getMaxValue() const;
    double getMinValue() const;
    unsigned int    getfirstItrID() const;
    cube::CalculationFlavour getcalcType() const;
private:
    QColor color;
    plotWidget::Style style;
    bool _isScaled;
    int metricIndex;
    double scaleMinValue;
    double scaleMaxValue;
    double minValue;
    double maxValue;
    QVector<double> data;
    bool _isVisible;
    int zOrder;
    plotWidget::MathOp mathOp;
    QString name;
    unsigned int firstItrID;
    cube::CalculationFlavour calcType;
};


#endif // CUBEDATAITEM_H
