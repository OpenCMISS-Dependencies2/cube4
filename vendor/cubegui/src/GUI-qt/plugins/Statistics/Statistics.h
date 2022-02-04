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



#ifndef STATISTICS_H
#define STATISTICS_H



#include "CubeProxy.h"
#include "BoxPlot.h"
#include "CubeServices.h"
#include "PluginServices.h"

#include <utility>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <map>

namespace cubegui
{
class StatisticalInformation;
}

class Statistics
{
public:
    Statistics( cubepluginapi::PluginServices* service );
    bool
    existsStatFile() const;
    QString
    getStatFileName() const;
    bool
    existsStatistics( cube::Metric const* metric ) const;
    QStringList
    getStatistics( cube::Metric const* metric ) const;
    cubegui::StatisticalInformation
    getInfo( cube::Metric const* metric ) const;

    bool
    existsMaxSeverity( cube::Metric const* metric,
                       cube::Cnode const*  cnode = 0 ) const;
    bool
    existsMaxSeverity( cube::Metric const* metric,
                       cube::Cnode const*  cnode,
                       double&             enter,
                       double&             exit ) const;

    uint32_t
    findMaxSeverityId( cube::Metric const* metric ) const;

    void
    showMaxSeverityText( QWidget*            parent,
                         const QString&      label,
                         const cube::Metric* metric,
                         const cube::Cnode*  cnode = 0 ) const;

    ~Statistics();

private:
    struct SevereEvent
    {
        SevereEvent( std::ifstream& theFile );
        double   enter, exit;
        double   wastedTime;
        int      rank;
        uint32_t cnode_id;
    };
    typedef std::map<std::string,
                     std::pair<cubegui::StatisticalInformation,
                               std::vector<SevereEvent>
                               >
                     > severeEventsType;

    std::map<std::string,
             std::pair<cubegui::StatisticalInformation, std::vector<SevereEvent> >
             > severeEvents;

    SevereEvent const*
    findMaxSeverity( cube::Metric const* metric,
                     cube::Cnode const*  cnode ) const;

    QString                        fileName;
    bool                           fileExists;
    bool                           verbose;
    cubepluginapi::PluginServices* service;
};

#endif
