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


#ifndef TRANSFORMATION_DATA_H
#define TRANSFORMATION_DATA_H

// Qt includes
#include <QColor>
#include <QRect>

namespace cube_sunburst
{
/// Module for storing visualization settings.
class TransformationData
{
public:
    // public member methods
    /// Resets all variables.
    void
    initialize();

    /// Setter for boundingRect.
    void
    setBoundingRect( const QRect& value );

    /// Getter for boundingRect.
    QRect
    getBoundingRect();

    /// Setter for antialiasing.
    void
    setAntialiasing( bool value );

    /// Getter for antialiasing.
    bool
    antialiazed();

    /// Setter for zoomLevel.
    void
    setZoomLevel( int value );

    /// Decrease zoomLevel.
    void
    zoomIn();

    /// Increase zoomLevel.
    void
    zoomOut();

    /// Get the size zoom factor.
    qreal
    getZoomFactor();

    /// Setter for rotation.
    void
    setRotation( qreal value );

    /// Getter for rotation.
    qreal
    getRotation();

    /// Setter for frameLineColor.
    void
    setFrameLineColor( const QColor& value );

    /// Getter for frameLineColor.
    QColor
    getFrameLineColor();

    /// Setter for selectionLineColor.
    void
    setSelectionLineColor( const QColor& value );

    /// Getter for selectionLineColor.
    QColor
    getSelectionLineColor();

    /// Setter for frameSizeThreshold.
    void
    setFrameSizeThreshold( int value );

    /// Getter for frameSizeThreshold.
    int
    getFrameSizeThreshold();

    /// Getter for drawingShiftError.
    QPointF
    getDrawingShiftError();

private:
    // private member attributes
    /// The minimal bounding rectangle covering the whole object.
    QRect boundingRect;

    /// True, if the user wants the sunburst to be drawn with antialiasing.
    bool antialiasing;

    /// The zoom level of the current scene.
    int zoomLevel;

    /// Angle of a rotation transformation. Is always adjusted to the range from 0 to 359.
    qreal rotation;

    /// Frame line color of sunburst arcs.
    QColor frameLineColor;

    /// Selection line color of selected sunburst arcs.
    QColor selectionLineColor;

    /// Minimum size of arcs that have frames drawn.
    int frameSizeThreshold;

    /// Defines zoom speed.
    static const qreal zoomBaseFactor;

    /// The observed shift error in pixels that somehow exists and is corrected by this variable.
    static const QPointF drawingShiftError;
};
} // namespace cube_sunburst

#endif    /* TRANSFORMATION_DATA */
