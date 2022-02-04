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


#ifndef INFOWIDGET_H
#define INFOWIDGET_H

#include <QLayout>
#include <QSplitter>
#include <QTextBrowser>
#include <QPushButton>
#include <QTabWidget>
#include "TabInterface.h"

namespace cubegui
{
class HtmlWidget;

/**
 * @brief The InfoWidget class
 * The InfoWidget class displays a textual message in a tabbed widget.
 */
class InfoWidget : public QWidget, public TabInterface
{
    Q_OBJECT
public:
    enum ContentType { Info, TreeInfo, PluginInfo };

    static InfoWidget*
    getInstance();

    ~InfoWidget();

    void
    showTreeInfo( const QString& metric,
                  const QString& call,
                  const QString& system,
                  const QString& metricUrl,
                  const QString& callUrl,
                  const QString& global = "",
                  const QString& debug = ""
                  );

    void
    showPluginInfo( const QString& txt,
                    QPushButton*   button );

    /* The panel which contains the system item information is minimized per default. This function distributes
     * the horizontal space equally among all 3 trees info panels */
    void
    showSystemInfo();

    void
    activateTab( DisplayType type );

    // TabInterface interface
    QWidget*
    widget();
    QString
    label() const;
    QIcon
    icon() const;
    void
    setActive( bool );

public slots:
    /** adds the info tab to the system tab as subtab in "Other" */
    void
    activate();

    /** sets the info tab as current tab or raises the window, if the tab is detached */
    void
    toFront();

signals:
    void
    tabActivated();

private slots:
    void
    changeTreeInfoOrientation();

private:
    InfoWidget();

    // tree info components
    QSplitter*    horizontal;
    QSplitter*    vertical;
    QTextBrowser* metricInfo;
    QTextBrowser* callInfo;
    QTextBrowser* sysInfo;
    QTextBrowser* globalInfo;
    QTextBrowser* debugInfo;
    QTabWidget*   doc;

    QWidget*    treeInfo;
    ContentType content;

    static InfoWidget* single;
};
}
#endif // INFOWIDGET_H
