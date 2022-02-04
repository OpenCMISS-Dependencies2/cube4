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



#ifndef _DERIVED_METRICS_COLLECTION_H
#define _DERIVED_METRICS_COLLECTION_H

#include <QString>
#include <QObject>
#include <QStringList>

namespace metric_editor
{
enum DERIVED_METRIC_KEY { DM_NO_METRIC = 0,
                          DM_KENOBI,
                          DM_REGION_NAME,
                          DM_LEAVES,
                          DM_ROOTS,
                          DM_LEVEL,
                          DM_RECURSIONS_LEVEL,
                          DM_NUM_RECURSIONS,
                          DM_MAX_GROUP_TIME,
                          DM_PERFECT_PARALL,
                          DM_PARALLELIZATION,
                          DM_SIZE };

/*-------------------------------------------------------------------------*/
/**
 * @file  DerivedMetricsCollection.h
 * @brief Declaration of the class DerivedMetricsCollection
 *
 * This header file provides the declaration of the class DerivedMetricsCollection.
 */
/*-------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------*/
/**
 * @class DerivedMetricsCollection
 * @brief Provides a collection of Help textes.
 *
 * This class provides a simple collection of derived metrics, which can  be used
 * in analysis.
 */
/*-------------------------------------------------------------------------*/

class DerivedMetricsCollection
{
public:

    /// returns text of derived metrics settign for clipboard
    static QString&
    getDerivedMetricText( int );

    static QStringList
    getDerivedMetricsNames();

    static int
    size()
    {
        return DM_SIZE;
    }

private:
    static QString derived_metrics[ DM_SIZE ];
    static QString derived_metrics_names[ DM_SIZE ];
};
}
#endif
