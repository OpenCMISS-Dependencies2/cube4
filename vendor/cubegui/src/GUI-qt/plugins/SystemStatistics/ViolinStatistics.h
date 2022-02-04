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


#ifndef VIOLINSTATISTICS_H
#define VIOLINSTATISTICS_H

#include <cstdlib>
#include <vector>
#include <iostream>
#include <cmath>
#include "StatisticalInformation.h"

namespace system_statistics
{
enum DensityKernel { UNIFORM = 1, TRIANGULAR = 2, EPANECHNIKOV = 3, QUARTIC = 4, TRIWEIGHT = 5, GAUSSIAN = 7, COSINE = 8, LOGISTIC = 9, SIGMOID = 10, SILVERMAN = 11, TRICUBE = 6 };
enum TaylorOrder { TOTAL = 0, ONE = 1, TWO = 2, THREE = 3, FOUR = 4, FIVE = 5 };

//class for kernel condition stop in newdata-olddata
class KernelConditionLower
{
public:
    double _tx, _width;
    void
    setWidth( double val )
    {
        _width = val;
    }

    void
    setTx( double val )
    {
        _tx = val;
    }

    bool
    operator()
        ( double val )
    {
        if ( fabs( ( _tx - val ) ) / _width > 1.  )
        {
            return 1;
        }
        return 0;
    }
};

class KernelConditionUpper
{
public:
    double _tx, _width;
    void
    setWidth( double val )
    {
        _width = val;
    }

    void
    setTx( double val )
    {
        _tx = val;
    }

    bool
    operator()
        ( double val )
    {
        if ( fabs( ( _tx - val ) ) / _width < 1.  )
        {
            return 1;
        }
        return 0;
    }
};


class ViolinStatistics
{
public:
    ViolinStatistics()
    {
    };
    ViolinStatistics( std::vector<double>& data );

    cubegui::StatisticalInformation
    getStatistics() const;

    std::vector<double>
    getKernelDensity() const;

    double
    getKernelDensityMin() const;

    double
    getKernelDensityMax() const;

    std::vector<double>
    getNewdata() const;

private:
    //the kernels
    double
    UniformKernel( double  );

    double
    TriangularKernel( double,
                      TaylorOrder order = TOTAL );

    double
    EpanechnikovKernel( double,
                        TaylorOrder order = TOTAL );
    double
    QuarticKernel( double,
                   TaylorOrder order = TOTAL );

    /* double
         TriweightKernel( double, TaylorOrder );*/
    double
    TriweightKernel( double  );
    double
    GaussianKernel( double,
                    TaylorOrder order = TOTAL );
    double
    CosineKernel( double,
                  TaylorOrder order = TOTAL  );
    double
    LogisticKernel( double,
                    TaylorOrder order = TOTAL  );
    double
    SigmoidKernel( double,
                   TaylorOrder order = TOTAL  );
    double
    SilvermanKernel( double,
                     TaylorOrder order = TOTAL );
    double
    TricubeKernel( double,
                   TaylorOrder order = TOTAL );

    //kernel_selector
    void
    selectKernel( DensityKernel kernel,
                  double        width,
                  TaylorOrder   order,
                  double        minp,
                  double        maxp,
                  double        mind,
                  double        maxd );
    void
    calculateKernel();
    void
    calculateKernelPoints( std::vector<double>& data_batch,
                           double               minp,
                           double               maxp,
                           double               mind,
                           double               maxd );
    void
    optimalKernel( std::vector<double>& data_batch,
                   double               minp,
                   double               maxp,
                   double               mind,
                   double               maxd );

    //width function
    void
    make_width( double,
                double,
                double,
                double,
                double );
    void
    selectOrder( TaylorOrder order );

    bool
    checkKernelcondition( double );

    //-----------------------------------------------------------------------------------------

    cubegui::StatisticalInformation stat;
    double                          range;
    double                          width;
    std::vector<double>             newdata;
    std::vector<double>             kernel_density;
    std::vector<double>             newindex;
    double                          kernelDensityMax;
    double                          kernelDensityMin;
    double                          new_data_max;
    double                          new_data_min;
    DensityKernel                   used_kernel;
    TaylorOrder                     used_order;
    std::vector<double>             iterations_left;
    std::vector<double>             iterations_right;
    double                          tx;
};
}

#endif // VIOLINSTATISTICS_H
