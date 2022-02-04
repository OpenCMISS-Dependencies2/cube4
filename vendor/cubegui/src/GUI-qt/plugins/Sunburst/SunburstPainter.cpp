/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2016-2021                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2015-2016                                                **
**  RWTH Aachen University, JARA-HPC                                       **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


/*-------------------------------------------------------------------------*/
/**
 *  @file    SunburstPainter.cpp
 *  @ingroup CubeSunburst
 *  @brief   Definition of the functional module SunburstPainter.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include "SunburstPainter.h"

// introduce Cube namespaces
using namespace cubegui;
using namespace cubepluginapi;

using namespace cube_sunburst;


// local procedure used by drawSunburst
namespace cube_sunburst
{
namespace detail
{
void
drawArc( qreal     innerRadius,
         qreal     outerRadius,
         qreal     firstAngle,
         qreal     secondAngle,
         QRect     boundingRect,
         QColor    fillColor,
         QColor    frameColor,
         int       frameWidth,
         QPainter& painter )
{
    if ( secondAngle < firstAngle )
    {
        secondAngle += 360;
    }

    // define the bounding rectangle for the arc
    QRect outerBoundingRect;

    // set the size of the outer bounding rectangle with regard to the outer radius
    outerBoundingRect.setHeight( outerRadius * boundingRect.height() );
    outerBoundingRect.setWidth( outerRadius * boundingRect.width() );

    // set the center of the bounding rectangle to the center of the given boundingRect
    outerBoundingRect.moveCenter( boundingRect.center() );

    // Create a path describing one arc.
    QPainterPath  arcPath;
    QPainterPath* ringPath = NULL; // only needed if the arc is actually a full ring
    if ( innerRadius == 0 )
    {
        arcPath.addEllipse( outerBoundingRect );
    }
    else
    {
        /* Additionally to the bounding rectangle defining the outer radius of the arc, we define the inner radius
         * using another bounding rectangle. Define it the same way as above the outer rectangle. */
        QRect innerBoundingRect;
        innerBoundingRect.setHeight( innerRadius * boundingRect.height() );
        innerBoundingRect.setWidth( innerRadius * boundingRect.width() );
        innerBoundingRect.moveCenter( boundingRect.center() );

        // define the starting point of our "brush"
        arcPath.arcMoveTo( innerBoundingRect, firstAngle );

        // draw the arc
        // inner arc counter-clockwise
        arcPath.arcTo( innerBoundingRect, firstAngle, secondAngle - firstAngle );
        // outer arc clockwise which implicitely draws the connecting line from the previous arc
        arcPath.arcTo( outerBoundingRect, secondAngle, firstAngle - secondAngle );
        // the connecting line from the previous arc to the starting position
        arcPath.closeSubpath();

        // in case we want to draw a ring we need to have a seperate path skipping the connecting lines between the radia
        if ( fmod( secondAngle, 360 ) == firstAngle )
        {
            ringPath = new QPainterPath();
            // define the starting point of our "brush"
            ringPath->arcMoveTo( innerBoundingRect, firstAngle );

            // draw the ring
            // inner circle counter-clockwise
            ringPath->arcTo( innerBoundingRect, firstAngle, secondAngle - firstAngle );
            // skip the connecting line by moving the "brush" without drawing
            ringPath->arcMoveTo( outerBoundingRect, secondAngle );
            // outer circle clockwise
            ringPath->arcTo( outerBoundingRect, secondAngle, firstAngle - secondAngle );
            // not closing the path skips the second connection line
        }
    }

    arcPath.setFillRule( Qt::OddEvenFill );

    painter.fillPath( arcPath, fillColor );

    // perform changes to pen only temporarily
    painter.save();

    if ( frameWidth != 0 )
    {
        QPen pen;
        pen.setColor( frameColor );
        pen.setWidth( frameWidth );
        painter.setPen( pen );
        // draw ringPath if the level has only one item and arcPath otherwise
        if ( ringPath != NULL )
        {
            painter.drawPath( *ringPath );
            delete ringPath;
        }
        else
        {
            painter.drawPath( arcPath );
        }
    }

    // restore the previous pen settings
    painter.restore();
}
} // namespace detail
} // namespace cube_sunburst


void
cube_sunburst::drawSunburst( SunburstCursorData& cursorData,
                             SunburstShapeData&  shapeData,
                             TransformationData& transformationData,
                             QPainter&           painter )
{
    if ( !transformationData.getBoundingRect().isValid() || !shapeData.isValid() )
    {
        return;
    }

    // enable antialiasing if wanted
    if ( transformationData.antialiazed() )
    {
        painter.setRenderHint( QPainter::Antialiasing );
    }

    for ( int i = 0; i < shapeData.numberOfVisibleLevels(); i++ )
    {
        qreal innerRadius = shapeData.getInnerRadius( i );
        qreal outerRadius = shapeData.getOuterRadius( i );

        // scale the current zoom level which was previously only handled by post render transformations
        innerRadius *= transformationData.getZoomFactor();
        outerRadius *= transformationData.getZoomFactor();

        // count the elements at the current level
        QList< TreeItem* > levelList = detail::getElementsOfLevel(
            *shapeData.getTopLevelItem(),
            i );
        int numberOfElements = levelList.count();
        for ( int listIter = 0; listIter < numberOfElements; listIter++ )
        {
            // check whether the item current item is visible and has to be drawn
            if ( !shapeData.itemIsVisible( i, listIter ) )
            {
                continue;
            }

            // draw the arc
            qreal firstAngle, secondAngle;
            // check for a single item (ring)
            if ( numberOfElements == 1 )
            {
                firstAngle  = 0;
                secondAngle = 360;
            }
            else
            {
                firstAngle = shapeData.getAbsDegree( i, listIter );
                if ( listIter + 1 != numberOfElements )
                {
                    secondAngle = shapeData.getAbsDegree( i, listIter + 1 );
                }
                else
                {
                    secondAngle = shapeData.getAbsDegree( i, 0 );
                }

                // add the current level of rotation
                firstAngle  += transformationData.getRotation();
                secondAngle += transformationData.getRotation();

                // make sure we still have angles from 0 to 359
                firstAngle  = fmod( firstAngle + 360, 360 );
                secondAngle = fmod( secondAngle + 360, 360 );
            }

            // check for frameSizeThreshold
            int frameWidth = 1;
            if (  ( fmod( secondAngle - firstAngle + 360,
                          360 ) < transformationData.getFrameSizeThreshold() )
                  && ( fmod( secondAngle, 360 ) != firstAngle ) )
            {
                frameWidth = 0;
            }

            // draw the arc for level-index i and element-index listIter
            detail::drawArc( innerRadius, outerRadius, firstAngle, secondAngle,
                             transformationData.getBoundingRect(),
                             levelList[ listIter ]->getColor(),
                             transformationData.getFrameLineColor(), frameWidth,
                             painter );
        }
    }

    // draw button
    if ( cursorData.getButtonTouched()
         && shapeData.itemIsVisible( cursorData )
         && cursorData.level() != ( shapeData.getNumberOfLevels() - 1 ) )
    {
        // get the shape data of the sunburst
        qreal innerRadius = shapeData.getInnerRadius( cursorData.level() );
        qreal outerRadius = shapeData.getOuterRadius( cursorData.level() );

        // scale the current zoom level
        innerRadius *= transformationData.getZoomFactor();
        outerRadius *= transformationData.getZoomFactor();

        // calculate the angles of the button
        qreal firstAngle, secondAngle;
        if ( ( cursorData.level() == 0 ) || ( shapeData.getNumberOfElements( cursorData.level() ) == 1 ) )
        {
            // special case where we set secondAngle to 360 to have a full ring without a gap at 0 degrees
            firstAngle  = 0;
            secondAngle = 360;

            // this is also the only case where we can ignore any rotation since the ring is complete
        }
        else
        {
            firstAngle = shapeData.getAbsDegree( cursorData.level(), cursorData.index() );
            if ( cursorData.index() + 1 != shapeData.getNumberOfElements( cursorData.level() ) )
            {
                secondAngle = shapeData.getAbsDegree( cursorData.level(), cursorData.index() + 1 );
            }
            else
            {
                secondAngle = shapeData.getAbsDegree( cursorData.level(), 0 );
            }

            // add the current level of rotation
            firstAngle  += transformationData.getRotation();
            secondAngle += transformationData.getRotation();

            // make sure we still have angles from 0 to 359
            firstAngle  = fmod( firstAngle + 360, 360 );
            secondAngle = fmod( secondAngle + 360, 360 );
        }

        // define the width of the button
        qreal buttonWidth = shapeData.getExpandButtonWidth();
        if ( cursorData.level() < shapeData.numberOfCompleteLevels() - 1 )
        {
            buttonWidth = shapeData.getCollapseButtonWidth();
        }

        // decide which type of button (expand/collapse) we have to draw
        if ( shapeData.getExpanded( cursorData ) )
        {
            QColor buttonColor = Qt::white;
            QColor frameColor  = Qt::gray;
            detail::drawArc( innerRadius
                             + ( ( outerRadius - innerRadius ) * ( 1 - buttonWidth ) ),
                             outerRadius,
                             firstAngle,
                             secondAngle,
                             transformationData.getBoundingRect(),
                             buttonColor,
                             frameColor,
                             1,
                             painter );
        }
        else
        {
            QColor buttonColor = Qt::white;
            QColor frameColor  = Qt::white;
            buttonColor.setAlpha( 125 );
            frameColor.setAlpha( 75 );
            detail::drawArc( innerRadius
                             + ( ( outerRadius - innerRadius ) * ( 1 - buttonWidth ) ),
                             outerRadius,
                             firstAngle,
                             secondAngle,
                             transformationData.getBoundingRect(),
                             buttonColor,
                             frameColor,
                             1,
                             painter );
        }
    }

    // draw selections

    // currently only designed to work on SYSTEMTREE
    QList< TreeItem* > selectedItems = shapeData.getService()->getSelections( SYSTEM );
    foreach( TreeItem * arc, selectedItems )
    {
        // check whether the item current item is visible and has to be drawn
        if ( !detail::itemVisible( *arc ) )
        {
            continue;
        }

        // rename for improved readability
        const QPoint posItem      = detail::getItemLocation( *shapeData.getTopLevelItem(), *arc );
        const int    levelPosItem = posItem.x();
        const int    indexPosItem = posItem.y();

        if ( indexPosItem == -1 )
        {
            continue;
        }

        qreal innerRadius = shapeData.getInnerRadius( levelPosItem );
        qreal outerRadius = shapeData.getOuterRadius( levelPosItem );

        innerRadius *= transformationData.getZoomFactor();
        outerRadius *= transformationData.getZoomFactor();

        // draw the arc
        qreal firstAngle, secondAngle;
        int   numberOfElements = detail::getElementsOfLevel(
            *shapeData.getTopLevelItem(),
            levelPosItem ).count();
        // check for a single item (ring)
        if ( numberOfElements == 1 )
        {
            firstAngle  = 0;
            secondAngle = 360;
        }
        else
        {
            firstAngle = shapeData.getAbsDegree( levelPosItem, indexPosItem );
            if ( indexPosItem + 1 != numberOfElements )
            {
                secondAngle = shapeData.getAbsDegree( levelPosItem, indexPosItem + 1 );
            }
            else
            {
                secondAngle = shapeData.getAbsDegree( levelPosItem, 0 );
            }

            // add the current level of rotation
            firstAngle  += transformationData.getRotation();
            secondAngle += transformationData.getRotation();

            // make sure we still have angles from 0 to 359
            firstAngle  = fmod( firstAngle + 360, 360 );
            secondAngle = fmod( secondAngle + 360, 360 );
        }

        // check for frameSizeThreshold
        int frameWidth = 2;
        if (  ( fmod( secondAngle - firstAngle + 360,
                      360 ) < transformationData.getFrameSizeThreshold() )
              && ( fmod( secondAngle, 360 ) != firstAngle ) )
        {
            frameWidth = 1;
        }

        detail::drawArc( innerRadius, outerRadius, firstAngle, secondAngle,
                         transformationData.getBoundingRect(), QColor( 0, 0, 0, 0 ),
                         transformationData.getSelectionLineColor(), frameWidth,
                         painter );
    }
}
