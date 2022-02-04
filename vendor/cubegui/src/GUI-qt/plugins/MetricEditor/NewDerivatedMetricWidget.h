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



#ifndef _NEW_DERIVATED_METRIC_WIDGET_H
#define _NEW_DERIVATED_METRIC_WIDGET_H


#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QString>
#include <QLabel>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QTextStream>
#include <QKeyEvent>
#include <QMenu>
#include <QSettings>
#include <QCheckBox>

#include "CubeTypes.h"
#include "CubePLSyntaxHighlighter.h"
#include "DerivedMetricEditor.h"
#include "MetricData.h"
#include "PluginServices.h"

namespace cube
{
class CubeProxy;
class Metric;
}

namespace cubegui
{
class HelpBrowser;
class StatusBar;
}

namespace metric_editor
{
/*-------------------------------------------------------------------------*/
/**
 * @file  NewDerivatedMetricWidget.h
 * @brief Declaration of the widget, used to create a new derivated metric
 *
 * This header file provides the declaration of the class HelpBrowser.
 */
/*-------------------------------------------------------------------------*/

class NewDerivatedMetricWidget : public QDialog
{
    Q_OBJECT

public:
    NewDerivatedMetricWidget( cubepluginapi::PluginServices* _service,
                              cube::Metric*                  to_edit_metric,
                              cube::Metric*                  met_parent,
                              QWidget*                       parent,
                              QList<MetricData*>&            userMetrics );

    ~NewDerivatedMetricWidget();

    cube::Metric*
    get_created_metric( void )
    {
        return working_metric;
    }

    QList<QString>
    getReferredMetrics( MetricData* data );

    static const QString separator;
protected:
    void
    dragEnterEvent( QDragEnterEvent* event );
    void
    dropEvent( QDropEvent* event );
    void
    fillTheFormFromUrl( QUrl& );
    void
    fillTheFormFromClipboard();
    void
    fillTheForm( const QString& cubePL );
    void
    enableShareLink( bool );
    QString
    packDataToString();

private slots:
    void
    addUserMetric();
    void
    removeUserMetric();
    void
    selectMetricFromLibrary( int );
    void
    setMetricType( int );
    void
    evaluateMetricParentCombo( const int idx );
    void
    setDisplayName( const QString& );
    void
    setUniqName( const QString& );
    void
    setGhost( bool isGhost );
    void
    setRowWise( bool isRowWise );
    void
    setDescription();
    void
    setCubePLExpression( void );
    void
    setCubePLInitExpression( void );
    void
    setCubePLAggrPlusExpression( void );
    void
    setCubePLAggrMinusExpression( void );
    void
    setCubePLAggrAggrExpression( void );
    void
    showCubePLHelp( void );
    void
    showKindsOfDerivedMetricsHelp( void );
    void
    showMetricExpressionHelp( void );
    QStringList
    getCompletionList();
    void
    createMetricFromFile( void );
    void
    saveMetricToFile( void );
    void
    createMetricFromClipboard( void );
    void
    createMetric();
    void
    selectFileForFillingForm( void );
    void
    setUom( const QString& value );
    void
    setUrl( const QString& value );

private:
    cubepluginapi::PluginServices* service;
    cube::CubeProxy*               cube;
    cube::Metric*                  working_metric;

    QHash<QString, QString> metricLabelHash;

    bool isUnique; // true, if current metric name is unique
    bool isEdited; // true, if an existing metric is edited; false, if a new metric has been created

    MetricData*         metricData;
    QList<MetricData*>& userMetrics;
    QList<QString>      availableMetricNames;         // list of uniq names of all metrics that are defined in the cube file

    QHash<QString, MetricData*> predefinedMetricHash; // uniq names of all predefined metrics -> metric dataa
    QHash<QString, MetricData*> userMetricHash;       // uniq names of all user metrics -> metric dataa

    QComboBox*  metric_parent_selection;
    QComboBox*  metric_type_selection;
    QComboBox*  metric_library_selection;
    QLineEdit*  display_name_input;
    QLineEdit*  unique_name_input;
    QLineEdit*  uom_input;
    QLineEdit*  url_input;
    QTextEdit*  description_input;
    QTabWidget* calculations_container;
    QCheckBox*  isGhostCheck;
    QCheckBox*  isRowWiseCheck;
    QLabel*     metricDataType;

    DerivedMetricEditor* calculation_input;
    DerivedMetricEditor* calculation_init_input;
    DerivedMetricEditor* calculation_aggr_plus;
    DerivedMetricEditor* calculation_aggr_minus;
    DerivedMetricEditor* calculation_aggr_aggr;

    QPushButton* create_metric;
    QPushButton* _whatis_help;
    QPushButton* _metric_kinds_help;
    QPushButton* _metric_expt_help;
    QPushButton* _metric_lib_help;
    QPushButton* metric_from_file;
    QPushButton* metric_to_file;
    QPushButton* metric_from_clipboard;
    QLabel*      share_metric;

    CubePLSyntaxHighlighter* error_highlighter;
    CubePLSyntaxHighlighter* error_init_highlighter;
    CubePLSyntaxHighlighter* error_aggr_plus_highlighter;
    CubePLSyntaxHighlighter* error_aggr_minus_highlighter;
    CubePLSyntaxHighlighter* error_aggr_aggr_highlighter;

    cubegui::StatusBar* statusBar;    // for error messages

    QList<QString>
    getNextReferredMetrics( const QString& metricStr );
    bool
    prepareMetric();
    QString
    getPredefinedMetricText( const QString& metric );
    void
    addMetricItem( const cube::Metric* metric );
};
}
#endif
