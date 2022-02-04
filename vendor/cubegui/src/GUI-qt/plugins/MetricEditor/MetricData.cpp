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


#include "config.h"

#include <QTextStream>
#include <QDebug>

#include "CubeProxy.h"
#include "CubeMetric.h"
#include "CubePL1Driver.h"
#include "MetricData.h"
#include "Compatibility.h"

#ifdef HAS_QREGULAR_EXPRESSION
#include <QRegularExpression>
#define REGULAR_EXPRESSION QRegularExpression
#else
#include <QRegExp>
#define REGULAR_EXPRESSION QRegExp
#endif

using namespace metric_editor;

QString
MetricData::setCubePLExpression( const QString& str )
{
    std::string _to_test      = std::string( "<cubepl>" ) + str.toStdString() + std::string( "</cubepl>" );
    std::string error_message = "";
    calculation    = str;
    calculation_ok = cube ? cube->isCubePlExpressionValid( _to_test, error_message ) : false;
    return QString::fromStdString( error_message );
}

QString
MetricData::setCubePLInitExpression( const QString& str )
{
    std::string _to_test      = std::string( "<cubepl>" ) + str.toStdString() + std::string( "</cubepl>" );
    std::string error_message = "";
    init_calculation    = str;
    calculation_init_ok = cube ? cube->isCubePlExpressionValid( _to_test, error_message ) : false;
    return QString::fromStdString( error_message );
}

QString
MetricData::setCubePLAggrPlusExpression( const QString& str )
{
    std::string _to_test      = std::string( "<cubepl>" ) + str.toStdString() + std::string( "</cubepl>" );
    std::string error_message = "";
    calculation_plus    = str;
    calculation_plus_ok = cube ? cube->isCubePlExpressionValid( _to_test, error_message ) : false;
    return QString::fromStdString( error_message );
}

QString
MetricData::setCubePLAggrMinusExpression( const QString& str )
{
    std::string _to_test      = std::string( "<cubepl>" ) + str.toStdString() + std::string( "</cubepl>" );
    std::string error_message = "";
    calculation_minus    = str;
    calculation_minus_ok = cube ? cube->isCubePlExpressionValid( _to_test, error_message ) : false;
    return QString::fromStdString( error_message );
}

QString
MetricData::setCubePLAggrAggrExpression( const QString& str )
{
    std::string _to_test      = std::string( "<cubepl>" ) + str.toStdString() + std::string( "</cubepl>" );
    std::string error_message = "";
    calculation_aggr    = str;
    calculation_aggr_ok = cube ? cube->isCubePlExpressionValid( _to_test, error_message ) : false;
    return QString::fromStdString( error_message );
}

bool
MetricData::isValid()
{
    return cube &&
           ( metric_type == cube::CUBE_METRIC_POSTDERIVED ||
             metric_type == cube::CUBE_METRIC_PREDERIVED_EXCLUSIVE ||
             metric_type == cube::CUBE_METRIC_PREDERIVED_INCLUSIVE   )
           && ( !disp_name.isEmpty() )
           && ( !uniq_name.isEmpty() )
           && ( !calculation.isEmpty() )
           && ( calculation_ok )
           && ( calculation_init_ok )
           && ( calculation_plus_ok )
           && ( calculation_minus_ok )
           && ( calculation_aggr_ok );
}

cube::TypeOfMetric
MetricData::getMetric_type() const
{
    return metric_type;
}

void
MetricData::setMetric_type( const cube::TypeOfMetric& value )
{
    metric_type = value;
}

QString
MetricData::getUniq_name() const
{
    return uniq_name;
}

void
MetricData::setUniq_name( const QString& value )
{
    uniq_name = value;
}

QString
MetricData::getDisp_name() const
{
    return disp_name;
}

void
MetricData::setDisp_name( const QString& value )
{
    disp_name = value;
}

void
MetricData::setParentMetric( const QString& value )
{
    parent_metric_name = value;
    parent_metric      = getMetric( parent_metric_name );
}

QString
MetricData::getUom() const
{
    return uom;
}

void
MetricData::setUom( const QString& value )
{
    uom = value;
}

QString
MetricData::getVal() const
{
    return val;
}

void
MetricData::setVal( const QString& value )
{
    val = value;
}

QString
MetricData::getUrl() const
{
    return url;
}

void
MetricData::setUrl( const QString& value )
{
    url = value;
}

QString
MetricData::getDescr() const
{
    return descr;
}

void
MetricData::setDescr( const QString& value )
{
    descr = value;
}

cube::Metric*
MetricData::getParentMetric() const
{
    return parent_metric;
}

cube::Metric*
MetricData::createMetric()
{
    cube::Metric* metric = 0;
    if ( isValid() )
    {
        metric = cube->defineMetric(
            disp_name.toStdString(),
            uniq_name.toStdString(),
            ( parent_metric == NULL ) ? "DOUBLE" :  parent_metric->get_dtype(),
            uom.toStdString(),
            val.toStdString(),
            url.toStdString(),
            descr.toStdString(),
            parent_metric,
            metric_type,
            calculation.toStdString(),
            init_calculation.toStdString(),
            calculation_plus.toStdString(),
            calculation_minus.toStdString(),
            calculation_aggr.toStdString(),
            is_rowwise,
            is_ghost ? cube::CUBE_METRIC_GHOST : cube::CUBE_METRIC_NORMAL
            );
    }
    return metric;
}

MetricData::MetricData( cube::CubeProxy* _cube, cube::Metric* _parent ) : cube( _cube ), parent_metric( _parent )
{
    calculation_ok       = true;
    calculation_init_ok  = true;
    calculation_plus_ok  = true;
    calculation_minus_ok = true;
    calculation_aggr_ok  = true;
}

QString
MetricData::getCalculation() const
{
    return calculation;
}

QString
MetricData::getInit_calculation() const
{
    return init_calculation;
}

QString
MetricData::getCalculation_plus() const
{
    return calculation_plus;
}

QString
MetricData::getCalculation_minus() const
{
    return calculation_minus;
}

QString
MetricData::getCalculation_aggr() const
{
    return calculation_aggr;
}

/**
 * initializes the data from CubePL text
 */
void
MetricData::setCubePL( const QString& txt )
{
    enum TargetString
    {
        DM_PARSER_COMMENT               = -1,
        DM_PARSER_TYPE                  = 0,
        DM_PARSER_DISPLAY_NAME          = 1,
        DM_PARSER_UNIQUE_NAME           = 2,
        DM_PARSER_UOM                   = 3,
        DM_PARSER_URL                   = 4,
        DM_PARSER_DESCRIPTION           = 5,
        DM_PARSER_EXPRESSION            = 6,
        DM_PARSER_INIT_EXPRESSION       = 7,
        DM_PARSER_AGGR_PLUS_EXPRESSION  = 8,
        DM_PARSER_AGGR_MINUS_EXPRESSION = 9,
        DM_PARSER_AGGR_AGGR_EXPRESSION  = 10,
        DM_PARSER_PARENT_METRIC         = 11,
        DM_PARSER_GHOST_METRIC          = 12,
        DM_PARSER_ROWWISE_METRIC        = 13,
        SIZE                            = 14
    };

    TargetString       switcher = DM_PARSER_COMMENT;
    REGULAR_EXPRESSION dmetric_commentar( "^\\s*#" );
    REGULAR_EXPRESSION dmetric_type( "^\\s*metric type\\s*:" );
    REGULAR_EXPRESSION dmetric_display_name( "^\\s*display name\\s*:" );
    REGULAR_EXPRESSION dmetric_unique_name( "^\\s*unique name\\s*:" );
    REGULAR_EXPRESSION dmetric_uom( "^\\s*uom\\s*:" );
    REGULAR_EXPRESSION dmetric_url( "^\\s*url\\s*:" );
    REGULAR_EXPRESSION dmetric_description( "^\\s*description\\s*:" );
    REGULAR_EXPRESSION dmetric_expression( "^\\s*cubepl\\s+expression\\s*:" );
    REGULAR_EXPRESSION dmetric_init_expression( "^\\s*cubepl\\s+init\\s+expression\\s*:" );
    REGULAR_EXPRESSION dmetric_aggr_plus_expression( "^\\s*cubepl\\s+plus\\s+expression\\s*:" );
    REGULAR_EXPRESSION dmetric_aggr_minus_expression( "^\\s*cubepl\\s+minus\\s+expression\\s*:" );
    REGULAR_EXPRESSION dmetric_aggr_aggr_expression( "^\\s*cubepl\\s+aggr\\s+expression\\s*:" );
    REGULAR_EXPRESSION dmetric_parent_metric( "^\\s*parent metric\\s*:" );
    REGULAR_EXPRESSION dmetric_ghost_metric( "^\\s*ghost metric\\s*:" );
    REGULAR_EXPRESSION dmetric_rowwise_metric( "^\\s*rowwise metric\\s*:" );

    QString parts[ SIZE ];

    QString text = txt;
    text.replace( "\\\"", "\"" );
    QTextStream in( &text );
    while ( !in.atEnd() )
    {
        QString line = in.readLine().trimmed();

        if ( line.indexOf( dmetric_commentar ) != -1 )
        {
            continue;
        }
        else if ( line.indexOf( dmetric_type ) != -1 )
        {
            switcher = DM_PARSER_TYPE;
            line.remove( dmetric_type );
        }
        else if ( line.indexOf( dmetric_display_name ) != -1 )
        {
            switcher = DM_PARSER_DISPLAY_NAME;
            line.remove( dmetric_display_name );
        }
        else if ( line.indexOf( dmetric_unique_name ) != -1 )
        {
            switcher = DM_PARSER_UNIQUE_NAME;
            line.remove( dmetric_unique_name );
        }
        else if ( line.indexOf( dmetric_uom ) != -1 )
        {
            switcher = DM_PARSER_UOM;
            line.remove( dmetric_uom );
        }
        else if ( line.indexOf( dmetric_url ) != -1 )
        {
            switcher = DM_PARSER_URL;
            line.remove( dmetric_url );
        }
        else if ( line.indexOf( dmetric_description ) != -1 )
        {
            switcher = DM_PARSER_DESCRIPTION;
            line.remove( dmetric_description );
        }
        else if ( line.indexOf( dmetric_expression ) != -1 )
        {
            switcher = DM_PARSER_EXPRESSION;
            line.remove( dmetric_expression );
        }
        else if ( line.indexOf( dmetric_init_expression ) != -1 )
        {
            switcher = DM_PARSER_INIT_EXPRESSION;
            line.remove( dmetric_init_expression );
        }
        else if ( line.indexOf( dmetric_aggr_plus_expression ) != -1 )
        {
            switcher = DM_PARSER_AGGR_PLUS_EXPRESSION;
            line.remove( dmetric_aggr_plus_expression );
        }
        else if ( line.indexOf( dmetric_aggr_minus_expression ) != -1 )
        {
            switcher = DM_PARSER_AGGR_MINUS_EXPRESSION;
            line.remove( dmetric_aggr_minus_expression );
        }
        else if ( line.indexOf( dmetric_aggr_aggr_expression ) != -1 )
        {
            switcher = DM_PARSER_AGGR_AGGR_EXPRESSION;
            line.remove( dmetric_aggr_aggr_expression );
        }
        else if ( line.indexOf( dmetric_parent_metric ) != -1 )
        {
            switcher = DM_PARSER_PARENT_METRIC;
            line.remove( dmetric_parent_metric );
        }
        else if ( line.indexOf( dmetric_ghost_metric ) != -1 )
        {
            switcher = DM_PARSER_GHOST_METRIC;
            line.remove( dmetric_ghost_metric );
        }
        else if ( line.indexOf( dmetric_rowwise_metric ) != -1 )
        {
            switcher = DM_PARSER_ROWWISE_METRIC;
            line.remove( dmetric_rowwise_metric );
        }

        if ( switcher != DM_PARSER_COMMENT )
        {
            parts[ switcher ].append( line ).append( "\n" );
        }
    }

    if ( parts[ DM_PARSER_TYPE ].trimmed().toLower() == "postderived" )
    {
        metric_type = cube::CUBE_METRIC_POSTDERIVED;
    }
    else if ( parts[ DM_PARSER_TYPE ].trimmed().toLower() == "prederived_inclusive" )
    {
        metric_type = cube::CUBE_METRIC_PREDERIVED_INCLUSIVE;
    }
    else if ( parts[ DM_PARSER_TYPE ].trimmed().toLower() == "prederived_exclusive" )
    {
        metric_type = cube::CUBE_METRIC_PREDERIVED_EXCLUSIVE;
    }
    else
    {
        metric_type = cube::CUBE_METRIC_SIMPLE; // not valid for derived metrics
    }

    setDisp_name( parts[ DM_PARSER_DISPLAY_NAME ].trimmed() );
    setUniq_name( parts[ DM_PARSER_UNIQUE_NAME ].trimmed() );
    setUom( parts[ DM_PARSER_UOM ].trimmed() );
    setUrl( parts[ DM_PARSER_URL ].trimmed() );
    setDescr( parts[ DM_PARSER_DESCRIPTION ].trimmed() );
    setCubePLExpression( parts[ DM_PARSER_EXPRESSION ].trimmed() );
    setCubePLInitExpression( parts[ DM_PARSER_INIT_EXPRESSION ].trimmed() );
    setCubePLAggrMinusExpression( parts[ DM_PARSER_AGGR_MINUS_EXPRESSION ].trimmed() );
    setCubePLAggrPlusExpression( parts[ DM_PARSER_AGGR_PLUS_EXPRESSION ].trimmed() );
    setCubePLAggrAggrExpression( parts[ DM_PARSER_AGGR_AGGR_EXPRESSION ].trimmed() );
    setParentMetric( parts[ DM_PARSER_PARENT_METRIC ].trimmed() );
    setGhost( parts[ DM_PARSER_GHOST_METRIC ].trimmed() == "true" );
    setRowWise( parts[ DM_PARSER_ROWWISE_METRIC ].trimmed() == "true" );

    parent_metric = getMetric( parent_metric_name );
}


QString
MetricData::toString() const
{
    QString metricStr;
    if ( metric_type == cube::CUBE_METRIC_POSTDERIVED )
    {
        metricStr = "postderived";
    }
    else if ( metric_type == cube::CUBE_METRIC_PREDERIVED_INCLUSIVE )
    {
        metricStr = "prederived_inclusive";
    }
    else
    {
        metricStr = "prederived_exclusive";
    }

    QString sep = "\n\n";
    QString ret =   "metric type:" + metricStr + sep
                  + "display name:" + disp_name + sep
                  + "unique name:" + uniq_name + sep
                  + "parent metric:" + parent_metric_name + sep
                  + "ghost metric: " + ( is_ghost ? "true" : "false" ) + sep
                  + "rowwise metric: " + ( is_rowwise ? "true" : "false" ) + sep
                  + "uom:" + uom + sep
                  + "url:" + url + sep
                  + "description:" + descr + sep
                  + "cubepl expression:" + calculation + sep
                  + "cubepl init expression:" + init_calculation + sep;
    if ( metric_type == cube::CUBE_METRIC_PREDERIVED_INCLUSIVE || metric_type == cube::CUBE_METRIC_PREDERIVED_EXCLUSIVE )
    {
        ret +=  "cubepl aggr expression: " + calculation_aggr + sep;
        ret +=  "cubepl plus expression: " + calculation_plus + sep;
    }
    if ( metric_type == cube::CUBE_METRIC_PREDERIVED_INCLUSIVE )
    {
        ret +=  "cubepl minus expression: " + calculation_minus + sep;
    }

    ret.replace( "\"", "\\\"" );
    return ret;
}

bool
MetricData::isGhost() const
{
    return is_ghost;
}

void
MetricData::setGhost( bool value )
{
    is_ghost = value;
}

bool
MetricData::isRowWise() const
{
    return is_rowwise;
}

void
MetricData::setRowWise( bool value )
{
    is_rowwise = value;
}

cube::Metric*
MetricData::getMetric( const QString& uniqueName )
{
    if ( !cube || uniqueName.trimmed().isEmpty() )
    {
        return 0;
    }
    std::vector<cube::Metric*> metrics = cube->getMetrics();
    std::vector<cube::Metric*> ghost   = cube->getGhostMetrics();
    metrics.insert( metrics.end(), ghost.begin(), ghost.end() );

    for ( std::vector<cube::Metric*>::iterator m_iter = metrics.begin(); m_iter != metrics.end(); ++m_iter )
    {
        if ( *m_iter == NULL )
        {
            continue;
        }
        QString name = QString::fromStdString( ( *m_iter )->get_uniq_name() );
        if ( name == uniqueName )
        {
            return *m_iter;
        }
    }
    return 0;
}
