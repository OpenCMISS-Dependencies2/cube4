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


/*-------------------------------------------------------------------------*/
/**
 *  @file    SunburstCursorData.cpp
 *  @ingroup CubeSunburst
 *  @brief   Definition of the class SunburstCursorData.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include "SunburstCursorData.h"

using namespace cube_sunburst;
SunburstCursorData::SunburstCursorData() : QPoint()
{
    buttonTouched = false;
}


SunburstCursorData::SunburstCursorData( int level, int index ) : QPoint( level, index )
{
    buttonTouched = false;
}


int
SunburstCursorData::level()
{
    return x();
}


int
SunburstCursorData::index()
{
    return y();
}


bool
SunburstCursorData::getButtonTouched()
{
    return buttonTouched;
}


void
SunburstCursorData::setButtonTouched( bool value )
{
    buttonTouched = value;
}


bool
SunburstCursorData::getNearLowerBorder()
{
    return nearLowerBorder;
}


void
SunburstCursorData::setNearLowerBorder( bool value )
{
    nearLowerBorder = value;
}
