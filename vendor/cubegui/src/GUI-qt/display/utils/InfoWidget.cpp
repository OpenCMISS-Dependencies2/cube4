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


#include "config.h"

#include <QDebug>
#include <QApplication>
#include <QPushButton>
#include <QStyle>
#include <QLabel>
#include "InfoWidget.h"
#include "Globals.h"
#include "TabManager.h"
#include "HtmlWidget.h"

using namespace cubegui;

InfoWidget* InfoWidget::single = 0;

InfoWidget*
InfoWidget::getInstance()
{
    return single ? single : single = new InfoWidget();
}

InfoWidget::~InfoWidget()
{
    single = 0;
}

InfoWidget::InfoWidget()
{
    content = Info;
}

void
InfoWidget::showSystemInfo()
{
    QList<int> sizeList;
    sizeList << width() / 3 << width() / 3 << width() / 3;
    if ( horizontal )
    {
        horizontal->setSizes( sizeList );
    }
}

void
InfoWidget::showTreeInfo( const QString& metric, const QString& call, const QString& system,
                          const QString& metricUrl, const QString& callUrl,
                          const QString& global, const QString& debug
                          )
{
    if ( !isVisible() )
    {
        return;
    }
    if ( content != TreeInfo )
    {
        qDeleteAll( children() );
        metricInfo = new QTextBrowser();
        callInfo   = new QTextBrowser();
        sysInfo    = new QTextBrowser();
        globalInfo = new QTextBrowser();
        if ( Globals::optionIsSet( ExpertMode ) && Globals::optionIsSet( VerboseMode ) )
        {
            debugInfo = new QTextBrowser();
        }
        doc = new QTabWidget();
        doc->addTab( HtmlWidget::createHtmlWidget(), tr( "Metric Documentation" ) );
        doc->addTab( HtmlWidget::createHtmlWidget(), tr( "Call path/Region Documentation" ) );
        doc->addTab( globalInfo, tr( "Global Information" ) );
        if ( Globals::optionIsSet( ExpertMode ) && Globals::optionIsSet( VerboseMode ) )
        {
            doc->addTab( debugInfo, tr( "Debug Information" ) );
        }
        horizontal = new QSplitter( Qt::Horizontal );
        vertical   = new QSplitter( Qt::Vertical );
        horizontal->addWidget( metricInfo );
        horizontal->addWidget( callInfo );
        horizontal->addWidget( sysInfo );
        vertical->addWidget( horizontal );
        vertical->addWidget( doc );

        QList<int> sizeList;
        sizeList << width() / 2 << width() / 2 << 0;
        horizontal->setSizes( sizeList );

        sizeList.clear();
        sizeList << height() / 2 << height() / 2;
        vertical->setSizes( sizeList );

        setLayout( new QVBoxLayout );
        layout()->setContentsMargins( 0, 0, 0, 0 );
        layout()->addWidget( vertical );
    }

    metricInfo->setText( metric );
    callInfo->setText( call );
    sysInfo->setText( system );

    globalInfo->setText( global );
    if ( Globals::optionIsSet( ExpertMode ) && Globals::optionIsSet( VerboseMode ) )
    {
        debugInfo->setText( debug );
    }

    if ( doc->height() > 0 )
    {
        HtmlWidget* html = static_cast<HtmlWidget*> ( doc->widget( 0 ) );
        html->showUrl( metricUrl );
        html = static_cast<HtmlWidget*> ( doc->widget( 1 ) );
        html->showUrl( callUrl );
    }
    else
    {
        HtmlWidget* html = static_cast<HtmlWidget*> ( doc->widget( 0 ) );
        html->showUrl( "" );
        html = static_cast<HtmlWidget*> ( doc->widget( 1 ) );
        html->showUrl( "" );
    }
    toFront();

    content = TreeInfo;
}

void
InfoWidget::changeTreeInfoOrientation()
{
    if ( horizontal->orientation() == Qt::Horizontal )
    {
        horizontal->setOrientation( Qt::Vertical );
    }
    else
    {
        horizontal->setOrientation( Qt::Horizontal );
    }
}

void
InfoWidget::showPluginInfo( const QString& text, QPushButton* button )
{
    if ( !isVisible() )
    {
        return;
    }
    qDeleteAll( children() );
    QVBoxLayout* verticalLayout = new QVBoxLayout( this );
    verticalLayout->setSpacing( 6 );
    verticalLayout->setContentsMargins( 11, 11, 11, 11 );
    setLayout( verticalLayout );

    QTextBrowser* textBrowser = new QTextBrowser( this );
    textBrowser->setText( text );
    layout()->addWidget( textBrowser );
    layout()->addWidget( button );

    layout()->invalidate();
    content = PluginInfo;
}

/** activate the metric or call tree documentation tab */
void
InfoWidget::activateTab( DisplayType type )
{
    doc->setCurrentIndex( type == METRIC ? 0 : 1 );
}

void
InfoWidget::activate()
{
    if ( !this->isVisible() )
    {
        Globals::getTabManager()->getTab( SYSTEM )->addTabInterface( this, true, OTHER_PLUGIN_TAB );
    }
}

void
InfoWidget::toFront()
{
    Globals::getTabManager()->getTab( SYSTEM )->toFront( this );
}

QWidget*
InfoWidget::widget()
{
    return this;
}

QString
InfoWidget::label() const
{
    return tr( "Info" );
}

QIcon
InfoWidget::icon() const
{
    return QApplication::style()->standardIcon( QStyle::SP_MessageBoxInformation );
}

void
InfoWidget::setActive( bool active )
{
    if ( active )
    {
        emit tabActivated(); // send info if info widget tab has been selected
    }
}
