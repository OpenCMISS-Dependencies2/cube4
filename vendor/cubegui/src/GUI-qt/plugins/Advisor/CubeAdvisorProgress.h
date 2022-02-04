/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2019                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/



#ifndef CUBE_TOOLS_PROGRESS_H
#define CUBE_TOOLS_PROGRESS_H


#include <QApplication>
#include <QProgressBar>
#include <QHBoxLayout>
#include <QLabel>
#include <QWidget>

#define INIT_BUSY_WAIT_PROGRESS \
    advisor_progress_widget = new ThreadedProgressBar( NULL ); \


#define DELETE_BUSY_WAIT_PROGRESS

#define START_BUSY_WAIT \
    qApp->setOverrideCursor( QCursor( Qt::BusyCursor ) ); \
    analyses->setEnabled( false ); \
    advisor_progress_widget->show(); \
    QApplication::processEvents(); \



#define END_BUSY_WAIT \
    analyses->setEnabled( true ); \
    qApp->restoreOverrideCursor();  \
    _widget->setCursor( Qt::ArrowCursor ); \
    advisor_progress_widget->hide(); \

#define PROGRESS_BUSY_WAIT( s, x ) \
    advisor_progress_widget->setValue( s, x ); \
    QApplication::processEvents();


namespace advisor
{
class ThreadedProgressBar : public QWidget
{
private:
    int           progress;
    QProgressBar* advisor_progress_bar;
    QLabel*       _status_label;
public:
    ThreadedProgressBar( QWidget* parent )
        : QWidget( parent )
    {
        progress = 0;
        setWindowFlags( Qt::CustomizeWindowHint );
        QHBoxLayout* _status_l = new QHBoxLayout(); \
        setLayout( _status_l ); \
        _status_label        = new QLabel( QObject::tr( "Calculate : " ) ); \
        advisor_progress_bar = new QProgressBar(); \
        _status_l->addWidget( _status_label ); \
        _status_l->addWidget( advisor_progress_bar ); \
        advisor_progress_bar->setMinimum( 0 ); \
        advisor_progress_bar->setMaximum( 100 ); \
        advisor_progress_bar->setValue( progress ); \
        hide();
    };

    inline
    void
    setValue( QString s, int v )
    {
        progress = v;
        _status_label->setText( QObject::tr( "Calculate %1: " ).arg( s ) );
        advisor_progress_bar->setValue( progress );
    }

    virtual ~ThreadedProgressBar()
    {
    };
};

extern ThreadedProgressBar* advisor_progress_widget;
};

#endif
