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


#ifndef ABSTRACTPLOTAREA_H
#define ABSTRACTPLOTAREA_H

#include <QWidget>
#include <QList>
#include <QPoint>
#include <QMouseEvent>
#include "GraphWidgetGlobals.h"
#include "CubeDataItem.h"

#include <QtGui>
/**
 * Displays a widget that has a mesh corresponding to horizontal and vertical notches. It also responds to mouse events. This class should be extended by other plot widgets.
 * @see BarsPlotArea
 * @see HeatMapPlotArea
 */
class AbstractPlotArea : public QWidget
{
    Q_OBJECT
public:
    /**
     * Creates a new instance
     * @param parent The parent of the widget
     */
    explicit AbstractPlotArea(QWidget *parent = 0);
signals:
    /**
     * Emitted whenever the mouse is clicked inside the area
     * @param mouseEvent Contains information about the mouse click event, most importantly the mouse coordinates
     */
    void mouseIsClicked(QMouseEvent* mouseEvent);


public:
    /**
     * Paints the widget using a custom QPainter instance. This method is used either to paint the widget itself or to draw on something else such as an image file or a QPrinter.
     * @param viewport Where to paint, the paint will be scaled accordingly.
     * @param painter The painter that is used for printing, if the painter is attached to this widget then the widget repaints itself whenever the method is called
     */
    virtual void doPaint(QRect viewport, QPainter* painter);
    /**
     * Sets the horizontal major notches array
     * @param HMajorNotches
     */
    void setHMajorNotches(QList<int> HMajorNotches);
    /**
     * Gets the horizontal major notches array
     * @return
     */
    QList<int> getHMajorNotches() const;
    /**
     * Sets the vertical major notches array
     * @param VMajorNotches
     */
    void setVMajorNotches(QList<int> VMajorNotches);
    /**
     * Gets the vertical major notches array
     * @return
     */
    QList<int> getVMajorNotches() const;
    /**
     * Sets the horizontal minor notches array
     * @param HMinorNotches
     */
    void setHMinorNotches(QList<int> HMinorNotches);
    /**
     * Gets the horizontal minor notches array
     * @return
     */
    QList<int> getHMinorNotches() const;
    /**
     * Sets the vertical minor notches array
     * @param VMinorNotches
     */
    void setVMinorNotches(QList<int> VMinorNotches);
    /**
     * Gets the vertical minor notches array
     * @return
     */
    QList<int> getVMinorNotches() const;
    /**
     * Sets the alignment of the horizontal mouse axis, i.e. when the mouse is clicked the horizontal line can be drawn either to the left or to the right of the mouse.
     * @param alignment The alignment of the horizontal mouse axis, either left or right.
     * @see constants::Alignment
     */
    void setMouseAxisHorizontalAlignment(plotWidget::Alignment alignment);
    /**
     * Sets the alignment of the vertical mouse axis, i.e. when the mouse is clicked the vertical line can be drawn either to the top or to the bottom of the mouse.
     * @param alignment The alignment of the horizontal mouse axis, either top or bottom.
     * @see constants::Alignment
     */
    void setMouseAxisVerticalAlignment(plotWidget::Alignment alignment);
    /**
     * Returns the alignment of the horizontal mouse axis
     * @return
     */
    plotWidget::Alignment getMouseAxisHorizontalAlignment() const;
    /**
     * Returns the alignment of the vertical mouse axis
     * @return
     */
    plotWidget::Alignment getMouseAxisVerticalAlignment() const;
    /**
     * Sets the legend visibility
     * @param isLegendVisible
     */
    void setLegendVisible(bool isLegendVisible);
    /**
     * Gets the legend visibility state
     * @return
     */
    bool isLegendVisible() const;
    /**
     * Sets the background of the legend to either solid or transparent
     * @param legendBackgroundIsSolid
     */
    void setLegendBackgroundSolid(bool legendBackgroundIsSolid);
    /**
     * Gets the state of whether the legend background is solid or transparent
     * @return The state of the legend background
     */
    bool isLegendBackgroundSolid() const;
    /**
     * Sets the legend horizontal alignment (left or right)
     * @param legendXAlignment the horizontal alignment of the legend
     */
    void setLegendXAlignment(plotWidget::Alignment legendXAlignment);
    /**
     * Gets the legend horizontal alignment
     * @return the legend horizontal alignment
     */
    plotWidget::Alignment getLegendXAlignment() const;
    /**
     * Sets the legend vertical alignment (top or bottom)
     * @param legendYAlignment the vertical alignment of the legend
     */
    void setLegendYAlignment(plotWidget::Alignment legendYAlignment);
    /**
     * Gets the legend vertical alignment
     * @return the legend vertical alignment
     */
    plotWidget::Alignment getLegendYAlignment() const;
    bool getDataToDisplay();


protected:
    virtual void initComponents();
    virtual QPixmap generateLegend() = 0;
    bool legendIsVisible;
    bool legendBackgroundIsSolid;
    plotWidget::Alignment legendXAlignment, legendYAlignment;
    QList<int> HMajorNotches, HMinorNotches, VMajorNotches, VMinorNotches;
    bool leftMouseIsClicked;
    bool showMouseAxis;
    QPoint mousePosition;
    plotWidget::Alignment mouseAxisHorizontalAlignment, mouseAxisVerticalAlignment;
    virtual void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    bool dataToDisplay;
};

#endif // ABSTRACTPLOTAREA_H
