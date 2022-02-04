/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2020                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2009-2015                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/



#include "config.h"

#include "DataProvider.h"

#include "GraphWidgetGlobals.h"
#include <QDebug>
#include <iterator>
#include <algorithm>

using namespace cubepluginapi;

DataProvider* DataProvider::cubeDataProvider = NULL;

DataProvider::DataProvider( CubeRead* cube ) : QObject( NULL ) /*it should be explicitly deleted in the destructor of GraphWidget class*/
{
    metricNames.clear();
    this->cubeReader = cube;

    int metricNamesSize = cubeReader->getMetricNameSize();
    for ( int i = 0; i < metricNamesSize; i++ )
    {
        metricNames.append( cubeReader->getMetricName( i ) );
    }

    globalLimits.resize( this->cubeReader->getMetricNameSize() );

    for ( int i = 0; i < globalLimits.size(); i++ )
    {
        globalLimits[ i ].first = globalLimits[ i ].second = -1;
    }
    minValueIsSetManually = false;
    maxValueIsSetManually = false;
}

DataProvider*
DataProvider::getInstance( CubeRead* cube )
{
    if ( cubeDataProvider == NULL )
    {
        cubeDataProvider = new DataProvider( cube );
    }
    return cubeDataProvider;
}
void
DataProvider::generateHeatMapIteration( const QList<cube::Cnode*>& func_id, cube::CalculationFlavour calcType, PluginServices* service )
{
    cubeReader->createListOfSubCnodes( func_id );
    cubeReader->calcAndFill_subItr( calcType );
    this->heatMap = this->getAllResult_Itr();
    emit this->changeCalibrateStatus( true );
    emit this->vLimitsHaveChangedHeatmap( 0, this->getThreadsCount() );    // it should be modified. now assumes that only process on vertical ruler
    emit this->hLimitsHaveChangedHeatmap( 0, this->getIterationsCount() ); // it should be modified. now assumes that only process on vertical ruler
    emit this->heatMapUpdated( this->heatMap, cubeReader->getMinMaxValue_Itr().first, cubeReader->getMinMaxValue_Itr().second, service );
}
void
DataProvider::DisableHeatMap()
{
    emit this->heatMapDisable();
    emit this->changeCalibrateStatus( false );
    emit this->vLimitsHaveChangedHeatmap( -1, -1 );
    emit this->hLimitsHaveChangedHeatmap( -1, -1 );
}
void
DataProvider::DisableBarPlot()
{
    removeCubeDataElement();
    emit this->barPlotDisable();
    emit this->changeCalibrateStatus( false ); // not show axis
    emit this->limitsHaveChanged( -1.0, -1.0 );
    emit this->hLimitsHaveChangedHeatmap( -1, -1 );
}
void
DataProvider::setAddMetricSettings()
{
    emit this->changeCalibrateStatus( true ); // show axis
    QList<cube::Cnode*> temp = this->getIterationsFunc();

    if ( topStack == true ) // means keep top row of stack
    {
        topStack = false;   // back to default status : no click
    }
    else
    {
        removeTopCubeDataElement();
    }

    if ( this->getOpNameBarPlot() >= plotWidget::MathOpUnknown )
    {
        addMetric( cubeReader->getLastMetricUName(), plotWidget::Minimum, plotWidget::Filled, Qt::green, temp, this->getCalcType() );
    }
    else
    {
        addMetric( cubeReader->getLastMetricUName(), this->getOpNameBarPlot(), plotWidget::Filled, this->getColorBarPlot(), temp, this->getCalcType() );
    }
}
void
DataProvider::changePlotColor()
{
    changeColor( colorBarPlot );
}

void
DataProvider::addMetric( QString name, plotWidget::MathOp opName, plotWidget::Style style, QColor color, QList<cube::Cnode*>& func, cube::CalculationFlavour calcType )
{
    cube::Cnode* cnode = NULL;
    foreach( cnode, func )
    {
        if ( cnode == NULL )
        {
            // there's no data to process, i.e. no functions
            continue;
        }
    }
    if ( cnode == NULL )
    {
        // there's no data to process, i.e. no functions
        return;
    }
    // check if there are any visible elements
    bool someElementsAreVisible = false;
    for ( int i = 0; i < data.size(); i++ )
    {
        if ( data.at( i ).isVisible() )
        {
            someElementsAreVisible = true;
            break;
        }
    }

    if ( colorIsUsed( color ) )
    {
        QStringList colorNames = createColorPalettesBarplot();

        for ( int j = 0; j < colorNames.size(); j++ )
        {
            bool flag = false;
            for ( QList<CubeDataItem>::const_iterator i = data.constBegin(); i != data.constEnd(); i++ )
            {
                if ( ( *i ).getColor() == QColor( colorNames[ j ] ) )
                {
                    flag = true;
                    break;     // exit from inner loop
                }
            }
            if ( flag == false )     // not repeated color
            {
                color = QColor( colorNames[ j ] );
                break;     // exit from outer loop
            }
        }
        emit updateColorCombobox( color );
    }
    int metric = metricNames.indexOf( name );
    if ( someElementsAreVisible )
    {
        // try to retrieve result from cache as usual, the cache-retrieval part if no element is visible
        // works only when we know the limits of the original metric (see below)
        // 1. look for a metric in the cache that matches our current limits
        for ( int i = 0; i < data.size(); i++ )
        {
            if ( data[ i ].getMetricIndex() == metric &&
                 data[ i ].getMathOp() == opName &&
                 data[ i ].getfirstItrID() == cnode->get_id() &&
                 data[ i ].getcalcType() == calcType )
            {
                if ( data[ i ].isScaled() == false )
                {
                    continue;                         // never use not-scaled metric because it is just a reference for creating scaled metrics
                }
                else
                {
                    if ( data[ i ].getScaleMaxValue() == this->getCurrentLimitMaxValue() &&
                         data[ i ].getScaleMinValue() == this->getCurrentLimitMinValue() )
                    {
                        emit information( tr( "Result retrieved from cache!" ) );
                        data[ i ].setVisible( true );
                        data[ i ].setName( name );
                        data[ i ].setColor( color );
                        data[ i ].setStyle( style );
                        data[ i ].setZOrder( -1 );
                        reorderZData();
                        emit dataUpdated( data );
                        printMemoryContent();
                        return;
                    }
                }
            }
        }
    }
    //Globals::debug( "DataProvider" ) << "Nothing is cache" << endl;
    // 2. look for a unscaled metric that matches this index and mathop
    int originalMetricIndex = -1;
    for ( int i = 0; i < data.size(); i++ )
    {
        if ( data[ i ].getMetricIndex() == metric &&
             data[ i ].getMathOp() == opName &&
             data[ i ].getfirstItrID() == cnode->get_id() &&
             data[ i ].getcalcType() == calcType &&
             data[ i ].isScaled() == false )
        {
            originalMetricIndex = i;
        }
    }
    // 2.1. If it is not found generate it
    if ( originalMetricIndex == -1 )
    {
        CubeDataItem newcubeDataItem;
        newcubeDataItem.setMetricIndex( metric );
        newcubeDataItem.setMathOp( opName );
        newcubeDataItem.setName( name );
        newcubeDataItem.setColor( color );
        newcubeDataItem.setStyle( style );
        newcubeDataItem.setScaled( false );
        newcubeDataItem.setVisible( false );
        newcubeDataItem.setfirstItrID( cnode->get_id() );
        newcubeDataItem.setcalcType( calcType );
        double metricMax, metricMin;
        genMetric( metric, newcubeDataItem.getData(), opName, metricMax, metricMin, func,  calcType );
        newcubeDataItem.setMaxValue( metricMax );
        newcubeDataItem.setMinValue( metricMin );
        newcubeDataItem.setScaleMaxValue( -1 );
        newcubeDataItem.setScaleMinValue( -1 );

        // we need to check if its in-bound
        if ( data.size() == 0 )
        {
            // a check is needed to know if the user manually set the values
            if ( maxValueIsSetManually == false )
            {
                this->currentLimitMaxValue = newcubeDataItem.getMaxValue();
            }
            if ( minValueIsSetManually == false )
            {
                this->currentLimitMinValue = newcubeDataItem.getMinValue();
            }
        }
        data.append( newcubeDataItem );
        originalMetricIndex = data.size() - 1;
    }
    if ( someElementsAreVisible == false )
    {
        // 1. set the vertical limits to the metric's limits
        if ( minValueIsSetManually == false )
        {
            this->currentLimitMinValue = data[ originalMetricIndex ].getMinValue();
        }
        if ( maxValueIsSetManually == false )
        {
            this->currentLimitMaxValue = data[ originalMetricIndex ].getMaxValue();
        }
        // 2. try to retrieve results from cache
        // 2.1. look for a metric in the cache that matches our current limits
        for ( int i = 0; i < data.size(); i++ )
        {
            if ( data[ i ].getMetricIndex() == metric && data[ i ].getMathOp() == opName && data[ i ].getfirstItrID() == cnode->get_id() && data[ i ].getcalcType() == calcType )
            {
                if ( data[ i ].isScaled() == false )
                {
                    continue;                         // never use not-scaled metric because it is just a reference for creating scaled metrics
                }
                else
                {
                    if ( data[ i ].getScaleMaxValue() == this->getCurrentLimitMaxValue() &&
                         data[ i ].getScaleMinValue() == this->getCurrentLimitMinValue() )
                    {
                        emit information( tr( "Result retrieved from cache!" ) );
                        data[ i ].setVisible( true );
                        data[ i ].setName( name );
                        data[ i ].setColor( color );
                        data[ i ].setStyle( style );
                        data[ i ].setZOrder( -1 );
                        reorderZData();
                        emit dataUpdated( data );
                        printMemoryContent();
                        return;
                    }
                }
            }
        }
        // 2.2. At this point there is not results from cache, build a new metric with the new limits
        // (hint: the original metric might be created already)
    }
    // 2.3. finally check the boundaries' possible violation
    if ( data[ originalMetricIndex ].getMaxValue() > this->getCurrentLimitMaxValue() )
    {
        if ( maxValueIsSetManually )
        {
            emit maxValueIsCalculatedAutomatically();
            maxValueIsSetManually = false;
        }
        maximizeUpperLimit( data[ originalMetricIndex ].getMaxValue() );
    }
    if ( data[ originalMetricIndex ].getMinValue() < this->getCurrentLimitMinValue() )
    {
        if ( minValueIsSetManually )
        {
            emit minValueIsCalculatedAutomatically();
            minValueIsSetManually = false;
        }
        minimizeLowerLimit( data[ originalMetricIndex ].getMinValue() );
    }
    // 3. Create a new scaled metric and make it visible
    CubeDataItem scaledDataItem;

    scaledDataItem.setMetricIndex( metric );
    scaledDataItem.setMathOp( opName );
    scaledDataItem.setName( name );
    scaledDataItem.setColor( color );
    scaledDataItem.setStyle( style );
    scaledDataItem.setScaled( true );
    scaledDataItem.setScaleMaxValue( this->currentLimitMaxValue );
    scaledDataItem.setScaleMinValue( this->currentLimitMinValue );
    scaledDataItem.setVisible( true );
    scaledDataItem.setMinValue( data[ originalMetricIndex ].getMinValue() );
    scaledDataItem.setMaxValue( data[ originalMetricIndex ].getMaxValue() );
    scaledDataItem.setfirstItrID( cnode->get_id() );
    scaledDataItem.setcalcType( calcType );
    scaleData( originalMetricIndex, scaledDataItem.getData() );
    data.append( scaledDataItem );

    emit limitsHaveChanged( this->currentLimitMinValue, this->currentLimitMaxValue );
    emit this->hLimitsHaveChangedHeatmap( 0, this->getIterationsCount() ); // it should be modified. now assumes that only process on vertical ruler
    emit dataUpdated( data );
    printMemoryContent();
}

void
DataProvider::scaleData( int originalCubeDataItemIndex, QVector<double>& scaledData )
{
    CubeDataItem originalCubeDataItem = data[ originalCubeDataItemIndex ];
    int          dataSize             = originalCubeDataItem.getData().size();
    scaledData.resize( dataSize );
    // values are always normalized between [0,1] along with the current limits

    double tempLimitMin, tempLimitMax;

    tempLimitMin = this->getCurrentLimitMinValue();
    tempLimitMax = this->getCurrentLimitMaxValue();

    if ( tempLimitMax == tempLimitMin ) // no scale
    {
        for ( int i = 0; i < originalCubeDataItem.getData().size(); i++ )
        {
            scaledData[ i ] =  originalCubeDataItem.getData().at( i );
        }
    }
    else
    {
        for ( int i = 0; i < originalCubeDataItem.getData().size(); i++ )
        {
            scaledData[ i ] = ( originalCubeDataItem.getData().at( i ) - tempLimitMin ) / ( tempLimitMax - tempLimitMin );
        }
    }
}

void
DataProvider::maximizeUpperLimit( double newUpperLimit )
{
    this->currentLimitMaxValue = newUpperLimit;
    // 1. Check visible metrics, make them invisible, return to their original unscaled metrics, create new metrics out
    // of them with the new upper limit
    for ( int i = 0; i < data.size(); i++ )
    {
        if ( data[ i ].isVisible() )
        {
            if ( data[ i ].getScaleMaxValue() == newUpperLimit )
            {
                continue;
            }
            else
            {
                data[ i ].setVisible( false );
                bool noMetricIsFound = true;
                // search for a metric with the same metricID and mathOp that matches new limits
                for ( int j = 0; j < data.size(); j++ )
                {
                    if ( data[ j ].getMetricIndex() == data[ i ].getMetricIndex() &&
                         data[ j ].getMathOp() == data[ i ].getMathOp()  &&
                         data[ j ].getfirstItrID() == data[ i ].getfirstItrID()  &&
                         data[ j ].getcalcType() == data[ i ].getcalcType()  &&
                         data[ j ].isScaled() == true &&
                         data[ j ].getScaleMaxValue() == this->currentLimitMaxValue &&
                         data[ j ].getScaleMinValue() == this->currentLimitMinValue )
                    {
                        // make all other plots of the metric index invisible and make metric (j) visible
                        // FIXME Probably this is not needed because there is only one metric with a specific scale values
                        for ( int k = 0; k < data.size(); k++ )
                        {
                            if ( data[ k ].getMetricIndex() == data[ j ].getMetricIndex() && data[ k ].getMathOp() == data[ j ].getMathOp() && data[ k ].getfirstItrID() == data[ j ].getfirstItrID() && data[ k ].getcalcType() == data[ j ].getcalcType() )
                            {
                                data[ k ].setVisible( false );
                            }
                        }
                        data[ j ].setVisible( true );
                        // set metric (j) style and color to the old metric
                        data[ j ].setStyle( data[ i ].getStyle() );
                        data[ j ].setColor( data[ i ].getColor() );
                        noMetricIsFound = false;
                        break;
                    }
                }
                if ( noMetricIsFound )
                {
                    int originalMetricIndex = getIndexOfUnscaledMetric( data[ i ].getMetricIndex(), data[ i ].getMathOp(), data[ i ].getfirstItrID(), data[ i ].getcalcType() );
                    if ( originalMetricIndex == -1 )
                    {
                        // this code should never be executed!
                        qDebug() << "CubeDataProvider::maximizeUpperLimit original Metric for "
                            + QString::number( data[ i ].getMetricIndex() ) + " is not found!";
                    }
                    // a new metric should be created with the new scaled limits
                    CubeDataItem scaledDataItem;
                    scaledDataItem.setMetricIndex( data[ i ].getMetricIndex() );
                    scaledDataItem.setMathOp( data[ i ].getMathOp() );
                    scaledDataItem.setName( data[ i ].getName() );
                    scaledDataItem.setColor( data[ i ].getColor() );
                    scaledDataItem.setStyle( data[ i ].getStyle() );
                    scaledDataItem.setMinValue( data[ i ].getMinValue() );
                    scaledDataItem.setMaxValue( data[ i ].getMaxValue() );
                    scaledDataItem.setScaleMaxValue( this->currentLimitMaxValue );
                    scaledDataItem.setScaleMinValue( this->currentLimitMinValue );
                    scaledDataItem.setScaled( true );
                    scaledDataItem.setVisible( true );
                    scaledDataItem.setfirstItrID( data[ i ].getfirstItrID() );
                    scaledDataItem.setcalcType( data[ i ].getcalcType() );
                    scaleData( originalMetricIndex, scaledDataItem.getData() );
                    //  data.removeAt(i);////////////////////////////////////*********************
                    data.replace( i, scaledDataItem );
                    //  data.append( scaledDataItem );
                }
            }
        }
    }
    // optimization needed (boolean flag to indicate that a change has actually happened
    //    emit limitsHaveChanged(this->currentLimitMinValue, this->currentLimitMaxValue);
    emit dataUpdated( data );
}

void
DataProvider::minimizeLowerLimit( double newLowerLimit )
{
    this->currentLimitMinValue = newLowerLimit;
    // 1. Check visible metrics, make them invisible, return to their original unscaled metrics, create new metrics out
    // of them with the new upper limit
    for ( int i = 0; i < data.size(); i++ )
    {
        if ( data[ i ].isVisible() )
        {
            if ( data[ i ].getScaleMinValue() == newLowerLimit )
            {
                continue;
            }
            else
            {
                data[ i ].setVisible( false );
                bool noMetricIsFound = true;
                // search for a metric with the same metricID and mathOp that matches new limits
                for ( int j = 0; j < data.size(); j++ )
                {
                    if ( data[ j ].getMetricIndex() == data[ i ].getMetricIndex() &&
                         data[ j ].getMathOp() == data[ i ].getMathOp() &&
                         data[ j ].getfirstItrID() == data[ i ].getfirstItrID() &&
                         data[ j ].getcalcType() == data[ i ].getcalcType() &&
                         data[ j ].isScaled() == true &&
                         data[ j ].getScaleMaxValue() == this->currentLimitMaxValue &&
                         data[ j ].getScaleMinValue() == this->currentLimitMinValue )
                    {
                        // make all other plots of the metric index invisible and make metric (j) visible
                        // FIXME Probably this is not needed because there is only one metric with a specific scale values
                        for ( int k = 0; k < data.size(); k++ )
                        {
                            if ( data[ k ].getMetricIndex() == data[ j ].getMetricIndex() &&
                                 data[ k ].getMathOp() == data[ j ].getMathOp() &&
                                 data[ k ].getfirstItrID() == data[ j ].getfirstItrID() &&
                                 data[ k ].getcalcType() == data[ j ].getcalcType() )
                            {
                                data[ k ].setVisible( false );
                            }
                        }
                        data[ j ].setVisible( true );
                        // set metric (j) style and color to the old metric
                        data[ j ].setStyle( data[ i ].getStyle() );
                        data[ j ].setColor( data[ i ].getColor() );
                        noMetricIsFound = false;
                        break;
                    }
                }
                if ( noMetricIsFound )
                {
                    int originalMetricIndex = getIndexOfUnscaledMetric( data[ i ].getMetricIndex(), data[ i ].getMathOp(), data[ i ].getfirstItrID(), data[ i ].getcalcType() );
                    if ( originalMetricIndex == -1 )
                    {
                        // this code should never be executed!
                        qDebug() << "CubeDataProvider::minimizeLowerLimit original Metric for "
                            + QString::number( data[ i ].getMetricIndex() ) + " is not found!";
                    }
                    // a new metric should be created with the new scaled limits
                    CubeDataItem scaledDataItem;
                    scaledDataItem.setMetricIndex( data[ i ].getMetricIndex() );
                    scaledDataItem.setMathOp( data[ i ].getMathOp() );
                    scaledDataItem.setName( data[ i ].getName() );
                    scaledDataItem.setColor( data[ i ].getColor() );
                    scaledDataItem.setStyle( data[ i ].getStyle() );
                    scaledDataItem.setMinValue( data[ i ].getMinValue() );
                    scaledDataItem.setMaxValue( data[ i ].getMaxValue() );
                    scaledDataItem.setScaleMaxValue( this->currentLimitMaxValue );
                    scaledDataItem.setScaleMinValue( this->currentLimitMinValue );
                    scaledDataItem.setScaled( true );
                    scaledDataItem.setVisible( true );
                    scaledDataItem.setfirstItrID( data[ i ].getfirstItrID() );
                    scaledDataItem.setcalcType( data[ i ].getcalcType() );
                    scaleData( originalMetricIndex, scaledDataItem.getData() );
                    //  data.removeAt(i);////////////////////////////////////*********************
                    data.replace( i, scaledDataItem );
                    //  data.append( scaledDataItem );
                }
            }
        }
    }
    // optimization needed (boolean flag to indicate that a change has actually happened
    //    emit limitsHaveChanged(this->currentLimitMinValue, this->currentLimitMaxValue);
    emit dataUpdated( data );
}

int
DataProvider::getIndexOfUnscaledMetric( int metricIndex, plotWidget::MathOp mathOp, unsigned int itrID, cube::CalculationFlavour tempCalcType )
{
    for ( int i = 0; i < data.size(); i++ )
    {
        if ( data[ i ].isScaled() == false &&
             data[ i ].getMetricIndex() == metricIndex &&
             data[ i ].getMathOp() == mathOp &&
             data[ i ].getfirstItrID() == itrID &&
             data[ i ].getcalcType() == tempCalcType )
        {
            return i;
        }
    }
    return -1;
}

void
DataProvider::resetLimitMaxValue()
{
    maxValueIsSetManually = false;
    if ( data.size() == 0 )
    {
        return;
    }
    double maxValue               = 0;
    bool   someElementsAreVisible = false;
    for ( QList<CubeDataItem>::const_iterator iterator = data.constBegin(); iterator != data.constEnd(); iterator++ )
    {
        if ( ( *iterator ).isVisible() )
        {
            maxValue               = ( *iterator ).getMaxValue();
            someElementsAreVisible = true;
            break;
        }
    }
    for ( QList<CubeDataItem>::const_iterator iterator = data.constBegin(); iterator != data.constEnd(); iterator++ )
    {
        if ( ( *iterator ).isVisible() )
        {
            if ( ( *iterator ).getMaxValue() > maxValue )
            {
                maxValue = ( *iterator ).getMaxValue();
            }
        }
    }
    if ( someElementsAreVisible )
    {
        maximizeUpperLimit( maxValue );
        emit this->hLimitsHaveChangedHeatmap( 0, this->getIterationsCount() ); // it should be modified. now assumes that only process on vertical ruler
        emit limitsHaveChanged( this->currentLimitMinValue, this->currentLimitMaxValue );
        printMemoryContent();
    }
}

void
DataProvider::resetLimitMinValue()
{
    minValueIsSetManually = false;
    if ( data.size() == 0 )
    {
        return;
    }
    double minValue               = 0;
    bool   someElementsAreVisible = false;
    for ( QList<CubeDataItem>::const_iterator iterator = data.constBegin(); iterator != data.constEnd(); iterator++ )
    {
        if ( ( *iterator ).isVisible() )
        {
            minValue               = ( *iterator ).getMinValue();
            someElementsAreVisible = true;
            break;
        }
    }
    for ( QList<CubeDataItem>::const_iterator iterator = data.constBegin(); iterator != data.constEnd(); iterator++ )
    {
        if ( ( *iterator ).isVisible() )
        {
            if ( ( *iterator ).getMinValue() < minValue )
            {
                minValue = ( *iterator ).getMinValue();
            }
        }
    }
    if ( someElementsAreVisible )
    {
        minimizeLowerLimit( minValue );
        emit this->hLimitsHaveChangedHeatmap( 0, this->getIterationsCount() ); // it should be modified. now assumes that only process on vertical ruler
        emit limitsHaveChanged( this->currentLimitMinValue, this->currentLimitMaxValue );
        printMemoryContent();
    }
}

void
DataProvider::genMetric( int metric, QVector<double>& result, plotWidget::MathOp opType, double& relativeMax, double& relativeMin, QList<cube::Cnode*>& func, cube::CalculationFlavour calcType )
{
    // cout<<"C11"<<endl;
    cubeReader->createListOfSubCnodes( func );
    cubeReader->calcAndFill_subItr( calcType );

    switch ( opType )
    {
        case plotWidget::Average:
        { calcAvg( metric, result, relativeMax, relativeMin );
        } break;
        case plotWidget::Minimum:
        { calcMin( metric, result, relativeMax, relativeMin );
        } break;
        case plotWidget::Maximum:
        { calcMax( metric, result, relativeMax, relativeMin );
        } break;
        case plotWidget::Median:
        { calcMed( metric, result, relativeMax, relativeMin );
        } break;
        case plotWidget::FirstQuartile:
        { calcQ1( metric, result, relativeMax, relativeMin );
        } break;
        case plotWidget::ThirdQuartile:
        { calcQ3( metric, result, relativeMax, relativeMin );
        } break;
        case plotWidget::MathOpUnknown:
        { qDebug( "Trying to handle unknown mathOp in CubeDataProvider::genMetric" );
        }
    }
}

void
DataProvider::calcAvg( int metric, QVector<double>& result, double& maxAvg, double& minAvg )
{
    ( void )metric;
    int numIterations = cubeReader->getIterations();
    result.clear();
    result.resize( numIterations );
    double          subSum = 0.0f;
    QVector<double> data   = cubeReader->getThreadsV( 0 /*first vector*/ );
    foreach( double item, data )
    subSum     += item;
    result[ 0 ] = subSum / ( double )data.size();
    maxAvg      = minAvg = result[ 0 ];
    for ( int i = 1; i < numIterations; i++ ) // &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
    {
        subSum = 0.0f;
        data   = cubeReader->getThreadsV( i );
        for ( int j = 0; j < data.size(); j++ )
        {
            subSum += data[ j ];
        }
        result[ i ] = subSum / ( double )data.size();
        if ( maxAvg < result[ i ] )
        {
            maxAvg = result[ i ];
        }
        if ( minAvg > result[ i ] )
        {
            minAvg = result[ i ];
        }
    }
}

void
DataProvider::calcMin( int metric, QVector<double>& result, double& maxMin, double& minMin )
{
    int numIterations = cubeReader->getIterations();
    result.clear();
    result.resize( numIterations );

    QVector<double> data = cubeReader->getThreadsV( 0 /*first vector*/ );
    result[ 0 ] = data[ 0 ];
    for ( int j = 0; j < data.size(); j++ )
    {
        if ( result[ 0 ] > data[ j ] )
        {
            result[ 0 ] = data[ j ];
        }
    }
    maxMin = minMin = result[ 0 ];
    for ( int i = 1; i < numIterations; i++ ) // for each iteration &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
    {
        QVector<double> data = cubeReader->getThreadsV( i );
        result[ i ] = data[ 0 ];
        for ( int j = 0; j < data.size(); j++ ) // get the minimum of iteration &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
        {
            if ( result[ i ] > data[ j ] )
            {
                result[ i ] = data[ j ];
            }
        }
        if ( maxMin < result[ i ] )
        {
            maxMin = result[ i ];
        }
        if ( minMin > result[ i ] )
        {
            minMin = result[ i ];
        }
    }
    globalLimits[ metric ].first = minMin;
}

void
DataProvider::calcMax( int metric, QVector<double>& result, double& maxMax, double& minMax )
{
    int numIterations = cubeReader->getIterations();
    result.clear();
    result.resize( numIterations );
    // QVector<double> data = cubeReader->getProcessesV( metric, 1 /*first vector*/ );
    QVector<double> data = cubeReader->getThreadsV( 0 /*first vector*/ );
    result[ 0 ] = data[ 0 ];
    for ( int j = 0; j < data.size(); j++ ) // &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
    {
        if ( result[ 0 ] < data[ j ] )
        {
            result[ 0 ] = data[ j ];
        }
    }
    minMax = maxMax = result[ 0 ];
    for ( int i = 1; i < numIterations; i++ )
    {
        //  QVector<double> data = cubeReader->getProcessesV( metric, i );
        QVector<double> data = cubeReader->getThreadsV( i );
        result[ i ] = data[ 0 ];
        for ( int j = 0; j < data.size(); j++ ) // &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
        {
            if ( result[ i ] < data[ j ] )
            {
                result[ i ] = data[ j ];
            }
        }
        if ( maxMax < result[ i ] )
        {
            maxMax = result[ i ];
        }
        if ( minMax > result[ i ] )
        {
            minMax = result[ i ];
        }
    }
    globalLimits[ metric ].second = maxMax;
}

void
DataProvider::calcMed( int metric, QVector<double>& result, double& maxMed, double& minMed )
{
    ( void )metric;
    int numIterations = cubeReader->getIterations();
    result.clear();
    result.resize( numIterations );
    QVector<double> data = cubeReader->getThreadsV( 0 /*first vector*/ );
    if ( data.size() % 2 != 0 )
    {
        result[ 0 ] = data[ ( ( data.size() + 1 ) / 2 ) - 1 ];
    }
    else
    {
        result[ 0 ] = ( data[ data.size() / 2 ] + data[ ( data.size() / 2 ) - 1 ] ) / 2.0f;
    }
    maxMed = minMed = result[ 0 ];
    for ( int i = 1; i < numIterations; i++ ) // &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
    {
        data = cubeReader->getThreadsV( i );
        if ( data.size() % 2 != 0 )
        {
            result[ i ] = data[ ( ( data.size() + 1 ) / 2 ) - 1 ];
        }
        else
        {
            result[ i ] = ( data[ data.size() / 2 ] + data[ ( data.size() / 2 ) - 1 ] ) / 2.0f;
        }
        if ( maxMed < result[ i ] )
        {
            maxMed = result[ i ];
        }
        if ( minMed > result[ i ] )
        {
            minMed = result[ i ];
        }
    }
}

void
DataProvider::calcQ1( int metric, QVector<double>& result, double& maxQ1, double& minQ1 )
{
    ( void )metric;
    int numIterations = cubeReader->getIterations();
    result.clear();
    result.resize( numIterations );
    QVector<double> data = cubeReader->getThreadsV( 0 /*first vector*/ );
    // vector is not sorted, I need to first copy it, and then sort it, only one vector is used for economy
    QVector<double> sorted_copy_of_data = data;
    std::sort( sorted_copy_of_data.begin(), sorted_copy_of_data.end() );
    result[ 0 ] = sorted_copy_of_data[ ( sorted_copy_of_data.size() + 1 ) / 4 ];
    maxQ1       = minQ1 = result[ 0 ];
    for ( int i = 1; i < numIterations; i++ ) // &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
    {
        data                = cubeReader->getThreadsV( i );
        sorted_copy_of_data = data;
        std::sort( sorted_copy_of_data.begin(), sorted_copy_of_data.end() );
        result[ i ] = sorted_copy_of_data[ ( sorted_copy_of_data.size() + 1 ) / 4 ];
        if ( maxQ1 < result[ i ] )
        {
            maxQ1 = result[ i ];
        }
        if ( minQ1 > result[ i ] )
        {
            minQ1 = result[ i ];
        }
    }
}

void
DataProvider::calcQ3( int metric, QVector<double>& result, double& maxQ3, double& minQ3 )
{
    ( void )metric;
    int numIterations = cubeReader->getIterations();
    result.clear();
    result.resize( numIterations );
    QVector<double> data = cubeReader->getThreadsV(  0 /*first vector*/ );
    // vector is not sorted, I need to first copy it, and then sort it, only one vector is used for economy
    QVector<double> sorted_copy_of_data = data;
    std::sort( sorted_copy_of_data.begin(), sorted_copy_of_data.end() );
    result[ 0 ] = sorted_copy_of_data[ ( 3 * sorted_copy_of_data.size() + 3 ) / 4 ];
    maxQ3       = minQ3 = result[ 0 ];
    for ( int i = 1; i < numIterations; i++ ) // &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
    {
        data                = cubeReader->getThreadsV(  i );
        sorted_copy_of_data = data;
        std::sort( sorted_copy_of_data.begin(), sorted_copy_of_data.end() );
        result[ i ] = sorted_copy_of_data[ ( 3 * sorted_copy_of_data.size() + 3 ) / 4 ];
        if ( maxQ3 < result[ i ] )
        {
            maxQ3 = result[ i ];
        }
        if ( minQ3 > result[ i ] )
        {
            minQ3 = result[ i ];
        }
    }
}

void
DataProvider::normalize( QVector<double>& v, double max, double min, bool rescale )
{
    if ( rescale == true )
    {
        if ( max - min )
        {
            for ( int i = 0; i < v.size(); i++ ) // &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
            {
                ;                                // v[i] = v[i] / plotWidget::VERTICAL_PRECISION;
            }
        }
    }
    if ( max - min )
    {
        for ( int i = 0; i < v.size(); i++ )               // &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
        {
            v[ i ] = ( ( v[ i ] - min ) / ( max - min ) ); // * plotWidget::VERTICAL_PRECISION;
        }
    }
}

// TODO: avoid zeros
double
DataProvider::getMetricMinValue( int metric )
{
    if ( globalLimits[ metric ].first != -1 )
    {
        return globalLimits[ metric ].first;
    }
    else
    {
        int             numIterations = cubeReader->getIterations( metric );
        QVector<double> data          = cubeReader->getProcessesV( metric, 1 /*first vector*/ );
        double          infimum       = data[ 0 ];
        for ( int j = 0; j < data.size(); j++ ) // &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
        {
            if ( infimum > data[ j ] )
            {
                infimum = data[ j ];
            }
        }

        for ( int i = 0; i < numIterations; i++ ) // for each iteration //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 2
        {
            QVector<double> data = cubeReader->getProcessesV( metric, i );
            for ( int j = 0; j < data.size(); j++ )
            {
                if ( infimum > data[ j ] )
                {
                    infimum = data[ j ];
                }
            }
        }
        globalLimits[ metric ].first = infimum;
        return globalLimits[ metric ].first;
    }
}

double
DataProvider::getMetricMaxValue( int metric )
{
    if ( globalLimits[ metric ].second != -1 )
    {
        return globalLimits[ metric ].second;
    }
    else
    {
        int             numIterations = cubeReader->getIterations( metric );
        QVector<double> data          = cubeReader->getProcessesV( metric, 1 /*first vector*/ );
        double          suprimum      = data[ 0 ];
        for ( int j = 0; j < data.size(); j++ ) // &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
        {
            if ( suprimum < data[ j ] )
            {
                suprimum = data[ j ];
            }
        }

        for ( int i = 0; i < numIterations; i++ ) // for each iteration //&&&&&&&&&&&&&&&&&&&&&&&&&&&& 2
        {
            QVector<double> data = cubeReader->getProcessesV( metric, i );
            for ( int j = 0; j < data.size(); j++ )
            {
                if ( suprimum < data[ j ] )
                {
                    suprimum = data[ j ];
                }
            }
        }
        globalLimits[ metric ].second = suprimum;
        return globalLimits[ metric ].second;
    }
}

QVector<QVector<double> >
DataProvider::getAllProcesses( int metric )
{
    cubeReader->setLastMetricUName( metric );
    return cubeReader->getAllProcesses( metric );
}
QVector<QVector<double> >
DataProvider::getAllResult_Itr()
{
    return cubeReader->getAllResult_Itr();
}
QStringList
DataProvider::getMetricNames()
{
    if ( metricNames.size() == 0 )
    {
        int metricNamesSize = cubeReader->getMetricNameSize();
        for ( int i = 0; i < metricNamesSize; i++ )
        {
            metricNames.append( cubeReader->getMetricName( i ) );
        }
    }
    return metricNames;
}
void
DataProvider::setMetricName( QString metricName )
{
    int metricIndex = metricNames.indexOf( metricName );

    if ( metricIndex == -1 )
    {
        //qDebug() << metricName << ": ";
        //qDebug() << " Index not found!";

        // cubeReader->calcAndFill2();
        DisableHeatMap();
        return;
    }

    cubeReader->setLastMetricUName( metricIndex );
}
void
DataProvider::printMemoryContent()
{
#ifdef DEBUG_GRAPHWIDGET
    Globals::debug( tr( "DataProvider" ) ) << tr( "Memory Content =============================" ) << endl;
    Globals::debug( tr( "DataProvider" ) ) << tr( "current limits: " ) << this->getCurrentLimitMinValue() << "  ->  " << this->getCurrentLimitMaxValue() << endl;
    Globals::debug( tr( "DataProvider" ) ) << tr( "Max value overridden?   " ) << this->maxValueIsSetManually << tr( ",   Min value overridden?   " ) << this->minValueIsSetManually << endl;
    Globals::debug( tr( "DataProvider" ) ) << tr( "Name\t\tOp\titr\t\tscaled\t\tvisible\t\tMin\t\tMax\t\tMinScl\t\tMaxScl\t\tzOrder\tcolor" ) << endl;
    for ( QList<CubeDataItem>::const_iterator i = data.constBegin(); i != data.constEnd(); i++ )
    {
        Globals::debug( tr( "DataProvider" ) ) << ( *i ).getName() << "\t" << ( int )( ( *i ).getMathOp() ) << "\t" << ( *i ).getfirstItrID() << "\t" << ( *i ).isScaled() << "\t" << ( *i ).isVisible() << "\t"
                                               << ( *i ).getMinValue() << "\t" << ( *i ).getMaxValue() << "\t" << ( *i ).getScaleMinValue() << "\t" << ( *i ).getScaleMaxValue() << "\t" << ( *i ).getZOrder() << "\t\t" << ( *i ).getColor().name() << endl;
    }
    Globals::debug( tr( "DataProvider" ) ) << "============================================" << endl;
#endif
}

bool
DataProvider::itemExists( QString plotAndMetricName )
{
    for ( QList<CubeDataItem>::const_iterator i = data.constBegin(); i != data.constEnd(); i++ )
    {
        if ( ( *i ).getName() == plotAndMetricName )
        {
            return true;
        }
    }
    return false;
}

bool
DataProvider::colorIsUsed( QColor color )
{
    for ( QList<CubeDataItem>::const_iterator i = data.constBegin(); i != data.constEnd(); i++ )
    {
        if ( ( *i ).isVisible() == true &&
             ( *i ).getColor() == color )
        {
            return true;
        }
    }
    return false;
}
void
DataProvider::changeColor( QColor color )
{
    bool flag = false;
    //  for ( QList<CubeDataItem>::const_iterator i = data.constBegin(); i != data.constEnd(); i++ )
    for ( int i = 0; i < data.size(); i++ )
    {
        if ( data[ i ].getColor() == PrevColorBarPlot )
        {
            flag = true;
            // ( *i ).setColor( color );
            data[ i ].setColor( color );
        }
    }
    if ( flag == true )
    {
        emit dataUpdated( data );
    }

    printMemoryContent();
}
void
DataProvider::removeCubeDataElement( int zOrderIndex )
{
    /**
       TODO If an element is deleted, it might be having one of the maximum boundaries (say top notch value).
       The question is whether to rescale everything to the next maximum value (if no other element exists, then
       we reset the max value and the min value). Or to keep it assuming it is set by the user and it is not a good idea
       to override his settings.
       One solution would be to keep the state of the limits as either being calculated automatically or set manually by the user.
     */
    for ( int i = 0; i < data.size(); i++ )
    {
        if ( data[ i ].isVisible() && data[ i ].getZOrder() == zOrderIndex )
        {
            data[ i ].setVisible( false );
        }
    }
    emit dataUpdated( data );
}
void
DataProvider::removeCubeDataElement()
{
    /**
       TODO If an element is deleted, it might be having one of the maximum boundaries (say top notch value).
       The question is whether to rescale everything to the next maximum value (if no other element exists, then
       we reset the max value and the min value). Or to keep it assuming it is set by the user and it is not a good idea
       to override his settings.
       One solution would be to keep the state of the limits as either being calculated automatically or set manually by the user.
     */
    for ( int i = 0; i < data.size(); i++ )
    {
        data[ i ].setVisible( false );
    }
    data.clear();
    emit dataUpdated( data );
}
void
DataProvider::removeTopCubeDataElement()
{
    /**
       TODO If an element is deleted, it might be having one of the maximum boundaries (say top notch value).
       The question is whether to rescale everything to the next maximum value (if no other element exists, then
       we reset the max value and the min value). Or to keep it assuming it is set by the user and it is not a good idea
       to override his settings.
       One solution would be to keep the state of the limits as either being calculated automatically or set manually by the user.
     */
    int i =  data.size() - 1;
    if ( i >= 0 )
    {
        data[ i ].setVisible( false );
        data.removeAt( i );
        emit dataUpdated( data );
    }
}
void
DataProvider::updateCubeDataElementsOrder( const QVector<int> newOrder )
{
    int visibleDataItemsCounter = 0;
    for ( int i = 0; i < data.size(); i++ )
    {
        if ( data[ i ].isVisible() )
        {
            data[ i ].setZOrder( newOrder[ visibleDataItemsCounter ] );
            visibleDataItemsCounter++;
        }
    }
    std::sort( data.begin(), data.end(), CubeDataItem::cubeDataItemOrderComparator );
    emit dataUpdated( data );
}

double
DataProvider::getCurrentLimitMaxValue()
{
    return this->currentLimitMaxValue;
}

double
DataProvider::getCurrentLimitMinValue()
{
    return this->currentLimitMinValue;
}

void
DataProvider::setCurrentLimitMaxValue( double newLimitMaxValue )
{
    double maxValue               = 0;
    bool   someElementsAreVisible = false;
    for ( QList<CubeDataItem>::const_iterator iterator = data.constBegin(); iterator != data.constEnd(); iterator++ )
    {
        if ( ( *iterator ).isVisible() )
        {
            maxValue               = ( *iterator ).getMaxValue();
            someElementsAreVisible = true;
            break;
        }
    }
    for ( QList<CubeDataItem>::const_iterator iterator = data.constBegin(); iterator != data.constEnd(); iterator++ )
    {
        if ( ( *iterator ).isVisible() )
        {
            if ( ( *iterator ).getMaxValue() > maxValue )
            {
                maxValue = ( *iterator ).getMaxValue();
            }
        }
    }
    if ( someElementsAreVisible && newLimitMaxValue < maxValue )
    {
        QString str;
        str  = tr( "The given value: " ) + QString::number( newLimitMaxValue ) + tr( " is less than the current maximum value of a drawn plot(s): " ) + QString::number( maxValue );
        str += tr( "\nResetting the max limit calculation to Automatic ..." );
        emit information( str );
        emit maxValueIsCalculatedAutomatically();
        maxValueIsSetManually = false;
        return;
    }
    maxValueIsSetManually = true;
    maximizeUpperLimit( newLimitMaxValue );
    emit this->hLimitsHaveChangedHeatmap( 0, this->getIterationsCount() ); // it should be modified. now assumes that only process on vertical ruler
    emit limitsHaveChanged( this->currentLimitMinValue, this->currentLimitMaxValue );
    printMemoryContent();
}

void
DataProvider::setCurrentLimitMinValue( double newLimitMinValue )
{
    double minValue               = 0;
    bool   someElementsAreVisible = false;
    for ( QList<CubeDataItem>::const_iterator iterator = data.constBegin(); iterator != data.constEnd(); iterator++ )
    {
        if ( ( *iterator ).isVisible() )
        {
            minValue               = ( *iterator ).getMinValue();
            someElementsAreVisible = true;
            break;
        }
    }
    for ( QList<CubeDataItem>::const_iterator iterator = data.constBegin(); iterator != data.constEnd(); iterator++ )
    {
        if ( ( *iterator ).isVisible() )
        {
            if ( ( *iterator ).getMinValue() < minValue )
            {
                minValue = ( *iterator ).getMinValue();
            }
        }
    }
    if ( someElementsAreVisible && newLimitMinValue > minValue )
    {
        QString str;
        str  = tr( "The given value: " ) + QString::number( newLimitMinValue ) + tr( " is larger than the current minimum value of a drawn plot(s): " ) + QString::number( minValue );
        str += tr( "\nResetting the min limit calculation to Automatic ..." );
        emit information( str );
        emit minValueIsCalculatedAutomatically();
        minValueIsSetManually = false;
        return;
    }
    minValueIsSetManually = true;
    minimizeLowerLimit( newLimitMinValue );
    emit this->hLimitsHaveChangedHeatmap( 0, this->getIterationsCount() ); // it should be modified. now assumes that only process on vertical ruler
    emit limitsHaveChanged( this->currentLimitMinValue, this->currentLimitMaxValue );
    printMemoryContent();
}

int
DataProvider::getProcessesCount()
{
    const QVector<QVector<double> > allProcesses = cubeReader->getAllProcesses( 0 );
    if ( allProcesses.size() > 0 )
    {
        return allProcesses[ 0 ].size();
    }
    else
    {
        return -1;
    }
}
int
DataProvider::getThreadsCount()
{
    int tempSize = cubeReader->getThreadsSize();
    if ( tempSize > 0 )
    {
        return tempSize;
    }
    else
    {
        return -1;
    }
}
int
DataProvider::getIterationsCount()
{
    const QVector<QVector<double> > allProcesses = cubeReader->getAllProcesses( 0 );
    if ( allProcesses.size() > 0 )
    {
        return allProcesses.size();
    }
    else
    {
        return -1;
    }
}

void
DataProvider::clearCache()
{
    data.clear();
    emit dataUpdated( data );
}
void
DataProvider::setIterationsFunc( QList<cube::Cnode*> iterations_funcTemp )
{
    cItrlist = iterations_funcTemp;
}

QList<cube::Cnode*>
DataProvider::getIterationsFunc()
{
    return cItrlist;
}
void
DataProvider::setCalcType( cube::CalculationFlavour calcTypeTemp )
{
    cubeReader->setCalcType( calcTypeTemp );
}

cube::CalculationFlavour
DataProvider::getCalcType()
{
    return cubeReader->getCalcType();
}


void
DataProvider::setOpNameBarPlot( plotWidget::MathOp opNameBarPlotTemp )
{
    opNameBarPlot = opNameBarPlotTemp;
}

plotWidget::MathOp
DataProvider::getOpNameBarPlot()
{
    return opNameBarPlot;
}

void
DataProvider::setStyleBarPlot( plotWidget::Style styleBarPlotTemp )
{
    styleBarPlot = styleBarPlotTemp;
}

plotWidget::Style
DataProvider::getStyleBarPlot()
{
    return styleBarPlot;
}

void
DataProvider::setColorBarPlot( QColor colorBarPlotTemp )
{
    PrevColorBarPlot = colorBarPlot;
    colorBarPlot     = colorBarPlotTemp;
}

QColor
DataProvider::getColorBarPlot()
{
    return colorBarPlot;
}
void
DataProvider::setTopStack( bool temptopStack )
{
    topStack = temptopStack;
}
QStringList
DataProvider::createColorPalettesBarplot()
{
    QStringList list;
    // QList<QColor> list;
    QStringList colorNames = QColor::colorNames();
    for ( int j = 0; j < colorNames.size(); j++ )
    {
        if ( QColor( colorNames[ j ] ) == "aqua" ||
             QColor( colorNames[ j ] ) == "black" ||
             QColor( colorNames[ j ] ) == "blue" ||
             QColor( colorNames[ j ] ) == "brown" ||
             QColor( colorNames[ j ] ) == "cadetblue" ||
             QColor( colorNames[ j ] ) == "chartreuse" ||
             QColor( colorNames[ j ] ) == "coral" ||
             QColor( colorNames[ j ] ) == "crimson" ||
             QColor( colorNames[ j ] ) == "deeppink" ||
             QColor( colorNames[ j ] ) == "gold" ||
             QColor( colorNames[ j ] ) == "red" ||
             QColor( colorNames[ j ] ) == "green" ||
             QColor( colorNames[ j ] ) == "violet" ||
             QColor( colorNames[ j ] ) == "yellow" ||
             QColor( colorNames[ j ] ) == "darkMagenta" ||
             QColor( colorNames[ j ] ) == "darkRed"
             )
        {
            list.push_back( colorNames[ j ] );
        }
    }
    return list;
}

void
DataProvider::reorderZData()
{
    /**
     * Code taken from Barplot/PlotsListController.cpp, which seems to be abandoned.
     */

    int maxZOrder = -1;
    // / while finding the max element, set the zOrder of any invisible data item to -1
    // / this guarantees that no two data items will get the same (positive) zOrder
    for ( int i = 0; i < data.size(); i++ )
    {
        if ( data[ i ].isVisible() )
        {
            maxZOrder = qMax( maxZOrder, data[ i ].getZOrder() );
        }
        else
        {
            data[ i ].setZOrder( -1 );
        }
    }
    // / clean up the view from any existing data items
    // / fix zOrder for any new visible item (the zOrder of any new item is maximum zOrder of exiting elements + 1)
    for ( int i = 0; i < data.size(); i++ )
    {
        if ( data[ i ].isVisible() )
        {
            if ( data[ i ].getZOrder() == -1 )
            {
                data[ i ].setZOrder( ++maxZOrder );
            }
            plotWidget::MathOp itemOpType = data[ i ].getMathOp();
            if ( itemOpType < plotWidget::Minimum || itemOpType >= plotWidget::MathOpUnknown )
            {
                qDebug( "Trying to add a plot to plot-list of unknown math operation type!" );
            }
        }
    }

    // lowest z order at the bottom
    std::sort( data.begin(), data.end(), CubeDataItem::cubeDataItemOrderComparator );
}
