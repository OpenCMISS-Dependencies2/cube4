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


#ifndef SETTINGSTOOLBAR_H
#define SETTINGSTOOLBAR_H

#include <QToolBar>
#include <QSettings>
#include <QDialog>
#include <QSettings>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QListWidget>
#include "Settings.h"

namespace cubegui
{
/**
 * @brief The BookmarkToolBar class provides a tool bar to load or save user defined settings for cube files
 */
class BookmarkToolBar : public QToolBar
{
    Q_OBJECT
public:
    explicit
    BookmarkToolBar();

    /** sets/updates the list of bookmarks */
    void
    setBookmarks( QList<Bookmark> bookmarks );

signals:
    /** request to save the bookmark with the given description */
    void
    saveBookmark( const QString&,
                  const QString& );

    /** request to save all changes in the given list of bookmarks */
    void
    changeBookmarks( const QList<Bookmark>& );

    /** request to load the given bookmark */
    void
    loadBookmark( const QString& );

private slots:
    void
    loadSettings();
    void
    saveSettings();
    void
    editSettings();
    void
    setMenuGeometry();
    void
    loadBookmarkSlot();

private:
    QList<Bookmark> bookmarks;
    QMenu*          loadMenu;
    QAction*        loadAct;
};

//==========================================
class LoadBookmarksDialog : public QDialog
{
    Q_OBJECT
public:
    LoadBookmarksDialog( const QList<Bookmark>& bookmarks,
                         QWidget*               parent = 0 );
    QString
    getName() const;

private slots:
    void
    updateDescription( int index );

private:
    QListWidget*    list;
    QTextEdit*      desc;
    QString         selected;
    QList<Bookmark> bookmarks;
};

//==========================================
class SaveBookmarksDialog : public QDialog
{
    Q_OBJECT
public:
    SaveBookmarksDialog( const QList<Bookmark>& bm,
                         QWidget*               parent = 0 );
    QString
    getName() const;
    QString
    getDescription() const;

private slots:
    void
    bookmarkSelected( int );

private:
    QList<Bookmark> bookmarks;
    QComboBox*      name;
    QTextEdit*      desc;
};

//==========================================
class EditBookmarksDialog : public QDialog
{
    Q_OBJECT
public:
    EditBookmarksDialog( const QList<Bookmark>& bookmarks,
                         QWidget*               parent = 0 );
    QString
    getName() const;
    QString
    getDescription() const;

    QList<Bookmark>
    getBookmarks() const;

private slots:
    void
    updateDescription( int index );
    void
    deleteEntry();
    void
    renameEntry( QListWidgetItem* );
    void
    descriptionChanged();

private:
    QList<Bookmark> bookmarks;
    QString         selected;
    QListWidget*    list;
    QTextEdit*      desc;
};
}

#endif // SETTINGSTOOLBAR_H
