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


#include "AbstractPlotAreaController.h"

AbstractPlotAreaController::AbstractPlotAreaController( QWidget* parent ) : QObject( parent )
{
}

void
AbstractPlotAreaController::setConnections()
{
    QObject::connect( view, SIGNAL( mouseIsClicked( QMouseEvent* ) ), this, SIGNAL( mouseIsClicked( QMouseEvent* ) ) );
}

AbstractPlotArea*
AbstractPlotAreaController::getView()
{
    return view;
}

void
AbstractPlotAreaController::updateVerticalMajorMeshLines( QList<int> VMajorNotches )
{
    view->setVMajorNotches( VMajorNotches );
    view->update();
}

void
AbstractPlotAreaController::updateVerticalMinorMeshLines( QList<int> VMinorNotches )
{
    view->setVMinorNotches( VMinorNotches );
    view->update();
}

void
AbstractPlotAreaController::updateHorizontalMajorMeshLines( QList<int> HMajorNotches )
{
    view->setHMajorNotches( HMajorNotches );
    view->update();
}

void
AbstractPlotAreaController::updateHorizontalMinorMeshLines( QList<int> HMinorNotches )
{
    view->setHMinorNotches( HMinorNotches );
    view->update();
}

void
AbstractPlotAreaController::updateMouseAxisHorizontalAlignment( plotWidget::Alignment alignment )
{
    view->setMouseAxisHorizontalAlignment( alignment );
    view->update();
}

void
AbstractPlotAreaController::updateMouseAxisVerticalAlignment( plotWidget::Alignment alignment )
{
    view->setMouseAxisVerticalAlignment( alignment );
    view->update();
}

void
AbstractPlotAreaController::setLegendVisible( bool isLegendVisible )
{
    view->setLegendVisible( isLegendVisible );
    view->update();
}

void
AbstractPlotAreaController::updateLegendXAlignment( plotWidget::Alignment newXAlignment )
{
    view->setLegendXAlignment( newXAlignment );
    view->update();
}

void
AbstractPlotAreaController::updateLegendYAlignment( plotWidget::Alignment newYAlignment )
{
    view->setLegendYAlignment( newYAlignment );
    view->update();
}

void
AbstractPlotAreaController::setLegendBackgroundSolid( bool isLegendBackgroundSolid )
{
    view->setLegendBackgroundSolid( isLegendBackgroundSolid );
    view->update();
}
