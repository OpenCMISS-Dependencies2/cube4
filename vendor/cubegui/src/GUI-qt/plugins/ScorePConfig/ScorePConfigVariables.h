/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2017                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef CUBE_SCOREP_CONFIG_VARIABLES_H
#define CUBE_SCOREP_CONFIG_VARIABLES_H

#include <QMap>
#include <QString>

namespace scorepconfigplugin
{
class ScorePConfigVariables
{
private:
    QMap<QString, QString> help;
    QString                none;

public:
    ScorePConfigVariables();

    const QString&
    getHelp( const QString& ) const;
};
};
#endif // CUBE_TOOLS_H
