/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2017                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef SourceCodeEditor_H
#define SourceCodeEditor_H

#include <QPlainTextEdit>

class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;
class LineNumberWidget;

namespace editor_plugin
{
static const QColor MarkBlockColor( 240, 240, 240 );

class SourceCodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    SourceCodeEditor( QWidget* parent = 0 );

    void
    paintLineNumber( QPaintEvent* event );
    int
    lineNumberWidth();
    void
    markRegion( int startLine,
                int endLine );
    void
    setText( const QString& txt );

    void
    addToContextMenu( QAction* action );

protected:
    void
    resizeEvent( QResizeEvent* event );

    void
    contextMenuEvent( QContextMenuEvent* event );

private slots:
    void
    updateLineNumbers( const QRect& );

private:
    int             padding; // padding (left and right) of the line number widget
    QWidget*        lineNumberWidget;
    QList<QAction*> contextMenuActions;
};


class LineNumberWidget : public QWidget
{
public:
    LineNumberWidget( SourceCodeEditor* editor ) : QWidget( editor )
    {
        this->editor = editor;
    }

    QSize
    sizeHint() const
    {
        return QSize( editor->lineNumberWidth(), 0 );
    }

protected:
    void
    paintEvent( QPaintEvent* event )
    {
        editor->paintLineNumber( event );
    }

private:
    SourceCodeEditor* editor;
};
}

#endif
