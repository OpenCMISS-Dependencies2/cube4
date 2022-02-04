/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2019                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include <QtPlugin>
#include <QDebug>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFileDialog>
#include "ContextFreePluginExample.h"
#include "ContextFreeServices.h"
#include "CubeProxy.h"

using namespace contextfreepluginexample;
using namespace cubepluginapi;

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2( ContextFreePluginExample, ContextFreePluginExample ) // ( PluginName, ClassName )
#endif

void
ContextFreePluginExample::version( int& major, int& minor, int& bugfix ) const
{
    major  = 1;
    minor  = 0;
    bugfix = 0;
}

QString
ContextFreePluginExample::name() const
{
    return "Context Free Demo";
}

void
ContextFreePluginExample::opened( ContextFreeServices* service )
{
    this->service = service;
    qDebug() << "context free plugin opened";

    QWidget*     widget = service->getWidget();
    QVBoxLayout* layout = new QVBoxLayout();
    widget->setLayout( layout );

    QPushButton* but = new QPushButton( "Load cube file" );
    layout->addWidget( but );

    connect( but, SIGNAL( clicked() ), this, SLOT( startAction() ) );
}

void
ContextFreePluginExample::startAction()
{
    QString openFileName = QFileDialog::getOpenFileName( service->getWidget(),
                                                         tr( "Choose a file to open" ),
                                                         "",
                                                         tr( "Cube3/4 files (*cube *cube.gz *.cubex);;Cube4 files (*.cubex);;Cube3 files (*.cube.gz *.cube);;All files (*.*);;All files (*)" ) );
    std::vector<std::string> fileNames;
    fileNames.push_back( openFileName.toStdString() );
    cube::CubeProxy* cube = cube::CubeProxy::create( cube::ALGORITHM_EMPTY, fileNames );
    service->openCube( cube ); // will be deleted automatically, if user closes cube
}

void
ContextFreePluginExample::closed()
{
    qDebug() << "context free plugin closed";
    // free all resources allocated in ContextFreePluginExample::opened()
    // children of service->getWidget() will be deleted automatically
}

QString
ContextFreePluginExample::getHelpText() const
{
    return "context free plugin help text";
}
