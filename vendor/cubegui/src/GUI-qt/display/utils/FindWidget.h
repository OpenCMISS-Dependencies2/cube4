/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2018                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QStandardItemModel>

#ifndef FINDWIDGET_H
#define FINDWIDGET_H

namespace cubegui
{
class TreeModelInterface;

enum FindAction { MARK, SELECT, EXPAND };

/**
 * creates dialog to allow the user to enter a search string
 */
class FindWidget : public QWidget
{
    Q_OBJECT
public:
    FindWidget( QWidget*            parent,
                TreeModelInterface* treeModelInterface,
                bool                enableSelection );
    QString
    getText() const;

    void
    setText( const QString& txt );

signals:
    void
    search( const QString& searchString,
            FindAction     action = MARK );
    void
    previous();
    void
    next();
    void
    close();

private slots:
    void
    autoComplete( const QString& text );
    void
    insertCompletion( const QString& text );
    void
    emitSearch();
    void
    selectItems();
    void
    expandItems();
    void
    upSelected();
    void
    downSelected();

private:
    TreeModelInterface* modelInterface;
    QLineEdit*          lineEdit;
    QPushButton*        findButton;
    QPushButton*        selectButton;
    QPushButton*        expandButton;
    QStandardItemModel  completerModel;
    QCompleter*         completer;
    FindAction          findAction;
    void
    enableButtons( bool enable );
};
}
#endif
