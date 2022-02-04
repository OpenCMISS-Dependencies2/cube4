/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2016                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/



#ifndef TEXTTABLE_H
#define TEXTTABLE_H

#include <QStringList>
#include <QWidget>

namespace cubegui
{
class TableWidget : public QWidget
{
    Q_OBJECT
public:
    TableWidget( const QStringList& columns,
                 bool               isTooltip );
    void
    paintEvent( QPaintEvent* );

    virtual QSize
    sizeHint() const
    {
        return size;
    }

    static QDialog*
    showInToolTip( QWidget*           parent,
                   const QPoint&      pos,
                   const QStringList& cols );

    static void
    showInWindow( QWidget*           parent,
                  const QString&     title,
                  const QString&     label,
                  const QStringList& cols );

private slots:
    void
    toClipboard();

private:
    QWidget*    parent;
    QStringList columns;
    bool        first;
    bool        isTooltip;
    QSize       size;
};
}
#endif // TEXTTABLE_H
