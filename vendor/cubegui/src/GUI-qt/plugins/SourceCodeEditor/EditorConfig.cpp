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
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QComboBox>
#include <QFormLayout>

#include "EditorConfig.h"

using namespace editor_plugin;

EditorConfig::EditorConfig( QWidget* parent, QHash<QString, QStringList>& externalEditors, QString& externalEditor  )
    : QDialog( parent ), editorHash( externalEditors ), editorName( externalEditor )
{
    tmpEditorHash = editorHash;
    setWindowTitle( tr( "Configure external editor" ) );

    //add ok and cancel buttons to the dialog
    QDialogButtonBox* buttonBox = new QDialogButtonBox( this );
    buttonBox->addButton( QDialogButtonBox::Ok );
    //QPushButton* applyButton = buttonBox->addButton( QDialogButtonBox::Apply );
    buttonBox->addButton( QDialogButtonBox::Cancel );
    connect( buttonBox, SIGNAL( accepted() ), this, SLOT( accept() ) );
    connect( buttonBox, SIGNAL( rejected() ), this, SLOT( reject() ) );
    //connect( applyButton, SIGNAL( clicked() ), this, SLOT( apply() ) );

    editorCombo = new QComboBox();
    editorCombo->addItems( editorHash.keys() );
    editorCombo->setCurrentIndex( editorCombo->findText( editorName ) );
    connect( editorCombo, SIGNAL( currentIndexChanged( int ) ), this, SLOT( editorChanged( int ) ) );
    editorNameInput     = new QLineEdit();
    editorCommandInput1 = new QLineEdit();
    editorCommandInput2 = new QLineEdit();
    int minimumWidth = editorCommandInput1->fontMetrics().boundingRect( QChar( '0' ) ).width() * 40;
    editorCommandInput1->setMinimumWidth( minimumWidth );

    configWidget = new QWidget();
    configWidget->setLayout( new QVBoxLayout() );
    QFormLayout* vbox     = new QFormLayout();
    QGroupBox*   groupBox = new QGroupBox( tr( "Select External Editor" ), this );
    groupBox->setLayout( vbox );
    vbox->addRow( "", editorCombo );
    vbox->addRow( tr( "editor name" ), editorNameInput );
    vbox->addRow( tr( "initial command" ), editorCommandInput1 );
    vbox->addRow( tr( "command" ), editorCommandInput2 );

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget( groupBox );
    layout->addWidget( configWidget );
    layout->addSpacerItem( new QSpacerItem( 1, 1, QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );
    layout->addWidget( buttonBox );
    setLayout( layout );

    if ( editorHash.contains( editorName ) )
    {
        editorChanged( editorCombo->findText( editorName ) );
    }
    else
    {
        editorChanged( editorCombo->currentIndex() );
    }
    this->exec();
}

void
EditorConfig::editorChanged( int index )
{
    QString     name     = editorCombo->itemText( index );
    QStringList commands = editorHash.value( name );
    if ( commands.size() != 2 )
    {
        return;
    }
    editorNameInput->setText( name );
    editorCommandInput1->setText( commands.at( 0 ) );
    editorCommandInput2->setText( commands.at( 1 ) );
}

void
EditorConfig::close()
{
    this->setVisible( false );
    this->deleteLater();
}

void
EditorConfig::accept()
{
    editorName = editorNameInput->text();
    if ( !editorName.isEmpty() )
    {
        QStringList commands = QStringList() << editorCommandInput1->text() << editorCommandInput2->text();
        editorHash.insert( editorName, commands );
        setVisible( false ); // reject gets called, if setVisible(false) isn't called before close
    }
    close();
}

void
EditorConfig::apply()
{
}

void
EditorConfig::reject() // todo restore view setting
{
    close();
}
