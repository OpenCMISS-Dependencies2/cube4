/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2021                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/



#ifndef CUBE_TOOLS_ADVISOR_H
#define CUBE_TOOLS_ADVISOR_H

#include <QPushButton>
#include <QLabel>
#include <QCheckBox>
#include <QListWidget>
#include <QTableWidget>
#include "CubeRatingWidget.h"
#include "CubePlugin.h"
#include "PluginServices.h"
#include "DefaultMarkerLabel.h"

#include "CubeAdvisorToolBar.h"

#include "POPAuditAnalysis.h"
#include "POPHybridAuditAnalysis.h"
#include "BSPOPHybridAuditAnalysis.h"
#include "POPHybridAuditAnalysisAdd.h"
#include "JSCHybridAuditAnalysis.h"
#include "KnlVectorizationAnalysis.h"
#include "KnlMemoryAnalysis.h"
#include "Future.h"

namespace advisor
{
class CubeAdvisor : public QObject, public cubepluginapi::TabInterface, public cubepluginapi::CubePlugin
{
    Q_OBJECT
    Q_INTERFACES( cubepluginapi::CubePlugin )
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA( IID "CubeAdvisorPlugin" )
#endif


public:
    CubeAdvisor();

    ~CubeAdvisor();

    // ContextFreePlugin interface
    virtual QString
    name() const;

    virtual bool
    cubeOpened( cubepluginapi::PluginServices* service );

    virtual void
    cubeClosed();


    virtual void
    version(        int& major,
                    int& minor,
                    int& bugfix ) const;

    virtual QString
    getHelpText() const;

    virtual QString
    getHelpURL() const
    {
        return "Advisor.html";
    }
    // TabInterface implementation
    virtual QWidget*
    widget();

    virtual QString
    label() const;

    QIcon
    icon() const;

    virtual void
    setActive( bool active );



private slots:
    void
    contextMenuIsShown( cubepluginapi::DisplayType type,
                        cubepluginapi::TreeItem*   item );

    void
    treeItemIsSelected( cubepluginapi::TreeItem* item );

    void
    treasholdChanged( int nv = 0 );


    void
    defineTreeItemMarker();


    void
    tableItemClicked( QTableWidgetItem* );

    void
    showAnalysis( int );

    void
    copyMetrics( bool );

    void
    runAnalysis();

    void
    recalculate( void );

    void
    automatic_recalculate( int );

    void
    direct_calculate( int );

    void
    initializationFinished( void );

    void
    analyseSubtree();



private:
    QWidget*                       _widget;
    cubegui::TreeItem*             context_menu_item;
    cubepluginapi::PluginServices* service;
    cube::CubeProxy*               cube;
    QStackedWidget*                analyses;
    QComboBox*                     analyses_selector;

    AdvisorToolBar* toolBar;
    int             treashold_value;
    double          total_execution;
    bool            initializationIsFinished;
    bool            startAnalysis;
    bool            direct_calculation_state;


    QList<const cubepluginapi::TreeItemMarker*> markerList;
    cubepluginapi::DefaultMarkerLabel*          markerLabel;

    cube::Metric*             execution;
    QList<cubegui::TreeItem*> root_callpaths;


    POPAuditPerformanceAnalysis*          pop_audit_analysis;
    POPHybridAuditPerformanceAnalysis*    pop_hybrid_audit_analysis;
    BSPOPHybridAuditPerformanceAnalysis*  pop_hybrid_audit_analysis_bs;
    POPHybridAuditPerformanceAnalysisAdd* pop_hybrid_additive_audit_analysis;
    JSCAuditPerformanceAnalysis*          jsc_audit_analysis;
    KnlVectorizationAnalysis*             knl_vectorization_analysis;
    KnlMemoryAnalysis*                    knl_memory_analysis;
    QPushButton*                          recalculate_button;
    QCheckBox*                            automatic_recalculation;
    QCheckBox*                            direct_calculation;
    QWidget*                              recalculate_widget;

    cubepluginapi::Future* future;
    /*
       CachePerformanceAnalysis *         cache_analysis;
       CPUAnalysis *                      cpu_analysis; */


    void
    createTests();

    void
    calculateOverallTests();

    void
    initialization_run();

    friend class InitialisationTask;
};

class InitialisationTask : public cubepluginapi::Task
{
public:
    InitialisationTask( CubeAdvisor* adv ) : advisor( adv )
    {
    }
    virtual void
    calculate()
    {
        advisor->initialization_run();
    }

protected:
    CubeAdvisor* advisor;
};
}

#endif // CUBE_TOOLS_H
