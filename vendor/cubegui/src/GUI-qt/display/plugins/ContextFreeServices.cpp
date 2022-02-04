/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2018                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include "config.h"

#include "ContextFreeServices.h"
#include "CubeProxy.h"

using namespace cubegui;
using namespace cubepluginapi;

ContextFreeServices* ContextFreeServices::single = 0;

ContextFreeServices::ContextFreeServices()
{
}

ContextFreeServices*
ContextFreeServices::getInstance()
{
    if ( single == 0 )
    {
        single = new ContextFreeServices();
    }
    return single;
}

void
ContextFreeServices::openCube( cube::CubeProxy* cube )
{
    emit openCubeRequest( cube );
}

QWidget*
ContextFreeServices::getWidget()
{
    return widget;
}

void
ContextFreeServices::setWidget( QWidget* w )
{
    widget = w;
}

void
ContextFreeServices::setMessage( const QString& str, MessageType type )
{
    Globals::setStatusMessage( str, type );
}
