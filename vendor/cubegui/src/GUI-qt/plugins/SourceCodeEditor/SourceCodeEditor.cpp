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

#include <QtGui>
#include <QScrollBar>
#include <QMenu>
#include "SourceCodeEditor.h"

using namespace editor_plugin;

SourceCodeEditor::SourceCodeEditor( QWidget* parent ) : QPlainTextEdit( parent )
{
    padding = 3;
    setLineWrapMode( QPlainTextEdit::NoWrap );
    setCenterOnScroll( true );
    lineNumberWidget = new LineNumberWidget( this );
    connect( this, SIGNAL( updateRequest( QRect, int ) ), this, SLOT( updateLineNumbers( QRect ) ) );
}

/** repaints the line numbers after the source has been scrolled */
void
SourceCodeEditor::updateLineNumbers( const QRect& rect )
{
    lineNumberWidget->update( 0, rect.y(), lineNumberWidget->width(), rect.height() );
}

/** sets the contents of the editor */
void
SourceCodeEditor::setText( const QString& txt )
{
    clear();
    setPlainText( txt );
    setViewportMargins( lineNumberWidth(), 0, 0, 0 ); // set free space for line numbers
}

/** calculate the width of the line number widget */
int
SourceCodeEditor::lineNumberWidth()
{
    int digits = 1;
    int max    = std::max( 1, blockCount() );
    while ( max >= 10 )
    {
        max /= 10;
        digits++;
    }
    int space = fontMetrics().boundingRect( QChar( '9' ) ).width() * digits + 2 * padding;

    return space;
}

/** set size and position of the line number widget */
void
SourceCodeEditor::resizeEvent( QResizeEvent* e )
{
    QPlainTextEdit::resizeEvent( e );

    QRect rect = contentsRect();
    lineNumberWidget->setGeometry( QRect( rect.left(), rect.top(), lineNumberWidth(), rect.height() ) );
}


/** marks the given region and scrolls to the first line of the region
 */
void
SourceCodeEditor::markRegion( int startLine, int endLine )
{
    // mark the start line of the item's code by a dark green background
    QList<QTextEdit::ExtraSelection> extras;

    // mark whole block
    if ( endLine != -1 )
    {
        if ( startLine == -1 ) // handle special OMPT case where only last line knonw.
        {
            startLine = endLine;
        }
        QTextEdit::ExtraSelection highlight2;
        highlight2.format.setBackground( MarkBlockColor );
        highlight2.cursor = this->textCursor();
        highlight2.cursor.movePosition( QTextCursor::Start );
        for ( int i = 0; i < startLine; i++ ) // move cursor to start line
        {
            highlight2.cursor.movePosition( QTextCursor::Down );
        }
        for ( int i = 1; i < endLine - startLine; i++ ) // move cursor to end line, but keep anchor to select whole area
        {
            highlight2.cursor.movePosition( QTextCursor::Down, QTextCursor::KeepAnchor );
        }
        highlight2.format.setProperty( QTextFormat::FullWidthSelection, true );
        highlight2.cursor.movePosition( QTextCursor::Down, QTextCursor::KeepAnchor );
        extras << highlight2;
    }

    // mark first line of the block
    if ( startLine > 0 )
    {
        QTextEdit::ExtraSelection highlight;
        highlight.cursor = this->textCursor();
        highlight.cursor.movePosition( QTextCursor::Start );
        for ( int i = 1; i < startLine; i++ )
        {
            highlight.cursor.movePosition( QTextCursor::Down );
        }
        highlight.cursor.movePosition( QTextCursor::StartOfLine );
        this->setTextCursor( highlight.cursor );
        highlight.cursor.movePosition( QTextCursor::Down, QTextCursor::KeepAnchor );
        highlight.format.setBackground( QColor( 0, 200, 0, 100 ) );

        extras << highlight;

        // scroll to selection
        this->verticalScrollBar()->setValue( this->verticalScrollBar()->maximum() );
        this->ensureCursorVisible();
    }
    setExtraSelections( extras );
}


/**
 * @brief SourceCodeEditor::paintLineNumber draws the line numbers into lineNumberWidget
 * @param event
 */
void
SourceCodeEditor::paintLineNumber( QPaintEvent* event )
{
    QColor lineNumberBackground = QColor( 240, 240, 240 );

    QPainter painter( lineNumberWidget );
    painter.fillRect( event->rect(), lineNumberBackground );

    QTextBlock block       = firstVisibleBlock();
    int        blockNumber = block.blockNumber();
    int        top         = ( int )blockBoundingGeometry( block ).translated( contentOffset() ).top();
    int        height      = ( int )blockBoundingRect( block ).height();

    int maxLines = blockCount() - 1;

    while ( top <= event->rect().bottom() && blockNumber < maxLines )
    {
        painter.setPen( Qt::black );
        painter.drawText( 0, top, lineNumberWidget->width() - padding, fontMetrics().height(),
                          Qt::AlignRight, QString::number( blockNumber + 1 ) );
        blockNumber++;
        top += height;
    }
}

void
SourceCodeEditor::addToContextMenu( QAction* action )
{
    contextMenuActions.append( action );
}

void
SourceCodeEditor::contextMenuEvent( QContextMenuEvent* event )
{
    QMenu* menu = createStandardContextMenu();
    foreach( QAction * action, contextMenuActions )
    {
        menu->addAction( action );
    }
    menu->exec( event->globalPos() );
    delete menu;
}
