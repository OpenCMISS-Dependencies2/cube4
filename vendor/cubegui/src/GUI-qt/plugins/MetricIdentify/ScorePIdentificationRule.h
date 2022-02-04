/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2021                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef SCOREP_METRIC_INDETIFICATION_RULE_H
#define SCOREP_METRIC_INDETIFICATION_RULE_H

#include <string>
#include "IdentificationRule.h"

namespace metric_identify
{
class ScorePMetricIdentificationRule : public MetricIdentificationRule
{
public:
    ScorePMetricIdentificationRule()
    {
        unique_names.push_back( "time" );
        unique_names.push_back( "visits" );
        unique_names.push_back( "min_time" );
        unique_names.push_back( "max_time" );
        unique_names.push_back( "bytes_sent" );
        unique_names.push_back( "bytes_rcvd" );
        unique_names.push_back( "bytes_received" );
        unique_names.push_back( "bytes_put" );
        unique_names.push_back( "bytes_get" );
        unique_names.push_back( "ALLOCATION_SIZE" );
        unique_names.push_back( "DEALLOCATION_SIZE" );
        unique_names.push_back( "bytes_leaked" );
        unique_names.push_back( "maximum_heap_memory_allocated" );
        unique_names.push_back( "number of threads" );
        unique_names.push_back( "hits" );
        unique_names.push_back( "task_migration_loss" );
        unique_names.push_back( "task_migration_win" );
        url_reg_rules.push_back( REGULAR_EXPRESSION( "@mirror@scorep_metrics" ) );
        url_reg_rules.push_back( REGULAR_EXPRESSION( "@mirror@scalasca_patterns" ) );
    }
};
};
#endif // CUBE_TOOLS_H
