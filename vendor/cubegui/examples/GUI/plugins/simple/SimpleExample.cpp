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


#include <QVBoxLayout>
#include <QtPlugin>
#include "SimpleExample.h"
#include "PluginServices.h"

/**
 * If the plugin doesn't load, start cube with -verbose to get detailed information.
 * If the error message "Plugin verification data mismatch" is printed, check if the
 * same compiler and Qt version have been used.
 */

using namespace cubepluginapi;
using namespace simpleexampleplugin;

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2( SimpleExamplePlugin, SimpleExample ); // ( PluginName, ClassName )
#endif

SimpleExample::SimpleExample()
{
    // The constructor should be empty, use cubeOpened to initialize. If Qt widgets or
    // signals/slots are used in constructor, they have to be deleted in destructor,
    // otherwise cube may crash if the plugin is unloaded.
}

bool
SimpleExample::cubeOpened( PluginServices* service )
{
    this->service = service;

    widget_ = new QWidget();
    qlabel_ = new QLabel( "example string" );
    QVBoxLayout* layout = new QVBoxLayout();
    widget_->setLayout( layout );
    layout->addWidget( qlabel_ );

    service->addTab( SYSTEM, this );

    connect( service, SIGNAL( treeItemIsSelected( cubepluginapi::TreeItem* ) ),
             this, SLOT( treeItemIsSelected( cubepluginapi::TreeItem* ) ) );
    return true; // initialisation is ok => plugin should be shown
}

void
SimpleExample::cubeClosed()
{
    delete widget_;
}

/** set a version number, the plugin with the highest version number will be loaded */
void
SimpleExample::version( int& major, int& minor, int& bugfix ) const
{
    major  = 1;
    minor  = 0;
    bugfix = 0;
}

/** unique plugin name */
QString
SimpleExample::name() const
{
    return "Simple Example";
}

QString
SimpleExample::getHelpText() const
{
    return "Just a simple example.";
}

/** widget that will be placed into the tab */
QWidget*
SimpleExample::widget()
{
    return widget_;
}

/** tab label */
QString
SimpleExample::label() const
{
    return "Example Plugin Label";
}

/** slot, which is called if a tree item is selected */
void
SimpleExample::treeItemIsSelected( TreeItem* item )
{
    QString txt = item->getName() + " " + QString::number( item->getValue() );
    qlabel_->setText( txt );
}
