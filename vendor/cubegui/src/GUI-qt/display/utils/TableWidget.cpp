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

#include "TableWidget.h"
#include <QPainter>
#include <QApplication>
#include <QDialog>
#include <QClipboard>
#include <QPushButton>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QScrollBar>
#include <QLabel>
#if QT_VERSION >= QT_VERSION_CHECK( 5, 0, 0 )
#include <QRegularExpression>
#else
#include <QRegExp>
#endif

using namespace cubegui;

/**
 * creates a widget which consists of several columns with text
 * @param columns list with the elements of each column, separated by '\n'
 */
TableWidget::TableWidget( const QStringList& columns,
                          bool               isTooltip = false )
{
    this->columns   = columns;
    this->isTooltip = isTooltip;
    first           = true;
    isTooltip       = false;

    for ( int i = 0; i < columns.size(); i++ )
    {
#if QT_VERSION >= QT_VERSION_CHECK( 5, 0, 0 )
        this->columns[ i ].remove( QRegularExpression( "\\s$" ) ); // remove whitespace/newline at end of table
#else
        this->columns[ i ].remove( QRegExp( "\\s$" ) );            // remove whitespace/newline at end of table
#endif
    }

    int rowheight = 14;
    size = QSize( 10, columns[ 0 ].split( "\n" ).size() * rowheight ); // estimated initial size: rows * rowheight
    resize( size );
    update();
    setMinimumSize( width(), height() );
}

void
TableWidget::paintEvent( QPaintEvent* )
{
    if ( columns.size() < 1 )
    {
        return;
    }

    QPainter painter( this );
    if ( isTooltip )
    {
        painter.fillRect( rect(), Qt::white );
        painter.drawRect( rect().x(), rect().y(), rect().width() - 1, rect().height() - 1 );
    }

    QList<QRect > rect;
    for ( int i = 0; i < columns.size(); i++ )
    {
        rect.append( painter.boundingRect( 0, 0, 0, 0, Qt::AlignLeft, columns.at( i ) ) );
    }

    int pad    = 10;
    int middle = 20;
    if ( first )
    {
        int width  = 0;
        int height = 0;
        for ( int i = 0; i < columns.size(); i++ )
        {
            width += rect[ i ].width();
            if ( rect[ i ].height() > height )
            {
                height = rect[ i ].height();
            }
        }
        width += ( columns.size() - 1 ) * middle;

        if ( width < 50 )
        {
            width = 50;
        }
        if ( height < 50 )
        {
            height = 50;
        }
        size = QSize( width + 2 * pad, height + 2 * pad );
        resize( size );
        setMinimumSize( size );
        setMaximumSize( size );
        first = false;
    }

    int posX = pad;
    painter.drawText( posX, pad, rect[ 0 ].width(), rect[ 0 ].height(),
                      Qt::AlignLeft, columns[ 0 ] );
    for ( int i = 1; i < columns.size(); i++ )
    {
        posX += rect[ i - 1 ].width() + middle;
        rect[ i ].moveTo( pad, pad );
        painter.drawText( posX, pad, rect[ i ].width(), rect[ i ].height(),
                          Qt::AlignRight, columns[ i ] );
    }
}


void
TableWidget::toClipboard()
{
    QList<QStringList > contents;
    int                 max = 0;

    for ( int i = 0; i < columns.size(); i++ )
    {
        contents.append( columns[ i ].split( "\n" ) );
        if ( max < contents[ i ].size() )
        {
            max = contents[ i ].size();
        }
    }

    QString txt;
    for ( int i = 0; i < max; i++ )
    {
        if ( i < contents[ 0 ].size() )
        {
            txt += contents[ 0 ][ i ];
        }
        for ( int j = 1; j < contents.size(); j++ )
        {
            if ( i < contents[ j ].size() )
            {
                txt += "\t\t" + contents[ j ][ i ];
            }
        }
        txt += "\n";
    }
    QApplication::clipboard()->setText( txt );
}

QDialog*
TableWidget::showInToolTip( QWidget* parent, const QPoint& pos, const QStringList& cols )
{
    QDialog* dialog = new QDialog( parent );
    dialog->setWindowFlags( Qt::ToolTip ); // create seperate window

    TableWidget* textTable = new TableWidget( cols, true );

    QVBoxLayout* layout = new QVBoxLayout();
    layout->setContentsMargins( 0, 0, 0, 0 );
    layout->addWidget( textTable );
    dialog->setLayout( layout );

    dialog->move( pos );
    dialog->show();
    return dialog;
}


class MyScrollArea : public QScrollArea
{
private:
    QSize size;
public:
    virtual QSize
    sizeHint() const
    {
        QSize size = widget()->size();
        size += QSize( verticalScrollBar()->width(), horizontalScrollBar()->height() );
        size.setHeight( std::min( size.height(), 300 ) ); // maximal initial size: 300x600
        size.setWidth( std::min( size.width(), 600 ) );

        return size;
    }
};

/**
 * @brief TableWidget::showInWindow
 * @param parent parent window
 * @param title title of the window
 * @param label label above the table
 * @param cols Each element of the StringList represents one column. Each column is divided into rows by "\n"
 */
void
TableWidget::showInWindow( QWidget* parent, const QString& title, const QString& label,
                           const QStringList& cols )
{
    QDialog* dialog = new QDialog( parent );
    dialog->setAttribute( Qt::WA_DeleteOnClose );
    dialog->setWindowTitle( title );

    QVBoxLayout* layout = new QVBoxLayout();
    if ( label.length() > 0 )
    {
        QLabel*      labelWidget = new QLabel( label );
        QScrollArea* scroll      = new MyScrollArea();
        scroll->setFocusPolicy( Qt::NoFocus );
        scroll->setWidget( labelWidget );
        scroll->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Expanding );
        layout->addWidget( scroll );
    }

    TableWidget* textTable = new TableWidget( cols );
    layout->addWidget( textTable );

    QWidget*     buttons = new QWidget( dialog );
    QHBoxLayout* bLayout = new QHBoxLayout();
    buttons->setLayout( bLayout );

    bLayout->addItem( new QSpacerItem( 10, 10, QSizePolicy::Expanding, QSizePolicy::Minimum ) );
    QPushButton* clip = new QPushButton( tr( "To Clipboard" ), dialog );
    connect( clip, SIGNAL( pressed() ), textTable, SLOT( toClipboard() ) );
    bLayout->addWidget( clip );
    QPushButton* okButton = new QPushButton( tr( "Close" ), dialog );
    connect( okButton, SIGNAL( pressed() ), dialog, SLOT( accept() ) );
    bLayout->addWidget( okButton );
    layout->addWidget( buttons );

    dialog->setLayout( layout );

    dialog->setModal( false );
    dialog->show();
}
