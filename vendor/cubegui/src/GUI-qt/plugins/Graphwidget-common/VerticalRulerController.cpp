/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2018                                                **
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



#include "config.h"

#include "VerticalRulerController.h"
#include "VerticalRuler.h"
#include <cmath>

VerticalRulerController::VerticalRulerController( QWidget* parent ) : AbstractRulerController( parent )
{
    this->view = new VerticalRuler( parent );
    initComponents();
    setConnections();
}

void
VerticalRulerController::doPaint( QRect viewport, QPainter& painter )
{
    //1. scale the notches and update the view accordingly
    this->calibrateAllNotches( viewport.height() );
    //2. make the view do the drawing
    this->getView()->doPaint( viewport, &painter );
    //3. restore everything
    this->calibrateAllNotches();
    this->getView()->update();
}

int
VerticalRulerController::pixelCoordToRulerCoord( int pixelCoord, bool oppositeDirection )
{
    return static_cast<int>( floorl( pixelCoordToRulerCoordF( pixelCoord, oppositeDirection ) ) );
}

double
VerticalRulerController::pixelCoordToRulerCoordF( int pixelCoord, bool oppositeDirection )
{
    if ( view->height() == 0 )
    {
        return -1.0;
    }
    ( void )oppositeDirection;
    // if(!oppositeDirection)
    //  {
    // return ((this->maximumLimit * pixelCoord) / view->height()) + this->minimumLimit;
    // }
    //return this->maximumLimit - ( ( this->maximumLimit * pixelCoord ) / view->height() );
    return this->maximumLimit - ( (  ( this->maximumLimit - this->minimumLimit ) * pixelCoord ) / view->height() );
}
