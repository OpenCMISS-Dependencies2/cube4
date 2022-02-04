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

#include "CubeRead.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <QDebug>

#include "CubeMetric.h"
#include "CubeProxy.h"
#include "CubeRegion.h"

using namespace std;
using namespace cube;


CubeRead::CubeRead( cube::CubeProxy* cube ) :  cube( cube )
{
    if ( cube == NULL )
    {
        throw string( QObject::tr( "Cube is not loaded yet." ).toUtf8().data() );       // /< temporary exception to signalize that one cannot operate. Should be replaces.
    }
    generateData();
}

QVector<double>
CubeRead::getProcessesV( int metNum, int iterNum )
{
    return values.at( metNum ).at( iterNum );
}
QVector<double>
CubeRead::getThreadsV( int iterNum ) // REY
{
    return values_subItr.at( iterNum );
}

QString
CubeRead::getMetricName( int metNum )
{
    return metricsNames.at( metNum );
}
void
CubeRead::setLastMetricUName( int metNum )       // REY
{
    lastMetricUName = metricsNames.at( metNum ); // .toLower();
}
QString
CubeRead::getLastMetricUName() // REY
{
    return lastMetricUName;
}
unsigned int
CubeRead::getMetricNameSize()
{
    return metricsNames.size();
}

int
CubeRead::getIterations( QString metricName )
{
    for ( int i = 0; i < metricsNames.size(); i++ )
    {
        if ( metricsNames.at( i ).compare( metricName ) == 0 )
        {
            return values.at( i ).size();
        }
    }
    return -1;
}

int
CubeRead::getIterations( int metricNum )
{
    return getIterations( getMetricName( metricNum ) );
}
int
CubeRead::getIterations() // REY
{
    int tempSize =  values_subItr.size();
    if ( tempSize > 0 )
    {
        return tempSize;
    }
    return -1;
}


QVector<QVector<double> >
CubeRead::getAllProcesses( int metNum ) // REY
{
    return values.at( metNum );
}
int
CubeRead::getThreadsSize() // REY
{
    return values_subItr[ 0 ].size();
}
QVector<QVector<double> >
CubeRead::getAllResult_Itr() // REY
{
    return values_subItr;
}
pair<double, double>
CubeRead::getMinMaxValue_Itr() // REY
{
    return globalMinMax_Itr;
}
// -------------- protected methods --------------------


void
CubeRead::generateData()
{
    createListOfMetrics();
    createListOfCnodes();
    createListOfProcesses();
    createListOfThreads();

    calcAndFill();
}

void
CubeRead::createListOfMetrics()
{
    metricsNames.clear();
    metrics = cube->getMetrics();
    for ( std::vector<cube::Metric*>::iterator m_iter = metrics.begin(); m_iter != metrics.end(); m_iter++ )
    {
        // metricsNames.push_back( QString::fromStdString( ( *m_iter )->get_uniq_name() ) );
        if ( ( *m_iter ) != NULL )
        {
            metricsNames.push_back( QString::fromStdString( ( *m_iter )->get_disp_name() ) );
        }
    }
}
void
CubeRead::createListOfCnodes()
{
    iterations.clear();
    std::vector<cube::Cnode*> _all_cnodes = cube->getCnodes();


    std::string _iteration_regex = "iteration=";
//      QString _str_number = QString("%1").arg(loop_number);
//      _iteration_regex += _str_number.toUtf8().data();
    _iteration_regex += "[0-9]+$";
    std::regex rgT( _iteration_regex );

    for ( std::vector<cube::Cnode*>::iterator c_iter =  _all_cnodes.begin(); c_iter !=  _all_cnodes.end(); c_iter++ )
    {
        //   string s = ( *c_iter )->get_callee()->get_name() ;
        //  qDebug()<<s.c_str()<<"  "<< ( *c_iter )->get_id();


        if ( isIterationCnode( rgT, ( *c_iter )->get_callee()->get_name() ) )
        {
            iterations.push_back( *c_iter );
        }
    }
    if ( iterations.empty() )
    {
        setCubeHasItr( false );
        // throw string( QString( "Cannot find iterations" ).toUtf8().data() );     // didnt find any regions.
    }
    else
    {
        setCubeHasItr( true );
    }
}
void
CubeRead::createListOfSubCnodes( const QList<cube::Cnode*>& func )
{
    iterations_func.clear();
    iterations_func.assign( func.begin(), func.end() );
}
void
CubeRead::createListOfProcesses()
{
    /// @todo FIXME This method does not do anything useful.
    processes = cube->getLocationGroups();
}

void
CubeRead::createListOfThreads()
{
    threadsNames.clear();
    threads = cube->getLocations();
    for ( std::vector<cube::Thread*>::iterator t_iter = threads.begin(); t_iter != threads.end(); t_iter++ )
    {
        threadsNames.push_back( QString::fromStdString( ( *t_iter )->get_name() ) );
    }
    // qDebug()<<QString::fromStdString( ( *t_iter )->get_name() ) ;
}

void
CubeRead::calcAndFill()
{
    //  cout << "GENERATE DATA " << endl;

    // resize the arrays accordingly
    values.resize( metrics.size() );
    for ( size_t m = 0; m < metrics.size(); m++ )
    {
        values[ m ].resize( iterations.size() );
        for ( size_t i = 0; i < iterations.size(); i++ )
        {
            values[ m ][ i ].resize( processes.size() );
        }
    }


    unsigned i = 0;
    for ( std::vector<cube::Metric*>::iterator m_iter = metrics.begin(); m_iter != metrics.end(); m_iter++, i++ )
    {
        list_of_metrics metric_selection;
        metric_selection.push_back( make_pair( *m_iter, CUBE_CALCULATE_INCLUSIVE ) );

        unsigned j = 0;
        for ( std::vector<cube::Cnode*>::iterator c_iter =  iterations.begin(); c_iter !=  iterations.end(); c_iter++, j++ )
        {
            list_of_cnodes cnode_selection;
            cnode_selection.push_back( make_pair( *c_iter, CUBE_CALCULATE_INCLUSIVE ) );

            vector<Value*> inclusive_values;
            vector<Value*> exclusive_values;

            cube->getSystemTreeValues( metric_selection,
                                       cnode_selection,
                                       inclusive_values,
                                       exclusive_values );

            unsigned k = 0;
            //    cout << " *************************************** " << endl;
            for ( std::vector<cube::Process*>::iterator p_iter = processes.begin(); p_iter != processes.end(); p_iter++, k++ )
            {
                cube::Value* _v = inclusive_values[ ( *p_iter )->get_sys_id() ];

                if ( _v != NULL )
                {
                    values[ i ][ j ][ k ] = _v->getDouble();
                }
                else
                {
                    values[ i ][ j ][ k ] = 0.;
                }
                //    cout << values[ i ][ j ][ k ] << endl;

                delete _v;
            }
        }
        //   cout << endl;
    }
}
void
CubeRead::calcAndFill_subItr( cube::CalculationFlavour calcType )
{
    vector <cube::Metric*> x = cube->getMetrics();
    cube::Metric*          m = NULL;

    for ( std::vector<cube::Metric*>::iterator m_iter = x.begin(); m_iter != x.end(); m_iter++ )
    {
        if ( ( *m_iter )->get_disp_name()  ==  getLastMetricUName().toStdString() )
        {
            m = cube->getMetric( ( *m_iter )->get_uniq_name() );
            break;
        }
    }
    if ( m == NULL )
    {
        qDebug() << QObject::tr( " metric not found!" );
        return;
    }

    // resize the arrays accordingly
    values_subItr.resize( iterations_func.size() );
    for ( size_t i = 0; i < iterations_func.size(); i++ )
    {
        values_subItr[ i ].resize( threads.size() );
    }


    list_of_metrics metric_selection;
    metric_selection.push_back( make_pair( m, CUBE_CALCULATE_INCLUSIVE ) );
    bool     flag = false;
    unsigned j    = 0;
    for ( std::vector<cube::Cnode*>::iterator c_iter =  iterations_func.begin(); c_iter !=  iterations_func.end(); c_iter++, j++ )
    {
        if ( *c_iter == NULL )
        {
            continue;
        }
        list_of_cnodes cnode_selection;
        cnode_selection.push_back( make_pair( *c_iter, CUBE_CALCULATE_INCLUSIVE ) );

        vector<Value*> inclusive_values;
        vector<Value*> exclusive_values;

        cube->getSystemTreeValues( metric_selection,
                                   cnode_selection,
                                   inclusive_values,
                                   exclusive_values );
        unsigned k = 0;
        for ( std::vector<cube::Thread*>::iterator p_iter = threads.begin(); p_iter != threads.end(); p_iter++, k++ )
        {
            cube::Value* _v = NULL;
            if ( *c_iter )
            {
                _v = ( calcType == CUBE_CALCULATE_INCLUSIVE ?
                       inclusive_values[ ( *p_iter )->get_sys_id() ] :
                       exclusive_values[ ( *p_iter )->get_sys_id() ] );
            }

            if ( _v != NULL )
            {
                values_subItr[ j ][ k ] = _v->getDouble();
            }
            else
            {
                values_subItr[ j ][ k ] = -DBL_MAX; // invalid values are marked with gray, e.g. if not called in current iteration
                continue;
            }

// cout<<values_subItr[ j ][ k ]<<endl;
            if ( flag == false )   // first comparison
            {
                globalMinMax_Itr.first = globalMinMax_Itr.second = values_subItr[ j ][ k ];
                flag                   = true;
            }
            else
            {
                if ( values_subItr[ j ][ k ] > globalMinMax_Itr.second  )
                {
                    globalMinMax_Itr.second = values_subItr[ j ][ k ];
                }

                if ( values_subItr[ j ][ k ] < globalMinMax_Itr.first  )
                {
                    globalMinMax_Itr.first = values_subItr[ j ][ k ];
                }
            }

            delete _v;
        }
    }
}

bool
CubeRead::isIterationCnode( const std::regex& rgT, const std::string& region_name )
{
    return std::regex_search( region_name, rgT );
}

bool
CubeRead::isIterationFuncCnode( vector<int> listID, int testID )
{
    for ( std::vector<int>::iterator iter = listID.begin(); iter !=  listID.end(); iter++ )
    {
        if ( ( *iter ) == testID )
        {
            return true;
        }
    }
    return false;
}
bool
CubeRead::getCubeHasItr()
{
    return cubeHasItr;
}
void
CubeRead::setCubeHasItr( bool temp )
{
    cubeHasItr = temp;
}
// QList<cube::Cnode*>
// CubeRead::getIterationsFunc()
// {
//   return iterations_func;
// }
// void
// CubeRead::setIterationsFunc(QList<cube::Cnode*>  iterations_funcTemp)
// {
//    iterations_func =  iterations_funcTemp;
// }
void
CubeRead::setCalcType( cube::CalculationFlavour calcTypeTemp )
{
    calcType = calcTypeTemp;
}

cube::CalculationFlavour
CubeRead::getCalcType()
{
    return calcType;
}
