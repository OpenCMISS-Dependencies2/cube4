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


#ifndef CALCULATION_H
#define CALCULATION_H

/** interface to use for parallel calculation of tasks */

namespace cubegui
{
class Task
{
public:
    /** if isReady returns false, the task is executed after all other tasks have been processed */
    virtual bool
    isReady()
    {
        return true;
    }

    /** this function has to be implemented with calculations, that should be done in parallel */
    virtual void
    calculate() = 0;

    virtual ~Task()
    {
    }
};
}

#endif
