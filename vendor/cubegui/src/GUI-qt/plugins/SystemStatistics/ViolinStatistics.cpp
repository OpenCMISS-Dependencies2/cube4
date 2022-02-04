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


#include "ViolinStatistics.h"
#include <math.h>
#include <cmath>
#include <algorithm>
#include <iterator>
#include <time.h>
#include <QDebug>
#include <limits>

#define epsilon 0.00001
#define pi 3.141592
#define e  2.718281

using namespace system_statistics;
using namespace std;
using cubegui::StatisticalInformation;

ViolinStatistics::ViolinStatistics( vector<double>& data )
{
    if ( data.size() == 0 )
    {
        return;
    }
    stat = StatisticalInformation( data );

    range = ( stat.getMaximum() - stat.getMinimum() );
    width = 0.06 * range;

    //kernel density estimation
    // selectKernel( UNIFORM, 1000, 0, minimum, maximum );
    // selectKernel(  UNIFORM, width, TOTAL, 1000., 0., minimum, maximum );
    optimalKernel( data, 1000, 0, stat.getMinimum(), stat.getMaximum() );
    // todo:check if required  valid_values = isStatisticalInformationValid();
}

void
ViolinStatistics::calculateKernelPoints( vector<double>& data, double minp, double maxp, double mind, double maxd )
{
    newdata.clear();
    newindex.clear();
    iterations_left.clear();
    iterations_right.clear();
    vector<double>::iterator pos, glob_pos;
    KernelConditionLower     k_condition_lower;
    KernelConditionUpper     k_condition_upper;
    k_condition_lower.setWidth( width );
    k_condition_upper.setWidth( width );

    for ( int i = minp; i >= maxp; --i )
    {
        tx = ( minp - i ) / ( minp - maxp ) * ( maxd - mind ) + mind;
        k_condition_lower.setTx( tx );
        k_condition_upper.setTx( tx );
        newdata.push_back( tx );
        glob_pos = std::lower_bound( data.begin(), data.end(), tx );
        newindex.push_back( glob_pos - data.begin() );
        pos = std::find_if( data.begin(), glob_pos, k_condition_upper );
        iterations_left.push_back( std::distance( pos, glob_pos ) );
        pos = std::find_if( glob_pos, data.end(), k_condition_lower );
        iterations_right.push_back( std::distance( glob_pos, pos ) );
    }
}

void
ViolinStatistics::optimalKernel( vector<double>& data, double minp, double maxp, double mind, double maxd )
{
    // clock_t startime, endtime; startime = clock();
    unsigned npixels = minp - maxp;
    kernel_density.clear();
    int jpos       = 0;
    int nrit_left  = 0;
    int nrit_right = 0;
    calculateKernelPoints( data, minp, maxp, mind, maxd );
    kernelDensityMin = std::numeric_limits<double>::max();
    kernelDensityMax = -kernelDensityMin;
    double summation = 0;
    // safety exit
    if ( newdata.size() == 0 )
    {
        return;
    }
    for ( unsigned itt = 0; itt < npixels; ++itt )
    {
        jpos       = newindex.at( itt );
        nrit_left  = iterations_left.at( itt );
        nrit_right = iterations_right.at( itt ) + 1;
        double rez = ( newdata.at( itt ) - data.at( jpos ) ) / width;
        summation = ( nrit_left + nrit_right ) * UniformKernel( rez );
        /*  switch ( used_kernel )
               {
                  default:
                  case UNIFORM:
                      summation = ( nrit_left + nrit_right ) * UniformKernel( rez );
                      break;
                  case TRIANGULAR:
                      summation = ( nrit_left + nrit_right ) *  TriangularKernel( rez, used_order );
                      break;
                  case EPANECHNIKOV:
                      summation = ( nrit_left + nrit_right ) *  EpanechnikovKernel( rez, used_order );
                      break;
                  case QUARTIC:
                      summation = ( nrit_left + nrit_right ) *  QuarticKernel( rez, used_order );
                      break;
                  case TRIWEIGHT:
                      summation = ( nrit_left + nrit_right ) * TriweightKernel( rez, used_order );
                      break;
                  case TRICUBE:
                      summation = ( nrit_left + nrit_right ) *  TricubeKernel( rez, used_order );
                      break;
                  case GAUSSIAN:
                      summation = ( nrit_left + nrit_right ) *  GaussianKernel( rez, used_order );
                      break;
                  case COSINE:
                      summation = ( nrit_left + nrit_right ) *  CosineKernel( rez, used_order );
                      break;
                  case LOGISTIC:
                      summation = ( nrit_left + nrit_right ) *  LogisticKernel( rez, used_order );
                      break;
                  case SIGMOID:
                      summation = ( nrit_left + nrit_right ) *  SigmoidKernel( rez, used_order );
                      break;
                  case SILVERMAN:
                      summation = ( nrit_left + nrit_right ) * SilvermanKernel( rez, used_order );
                      break;
               }*/
        // jpos=itt/(minp-maxp)*count;
        //taylor optimization

        //optimized for log2N
        /*  for ( int j = jpos; j < count; ++j )
               {
                  double rez = ( newdata.at( itt ) - data_batch.at( j ) ) / width;
                  if ( abs( rez ) > 1 )
                  {
                      break;
                  }
                  switch ( used_kernel )
                  {
                      default:
                      case UNIFORM:
                          summation += UniformKernel( rez );
                          break;
                      case TRIANGULAR:
                          summation += TriangularKernel( rez, used_order );
                          break;
                      case EPANECHNIKOV:
                          summation += EpanechnikovKernel( rez, used_order );
                          break;
                      case QUARTIC:
                          summation += QuarticKernel( rez, used_order );
                          break;
                      case TRIWEIGHT:
                          summation += TriweightKernel( rez, used_order );
                          break;
                      case TRICUBE:
                          summation += TricubeKernel( rez, used_order );
                          break;
                      case GAUSSIAN:
                          summation += GaussianKernel( rez, used_order );
                          break;
                      case COSINE:
                          summation += CosineKernel( rez, used_order );
                          break;
                      case LOGISTIC:
                          summation += LogisticKernel( rez, used_order );
                          break;
                      case SIGMOID:
                          summation += SigmoidKernel( rez, used_order );
                          break;
                      case SILVERMAN:
                          summation += SilvermanKernel( rez, used_order );
                          break;
                  }
                  //std::cout << j << " " << summation << std::endl;
               }
               for ( int j = jpos - 1; j >= 0; --j )
               {
                  double rez = ( newdata.at( itt ) - data_batch.at( j ) ) / width;
                  if ( abs( rez ) > 1 )
                  {
                      break;
                  }
                  switch ( used_kernel )
                  {
                      default:
                      case UNIFORM:
                          summation += UniformKernel( rez );
                          break;
                      case TRIANGULAR:
                          summation += TriangularKernel( rez, used_order );
                          break;
                      case EPANECHNIKOV:
                          summation += EpanechnikovKernel( rez, used_order );
                          break;
                      case QUARTIC:
                          summation += QuarticKernel( rez, used_order );
                          break;
                      case TRIWEIGHT:
                          summation += TriweightKernel( rez, used_order );
                          break;
                      case TRICUBE:
                          summation += TricubeKernel( rez, used_order );
                          break;
                      case GAUSSIAN:
                          summation += GaussianKernel( rez, used_order );
                          break;
                      case COSINE:
                          summation += CosineKernel( rez, used_order );
                          break;
                      case LOGISTIC:
                          summation += LogisticKernel( rez, used_order );
                          break;
                      case SIGMOID:
                          summation += SigmoidKernel( rez, used_order );
                          break;
                      case SILVERMAN:
                          summation += SilvermanKernel( rez, used_order );
                          break;
                          //   std::cout << j << " " << summation << std::endl;
                  }
               }*/

        summation = summation / ( stat.getCount() * width );

        if ( summation > kernelDensityMax )
        {
            kernelDensityMax = summation;
        }
        if ( summation < kernelDensityMin )
        {
            kernelDensityMin = summation;
        }
        kernel_density.push_back( summation );
        summation = 0;
    }
    // endtime    = clock();
    // double time_spent = ( double )( endtime - startime ) / CLOCKS_PER_SEC;
    // std::cout << time_spent << std::endl;
}

cubegui::StatisticalInformation
ViolinStatistics::getStatistics() const
{
    return stat;
}

std::vector<double> ViolinStatistics::getKernelDensity() const
{
    return kernel_density;
}

double
ViolinStatistics::getKernelDensityMin() const
{
    return kernelDensityMin;
}

double
ViolinStatistics::getKernelDensityMax() const
{
    return kernelDensityMax;
}

std::vector<double> ViolinStatistics::getNewdata() const
{
    return newdata;
}
/*
       void
       ViolinStatistics::calculateKernel()
       {
        kernel_density.clear();
        kernel_density_min = std::numeric_limits<double>::max();
        kernel_density_max = -kernel_density_min;
        double summation = 0;
        for ( unsigned itt = 0; itt < count; ++itt )
        {
            for ( unsigned j = 0; j < count; ++j )
            {

                double rez = ( data_batch.at( itt ) - data_batch.at( j ) ) / width;
                switch ( used_kernel )
                {
                    default:
                    case UNIFORM:
                        summation += UniformKernel( rez );
                        break;
                    case TRIANGULAR:
                        summation += TriangularKernel( rez );
                        break;
                    case EPANECHNIKOV:
                        summation += EpanechnikovKernel( rez );
                        break;
                    case QUARTIC:
                        summation += QuarticKernel( rez );
                        break;
                    case TRIWEIGHT:
                        summation += TriweightKernel( rez );
                        break;
                    case TRICUBE:
                        summation += TricubeKernel( rez );
                        break;
                    case GAUSSIAN:
                        summation += GaussianKernel( rez );
                        break;
                    case COSINE:
                        summation += CosineKernel( rez );
                        break;
                    case LOGISTIC:
                        summation += LogisticKernel( rez );
                        break;
                    case SIGMOID:
                        summation += SigmoidKernel( rez );
                        break;
                    case SILVERMAN:
                        summation += SilvermanKernel( rez );
                        break;
                }
            }
            summation = summation / ( count * width );


            switch ( order )
            {
                default:
                case ONE:
                    return 1;
                case TWO:
                    return 1-arg;
                case THREE:
                    return 1-arg;
                case FOUR:
                    return 1-arg;
                case FIVE:
                return 1-arg;
                case TOTAL:
                    return 1-arg;
            }
            if ( summation > kernel_density_max )
            {
                kernel_density_max = summation;
            }
            if ( summation < kernel_density_min )
            {
                kernel_density_min = summation;
            }



            kernel_density.push_back( summation );
            summation = 0;
        }
       }*/
/*
       void
       ViolinStatistics::selectOrder( TaylorOrder order )
       {
        if ( used_order != order )
        {
            used_order = order;
        }
       }

       void
       ViolinStatistics::make_width( double hparam, double minp, double maxp, double mind, double maxd )
       {
        if ( width != hparam * range )
        {
            width = hparam * range + epsilon;
            optimalKernel( minp, maxp, mind, maxd );
        }
       }
 */


/*void
       ViolinStatistics::selectKernel(  DensityKernel kernel, double hparam, TaylorOrder order, double minp, double maxp, double mind, double maxd )
       {
        if ( used_kernel != kernel || width != hparam * range || used_order != order )
        {
            used_kernel = kernel;
            width       = hparam * range + epsilon;
            used_order  = order;
            //  std::cout << used_kernel << " " << width << " " << used_order << std::endl;
            optimalKernel( minp, maxp, mind, maxd );
        }
       }*/

// Epanechnikov kernel
double
ViolinStatistics::EpanechnikovKernel( double arg, TaylorOrder order )
{
    double rez = 0.75 - 0.75 * arg * arg;
    switch ( order )
    {
        default:
        case ONE:
            return 0.75;
        case TWO:
            return rez;
        case THREE:
            return rez;
        case FOUR:
            return rez;
        case FIVE:
            return rez;
        case TOTAL:
            return 0.75 * ( 1 - arg * arg );
    }
}

//uniform kernel
double
ViolinStatistics::UniformKernel( double )
{
    return 0.5;
}

//triangular kernel
double
ViolinStatistics::TriangularKernel( double arg, TaylorOrder order )
{
    //taylor
    if ( arg < 0 )
    {
        switch ( order )
        {
            default:
            case ONE:
                return 1;
            case TWO:
                return 1 - arg;
            case THREE:
                return 1 - arg;
            case FOUR:
                return 1 - arg;
            case FIVE:
                return 1 - arg;
            case TOTAL:
                return 1 - arg;
        }
    }
    else // if ( arg >= 0 )
    {
        switch ( order )
        {
            default:
            case ONE:
                return 1;
            case TWO:
                return 1 + arg;
            case THREE:
                return 1 + arg;
            case FOUR:
                return 1 + arg;
            case FIVE:
                return 1 + arg;
            case TOTAL:
                return 1 + arg;
        }
    }
}

//quartic kernel
double
ViolinStatistics::QuarticKernel( double arg, TaylorOrder order )
{
    switch ( order )
    {
        default:
        case ONE:
            return 15. / 16.;
        case TWO:
            return 5. / 16. - 15. / 8. * arg * arg;
        case THREE:
            return 15. / 16. - 15. / 8. * arg * arg + 15. / 16. * pow( arg, 4 );
        case FOUR:
            return 15. / 16. - 15. / 8. * arg * arg + 15. / 16. * pow( arg, 4 );
        case FIVE:
            return 15. / 16. - 15. / 8. * arg * arg + 15. / 16. * pow( arg, 4 );
        case TOTAL:
            return 15. / 16. * ( 1 - arg * arg ) * ( 1 - arg * arg );
    }
}

//triweight kernel
/*double
       ViolinStatistics::TriweightKernel( double arg, TaylorOrder order )
       {
        switch ( order )
        {
            default:
            case ONE:
                return 35. / 32;
            case TWO:
                return 35. / 32. - 105. / 32. * arg * arg;
            case THREE:
                return 35. / 32. - 105. / 32 * arg * arg + 105. / 32. * pow( arg, 4 );
            case FOUR:
                return 35. / 32. - 105. / 32. * arg * arg + 105. / 32. * pow( arg, 4 ) - 35. / 32. * pow( arg, 6 );
            case FIVE:
                return 35. / 32. - 105. / 32. * arg * arg + 105. / 32. * pow( arg, 4 ) - 35. / 32. * pow( arg, 6 );
            case TOTAL:
                return 35. / 32. * ( 1 - arg * arg ) * ( 1 - arg * arg ) * ( 1 - arg * arg );
        }
       }
 */
double
ViolinStatistics::TriweightKernel( double arg )
{
    return 35. / 32. * ( 1 - arg * arg ) * ( 1 - arg * arg ) * ( 1 - arg * arg );
}
//tricube kernel
double
ViolinStatistics::TricubeKernel( double arg, TaylorOrder order  )
{
    double modul = abs( arg );
    /*if ( modul > 1 )
           {
            return 0;
           }*/
    //taylor
    if ( arg < 0 )
    {
        switch ( order )
        {
            default:
            case ONE:
                return 70. / 81.;
            case TWO:
                return 70. / 81 - 70. / 27. * pow( arg, 3 );
            case THREE:
                return 70. / 81 - 70. / 27. * pow( arg, 3 ) + 70. / 27. * pow( arg, 6 );
            case FOUR:
                return 70. / 81 - 70. / 27. * pow( arg, 3 ) + 70. / 27. * pow( arg, 6 ) + 70. / 81. * pow( arg, 9 );
            case FIVE:
                return 70. / 81 - 70. / 27. * pow( arg, 3 ) + 70. / 27. * pow( arg, 6 ) + 70. / 81. * pow( arg, 9 );
            case TOTAL:
                return ( 70.0 / 81.0 ) * ( 1 - modul * modul * modul ) * ( 1 - modul * modul * modul ) * ( 1 - modul * modul * modul );
        }
    }
    else // if ( arg >= 0 )
    {
        switch ( order )
        {
            default:
            case ONE:
                return 70. / 81.;
            case TWO:
                return 70. / 81 + 70. / 27. * pow( arg, 3 );
            case THREE:
                return 70. / 81 + 70. / 27. * pow( arg, 3 ) + 70. / 27. * pow( arg, 6 );
            case FOUR:
                return 70. / 81 + 70. / 27. * pow( arg, 3 ) + 70. / 27. * pow( arg, 6 ) + 70. / 81. * pow( arg, 9 );
            case FIVE:
                return 70. / 81 + 70. / 27. * pow( arg, 3 ) + 70. / 27. * pow( arg, 6 ) + 70. / 81. * pow( arg, 9 );
            case TOTAL:
                return ( 70.0 / 81.0 ) * ( 1 - modul * modul * modul ) * ( 1 - modul * modul * modul ) * ( 1 - modul * modul * modul );
        }
    }
}

//gaussian kernel
double
ViolinStatistics::GaussianKernel( double arg, TaylorOrder order )
{
    double ct, exp;
    exp = 0.5 * arg * arg;
    ct  = 1 / sqrt( 2 * pi );
    switch ( order )
    {
        case ONE:
        default:
            return 0.5 * sqrt( 2 / pi );
        case TWO:
            return 0.5 * sqrt( 2 / pi ) - 0.25 * sqrt( 2. / pi ) * arg * arg;
        case THREE:
            return 0.5 * sqrt( 2 / pi ) - 0.25 * sqrt( 2. / pi ) * arg * arg + 1. / 16. * sqrt( 2. / pi ) * pow( arg, 4 );
        case FOUR:
            return 0.5 * sqrt( 2 / pi ) - 0.25 * sqrt( 2. / pi ) * arg * arg + 1. / 16. * sqrt( 2. / pi ) * pow( arg, 4 ) - 1. / 96. * sqrt( 2. / pi ) * pow( arg, 6 );
        case FIVE:
            return 0.5 * sqrt( 2 / pi ) - 0.25 * sqrt( 2. / pi ) * arg * arg + 1. / 16. * sqrt( 2. / pi ) * pow( arg, 4 ) - 1. / 96. * sqrt( 2. / pi ) * pow( arg, 6 ) + 1. / 768. * sqrt( 2. / pi ) * pow( arg, 8 );
        case TOTAL:
            return ct * pow( e, -exp );
    }
}

//cosine kernel
double
ViolinStatistics::CosineKernel( double arg, TaylorOrder order )
{
    switch ( order )
    {
        default:
        case ONE:
            return 0.25 * pi;
        case TWO:
            return 0.25 * pi - 1. / 32. * pow( pi, 3 ) * arg * arg;
        case THREE:
            return 0.25 * pi - 1. / 32. * pow( pi, 3 ) * arg * arg + 1. / 1536. * pow( pi, 4 ) * pow( arg, 4 );
        case FOUR:
            return 0.25 * pi - 1. / 32. * pow( pi, 3 ) * arg * arg + 1. / 1536. * pow( pi, 4 ) * pow( arg, 4 ) - 1. / 184320. * pow( pi, 7 ) * pow( arg, 6 );
        case FIVE:
            return 0.25 * pi - 1. / 32. * pow( pi, 3 ) * arg * arg + 1. / 1536. * pow( pi, 4 ) * pow( arg, 4 ) - 1. / 184320. * pow( pi, 7 ) * pow( arg, 6 ) + 1. / 41287680. * pow( pi, 9 ) * pow( arg, 8 );
        case TOTAL:
            return ( pi / 4.0 ) * cos( pi * arg * 0.5 );
    }
}

//logistic kernel
double
ViolinStatistics::LogisticKernel( double arg, TaylorOrder order )
{
    switch ( order )
    {
        default:
        case ONE:
            return 0.25;
        case TWO:
            return 0.25 - 1. / 16. * arg * arg;
        case THREE:
            return 0.25 - 1. / 16. * arg * arg + 1. / 96. * pow( arg, 4 );
        case FOUR:
            return 0.25 - 1. / 16. * arg * arg + 1. / 96. * pow( arg, 4 ) - 17. / 11520. * pow( arg, 6 );
        case FIVE:
            return 0.25 - 1. / 16. * arg * arg + 1. / 96. * pow( arg, 4 ) - 17. / 11520. * pow( arg, 6 ) + 31. / 161280. * pow( arg, 8 );
        case TOTAL:
            return 1.0 / ( pow( e, arg ) + 2 + pow( e, -arg ) );
    }
}

//sigmoid kernel
double
ViolinStatistics::SigmoidKernel( double arg, TaylorOrder order )
{
    switch ( order )
    {
        default:
        case ONE:
            return 1. / pi;
        case TWO:
            return 1. / pi - 1. / ( 2 * pi ) * arg * arg;
        case THREE:
            return 0.25 - 1. / ( 2 * pi ) * arg * arg + 5. / ( 24 * pi ) * pow( arg, 4 );
        case FOUR:
            return 0.25 - 1. / ( 2 * pi ) * arg * arg + 5. / ( 24 * pi ) * pow( arg, 4 ) - 61. / ( 72 * pi ) * pow( arg, 6 );
        case FIVE:
            return 0.25 - 1. / ( 2 * pi ) * arg * arg + 5. / ( 24 * pi ) * pow( arg, 4 ) - 61. / ( 72 * pi ) * pow( arg, 6 ) + 277. / ( 8064 * pi ) * pow( arg, 8 );
        case TOTAL:
            return ( 2.0 / pi ) * ( 1.0 / ( pow( e, arg ) + pow( e, -arg ) ) );
    }
}

//silverman kernel
double
ViolinStatistics::SilvermanKernel( double arg, TaylorOrder order )
{
    double modul = abs( arg );
    /*if ( modul > 1 )
           {
            return 0;
           }
           double rez;
           rez = pow( e, -( modul / sqrt( 2 ) ) ) * sin( modul / sqrt( 2 ) + 0.25 * pi );
           return 0.5 * rez;*/
    if ( arg < 0 )
    {
        switch ( order )
        {
            default:
            case ONE:
                return sqrt( 2. ) / 4.;
            case TWO:
                return sqrt( 2. ) / 4. - sqrt( 2. ) / 8. * arg * arg;
            case THREE:
                return sqrt( 2. ) / 4. - sqrt( 2. ) / 8. * arg * arg - 1. / 12. * pow( arg, 3 );
            case FOUR:
                return sqrt( 2. ) / 4. - sqrt( 2. ) / 8. * arg * arg - 1. / 12. * pow( arg, 3 ) - 1. / 96. * sqrt( 2. ) * pow( arg, 4 );
            case FIVE:
                return sqrt( 2. ) / 4. - sqrt( 2. ) / 8. * arg * arg - 1. / 12. * pow( arg, 3 ) - 1. / 96. * sqrt( 2. ) * pow( arg, 4 ) + 1. / 2880. * sqrt( 2. ) * pow( arg, 6 );
            case TOTAL:
                return pow( e, -( modul / sqrt( 2. ) ) ) * sin( modul / sqrt( 2. ) + 0.25 * pi );
        }
    }
    else // if ( arg >= 0 )
    {
        switch ( order )
        {
            default:
            case ONE:
                return sqrt( 2. ) / 4.;
            case TWO:
                return sqrt( 2. ) / 4. - sqrt( 2. ) / 8. * arg * arg;
            case THREE:
                return sqrt( 2. ) / 4. - sqrt( 2. ) / 8. * arg * arg + 1. / 12. * pow( arg, 3 );
            case FOUR:
                return sqrt( 2. ) / 4. - sqrt( 2. ) / 8. * arg * arg + 1. / 12. * pow( arg, 3 ) - 1. / 96. * sqrt( 2. ) * pow( arg, 4 );
            case FIVE:
                return sqrt( 2. ) / 4. - sqrt( 2. ) / 8. * arg * arg + 1. / 12. * pow( arg, 3 ) - 1. / 96. * sqrt( 2. ) * pow( arg, 4 ) + 1. / 2880. * sqrt( 2. ) * pow( arg, 6 );
            case TOTAL:
                return pow( e, -( modul / sqrt( 2. ) ) ) * sin( modul / sqrt( 2. ) + 0.25 * pi );
        }
    }
}
