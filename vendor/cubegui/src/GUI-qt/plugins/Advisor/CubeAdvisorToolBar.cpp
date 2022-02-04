/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2015-2020                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include <config.h>
#include "CubeAdvisorToolBar.h"
#include "PluginServices.h"
#include <QToolButton>
#include <QMessageBox>
#include <QInputDialog>
#include <QMenu>
#include <assert.h>

using namespace advisor;


AdvisorToolBar::AdvisorToolBar( cubepluginapi::PluginServices* service ) : QToolBar( tr( "Cube Advisor Toolbar" ) )
{
    this->service = service;

    analysisComboBox = new QComboBox;
    this->addWidget( analysisComboBox );


    runAnalysis = this->addAction( QIcon( ":/images/advisor-analyse-run.png" ), tr( "Run selected analysis" ) );
    copyAction  = this->addAction( QIcon( ":/images/advisor-metrics-copy.png" ), tr( "Copy values of selected metrics into clipboard" ) );
    ;



    this->addSeparator();



    QLabel* _label = new QLabel( tr( "Runtime threshold: " ) );
    this->addWidget( _label );
    set_treashold = new QSlider( Qt::Horizontal );
    set_treashold->setMinimum( 0 );
    set_treashold->setMaximum( 200 );
    set_treashold->setTracking( true );
    this->addWidget( set_treashold );
    treashold = new QLabel( "-" );
    this->addWidget( treashold );

    connect( set_treashold, SIGNAL( valueChanged( int ) ), this, SLOT( setTreashold( int ) ) );
    set_treashold->setValue( 5 );
    setTreashold( 5 );

    this->addSeparator();

    analysisWidgets = new QStackedWidget();
    this->addWidget( analysisWidgets );
}

void
AdvisorToolBar::setTreashold( int nv )
{
    treashold_value = nv;
    treashold->setText( QString( tr( "%1 % runtime" ) ).arg( nv / 2. ) );
    emit treasholdChanged( nv );
}
