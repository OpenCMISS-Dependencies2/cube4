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


#ifndef INFO_TOOL_TIP_H
#define INFO_TOOL_TIP_H

// Qt includes
#include <QFrame>
#include <QLabel>
#include <QMouseEvent>

namespace cube_sunburst
{
// modified copy from src/GUI-qt/plugins/SystemTopology/SystemTopologyDrawing.h
class InfoToolTip
    : public QFrame
{
    QString left;
    QString right;
    QLabel* left_label;
    QLabel* right_label;

public:
    InfoToolTip();

    void
    showInfo( const QPoint&      pos,
              const QStringList& tooltipText );

    // modifications:
protected:
    bool
    eventFilter( QObject* obj,
                 QEvent*  event );

private:
    // Needed to avoid cases where Qt calls the MouseMoveEvent despite the cursor position not changing.
    QPoint moveCursorCache;
};
} // namespace cube_sunburst

#endif    /* INFO_TOOL_TIP_H */
