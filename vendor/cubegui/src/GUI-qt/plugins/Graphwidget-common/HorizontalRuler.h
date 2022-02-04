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


#ifndef HORIZONTALRULER_H
#define HORIZONTALRULER_H

#include "AbstractRuler.h"

class HorizontalRuler : public AbstractRuler
{
public:
    HorizontalRuler(QWidget* parent=0);
    virtual void forceIndexAtMousePositionChangedSignal(QMouseEvent* mouseEvent);
protected:
    virtual int getRulerLength();
    virtual void doPaint(QRect viewport, QPainter *painter);
    void mouseMoveEvent(QMouseEvent *);
    void paintEvent(QPaintEvent *);
private:
    void drawNotch(double number, int xDisplacement, QPainter* painter, int notchSize);
    void initComponents();
};

#endif // HORIZONTALRULER_H
