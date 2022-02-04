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



#ifndef ORDERWIDGET_H
#define ORDERWIDGET_H
#include <iostream>

#include <QWidget>
#include <vector>
#include <QFrame>
#include <QPaintEvent>
#include <QMouseEvent>

/**
   Widget to merge the selected dimensions of topologies
 */
class OrderWidget : public QFrame
{
    Q_OBJECT
public:
    explicit
    OrderWidget( const std::vector<long>& ndim,
                 const QStringList&       dimnames );
    void
    paintEvent( QPaintEvent* );
    virtual QSize
    minimumSizeHint() const;
    virtual QSize
    sizeHint() const;

    std::vector<std::vector<int> >
    getFoldingVector() const;
    void
    setFoldingVector( std::vector<std::vector<int> >);

signals:
    void
    foldingDimensionsChanged();

protected:
    void
    mousePressEvent( QMouseEvent* event );
    void
    mouseMoveEvent( QMouseEvent* event );
    void
    mouseReleaseEvent( QMouseEvent* event );

private:
    void
    drawElement( QPainter& painter,
                 int       x,
                 int       y,
                 int       elemnr );

    int                            ndim;       // number of total dims
    int                            cellwidth;  // width of each element
    int                            cellheight; // height of each dimension cell
    int                            labelwidth; // width of the legend
    int                            moveX;      // index of element to move
    int                            moveY;
    int                            columns;
    QPoint                         currentPos; // current position of mouse while dragging
    std::vector<long>              dims;
    QStringList                    dimnames;
    std::vector<std::vector<int> > place;
};

#endif // ORDERWIDGET_H
