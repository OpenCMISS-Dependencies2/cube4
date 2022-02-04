/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2016-2017                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2015-2016                                                **
**  RWTH Aachen University, JARA-HPC                                       **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef UI_EVENT_WIDGET_H
#define UI_EVENT_WIDGET_H

// Sunburst includes
#include "DataAccessFunctions.h"
#include "InfoToolTip.h"
#include "SunburstCursorData.h"
#include "SunburstPainter.h"
#include "SunburstShapeData.h"
#include "TransformationData.h"

// Qt includes
#include <QTimer>

namespace cube_sunburst
{
/// enum for the left drag status
enum LeftDrag
{
    CLICK, ROTATION, RESIZE, SHIFT
};

/// enum for the resize states that prevent overrotating
enum ResizeStatus
{
    VALID, TOO_BIG, TOO_SMALL, REQUIRES_BIG, REQUIRES_SMALL
};


class UIEventWidget
    : public QWidget
{
    Q_OBJECT

public:
    // public member methods
    UIEventWidget( QWidget* parent = 0 );

    /// Work on the given SunburstShapeData.
    void
    useShapeData( SunburstShapeData* );

    /// Work on the given TransformationData.
    void
    useTransformationData( TransformationData* );

    /// Setter for showToolTip.
    void
    setShowToolTip( bool value );

    /// Setter for markZero.
    void
    setMarkZero( bool value );

    /// Setter for contextMenu.
    void
    setContextMenu( QMenu* value );

    /// Setter for zoomTowardsCursor.
    void
    setZoomTowardsCursor( bool value );

    /// Setter for zoomInverted.
    void
    setZoomInverted( bool value );


    /// Sets degree offset variables to 0.
    void
    resetDegreeOffset();

    /// Resets any changes to arc sizes.
    void
    resetArcSizes();

    /// Resets zoom.
    void
    resetZoom();

    /// Resets position of the sunburst.
    void
    resetSunburstPosition();

    /// Resets all user interactions.
    void
    resetAll();

protected:
    // protected member methods
    // Begin: overwritten functions for event handling
    /// Called whenever the widget size changes. Useful to initialize/ update size dependedent content.
    void
    resizeEvent( QResizeEvent* event );

    /// Called whenever the user pressed a key.
    void
    keyPressEvent( QKeyEvent* event );

    /// Called whenever the user released a key.
    void
    keyReleaseEvent( QKeyEvent* event );

    /// Called whenever the widget catches a mouse click.
    void
    mousePressEvent( QMouseEvent* event );

    /// Called whenever the widget catches a mouse release.
    void
    mouseReleaseEvent( QMouseEvent* event );

    /// Called whenever the widget catches a mouse move.
    void
    mouseMoveEvent( QMouseEvent* event );

    /// Called whenever the cursor leaves the widget.
    void
    leaveEvent( QEvent* event );

    /// Called whenever the mouse wheel is used.
    void
    wheelEvent( QWheelEvent* event );

    // End: overwritten functions for event handling

    /// Draws the Sunburst. Overwritten function.
    void
    paintEvent( QPaintEvent* event );


private:
    // private member methods
    // Begin: new functions for more specific event handling
    /// The function that is called when the user left-clicks the widget
    void
    leftClickHandler( const QPoint& position );

    /// The function that is called when the user right-clicks the widget
    void
    rightClickHandler( const QPoint& position );

    /// The function that is called after the user left-clicked the widget without drag between mouse press and release
    void
    leftClickReleaseHandler( const QPoint& position );

    /// The function that is constantly called when moving the cursor over the widget
    void
    cursorMoveHandler( const QPoint& position );

    /// The function that is constantly called when dragging with the left mouse button held
    void
    leftDragHandler( const QPoint& position );

    /// The function called to update variables after rotating the sunburst
    void
    finishRotating();

    /// The function called to update variables after shifting the sunburst
    void
    finishShifting();

    /// The function called to update variables after resizing an arc of the sunburst
    void
    finishResizing();

    // End: new functions for more specific event handling

    /// Checks the pointers shapeData and transformationData to contain valid objects
    bool
    initialized();

    // private member attributes
    /// Remembers the point of a click to distinguish between a single click event and drag and drop
    QPoint dragStartPosition;

    /// Remembers the fraction of a pixel that could not be applied to the last shift (after zooming).
    QPointF shiftError;

    /// The shape data of the sunburst.
    SunburstShapeData* shapeData;

    /// Information of the appearence of the sunburst.
    TransformationData* transformationData;

    /// The InfoToolTip object.
    InfoToolTip toolTip;

    /// Drag status to distinguish a click (0) from several left mouse drag interactions (>0).
    LeftDrag dragStatus;

    /// Needed for resizing drag interaction to check whether we are at the limit in one direction
    ResizeStatus lastResizeStatus;

    // drag position variables
    /// Remembers the degree of dragStartPosition relative to the sunburst (avoids recalculating it when needed)
    qreal dragStartDegree;

    /// Stores the degree offset due to the rotation/resizing drag interaction
    qreal degreeOffset;

    /// Stores the old offset due to any finished rotation drag interaction
    qreal oldDegreeOffset;

    /// A buffer used to store the current rotation caused by only the rotation interaction
    qreal rotationBuffer;

    /// Needed to determine the path of the cursor for the shift.
    QPoint shiftCursorCache;

    /// Needed to avoid cases where Qt calls the MouseMoveEvent despite the cursor position not changing.
    QPoint moveCursorCache;

    /// Remembers the old degree of the selected arc when starting a new resize drag interaction.
    qreal oldResizedItemDegree;

    // key status variables
    /// The next or active drag will be a shift (OVERWRITES ROTATION AND ARC RESIZING).
    bool shiftKeyPressed;

    /// The next or active drag will resize an arc (OVERWRITES ROTATION BUT NOT SHIFT). Also enables multiselection of arcs.
    bool ctrlKeyPressed;

    /// Remembers whether there is currently a button drawn and indicates whether the button has to be drawn (for drawing efficiency).
    bool showButton;

    /// True if information about the touched element should be displayed by a tooltip.
    bool showToolTip;

    /// True if an angle orientation line should be drawn at 0 degrees.
    bool markZero;

    /// True if the cursor is the fix point of a zoom instead of the sunburst center.
    bool zoomTowardsCursor;

    /// True if the zoom direction should be inverted.
    bool zoomInverted;

    /// Pointer to the context menu showing after right clicking
    QMenu* contextMenu;

    /// A timer to determine whether it is time to show a tooltip after the cursor didnt move for some time
    QTimer toolTipTimer;

    /// Includes the item touched by the cursor, whether the button area of the item is touched and which border is near the cursor.
    SunburstCursorData cursorData;

    // design decisions
    /// Space in pixels to distinguish between drag and click.
    static const int startDragDistance = 5;

private slots:
    /// Qt slot: Called when toolTipTimer times out
    void
    toolTipTimeOut();
};
} // namespace cube_sunburst

#endif    /* UI_EVENT_WIDGET_H */
