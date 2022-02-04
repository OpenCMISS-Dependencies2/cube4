/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2016                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include <QWidget>
#ifndef ColorScale_H
#define ColorScale_H

namespace cubegui
{
/**
 * simple color scale widget
 */
class ColorScale : public QWidget
{
public:
    ColorScale();
    void
    paintEvent( QPaintEvent* );
};
}
#endif
