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


#ifndef CUBEREAD_H
#define CUBEREAD_H

#include <QList>
#include <QVector>
#include <QString>
#include <iostream>
#include <sys/types.h>
#include <string>
#include <regex>
#include <vector>
#include <utility>
#include <CubeProxy.h>


// Reads the custom cube data file.
class CubeRead
{
protected:
    cube::CubeProxy*           cube;
    std::vector<cube::Metric*> metrics;
    std::vector<cube::Cnode*>  iterations;
    std::vector<cube::Cnode*>  iterations_func;
    cube::CalculationFlavour   calcType; // exclusive or inclusive

    std::vector<cube::Process*> processes;
    std::vector<cube::Thread*>  threads;

    QString lastMetricUName; // to keep the prevous selested metric

    QVector<QVector<QVector< double > > > values;
    QVector<QVector< double >  >          values_subItr;

    QVector<QString> metricsNames;
    QVector<QString> processNames;
    QVector<QString> threadsNames;

    std::pair<double, double> globalMinMax_Itr;
    bool                      cubeHasItr; // true means cube file has itr

    void
    generateData();                                                                     // /< Parses cube and build std::vector<std::vector<std::vector<double>>> of data.

    void
    createListOfMetrics();                                                      // /< Fills the list of available metrics;

    void
    createListOfCnodes();                                                       // /< Parses cnodes and finds a itrerations cnode root

    void
    createListOfProcesses();                                                    // /< Gathers toghether processes;

    void
    createListOfThreads();

    void
    calcAndFill();                                                                      // /< Actually calculates and stores data;

    bool
    isIterationCnode( const std::regex&, const std::string& );                            // /< signalizes, that code corresponds to the pattern and should be take into the list

    bool
    isIterationFuncCnode( std::vector<int> listID,
                          int              testID );

//    void
//  createListOfSubCnodes(std::vector<int> func_id);

public:
    void
    createListOfSubCnodes( const QList<cube::Cnode*>& func );
    void
    calcAndFill_subItr( cube::CalculationFlavour calcType );

    CubeRead( cube::CubeProxy* );
    virtual
    ~CubeRead()
    {
    };
    void
    setLastMetricUName( int metNum );
    QString
    getLastMetricUName();

    QVector<double>
    getProcessesV( int metNum,
                   int iterNum );

    QVector<double>
    getThreadsV( int iterNum );

    QVector<QVector<double> >
    getAllProcesses( int metNum );

    QVector<QVector<double> >
    getAllResult_Itr();

    std::pair<double, double>
    getMinMaxValue_Itr();
    QString
    getMetricName( int metNum );
    unsigned int
    getMetricNameSize();
    int
    getIterations( QString metricName );
    int
    getIterations( int metricName );
    int
    getIterations();
    int
    getThreadsSize();
    void
    setCubeHasItr( bool temp );
    bool
    getCubeHasItr();

    // void
    // setIterationsFunc(std::vector<cube::Cnode*>   iterations_funcTemp);
    // std::vector<cube::Cnode*>
    // getIterationsFunc();
    void
    setCalcType( cube::CalculationFlavour calcTypeTemp );
    cube::CalculationFlavour
    getCalcType();
};



#endif // CUBEREAD_H
