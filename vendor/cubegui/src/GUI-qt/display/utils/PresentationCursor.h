/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2020                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef PRESENTATIONCURSOR_H
#define PRESENTATIONCURSOR_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QTimer>
#include <map>

namespace cubegui
{
enum class CursorType { DEFAULT, LEFT, RIGHT, WHEEL };

class PresentationCursor : public QWidget
{
    Q_OBJECT
public:
    PresentationCursor();

    void
    setCursor( CursorType type,
               int        delay = 0 );

private slots:
    void
    updateCursor();

private:
    std::map<CursorType, QPixmap> pixmaps;
    QLabel*                       label;
    CursorType                    type;
};
}

#endif // PRESENTATIONCURSOR_H
