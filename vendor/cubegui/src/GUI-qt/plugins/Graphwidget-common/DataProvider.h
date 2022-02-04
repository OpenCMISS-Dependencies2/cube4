/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2017                                                **
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


#ifndef DATAPROVIDER_H
#define DATAPROVIDER_H


#include <QVector>
#include <QList>
#include <QString>
#include "CubeRead.h"
#include "GraphWidgetGlobals.h"
#include "CubeDataItem.h"
#include "PluginServices.h"
#include <QObject>

#include <utility>


class DataProvider : public QObject
{
    Q_OBJECT
private:
    /**
     * Privatized constructor. This class should be instantiated only once.
     * @param cube A reference to the cube file reader
     * @see DataProvider::getInstance(CubeRead*)
     */
    // CubeRead* cubeReader;
    QVector<std::pair<double, double> > globalLimits;
    static DataProvider*       cubeDataProvider;
    QList<CubeDataItem>        data;
    QVector<QVector< double> > heatMap;
    QStringList                metricNames;


    plotWidget::MathOp  opNameBarPlot;
    plotWidget::Style   styleBarPlot;
    QColor              colorBarPlot;
    QColor              PrevColorBarPlot;
    QList<cube::Cnode*> cItrlist;
    bool                topStack; // check whether top row of stack should be removed or not


    void
    genMetric( int                      metric,
               QVector<double>&         result,
               plotWidget::MathOp       opType,
               double&                  relativeMax,
               double&                  relativeMin,
               QList<cube::Cnode*>&     func_id,
               cube::CalculationFlavour calcType );
    void
    calcAvg( int              metric,
             QVector<double>& result,
             double&          maxAvg,
             double&          minAvg );
    void
    calcMin( int              metric,
             QVector<double>& result,
             double&          maxMin,
             double&          minMin );
    void
    calcMax( int              metric,
             QVector<double>& result,
             double&          maxMax,
             double&          minMax );
    void
    calcMed( int              metric,
             QVector<double>& result,
             double&          maxMed,
             double&          minMed );
    void
    calcQ1( int              metric,
            QVector<double>& result,
            double&          maxQ1,
            double&          minQ1 );
    void
    calcQ3( int              metric,
            QVector<double>& result,
            double&          maxQ3,
            double&          minQ3 );
    void
    normalize( QVector<double>& v,
               double           max,
               double           min,
               bool             rescale );

    double currentLimitMinValue;
    double currentLimitMaxValue;
    /**
     * If user chooses to specify top notch in the customization dialog this flag becomes true
     */
    bool maxValueIsSetManually;
    /**
     * If user chooses to specify bottom notch in the customization dialog this flag becomes true
     */
    bool minValueIsSetManually;

public:
    DataProvider( CubeRead* cube );
    CubeRead* cubeReader;

    /**
     * Returns the minimum value of metric
     * @param metric Index of the metric as it exists in the cube file
     * @return Minimum value of the metric
     */
    double
    getMetricMinValue( int metric );

    /**
     * Returns the maximum value of metric
     * @param metric Index of the metric as it exists in the cube file
     * @return Maximum value of the metric
     */
    double
    getMetricMaxValue( int metric );
    bool
    itemExists( QString plotAndMetricName );
    bool
    colorIsUsed( QColor color );
    void
    changeColor( QColor color );
    void
    removeCubeDataElement( int zOrderIndex );
    void
    removeCubeDataElement();
    void
    removeTopCubeDataElement();

    /**
     * Updates the zOrder of data items and emits dataUpdated() signal afterwards
     * \details The method iterates over all visible data items and sets their new zOrder as given. It sorts then all data items according to their zOrder.
     * @param newZOrder The new zOrder of the visible data items
     */
    void
    updateCubeDataElementsOrder( const QVector<int> newZOrder );

public:
    // void addMetric(int metric, plotWidget::MathOp opType, QString name, QColor color, plotWidget::Style style);
    void
    scaleData( int              originalCubeDataItemIndex,
               QVector<double>& scaledData );

    /**
     * Changes the current maximum boundary so it matches the passed argument.
     * \details To set the maximum boundary to new value, the method iterates over all visible metrics, for each visible metric it checks whether it satisfies the
     * new boundary, if not then it tries to find a similar metric that satisfies the new boundary value, by similar it means having the same metric ID and math op.
     * If no metric is found, then the method refers to the already existing unscaled metric and creates a new scaled metric out of it with the new boundary value, and
     * makes it visible.
     * This method is robust as it utilizes the cache and creates a new metric when it is really necessary.
     * @param newUpperLimit New upper limit boundary
     */
    void
    maximizeUpperLimit( double newUpperLimit );

    /**
     * Changes the current minimum boundary so it matches the passed argument.
     * \details To set the minimum boundary to new value, the method iterates over all visible metrics, for each visible metric it checks whether it satisfies the
     * new boundary, if not then it tries to find a similar metric that satisfies the new boundary value, by similar it means having the same metric ID and math op.
     * If no metric is found, then the method refers to the already existing unscaled metric and creates a new scaled metric out of it with the new boundary value, and
     * makes it visible.
     * This method is robust as it utilizes the cache and creates a new metric when it is really necessary.
     * @param newLowerLimit New lower limit boundary
     */
    void
    minimizeLowerLimit( double newLowerLimit );

    /**
     * Gets the index of the unscaled version of a metric
     * @param metricIndex the index of the metric (its location in the original cube file)
     * @param mathOp the mathematical operation that is performed on the metric
     * @return The index of the unscaled metric, if nothing is found it returns -1
     */
    int
    getIndexOfUnscaledMetric( int                      metricIndex,
                              plotWidget::MathOp       mathOp,
                              unsigned int             itrID,
                              cube::CalculationFlavour tempCalcType );

    QVector< QVector<double> >
    getAllProcesses( int metric );

    QVector<QVector<double> >
    getAllResult_Itr();

    // static CubeDataProvider* getInstance(ZCubeReader* cube);
    static DataProvider*
    getInstance( CubeRead* cube );
    QStringList
    getMetricNames();
    void
    setMetricName( QString metricName );

    /**
     * Returns the current maximum boundary value
     * @return the current maximum boundary value
     */
    double
    getCurrentLimitMaxValue();

    /**
     * Returns the current minimum boundary value
     * @return the current minimum boundary value
     */
    double
    getCurrentLimitMinValue();

    int
    getProcessesCount();
    int
    getThreadsCount();

    int
    getIterationsCount();
    void
    DisableHeatMap();

    /**
     * Generates a new heat map and emit a heatMapUpdated() signal so the listening views can update themselves.
     * @param metricName Name of the metric
     */
    void
    generateHeatMapIteration( const QList<cube::Cnode*>&     func,
                              cube::CalculationFlavour       calcType,
                              cubepluginapi::PluginServices* service );
    void
    setIterationsFunc( QList<cube::Cnode*> iterations_funcTemp );

    QList<cube::Cnode*>
    getIterationsFunc();
    void
    setCalcType( cube::CalculationFlavour calcTypeTemp );
    cube::CalculationFlavour
    getCalcType();
    void
    setTopStack( bool temptopStack );

    void
    setOpNameBarPlot( plotWidget::MathOp opNameBarPlotTemp );
    plotWidget::MathOp
    getOpNameBarPlot();

    void
    setStyleBarPlot( plotWidget::Style styleBarPlotTemp );
    plotWidget::Style
    getStyleBarPlot();

    void
    setColorBarPlot( QColor colorBarPlotTemp );
    QColor
    getColorBarPlot();
    QStringList
    createColorPalettesBarplot();

    void
    addMetric( QString                  metricName,
               plotWidget::MathOp       opName,
               plotWidget::Style        styleName,
               QColor                   color,
               QList<cube::Cnode*>&     func,
               cube::CalculationFlavour calcType );

    /**
     * Reorder the data - if the element is visible at it doesn't have correct Z position (< 0),
     * then put it at the top.
     */
    void
    reorderZData();

signals:
    /**
     * This signal is emitted when an internal event takes place. For example, generating a result or getting a result from the cache
     */
    void
    information( QString str );

    /**
     * This signal is emitted whenever the data is changed. It is useful to force drawing widget to repaint themselves
     * @param data The set of plots. This parameter is a Qt-managed container therefore a shallow copy (reference)
     * is made when passing the data around although it appears to be passed by value
     */
    void
    dataUpdated( QList<CubeDataItem>& data );
    void
    heatMapUpdated( QVector<QVector<double> >      heatMap,
                    double                         heatMapMin,
                    double                         heatMapMax,
                    cubepluginapi::PluginServices* service );
    void
    heatMapDisable();
    void
    barPlotDisable();
    void
    changeCalibrateStatus( bool newClaibratestatus );

    //  void changeDisplayStatus(bool newDisplaystatus);

    void
    limitsHaveChanged( double newMinimumLimitValue,
                       double newMaximumLimitValue );
    void
    vLimitsHaveChangedHeatmap( int newMinimumLimitValue,
                               int newMaximumLimitValue );
    void
    hLimitsHaveChangedHeatmap( int newMinimumLimitValue,
                               int newMaximumLimitValue );
    void
    maxValueIsCalculatedAutomatically();
    void
    minValueIsCalculatedAutomatically();
    void
    updateColorCombobox( QColor c );

public slots:
    /**
     * Generates a new metric and adds it to the data array. The boundaries are fixed if necessary.
     * @param metricName The name of the metric
     * @param opName The name of the operation to do on the metric
     * @param styleName The style of the drawing
     * @param color The color of the resulting plot
     */
    //   void
    //  addMetric(QString metricName, plotWidget::MathOp opName, plotWidget::Style styleName, QColor color, QList<cube::Cnode*>& func, cube::CalculationFlavour calcType );

    void
    setAddMetricSettings();
    void
    changePlotColor();

    /**
     * Sets the maximum boundary value to a new value. The new value should be less than the relative maximum value of any visible metric.
     * Otherwise the method will returns after emitting a signal with a warning string message
     * @see CubeDataProvider::information(QString)
     * @param newLimitMaxValue The new maximum boundary value
     */
    void
    setCurrentLimitMaxValue( double newLimitMaxValue );

    /**
     * Sets the minimum boundary value to a new value. The new value should be higher than the relative minimum value of any visible metric.
     * Otherwise the method will returns after emitting a signal with a warning string message
     * @see CubeDataProvider::information(QString)
     * @param newLimitMinValue The new maximum boundary value
     */
    void
    setCurrentLimitMinValue( double newLimitMinValue );

    /**
     * Resets the maximum boundary to the maximum value of one of the unscaled versions of the visible metrics. This method is robust as no metric
     * is actually generated.
     * @see maximizeUpperLimit(double newUpperLimit)
     */
    void
    resetLimitMaxValue();

    /**
     * Resets the minimum boundary to the minimum value of one of the unscaled versions of the visible metrics. This method is robust as no metric
     * is actually generated.
     * @see minimizeLowerLimit(double newLowerLimit)
     */
    void
    resetLimitMinValue();

    /**
     * Prints the contents of the cache to the console. This method is particularly useful to see how caching works and evolves over time.
     */
    void
    printMemoryContent();

    void
    clearCache();
    void
    DisableBarPlot();
};

#endif // DATAPROVIDER_H
