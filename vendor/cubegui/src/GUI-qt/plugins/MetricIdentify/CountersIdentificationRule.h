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


#ifndef COUNTER_METRIC_INDETIFICATION_RULE_H
#define COUNTER_METRIC_INDETIFICATION_RULE_H

#include <string>
#include "IdentificationRule.h"

namespace metric_identify
{
class CounterMetricIdentificationRule : public MetricIdentificationRule
{
public:
    CounterMetricIdentificationRule()
    {
        reg_rules.push_back( REGULAR_EXPRESSION( "^PAPI_" ) );
        reg_rules.push_back( REGULAR_EXPRESSION( "^ru_" ) );
        reg_rules.push_back( REGULAR_EXPRESSION( "^perf_" ) );
    }
};
};
#endif // CUBE_TOOLS_H
