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


#ifndef SUNBURST_CURSOR_DATA_H
#define SUNBURST_CURSOR_DATA_H

// Qt includes
#include <QPoint>

namespace cube_sunburst
{
/// A simple extension for QPoint which also includes all relevant cursor information apart from the touched item.
class SunburstCursorData : public QPoint
{
public:
    // public member methods
    SunburstCursorData();
    SunburstCursorData( int level,
                        int index );

    /// Just a more intuitive access to the level than x().
    int
    level();

    /// Just a more intuitive access to the level than y().
    int
    index();

    bool
    getButtonTouched();

    void
    setButtonTouched( bool value );

    bool
    getNearLowerBorder();

    void
    setNearLowerBorder( bool value );

private:
    // private member attributes
    /// Information whether the cursor also touches the button area of the item.
    bool buttonTouched;

    /// True if the cursor touches the item at the side near the lower angle border.
    bool nearLowerBorder;
};
} // namespace cube_sunburst

#endif    /* SUNBURST_CURSOR_DATA_H */
