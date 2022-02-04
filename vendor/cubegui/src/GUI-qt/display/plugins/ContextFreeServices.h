/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2019                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef CONTEXTFREESERVICES_H
#define CONTEXTFREESERVICES_H

#include <QWidget>
#include "Globals.h"

namespace cube
{
class CubeProxy;
}

namespace cubegui
{
class MainWidget;
enum MessageType;
}

namespace cubepluginapi
{
class ContextFreeServices : public QObject
{
    Q_OBJECT
public:
    static ContextFreeServices*
    getInstance();

    /** closes this plugin and loads the given cube */
    void
    openCube( cube::CubeProxy* cube );

    /** returns a widget on the main screen which can be used by the plugin. If the plugin is closed, all
     * children of the widget are deleted.
     */
    QWidget*
    getWidget();

    /** Writes the given message to the status line at the bottom of the cube window */
    void
    setMessage( const QString&       str,
                cubegui::MessageType type = cubegui::Information );

    friend class cubegui::MainWidget;
signals:
    void
    openCubeRequest( cube::CubeProxy* cube );

private:
    ContextFreeServices();
    void
    setWidget( QWidget* ); // called by MainWidget

    static ContextFreeServices* single;
    QWidget*                    widget;
};
}
#endif // CONTEXTFREESERVICES_H
