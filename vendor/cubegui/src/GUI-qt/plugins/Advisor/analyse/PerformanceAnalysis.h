/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2015-2019                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef PERFORMANCE_ANALYSIS_H
#define PERFORMANCE_ANALYSIS_H

#include <string>
#include <QObject>
#include <QToolBar>
#include <QList>
#include <QStringList>

#include "CubeProxy.h"
#include "TreeItem.h"

namespace advisor
{
class AdvisorAdvice;
class PerformanceTest;


class PerformanceAnalysis : public QObject
{
    Q_OBJECT
protected:

    cube::CubeProxy * cube;
    cube::Cnode* root_cnode;
    std::string  test_name;
    QToolBar*    toolbar;
    QStringList  header;


    void
    findRoot()
    {
        const std::vector<cube::Cnode*>& cnodes = cube->getRootCnodes();
        if ( cnodes.size() == 1 )
        {
            root_cnode = cnodes[ 0 ];
            return;
        }
        for ( std::vector<cube::Cnode*>::const_iterator iter = cnodes.begin(); iter != cnodes.end(); ++iter )
        {
            if ( ( ( *iter )->get_callee()->get_name().compare( "main" ) == 0 ) ||
                 ( ( *iter )->get_callee()->get_name().compare( "MAIN" ) == 0 ) )
            {
                root_cnode = *iter;
                return;
            }
        }
        root_cnode = nullptr;
    }

    void
    fillAdviceHeader();

public:
    PerformanceAnalysis( cube::CubeProxy* _cube ) : QObject()
    {
        cube = _cube;
        if ( cube != nullptr )
        {
            findRoot();
        }
        header.clear();
        fillAdviceHeader();
    }

    virtual
    ~PerformanceAnalysis()
    {
    };

    inline
    const
    std::string
    name() const
    {
        return test_name;
    }


    virtual
    QWidget*
    getToolBar() = 0;


    virtual
    QString
    getAnchorHowToMeasure() = 0;


    virtual
    QList<AdvisorAdvice>
    getCandidates( const QList<cubegui::TreeItem*>& ) = 0;

    virtual
    QList<PerformanceTest*>
    getPerformanceTests() = 0;

    QStringList&
    getAdviceHeader()
    {
        return header;
    }




// ------ overview tests ---------

    virtual
    bool
    isActive() const = 0;
};
};

#endif // ADVISER_RATING_WIDGET_H
