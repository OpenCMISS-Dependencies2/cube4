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


#include "config.h"

#include <QVBoxLayout>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QApplication>
#include <iostream>
#include "RemoteFileDialog.h"
#include "Globals.h"

using namespace cubegui;

static QString defaultServer = "cube://localhost:3300";

QString
RemoteFileDialog::getFileName( const QString& directory, QWidget* parent )
{
    try
    {
        RemoteFileDialog dialog( defaultServer, directory, parent );
        dialog.exec();
        dialog.disconnect();
        if ( !dialog.filename.trimmed().isEmpty() )
        {
            defaultServer = dialog.serverLine->text();
            return defaultServer + "/" + dialog.filename;
        }
    }
    catch ( const cube::NetworkError& e )
    {
        QString msg = QString( tr( "Remote file dialog: " ) ) + e.what();
        Globals::setStatusMessage( msg, Error );
    }
    return "";
}

RemoteFileDialog::RemoteFileDialog( const QString& server, const QString& directory, QWidget* parent ) : QDialog( parent )
{
    model = 0;
    proxy = new FileSortFilterProxyModel;
    tree  = new QTreeView();
    tree->setSortingEnabled( true );
    tree->sortByColumn( 0, Qt::AscendingOrder );
    tree->setModel( proxy );
    tree->setFocus();
    connect( tree, SIGNAL( activated( QModelIndex ) ), tree, SLOT( setRootIndex( QModelIndex ) ) );
    connect( tree, SIGNAL( activated( QModelIndex ) ), proxy, SLOT( itemActivated( QModelIndex ) ) );

    this->setWindowTitle( tr( "Open file on server" ) );
    this->setModal( true );
    resize( 600, 400 );

    serverLine = new QLineEdit();
    serverLine->setText( server );
    connect( serverLine, SIGNAL( returnPressed() ), this, SLOT( reconnect() ) );

    // line to insert server (default: cube://localhost:3300)
    QWidget*     serverWidget = new QWidget();
    QHBoxLayout* hlay         = new QHBoxLayout;
    hlay->setContentsMargins( 0, 0, 0, 0 );
    serverWidget->setLayout( hlay );
    hlay->addWidget( serverLine );
    QPushButton* but = new QPushButton( QApplication::style()->standardIcon( QStyle::SP_BrowserReload ), "" );
    but->setToolTip( tr( "Reconnect to server" ) );
    connect( but, SIGNAL( pressed() ), this, SLOT( reconnect() ) );
    hlay->addWidget( but );

    pathLine = new QLineEdit();
    connect( pathLine, SIGNAL( editingFinished() ), this, SLOT( setDirectory() ) );
    pathLine->setText( directory );

    QDialogButtonBox* buttonBox = new QDialogButtonBox();
    buttonBox->addButton( QDialogButtonBox::Cancel );
    buttonBox->addButton( QDialogButtonBox::Ok );
    connect( buttonBox,  SIGNAL( rejected() ), this, SLOT( close() ) );

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget( serverWidget );
    layout->addWidget( pathLine );
    layout->addWidget( tree );
    layout->addWidget( buttonBox );

    reconnect(); // connect to cube server

    tree->resizeColumnToContents( 0 );
    this->setLayout( layout );
}

void
RemoteFileDialog::disconnect()
{
    close();
    this->deleteLater();
    delete model;
    model = NULL;
}

void
RemoteFileDialog::fileSelected( QString fn )
{
    filename = fn;
    accept();
}

void
RemoteFileDialog::setDirectory()
{
    if ( model )
    {
        model->setDirectory( pathLine->text() );
    }
}

void
RemoteFileDialog::reconnect()
{
    proxy->setSourceModel( NULL ); // show empty tree
    delete model;
    model = NULL;
    try
    {
        model = new RemoteFileSystemModel( serverLine->text().trimmed() );
        connect( model, SIGNAL( fileSelected( QString ) ), this, SLOT( fileSelected( QString ) ) );
        connect( model, SIGNAL( directorySelected( QString ) ), pathLine, SLOT( setText( QString ) ) );
        proxy->setSourceModel( model );

        model->setDirectory( "." );
    }
    catch ( const cube::NetworkError& e )
    {
        Globals::setStatusMessage( QString( tr( "Remote file dialog: " ) ) + e.what(), Error );
    }
}
