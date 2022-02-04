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

#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QApplication>
#include <QPushButton>
#include <QFontComboBox>
#include <QColorDialog>
#include <QTreeView>
#include <QGroupBox>
#include <QLabel>
#include <QDebug>

#include "Globals.h"
#include "StyleSheetEditor.h"
#include "HelpBrowser.h"

using namespace cubegui;

StyleSheetEditor* StyleSheetEditor::single = 0;

StyleSheetEditor*
StyleSheetEditor::getInstance()
{
    if ( single == 0 )
    {
        single = new StyleSheetEditor( Globals::getMainWindow() );
    }
    return single;
}

/** minimal constructor to enable connection to slot */
StyleSheetEditor::StyleSheetEditor( QWidget* parent ) : QDialog( parent )
{
    editor = 0;
    style  = "";
}

void
StyleSheetEditor::init()
{
    this->setWindowTitle( tr( "Stylesheet editor" ) );

    QVBoxLayout* main = new QVBoxLayout;
    this->setLayout( main );

    QGroupBox*   groupBox    = new QGroupBox( tr( "Pick properties for font-family and color:" ) );
    QVBoxLayout* groupLayout = new QVBoxLayout();
    groupBox->setLayout( groupLayout );
    // add font selection widget
    QFontComboBox* fontCombo = new QFontComboBox( this );
    groupLayout->addWidget( fontCombo );

    // add color selection widget
    colorBut = new QPushButton( tr( "Choose Color " ) );
    colorBut->setFlat( true );
    colorBut->setAutoFillBackground( true );
    QPalette palette = colorBut->palette();
    QColor   color   = QTreeView().palette().highlight().color();
    palette.setColor( colorBut->backgroundRole(), color );
    colorBut->setPalette( palette );
    connect( colorBut, SIGNAL( pressed() ), this, SLOT( chooseColor() ) );
    groupLayout->addWidget( colorBut );

    editor = new QTextEdit();

    QDialogButtonBox* buttonBox = new QDialogButtonBox( QDialogButtonBox::Help | QDialogButtonBox::Cancel | QDialogButtonBox::Apply | QDialogButtonBox::Ok );
    QPushButton*      apply     = buttonBox->button( QDialogButtonBox::Apply );
    connect( buttonBox, SIGNAL( accepted() ), this, SLOT( ok() ) );
    connect( buttonBox, SIGNAL( rejected() ), this, SLOT( cancel() ) );
    connect( buttonBox, SIGNAL( helpRequested() ), this, SLOT( help() ) );
    connect( apply,     SIGNAL( pressed() ), this, SLOT( apply() ) );

    main->addWidget( new QLabel( tr( "Edit Stylesheet:" ) ) );
    main->addWidget( editor );
    main->addWidget( groupBox );
    main->addWidget( buttonBox );
    resize( 600, 400 );
    setVisible( true );
}

void
StyleSheetEditor::loadGlobalStartupSettings( QSettings& settings )
{
    style = settings.value( "StyleSheet", style ).toString();
    qApp->setStyleSheet( style );
}

void
StyleSheetEditor::saveGlobalStartupSettings( QSettings& settings )
{
    settings.setValue( "StyleSheet", style );
}

QString
StyleSheetEditor::settingName()
{
    return "StyleSheetEditor";
}

void
StyleSheetEditor::chooseColor()
{
    QPalette palette = colorBut->palette();
    QColor   color   = colorBut->palette().color( colorBut->backgroundRole() );
    color = QColorDialog::getColor( color, this );
    if ( color.isValid() )
    {
        palette.setColor( colorBut->backgroundRole(), color );
        colorBut->setText( tr( "Color: " ) + color.name() );
        colorBut->setPalette( palette );
    }
}

void
StyleSheetEditor::help()
{
    QString url = Globals::getOption( DocPath ) + "cubegui/guide/html/stylesheets.html";
    HelpBrowser::getInstance()->showUrl( url );
}

void
StyleSheetEditor::apply()
{
    qApp->setStyleSheet( editor->toPlainText() );
}

void
StyleSheetEditor::cancel()
{
    editor->setText( style );
    qApp->setStyleSheet( style );
    setVisible( false );
}

void
StyleSheetEditor::ok()
{
    apply();
    style = editor->toPlainText();
    setVisible( false );
}

void
StyleSheetEditor::configureStyleSheet()
{
    if ( editor == 0 )
    {
        init();
    }
    /* show template, if style is empty */
    if ( style.trimmed().isEmpty() )
    {
        style = "/* " + tr( "this is a template; uncomment to activate" ) + "\n\
    QWidget { \n\
        font-size: 10pt \n\
    } \n\
             \n\
    QTreeView { \n\
        color: black ;\n\
        background-color: lightgray; \n\
        selection-color:blue; \n\
        selection-background-color:yellow; \n\
        font-family: Sans Serif; \n\
        font-size: 10pt \n\
    }\n*/";
    }
    editor->setText( style );

    this->setVisible( true );
}
