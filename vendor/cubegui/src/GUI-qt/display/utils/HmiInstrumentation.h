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


#ifndef HMIINSTRUMENTATION_H
#define HMIINSTRUMENTATION_H

namespace cubegui
{
#ifdef MARKER

static const int CUSTOM_EVENT_START = 1001;
static const int CUSTOM_EVENT_END   = 1002;

#define START_TRANSITION()    QApplication::postEvent( this, new QEvent( QEvent::Type( CUSTOM_EVENT_START ) ) );
#define END_TRANSITION()      QApplication::postEvent( this, new QEvent( QEvent::Type( CUSTOM_EVENT_END ) ) );
#define DEFINE_SCOREP_FINALIZE()    extern "C" void scorep_finalize( void );
#define CALL_SCOREP_FINALIZE()   scorep_finalize();

#else

#define START_TRANSITION()
#define END_TRANSITION()
#define DEFINE_SCOREP_FINALIZE()
#define CALL_SCOREP_FINALIZE()
#endif
}
#endif // HMIINSTRUMENTATION_H
