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

#include "SettingsToolBar.h"
#include "PluginManager.h"
#include <QAction>
#include <QVBoxLayout>
#include <QMenu>
#include <QDebug>
#include <QInputDialog>

using namespace cubegui;

BookmarkToolBar::BookmarkToolBar() : QToolBar( tr( "Bookmark Toolbar" ) )
{
    // create menu to load previouly saved bookmarks (=experiment settings)
    loadAct  = new QAction( tr( "Load Bookmarks" ), this );
    loadMenu = new QMenu( this );
    loadAct->setMenu( loadMenu );
    addAction( loadAct );
#if QT_VERSION >= QT_VERSION_CHECK( 5, 1, 0 )
    loadMenu->setToolTipsVisible( true );
#endif
    connect( loadMenu, SIGNAL( aboutToShow() ), this, SLOT( setMenuGeometry() ) );
    connect( loadAct, SIGNAL( triggered() ), this, SLOT( loadSettings() ) );

    // create button to save bookmarks
    QAction* saveAct = new QAction( tr( "Save Bookmark" ), this );
    addAction( saveAct );
    connect( saveAct, SIGNAL( triggered() ), this, SLOT( saveSettings() ) );

    // create button to delete experiment settings
    QAction* editAct = new QAction( tr( "Edit Bookmark" ), this );
    addAction( editAct );
    connect( editAct, SIGNAL( triggered() ), this, SLOT( editSettings() ) );
}

void
BookmarkToolBar::setMenuGeometry()
{
    loadMenu->setMinimumWidth( widgetForAction( loadAct )->width() );
}

void
BookmarkToolBar::loadBookmarkSlot()
{
    QAction* action = dynamic_cast<QAction*> ( sender() );
    QString  name   = action->data().toString();
    emit     loadBookmark( name );
}

void
BookmarkToolBar::setBookmarks( QList<Bookmark> bookmarks )
{
    this->bookmarks = bookmarks;

    {   // fill bookmark menu
        loadMenu->clear();
        foreach( Bookmark bm, bookmarks )
        {
            QAction* action = loadMenu->addAction( bm.getName() );
            action->setData( bm.getName() );
            action->setToolTip( bm.getDescription() );
            connect( action, SIGNAL( triggered() ), this, SLOT( loadBookmarkSlot() ) );
        }
    }
}

void
BookmarkToolBar::loadSettings()
{
    QAction* action = dynamic_cast<QAction*> ( sender() );
    QWidget* parent = 0;
    if ( action )
    {
        parent = widgetForAction( action );
    }
    LoadBookmarksDialog* dialog = new LoadBookmarksDialog( bookmarks, parent );
    dialog->exec();
    QString name = dialog->getName();
    if ( !name.isEmpty() )
    {
        emit loadBookmark( name );
    }
}

void
BookmarkToolBar::saveSettings()
{
    SaveBookmarksDialog* dialog = new SaveBookmarksDialog( bookmarks, Globals::getMainWindow() );
    dialog->exec();
    QString name = dialog->getName();
    if ( !name.isEmpty() )
    {
        emit saveBookmark( name, dialog->getDescription() );
    }
}

void
BookmarkToolBar::editSettings()
{
    EditBookmarksDialog* dialog = new EditBookmarksDialog( bookmarks, Globals::getMainWindow() );
    dialog->exec();
    if ( dialog->result() == QDialog::Accepted )
    {
        emit changeBookmarks( dialog->getBookmarks() );
    }
}


//=================================================================================================================

LoadBookmarksDialog::LoadBookmarksDialog( const QList<Bookmark>& bookmarks, QWidget* parent ) : QDialog( parent )
{
    this->setWindowTitle( tr( "Load Bookmark" ) );
    this->bookmarks = bookmarks;

    //QPoint pos = parent->mapToGlobal( parent->pos() );
    //pos.ry() += parent->height() + this->frameSize().height();
    //this->move( pos.x(), pos.y() );

    list = new QListWidget();
    foreach( Bookmark bm, bookmarks )
    {
        QListWidgetItem* item = new QListWidgetItem( bm.getName() );
        list->addItem( item );
    }
    list->setMinimumWidth( 100 );
    desc = new QTextEdit();
    desc->setReadOnly( true );
    desc->setMinimumWidth( 300 );
    QDialogButtonBox* buttonBox = new QDialogButtonBox( QDialogButtonBox::Cancel | QDialogButtonBox::Ok );

    QVBoxLayout* main = new QVBoxLayout();
    setLayout( main );
    QWidget*     center = new QWidget();
    QGridLayout* grid   = new QGridLayout();
    center->setLayout( grid );
    grid->setColumnStretch( 0, 1 );
    grid->setColumnStretch( 1, 2 );
    grid->addWidget( new QLabel( tr( "Name:" ) ), 0, 0 );
    grid->addWidget( list, 1, 0 );
    grid->addWidget( new QLabel( tr( "Description:" ) ), 0, 1 );
    grid->addWidget( desc, 1, 1 );
    main->addWidget( center );
    main->addWidget( buttonBox );

    connect( list, SIGNAL( currentRowChanged( int ) ), this, SLOT( updateDescription( int ) ) );
    connect( buttonBox, SIGNAL( accepted() ), this, SLOT( accept() ) );
    connect( buttonBox, SIGNAL( rejected() ), this, SLOT( reject() ) );

    list->setCurrentRow( 0 );
}

void
LoadBookmarksDialog::updateDescription( int index )
{
    if ( index >= 0 )
    {
        desc->setText( bookmarks[ index ].getDescription() );
        selected = bookmarks[ index ].getName();
    }
}

QString
LoadBookmarksDialog::getName() const
{
    return ( result() == Accepted ) ? selected : "";
}

//=================================================================================================================

SaveBookmarksDialog::SaveBookmarksDialog( const QList<Bookmark>& bm, QWidget* parent ) : QDialog( parent ), bookmarks( bm )
{
    this->setWindowTitle( tr( "Save Bookmark" ) );

    QVBoxLayout* main = new QVBoxLayout();
    setLayout( main );
    main->addWidget( new QLabel( tr( "Name:" ) ) );
    name = new QComboBox();
    name->setMinimumWidth( 400 );
    name->setEditable( true );
    name->addItem( QString(), QString() );
    foreach( Bookmark bm, bookmarks )
    {
        name->addItem( bm.getName(), bm.getDescription() );
    }
    connect( name, SIGNAL( activated( int ) ), this, SLOT( bookmarkSelected( int ) ) );
    main->addWidget( name );
    main->addWidget( new QLabel( tr( "Description:" ) ) );
    desc = new QTextEdit();
    main->addWidget( desc );

    QDialogButtonBox* buttonBox = new QDialogButtonBox( QDialogButtonBox::Cancel | QDialogButtonBox::Ok );
    connect( buttonBox, SIGNAL( accepted() ), this, SLOT( accept() ) );
    connect( buttonBox, SIGNAL( rejected() ), this, SLOT( reject() ) );
    main->addWidget( buttonBox );
}

void
SaveBookmarksDialog::bookmarkSelected( int )
{
    desc->setText( name->currentText() );
}

QString
SaveBookmarksDialog::getName() const
{
    return ( result() == Accepted ) ? name->currentText() : "";
}

QString
SaveBookmarksDialog::getDescription() const
{
    return ( result() == Accepted ) ? desc->toPlainText() : "";
}

//=================================================================================================================

EditBookmarksDialog::EditBookmarksDialog( const QList<Bookmark>& bm, QWidget* parent ) : QDialog( parent ), bookmarks( bm )
{
    this->setWindowTitle( tr( "Edit Bookmarks" ) );

    list = new QListWidget();
    list->setMinimumWidth( 100 );

    //list->setDragEnabled( true ); change order of bookmarks, default is alphabetically
    //list->setDragDropMode( QAbstractItemView::InternalMove );

    foreach( Bookmark bm, bookmarks )
    {
        QListWidgetItem* item = new QListWidgetItem( bm.getName() );
        item->setFlags( item->flags() | Qt::ItemIsEditable );
        list->addItem( item );
    }
    connect( list, SIGNAL( itemChanged( QListWidgetItem* ) ), this, SLOT( renameEntry( QListWidgetItem* ) ) );

    desc = new QTextEdit();
    desc->setMinimumWidth( 300 );
    QDialogButtonBox* buttonBox = new QDialogButtonBox( QDialogButtonBox::Save | QDialogButtonBox::Cancel  );
    connect( desc, SIGNAL( textChanged() ), this, SLOT( descriptionChanged() ) );

    QPushButton* del = new QPushButton( tr( "Delete" ) );
    buttonBox->addButton( del, QDialogButtonBox::ActionRole );
    connect( del, SIGNAL( pressed() ), this, SLOT( deleteEntry() ) );

    QVBoxLayout* main = new QVBoxLayout();
    setLayout( main );
    QWidget*     center = new QWidget();
    QGridLayout* grid   = new QGridLayout();
    center->setLayout( grid );
    grid->setColumnStretch( 0, 1 );
    grid->setColumnStretch( 1, 2 );
    grid->addWidget( new QLabel( tr( "Name:" ) ), 0, 0 );
    grid->addWidget( list, 1, 0 );
    grid->addWidget( new QLabel( tr( "Description:" ) ), 0, 1 );
    grid->addWidget( desc, 1, 1 );
    main->addWidget( center );
    main->addWidget( buttonBox );

    connect( list, SIGNAL( currentRowChanged( int ) ), this, SLOT( updateDescription( int ) ) );
    connect( buttonBox, SIGNAL( accepted() ), this, SLOT( accept() ) );
    connect( buttonBox, SIGNAL( rejected() ), this, SLOT( reject() ) );

    list->setCurrentRow( 0 );
}

void
EditBookmarksDialog::deleteEntry()
{
    int idx = list->currentRow();
    list->takeItem( idx );
    bookmarks.removeAt( idx );
}

void
EditBookmarksDialog::descriptionChanged()
{
    int idx = list->currentRow();
    bookmarks[ idx ].setDescription( desc->toPlainText() );
}

QList<Bookmark> EditBookmarksDialog::getBookmarks() const
{
    return bookmarks;
}

void
EditBookmarksDialog::renameEntry( QListWidgetItem* item )
{
    int idx = list->currentRow();
    bookmarks[ idx ].rename( item->text() );
}

void
EditBookmarksDialog::updateDescription( int index )
{
    if ( index >= 0 )
    {
        desc->setText( bookmarks[ index ].getDescription() );
        selected = bookmarks[ index ].getName();
    }
}
