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


#ifndef ABSTRACTRULER_H
#define ABSTRACTRULER_H

#include <QWidget>
#include <QFont>
#include <QRect>
#include <QMap>
#include <QColor>
#include "GraphWidgetGlobals.h"

class AbstractRuler : public QWidget
{
    Q_OBJECT
public:
    explicit AbstractRuler(QWidget *parent = 0);
    virtual ~AbstractRuler();
    static const QFont RULER_TITLE_FONT;
    static const QFont MAJOR_NOTCH_FONT;
    static const QFont MINOR_NOTCH_FONT;
    static const int LINE_PADDING;
    static const QColor RULER_TITLE_COLOR;

    void setMajorNotchesIndexAndLocationF(QMap<double, int> newValue);
    void setMinorNotchesIndexAndLocationF(QMap<double, int> newValue);
    void setMajorNotchesIndexAndLocation(QMap<int, int> newValue);
    void setMinorNotchesIndexAndLocation(QMap<int, int> newValue);

    void
    clearMajorNotchesIndexAndLocation();
    void
    clearMinorNotchesIndexAndLocation();
    int
   getSizeMajorNotchesIndexAndLocation();
    int
    getSizeMinorNotchesIndexAndLocation();

    void setMaximumLimitInPixels(int newMaximumLimitInPixels);
    void setLimits(double minimumLimit, double maximumLimit);
    double getMaximumLimit();
    double getMinimumLimit();
    //this method causes all notches to be drawn in a specific area
    virtual void doPaint(QRect viewport, QPainter *painter) = 0;
    virtual int getRulerLength() = 0;
    void setNotchesAligment(plotWidget::Alignment);
    plotWidget::Alignment getNotchesAlignment();
protected:
    void mouseMoveEvent(QMouseEvent *) = 0;
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void resizeEvent(QResizeEvent *);

public slots:
    void setTitle(QString title, plotWidget::Alignment align=plotWidget::Center);
    QString getTitle();
    void setTitleVisible(bool isVisible);
    void setTitleAlignment(plotWidget::Alignment);
    plotWidget::Alignment getTitleAlignment();
    virtual void forceIndexAtMousePositionChangedSignal(QMouseEvent*) = 0;

protected:
    QString title;
    plotWidget::Alignment titleAlignment;
    plotWidget::Alignment notchesAlignment;
    bool isTitleVisible;
    QMap<double, int> majorNotchesIndexAndLocation;
    QMap<double, int> minorNotchesIndexAndLocation;
    int maximumLimitInPixels;
    double minimumLimit, maximumLimit;
    bool isMousePressed;
    bool printRealValues;

signals:
    //this signal is emitted when after size has changed
    void sizeChanged();
    //emitted when index under mouse position changes due to mouse movement
    void indexAtMousePositionChanged(double newIndex);

private:
    void initComponents();
};

#endif // ABSTRACTRULER_H
