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
 *  @file    UIEventWidget.cpp
 *  @ingroup CubeSunburst
 *  @brief   Definition of the class UIEventWidget.
 **/
/*-------------------------------------------------------------------------*/

#include <config.h>

#include "UIEventWidget.h"

// Qt includes
#include <QApplication>

// general includes
#include <cassert>
#include <cmath>

// introduce Cube namespaces
using namespace cubegui;
using namespace cubepluginapi;

using namespace cube_sunburst;


UIEventWidget::UIEventWidget( QWidget* parent )
    : QWidget( parent )
{
    // initialize drag status
    dragStatus       = CLICK;
    lastResizeStatus = VALID;

    // initialize wheel shift error
    shiftError = QPointF( 0, 0 );

    // initialize drag position
    degreeOffset    = 0;
    oldDegreeOffset = 0;
    rotationBuffer  = 0;
    cursorData      = SunburstCursorData( -1, 0 );

    // initialize cursor caches
    shiftCursorCache = QPoint( -1, -1 );
    moveCursorCache  = QPoint( -1, -1 );

    // initialize key status
    shiftKeyPressed = false;
    ctrlKeyPressed  = false;

    transformationData = NULL;
    shapeData          = NULL;
    contextMenu        = NULL;

    // setup toolTipTimer
    toolTipTimer.setSingleShot( true );
    toolTipTimer.setInterval( 500 );
    connect( &toolTipTimer, SIGNAL( timeout() ), this, SLOT( toolTipTimeOut() ) );

    // define the minimum distance to distinguish between single click events and dragging
    QApplication::setStartDragDistance( startDragDistance );

    // enable key events
    setFocusPolicy( Qt::StrongFocus );
}


void
UIEventWidget::useShapeData( SunburstShapeData* data )
{
    shapeData = data;
}


void
UIEventWidget::useTransformationData( TransformationData* data )
{
    transformationData = data;
}


void
UIEventWidget::setShowToolTip( bool value )
{
    showToolTip = value;
}


void
UIEventWidget::setMarkZero( bool value )
{
    markZero = value;
}


void
UIEventWidget::setContextMenu( QMenu* value )
{
    contextMenu = value;
}


void
UIEventWidget::setZoomTowardsCursor( bool value )
{
    zoomTowardsCursor = value;
}


void
UIEventWidget::setZoomInverted( bool value )
{
    zoomInverted = value;
}


void
UIEventWidget::resetDegreeOffset()
{
    if ( !initialized() )    // check pointers
    {
        return;
    }
    degreeOffset    = 0;
    oldDegreeOffset = 0;
    rotationBuffer  = 0;
}


void
UIEventWidget::resetArcSizes()
{
    shapeData->resetDegrees();
}


void
UIEventWidget::resetZoom()
{
    transformationData->setZoomLevel( 0 );
}


void
UIEventWidget::resetSunburstPosition()
{
    if ( transformationData == NULL )
    {
        return;
    }
    // the position if the sunburst is defined by the boundingRect in transformationData
    // the boundingRect has to be a square
    QRect screenRect = geometry();
    screenRect.setWidth( std::min( screenRect.width(),
                                   screenRect.height() ) );
    screenRect.setHeight( screenRect.width() );
    transformationData->setBoundingRect( screenRect );
}


void
UIEventWidget::resetAll()
{
    // reset expanded data of tree items in the cube tree
    for ( int i = 0; i < shapeData->numberOfVisibleLevels(); i++ )
    {
        QList< TreeItem* > levelList = detail::getElementsOfLevel(
            *shapeData->getTopLevelItem(),
            i );
        for ( int listIter = 0; listIter < levelList.count(); listIter++ )
        {
            // check whether the item current item is visible
            if ( !shapeData->itemIsVisible( i, listIter ) )
            {
                continue;
            }
            // update expanded of the tree data from the sunburst data
            levelList[ listIter ]->setExpanded( false );
        }
    }

    // reset cached visibility data stored in shapeData
    shapeData->resetVisibilityData();

    // reset other transformations
    resetDegreeOffset();
    resetArcSizes();
    resetZoom();
    resetSunburstPosition();
}


void
UIEventWidget::resizeEvent( QResizeEvent* event )
{
    if ( !initialized() )    // check pointers
    {
        return;
    }

    // update size dependent content
    const QRect oldRect = transformationData->getBoundingRect();
    QRect       newRect( oldRect.x(), oldRect.y(), std::min( width(),
                                                             height() ), std::min(
                             width(),
                             height() ) );
    transformationData->setBoundingRect( newRect );
    update();
    event->accept();
}


void
UIEventWidget::keyPressEvent( QKeyEvent* event )
{
    if ( !initialized() )    // check pointers
    {
        return;
    }
    if ( event->key() == Qt::Key_Control )
    {
        /* in case of a following click than a drag we enable the selection of multiple arcs
         * in case of a following drag we resize the arc of the current cursor position
         * both need the ctrl key to be pressed */
        ctrlKeyPressed = true;

        // overwhites an active rotation to a shift, the shift starts at the next mouse move event
        if ( dragStatus == ROTATION )
        {
            finishRotating();
            // a new drag is usually initiallised with a click event
            leftClickHandler( mapFromGlobal( cursor().pos() ) );
        }
        else
        {
            // just to update the cursor
            update();
        }
    }
    if ( event->key() == Qt::Key_Shift )
    {
        shiftKeyPressed = true;

        // overwhites an active rotation to a shift, the shift starts at the next mouse move event
        if ( dragStatus == ROTATION )
        {
            finishRotating();
            // a new drag is usually initiallised with a click event
            leftClickHandler( mapFromGlobal( cursor().pos() ) );
        }
        /* IF CTRL IS NO LONGER HOLD: (to be consistent with topology view plugin)
         * overwhites an active arc resizing to a shift, the shift starts at the next mouse move event */
        if (  !ctrlKeyPressed
              && ( dragStatus == RESIZE ) )
        {
            finishResizing();
            // a new drag is usually initiallised with a click event
            leftClickHandler( mapFromGlobal( cursor().pos() ) );
        }
    }
    event->accept();
}


void
UIEventWidget::keyReleaseEvent( QKeyEvent* event )
{
    if ( !initialized() )    // check pointers
    {
        return;
    }
    if ( event->key() == Qt::Key_Control )
    {
        ctrlKeyPressed = false;
        // just to update the cursor
        update();
    }
    if ( event->key() == Qt::Key_Shift )
    {
        shiftKeyPressed = false;
    }
    event->accept();
}


void
UIEventWidget::mousePressEvent( QMouseEvent* event )
{
    if ( !initialized() )    // check pointers
    {
        return;
    }

    // check whether the user pressed the left mouse button for either a single click or dragging
    if ( event->button() == Qt::LeftButton )
    {
        leftClickHandler( event->pos() );
    }
    if ( event->button() == Qt::RightButton )
    {
        rightClickHandler( event->pos() );
    }
    event->accept();
}


void
UIEventWidget::mouseReleaseEvent( QMouseEvent* event )
{
    if ( !initialized() )    // check pointers
    {
        return;
    }
    if ( event->button() == Qt::LeftButton )
    {
        // check whether the user pressed and released the left mouse button without dragging
        if (  ( dragStatus == CLICK )
              && ( ( event->pos() - dragStartPosition ).manhattanLength()
                   < QApplication::startDragDistance() ) )
        {
            // the user just pressed  and released the left mouse button
            leftClickReleaseHandler( event->pos() );
        }
        else
        {
            // user finished dragging
            switch ( dragStatus )
            {
                case ROTATION:
                    finishRotating();
                    break;

                case SHIFT:
                    finishShifting();
                    break;

                case RESIZE:
                    finishResizing();
                    break;

                default:
                    break;
            }
        }
    }
    event->accept();
}


void
UIEventWidget::mouseMoveEvent( QMouseEvent* event )
{
    if ( !initialized() )    // check pointers
    {
        return;
    }
    /* Somehow Qt calls this event when the mouse focus switches to another widget without the cursor being moved
     * (in the case where toolTip is generated on top of the cursor). We catch this case and just return since
     * the following handler would hide the toolTip again when it just got generated. */
    if ( event->pos() == moveCursorCache )
    {
        event->ignore();
        return;
    }
    moveCursorCache = event->pos();

    // distinguish between normal mouse move or dragging
    if (  !( event->buttons() & Qt::LeftButton )
          && !( event->buttons() & Qt::RightButton ) )
    {
        // no drag
        cursorMoveHandler( event->pos() );
        return;
    }
    if ( event->buttons() & Qt::LeftButton )
    {
        // left mouse drag
        if ( dragStatus != CLICK )
        {
            // continue handling the drag
            leftDragHandler( event->pos() );
        }
        else
        {
            // check whether the user actually dragged the cursor
            if ( ( event->pos() - dragStartPosition ).manhattanLength()
                 >= QApplication::startDragDistance() )
            {
                // dependent on shiftKeyPressed/ctrlKeyPressed set the corresponding flag...
                if ( shiftKeyPressed )
                {
                    // HIGHEST PRIORITY
                    dragStatus = SHIFT;
                }
                else if ( ctrlKeyPressed )
                {
                    // 2nd HIGHEST PRIORITY
                    cursorData = detail::getCursorData( *shapeData,
                                                        *transformationData,
                                                        dragStartPosition );
                    if ( shapeData->itemExists( cursorData ) )
                    {
                        dragStatus = RESIZE;

                        // reset drag start degree to the arc degree (this way its easier to evenly handle any drag starting anywhere inside the arc).
                        if ( cursorData.getNearLowerBorder() )
                        {
                            oldResizedItemDegree = shapeData->getAbsDegree( cursorData.x(),
                                                                            cursorData.y() );
                        }
                        else
                        {
                            oldResizedItemDegree = shapeData->getSuccAbsDegree( cursorData.x(),
                                                                                cursorData.y() );
                        }
                    }
                    else
                    {
                        // if the cursor does not point to an arc rotate instead of resizing an arc
                        dragStatus = ROTATION;
                    }
                }
                else
                {
                    dragStatus = ROTATION;
                }

                // ...and handle the event
                leftDragHandler( event->pos() );
            }
            else
            {
                // dependent on shiftKeyPressed start a new shift starting from the current position (overwrite click)
                if ( shiftKeyPressed )
                {
                    dragStartPosition = event->pos();
                    dragStatus        = SHIFT;
                }
            }

            // disable the tooltip during any drag or shift interaction
            toolTip.hide();
            toolTipTimer.stop();
        }
    }
    event->accept();
}


void
UIEventWidget::leaveEvent( QEvent* event )
{
    if ( !initialized() )    // check pointers
    {
        return;
    }
    switch ( dragStatus )
    {
        case ROTATION:
            finishRotating();
            break;
        case RESIZE:
            finishResizing();
            break;
        case SHIFT:
            finishShifting();
            break;
        default:
            break;
    }
    event->accept();
}


void
UIEventWidget::wheelEvent( QWheelEvent* event )
{
    if ( !initialized() )    // check pointers
    {
        return;
    }
#if QT_VERSION < QT_VERSION_CHECK( 5, 14, 0 )
    const QPoint cursorPos = event->pos();
    double       angle     = event->delta();
#else
    const QPointF cursorPos = event->position();
    double        angle     = event->angleDelta().y();
#endif
    // needed for later
    const qreal oldZoomFactor = transformationData->getZoomFactor();

    // check whether the user zoomed in or out
    if ( ( !zoomInverted && angle > 0 ) || ( zoomInverted && angle <= 0 ) )
    {
        transformationData->zoomIn();
    }
    else
    {
        transformationData->zoomOut();
    }

    // if the cursor should be the fix point perform the corresponding shift of the sunburst
    if ( zoomTowardsCursor )
    {
        // calculate the shift
        QPointF cursorDiffReal;
        if ( angle > 0 )
        {
            cursorDiffReal =
                ( QPointF( transformationData->getBoundingRect().center() - cursorPos )
                  - QPointF( transformationData->getBoundingRect().center() - cursorPos ) * oldZoomFactor / transformationData->getZoomFactor() );
        }
        else
        {
            cursorDiffReal =
                ( QPointF( transformationData->getBoundingRect().center() - cursorPos ) / oldZoomFactor * transformationData->getZoomFactor()
                  - QPointF( transformationData->getBoundingRect().center() - cursorPos ) );
        }
        cursorDiffReal += shiftError;
        const QPoint cursorDiffInt = cursorDiffReal.toPoint();

        // remember the rounding error to apply it on the next wheel event
        shiftError = cursorDiffReal - QPointF( cursorDiffInt );

        // apply the shift
        transformationData->setBoundingRect(
            transformationData->getBoundingRect().translated( cursorDiffInt ) );
    }

    // reset toolTip
    if ( showToolTip )
    {
        toolTip.hide();
        cursorData = detail::getCursorData( *shapeData,
                                            *transformationData,
                                            cursorPos );
        toolTipTimer.start();
    }
    update();
    event->accept();
}


void
UIEventWidget::leftClickHandler( const QPoint& position )
{
    // needed in mouseReleaseEvent and mouseMoveEvent to distinguish between click and dragging
    dragStartPosition = position;
    shiftCursorCache  = position;

    // calculate the degree of dragStartPosition relative to the sunburst (needed for the drag event handling)
    if ( !transformationData->getBoundingRect().isValid() )
    {
        return;
    }
    const QPoint center = transformationData->getBoundingRect().center();
    QPointF      diff( position.x() - center.x(), position.y() - center.y() );
    if ( diff.x() == 0 )
    {
        dragStartDegree = 0;
    }
    else
    {
        dragStartDegree = atan( -1 * diff.y() / diff.x() ) / ( 2 * M_PI ) * 360;
        if ( position.x() < center.x() )
        {
            dragStartDegree += 180;
        }
        else if ( position.y() > center.y() )
        {
            dragStartDegree += 360;
        }
    }
}


void
UIEventWidget::rightClickHandler( const QPoint& position )
{
    if ( contextMenu == NULL )
    {
        return;
    }
    contextMenu->popup( mapToGlobal( position ) );
}


void
UIEventWidget::leftClickReleaseHandler( const QPoint& position )
{
    if ( !initialized() || ( shapeData->getTopLevelItem() == NULL ) )
    {
        return;
    }

    SunburstCursorData item = detail::getCursorData( *shapeData,
                                                     *transformationData,
                                                     position );
    if ( shapeData->itemIsVisible( item ) )
    {
        cubegui::TreeItem* tItem = detail::getTreeItem( *shapeData, item );
        if ( item.getButtonTouched() && !tItem->isLeaf() )
        {
            // store some level information that might change after expanding/ collapsing the item
            int oldCompleteLevels = shapeData->numberOfCompleteLevels();
            int oldVisibleLevels  = shapeData->numberOfVisibleLevels();

            shapeData->setExpanded( item, !shapeData->getExpanded( item ) );

            // if one of the both counters change, we need to recalculate the sizes of the levels
            if ( ( oldCompleteLevels != shapeData->numberOfCompleteLevels() ) || ( oldVisibleLevels != shapeData->numberOfVisibleLevels() ) )
            {
                shapeData->updateLevelSizes();
            }

            // also update cube data
            tItem->setExpanded( !tItem->isExpanded() );
        }
        else
        {
            // multiselection if ctrlKeyPressed
            shapeData->getService()->selectItem( tItem, ctrlKeyPressed );
        }
        update();
    }
}


void
UIEventWidget::cursorMoveHandler( const QPoint& position )
{
    if ( !initialized() )    // check pointers
    {
        return;
    }

    SunburstCursorData oldCursorData = cursorData;
    cursorData = detail::getCursorData( *shapeData,
                                        *transformationData,
                                        position );

    // hide the tooltip (the tooltip is only shown if the cursor is not moving for a while)
    toolTip.hide();

    if ( showToolTip && shapeData->itemExists( cursorData ) )
    {
        // (re)start the timer for the delayed tooltip
        toolTipTimer.start();
    }

    // check whether we have to update the widget because cursorData changed
    if ( oldCursorData.getButtonTouched() != cursorData.getButtonTouched()
         || oldCursorData.level() != cursorData.level()
         || oldCursorData.index() != cursorData.index() )
    {
        update();
    }
}


void
UIEventWidget::leftDragHandler( const QPoint& position )
{
    if ( !initialized() )    // check pointers
    {
        return;
    }

    // check whether we are shifting, resizing an arc or rotating
    if ( dragStatus == SHIFT )
    {
        if ( !transformationData->getBoundingRect().isValid() )
        {
            return;
        }
        transformationData->setBoundingRect(
            transformationData->getBoundingRect().translated( position - shiftCursorCache ) );
        shiftCursorCache = position;
    }
    else if ( ( dragStatus == RESIZE ) || ( dragStatus == ROTATION ) )
    {
        // calculate the degree of the current cursor position relative to the sunburst (needed for both resizing and rotating)
        qreal currentCursorDegree;
        if ( !transformationData->getBoundingRect().isValid() )
        {
            return;
        }
        const QPoint center = transformationData->getBoundingRect().center();
        QPointF      diff( position.x() - center.x(), position.y() - center.y() );
        if ( diff.x() == 0 )
        {
            // since the division with diff.x() in the else block would not work, we have to define the both cases manually
            if ( position.y() < center.y() )
            {
                currentCursorDegree = 90;
            }
            else
            {
                currentCursorDegree = 270;
            }
        }
        else
        {
            currentCursorDegree = atan( -1 * diff.y() / diff.x() ) / ( 2 * M_PI )
                                  * 360;
            if ( position.x() < center.x() )
            {
                currentCursorDegree += 180;
            }
        }
        degreeOffset = fmod( currentCursorDegree - dragStartDegree + 720, 360 );
        ResizeStatus currentResizeStatus;

        // now distinguish between resizing and rotation
        switch ( dragStatus )
        {
            case RESIZE:
                currentResizeStatus = ( ResizeStatus )detail::checkAngle( *shapeData,
                                                                          cursorData,
                                                                          fmod( oldResizedItemDegree + degreeOffset, 360 ) );
                if ( currentResizeStatus == VALID && lastResizeStatus < 3 )
                {
                    lastResizeStatus = VALID;
                }
                else
                if ( currentResizeStatus == TOO_BIG && lastResizeStatus == VALID )
                {
                    lastResizeStatus = TOO_BIG;
                }
                else
                if ( currentResizeStatus == TOO_SMALL && lastResizeStatus == VALID )
                {
                    lastResizeStatus = TOO_SMALL;
                }
                else
                if ( currentResizeStatus == TOO_BIG && lastResizeStatus == TOO_SMALL )
                {
                    lastResizeStatus = REQUIRES_SMALL;
                }
                else
                if ( currentResizeStatus == TOO_SMALL && lastResizeStatus == TOO_BIG )
                {
                    lastResizeStatus = REQUIRES_BIG;
                }
                else
                if ( currentResizeStatus == TOO_BIG && lastResizeStatus == REQUIRES_BIG )
                {
                    lastResizeStatus = TOO_BIG;
                }
                else
                if ( currentResizeStatus == TOO_SMALL && lastResizeStatus == REQUIRES_SMALL )
                {
                    lastResizeStatus = TOO_SMALL;
                }
                // do not allow a jump from increasing to decreasing (and the other way) by endlessly rotating in one direction
                if ( lastResizeStatus == VALID )
                {
                    /* Calculate a new angle for the resizedItem:
                     *
                     * oldResizedItemDegree is the base for the calculation of the new angle. It is the old degree of
                     * the arc when the resize interaction started. Its constand during the interaction.
                     *
                     * degreeOffset is the current difference in degrees between the old arc size and the
                     * current arc size. It changes for any mouse move during the interaction.
                     */
                    qreal oldAbsDegreeOffset = shapeData->getAbsDegreeOffset();
                    detail::resizeArc( *shapeData,
                                       cursorData,
                                       fmod( oldResizedItemDegree + degreeOffset, 360 ) );
                    /* Modify oldResizedItemDegree due to the fact that shapeData->getAbsDegreeOffset changed the rotation
                     * in transformationData which actually moves the position of the arc. Thus, we subtract the exact
                     * difference of the absDegreeOffset before and after the function call to restore the exact position. */
                    oldResizedItemDegree = fmod( oldResizedItemDegree - shapeData->getAbsDegreeOffset() + oldAbsDegreeOffset + 360, 360 );
                }
                break;

            case ROTATION:

                // update the current overall rotation
                rotationBuffer = degreeOffset + oldDegreeOffset;
                break;

            default:
                break;
        }
    }
    update();
}


void
UIEventWidget::finishRotating()
{
    if ( !initialized() )    // check pointers
    {
        return;
    }
    dragStatus       = CLICK;
    oldDegreeOffset += degreeOffset;
    oldDegreeOffset  = fmod( oldDegreeOffset, 360 );
    degreeOffset     = 0;
    rotationBuffer   = oldDegreeOffset;
    // reenable toolTip
    if ( showToolTip )
    {
        cursorData = detail::getCursorData( *shapeData,
                                            *transformationData,
                                            mapFromGlobal( cursor().pos() ) );
        toolTipTimer.start();
    }
    update();
}


void
UIEventWidget::finishShifting()
{
    dragStatus = CLICK;
    // reenable toolTip
    if ( showToolTip )
    {
        cursorData = detail::getCursorData( *shapeData,
                                            *transformationData,
                                            mapFromGlobal( cursor().pos() ) );
        toolTipTimer.start();
    }
    update();
}


void
UIEventWidget::finishResizing()
{
    dragStatus       = CLICK;
    lastResizeStatus = VALID;
    degreeOffset     = 0;
    // reenable toolTip
    if ( showToolTip )
    {
        cursorData = detail::getCursorData( *shapeData,
                                            *transformationData,
                                            mapFromGlobal( cursor().pos() ) );
        toolTipTimer.start();
    }
    update();
}


void
UIEventWidget::paintEvent( QPaintEvent* event )
{
    if ( !initialized() || !shapeData->isValid() )
    {
        return;
    }

    // submit the overall rotation (resize interaction + rotation interaction) to the transformationData
    transformationData->setRotation( shapeData->getAbsDegreeOffset() + rotationBuffer );

    QPainter painter( this );

    // clear the screen of the widget
    painter.fillRect( rect(), this->palette().brush( QPalette::Window ) );

    // draw the sunburst on the widget
    drawSunburst( cursorData, *shapeData, *transformationData, painter );

    // draw an angle orientation line if demanded
    if ( markZero )
    {
        // rename for improved readability
        const QPoint sunburstCenter = transformationData->getBoundingRect().center();
        const int    sunburstWidth  = transformationData->getBoundingRect().width() / 3.0 * transformationData->getZoomFactor();
        const qreal  rotation       = -transformationData->getRotation() / 180.0 * M_PI;
        painter.drawLine( sunburstCenter, sunburstCenter + QPoint( cos( rotation ) * sunburstWidth, sin( rotation ) * sunburstWidth ) );
    }

    // chose cursor style dependent on the current interaction
    switch ( dragStatus )
    {
        case ROTATION:
            setCursor( QCursor( Qt::ClosedHandCursor ) );
            break;
        case RESIZE:
            setCursor( QCursor( Qt::ClosedHandCursor ) );
            break;
        case SHIFT:
            setCursor( QCursor( Qt::OpenHandCursor ) );
            break;
        default:
            if ( ctrlKeyPressed )
            {
                setCursor( QCursor( Qt::OpenHandCursor ) );
            }
            else
            {
                setCursor( QCursor( Qt::ArrowCursor ) );
            }
            break;
    }
    event->accept();
}


bool
UIEventWidget::initialized()
{
    return ( shapeData != NULL ) && ( transformationData != NULL );
}


void
UIEventWidget::toolTipTimeOut()
{
    toolTipTimer.stop();
    // show a tooltip with information about the touched item if showToolTip is set
    if ( showToolTip && shapeData->itemIsVisible( cursorData ) )
    {
        // display in top left corner of the window
        toolTip.showInfo( mapToGlobal( mapFrom( static_cast<QWidget*>( parent() ), QPoint( 0, 0 ) ) ),
                          detail::getTooltipText( *detail::getTreeItem( *shapeData, cursorData ),
                                                  *shapeData->getService() ) );
    }
    update();
}
