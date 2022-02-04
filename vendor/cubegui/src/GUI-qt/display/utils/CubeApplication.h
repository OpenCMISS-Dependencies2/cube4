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

#ifndef CUBEAPPLICATION_H
#define CUBEAPPLICATION_H
#include <QApplication>

namespace cubegui
{
class PresentationCursor;
class MainWidget;

/** CubeApplication is required for OS X to open cube by double-clicking on a cube input file and for the presentation cursor
 */
class CubeApplication : public QApplication
{
public:
    CubeApplication( int&   argc,
                     char** argv );

    /** shows an additional mouse image next to the cursor, if enabled  */
    void
    setPresentationMode( bool enabled = true );

    void
    setMain( cubegui::MainWidget* m );

    /** required for OS X to open cube by double-clicking on a cube input file */
    bool
    event( QEvent* event );

    /** required for special cursor in presentation mode */
    bool
    eventFilter( QObject* obj,
                 QEvent*  event );

private:
    cubegui::MainWidget* mainWidget;
    PresentationCursor*  pcursor;
};
}

#endif // CUBEAPPLICATION_H
