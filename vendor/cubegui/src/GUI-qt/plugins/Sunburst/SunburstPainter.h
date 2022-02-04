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


#ifndef SUNBURST_PAINTER_H
#define SUNBURST_PAINTER_H

// Sunburst includes
#include "DataAccessFunctions.h"
#include "PluginServices.h"
#include "SunburstCursorData.h"
#include "SunburstShapeData.h"
#include "TransformationData.h"

namespace cube_sunburst
{
/// @brief Draws a sunburst of the given shape and transformation on the given painter.
///
/// @param shapeData
///     shapeData of a sunburst
/// @param transformationData
///     transformationData of a scene
/// @param painter
///     panter used to draw the sunburst
///
void
drawSunburst( SunburstCursorData& cursorData,
              SunburstShapeData&  shapeData,
              TransformationData& transformationData,
              QPainter&           painter );
} // namespace cube_sunburst

#endif    /* SUNBURST_PAINTER_H */
