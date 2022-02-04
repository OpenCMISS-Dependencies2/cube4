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


#ifndef METRIC_INDETIFICATION_RULE_H
#define METRIC_INDETIFICATION_RULE_H

#include <QMap>
#include "Compatibility.h"

#ifdef HAS_QREGULAR_EXPRESSION
#include <QRegularExpression>
#define REGULAR_EXPRESSION QRegularExpression
#else
#include <QRegExp>
#define REGULAR_EXPRESSION QRegExp
#endif

namespace metric_identify
{
class MetricIdentificationRule
{
protected:
    std::vector<std::string>        unique_names;
    std::vector<REGULAR_EXPRESSION> reg_rules;
    std::vector<REGULAR_EXPRESSION> url_reg_rules;     // this rule is applied not on uniq_name, but on url field.


public:
    MetricIdentificationRule()
    {
    }


    bool
    isApply( const cube::Metric* met ) const
    {
        if ( met == NULL )
        {
            return false;
        }
        std::string uniq_name   = met->get_uniq_name();
        QString     q_uniq_name = QString::fromStdString( uniq_name );

        for ( std::vector<std::string>::const_iterator iter = unique_names.begin();
              iter != unique_names.end();
              ++iter
              )
        {
            if ( uniq_name.compare( *iter ) == 0 && isApplyToURL( met ) )
            {
                return true;
            }
        }
        for ( std::vector<REGULAR_EXPRESSION>::const_iterator iter = reg_rules.begin();
              iter != reg_rules.end();
              ++iter
              )
        {
            if ( q_uniq_name.contains( *iter ) && isApplyToURL( met ) )
            {
                return true;
            }
        }



        return false;
    }

    bool
    isApplyToURL( const cube::Metric* met ) const
    {
        if ( met == NULL )
        {
            return false;
        }

        if ( url_reg_rules.size() == 0 )
        {
            return true; // return "yes" if no special rule is specified
        }
        std::string        url   = met->get_url();
        QString            q_url = QString::fromStdString( url );
        REGULAR_EXPRESSION empty( "^\\s*$" );     // empty string... then it should be as true
        if ( q_url.contains( empty ) )
        {
            return true;
        }

        for ( std::vector<REGULAR_EXPRESSION>::const_iterator iter = url_reg_rules.begin();
              iter != url_reg_rules.end();
              ++iter
              )
        {
            if ( q_url.contains( *iter ) )
            {
                return true;
            }
        }
        return false;
    }
};
};
#endif // CUBE_TOOLS_H
