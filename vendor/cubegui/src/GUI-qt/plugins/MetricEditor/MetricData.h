/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2020                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/



#ifndef _METRIC_DATA_H
#define _METRIC_DATA_H

#include "CubeTypes.h"
#include "Cube.h"

namespace cube
{
class CubeProxy;
class Metric;
}

namespace metric_editor
{
/**
 * MetricData contains all required data to create a cube metric
 */
class MetricData
{
public:
    MetricData( cube::CubeProxy* _cube = 0,
                cube::Metric*    _parent = 0 );
    cube::Metric*
    createMetric();
    void
    setCubePL( const QString& txt );
    bool
    isValid();

    // ===== getter and setter for single attributes  =====
    QString
    getCalculation() const;
    QString
    getInit_calculation() const;
    QString
    getCalculation_plus() const;
    QString
    getCalculation_minus() const;
    QString
    getCalculation_aggr() const;
    cube::TypeOfMetric
    getMetric_type() const;
    void
    setMetric_type( const cube::TypeOfMetric& value );
    QString
    getUniq_name() const;
    void
    setUniq_name( const QString& value );
    QString
    getDisp_name() const;
    void
    setDisp_name( const QString& value );
    QString
    getUom() const;
    QString
    getVal() const;
    void
    setVal( const QString& value );
    QString
    getUrl() const;
    QString
    getKind() const;
    QString
    getDescr() const;
    void
    setDescr( const QString& value );
    cube::Metric*
    getParentMetric() const;
    void
    setParentMetric( const QString& str );
    QString
    setCubePLExpression( const QString& str );
    QString
    setCubePLInitExpression( const QString& str );
    QString
    setCubePLAggrPlusExpression( const QString& str );
    QString
    setCubePLAggrMinusExpression( const QString& str );
    QString
    setCubePLAggrAggrExpression( const QString& str );
    void
    setUom( const QString& value );
    void
    setUrl( const QString& value );

    QString
    toString() const;

    bool
    isGhost() const;
    void
    setGhost( bool value );

    bool
    isRowWise() const;
    void
    setRowWise( bool value );

private:
    cube::CubeProxy* cube;
    cube::Metric*    parent_metric;

    cube::TypeOfMetric metric_type;

    QString uniq_name;
    QString disp_name;
    QString uom;
    QString val;
    QString url;
    QString descr;
    QString parent_metric_name; // name of the parent metric, saved in Settings

    QString calculation;
    QString init_calculation;
    QString calculation_plus;
    QString calculation_minus;
    QString calculation_aggr;

    bool is_ghost;
    bool is_rowwise;


    bool calculation_ok;
    bool calculation_init_ok;
    bool calculation_plus_ok;
    bool calculation_minus_ok;
    bool calculation_aggr_ok;

    cube::Metric*
    getMetric( const QString& uniqueName );
};
}
#endif
