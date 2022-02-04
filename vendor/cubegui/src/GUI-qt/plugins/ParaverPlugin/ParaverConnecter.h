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


#ifndef PARAVER_CONNECTER_H
#define PARAVER_CONNECTER_H

#include <string>

class ParaverConnecter
{
public:
    ParaverConnecter( std::string const& file,
                      std::string const& configFile );
    virtual std::string
    InitiateAndOpenTrace();
    virtual std::string
    ZoomIntervall( double start,
                   double end,
                   int    zoomStep );
    virtual bool
    IsActive() const;

    virtual
    ~ParaverConnecter();
private:
    int         childID;
    std::string fileName;
    std::string configFileName;
    std::string commFileName;
};

#endif
