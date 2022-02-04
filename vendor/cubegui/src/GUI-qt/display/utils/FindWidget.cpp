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

#include <algorithm>
#include <QtGlobal>
#include <QAction>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QApplication>
#include <QDialog>
#include <QInputDialog>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QScrollBar>
#include <QCompleter>
#include <QListView>
#include "Compatibility.h"
#ifdef HAS_QREGULAR_EXPRESSION
#include <QRegularExpression>
#else
#include <QRegExp>
#endif

#include "TreeModelProxy.h"
#include "FindWidget.h"
#include "TreeItem.h"
#include "Globals.h"

using namespace std;
using namespace cubegui;

#define MIN_SEARCH_LENGTH 3

FindWidget::FindWidget( QWidget* parent, TreeModelInterface* treeModelInterface, bool enableSelection ) :
    QWidget( parent ), modelInterface( treeModelInterface )
{
    // ----- editor widget + completer -----
    lineEdit = new QLineEdit;
#if QT_VERSION >= 0x050000
    lineEdit->setClearButtonEnabled( true );
#endif
    completer = new QCompleter();
    completer->setCompletionMode( QCompleter::PopupCompletion );
    completer->setModel( &completerModel );
    completer->setWidget( lineEdit );
    completer->setCompletionRole( Qt::UserRole );
    QListView* box = new QListView();
    completer->setPopup( box );

    // ----- widgets -----
    QPushButton* exitButton = new QPushButton( QApplication::style()->standardIcon( QStyle::SP_TitleBarCloseButton ), "" );
    exitButton->setFlat( true );
    exitButton->setFixedSize( 16, 16 );
    QLabel* label = new QLabel( tr( "Regular expression to search for:" ) );
    label->setBuddy( lineEdit );
    findButton = new QPushButton( QApplication::style()->standardIcon( QStyle::SP_DialogApplyButton ), "" );
    findButton->setToolTip( "search" );
    selectButton = new QPushButton( tr( "select found" ) );
    expandButton = new QPushButton( tr( "expand found" ) );

    QPushButton* up = new QPushButton( QApplication::style()->standardIcon( QStyle::SP_ArrowUp ), "" );
    up->setToolTip( "select next found item" );
    QPushButton* down = new QPushButton( QApplication::style()->standardIcon( QStyle::SP_ArrowDown ), "" );
    down->setToolTip( "select previous found item" );

    // ----- layout -----

    QWidget*     exitLine = new QWidget;
    QHBoxLayout* tLayout  = new QHBoxLayout;
    exitLine->setLayout( tLayout );
    tLayout->setSpacing( 0 );
    QMargins margins = tLayout->contentsMargins();
    tLayout->setContentsMargins( margins.left(), 0, 0, 0 );
    tLayout->addWidget( label );
    tLayout->addStretch( 1 );
    tLayout->addWidget( exitButton );

    QWidget*     line    = new QWidget;
    QHBoxLayout* hLayout = new QHBoxLayout;
    hLayout->setContentsMargins( 0, 0, 0, 0 );
    line->setLayout( hLayout );
    hLayout->addWidget( lineEdit );
    hLayout->addWidget( findButton );
    hLayout->addWidget( up );
    hLayout->addWidget( down );

    QWidget*     options      = new QWidget;
    QHBoxLayout* optionLayout = new QHBoxLayout;
    optionLayout->setContentsMargins( 0, 0, 0, 0 );
    options->setLayout( optionLayout );
    optionLayout->addWidget( expandButton );
    if ( enableSelection )
    {
        optionLayout->addWidget( selectButton );
    }
    optionLayout->addStretch( 1 );

    QWidget*     main        = new QWidget;
    QVBoxLayout* outerLayout = new QVBoxLayout;
    this->setLayout( outerLayout );
    outerLayout->setContentsMargins( 0, 0, 0, 0 );
    outerLayout->setSpacing( 0 );
    outerLayout->addWidget( exitLine );
    outerLayout->addWidget( main );

    QVBoxLayout* mainLayout = new QVBoxLayout;
    main->setLayout( mainLayout );
    mainLayout->addWidget( line );
    mainLayout->addWidget( options );

    // ----- actions -----
    connect( findButton, SIGNAL( clicked() ), this, SLOT( emitSearch() ) );
    connect( lineEdit, SIGNAL( editingFinished() ), this, SLOT( emitSearch() ) );
    connect( lineEdit, SIGNAL( textChanged( QString ) ), this, SLOT( autoComplete( QString ) ) );
    connect( completer, SIGNAL( activated( QString ) ), this, SLOT( insertCompletion( QString ) ) );
    connect( exitButton, SIGNAL( pressed() ), this, SLOT( close() ) );
    connect( selectButton, SIGNAL( pressed() ), this, SLOT( selectItems() ) );
    connect( expandButton, SIGNAL( pressed() ), this, SLOT( expandItems() ) );
    connect( up, SIGNAL( pressed() ), this, SLOT( upSelected() ) );
    connect( down, SIGNAL( pressed() ), this, SLOT( downSelected() ) );
    up->setShortcut( QKeySequence( Qt::SHIFT | Qt::Key_F3 ) );
    down->setShortcut( QKeySequence( Qt::Key_F3 ) );

    lineEdit->setFocus();
    enableButtons( false );
}

void
FindWidget::upSelected()
{
    emit previous();
}

void
FindWidget::downSelected()
{
    emit next();
}

QString
FindWidget::getText() const
{
    return lineEdit->text();
}

void
FindWidget::setText( const QString& txt )
{
    lineEdit->blockSignals( true );
    lineEdit->setText( txt );
    lineEdit->selectAll();
    lineEdit->blockSignals( false );
    lineEdit->setFocus();
    enableButtons( txt.trimmed().size() >= MIN_SEARCH_LENGTH );
}

void
FindWidget::selectItems()
{
    emit search( getText(), SELECT );
}

void
FindWidget::expandItems()
{
    emit search( getText(), EXPAND );
}

void
FindWidget::enableButtons( bool enable )
{
    findButton->setEnabled( enable );
    selectButton->setEnabled( enable );
    expandButton->setEnabled( enable );
}

void
FindWidget::emitSearch()
{
    if ( getText().trimmed().size() >= MIN_SEARCH_LENGTH )
    {
        emit search( getText() );
    }
    /*else if ( getText().trimmed().size() > 0 )
       {
        //QString msg = "Search expression requires minimal size of " + QString::number( MIN_SEARCH_LENGTH );
        //Globals::setStatusMessage( msg, Warning, false );
       }*/
}

void
FindWidget::insertCompletion( const QString& text )
{
#ifdef HAS_QREGULAR_EXPRESSION
    lineEdit->setText( "^" + QRegularExpression::escape( text ) + "$" );
#else
    lineEdit->setText( "^" + QRegExp::escape( text ) + "$" );
#endif
    completer->popup()->hide();
    emitSearch();
}

void
FindWidget::autoComplete( const QString& text )
{
    if ( text.trimmed().length() < MIN_SEARCH_LENGTH )
    {
        QPalette colorFound;
        colorFound.setColor( QPalette::Base, Qt::white );
        lineEdit->setPalette( colorFound );

        completer->popup()->hide();
        enableButtons( false );
        return;
    }
    enableButtons( true );
#ifdef HAS_QREGULAR_EXPRESSION
    QModelIndexList foundItems = modelInterface->find( QRegularExpression( text ) );
#else
    QModelIndexList foundItems = modelInterface->find( QRegExp( text ) );
#endif
    if ( foundItems.size() > 0 )
    {
        Globals::setStatusMessage( tr( "Expression matches " ) + QString::number( foundItems.size() ) + tr( " items" ), Information, false );
        QPalette colorFound;
        colorFound.setColor( QPalette::Base, QColor( Qt::green ).lighter( 190 ) );
        lineEdit->setPalette( colorFound );

        QHash<QString, int> found;
        foreach( QModelIndex idx, foundItems )
        {
            TreeItem* item = ( TreeItem* )qvariant_cast<void*>( modelInterface->getModel()->data( idx, TreeItemRole ) );
            //QString label = modelInterface->getModel()->data( idx, Qt::DisplayRole ).toString();
            QString label = item->getName();
            int     count = found.value( label, 0 );
            found.insert( label, count + 1 );
        }
        // show completer tooltip, if not too many items match
        {
            completerModel.clear();
            QList<QString> list = found.keys();
            std::sort( list.begin(), list.end() );
            foreach( QString str, list )
            {
                QString label = str;
                if ( found.value( str ) > 1 )
                {
                    label += " (" + QString::number( found.value( str ) ) + ")"; // completer text to display
                }
                QStandardItem* item = new QStandardItem( label );
                item->setData( str, Qt::UserRole ); // completer text to insert
                completerModel.appendRow( item );
            }
            completer->complete();
        }
    }
    else
    {
        completer->popup()->hide();
        QPalette colorFound;
        colorFound.setColor( QPalette::Base, QColor( Qt::red ).lighter( 190 ) );
        lineEdit->setPalette( colorFound );
    }
}
