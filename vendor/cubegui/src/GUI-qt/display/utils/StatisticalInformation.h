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



#ifndef STATISTICAL_H
#define STATISTICAL_H

#include "Constants.h"

#include <utility>
#include <fstream>
#include <string>
#include <list>
#include <QStringList>
#include <vector>

/** used by Statistics and Boxplot */
namespace cubegui
{
class StatisticalInformation
{
public:
    StatisticalInformation( std::ifstream& theFile );
    StatisticalInformation();
    StatisticalInformation( int         theCount,
                            double      theSum,
                            double      theMean,
                            double      min,
                            double      q1,
                            double      theMedian,
                            double      q3,
                            double      max,
                            double      theVariance,
                            std::string theName = "",
                            std::string theUom = "" );
    /** Calculates the statistical data (min, max, mean ...) from the given list of values
     * @param data list of values */
    StatisticalInformation( std::vector<double>& data );

    QStringList
    print( std::string const& patternName = std::string( "" ),
           std::string const  uom = std::string( "" ),
           PrecisionFormat    format = FORMAT_DOUBLE
           ) const;
    int
    getCount() const;
    double
    getSum() const;
    double
    getMean() const;
    double
    getMinimum() const;
    double
    getQ1() const;
    double
    getMedian() const;
    double
    getQ3() const;
    double
    getMaximum() const;
    double
    getVariance() const;
    std::string
    getName() const;

    /** returns true, if the statistical values are valid, @see getDataStatusDescription() */
    bool
    isValid() const;

    /** if isValid() returns false, this function returns the reasons why the data is considered as invalid */
    QStringList
    getDataStatusDescription() const;

private:
    void
    checkValidity();

    int         count;
    double      sum, mean, minimum, q1, median, q3, maximum, variance;
    QStringList data_status_description;
    std::string name;
    std::string uom; // unit of measurement

    bool valid_values;
    bool statInformation;
};
}
#endif
