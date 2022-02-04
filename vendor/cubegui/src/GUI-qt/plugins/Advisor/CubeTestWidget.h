/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2015-2020                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef TEST_WIDGET_H
#define TEST_WIDGET_H
#include <QtGui>
#include <QLabel>
#include <QWidget>
#include "Bar.h"
#include "PerformanceTest.h"



namespace advisor
{
class CubeTestWidget : public QObject
{
    Q_OBJECT

private:

    PerformanceTest * test;
    Bar*     value;
    QLabel*  name;
    QLabel*  value_text;
    QLabel*  value_as_number;
    QString  comment;
    QString* howToMeasure;
    bool     calculated;

    QString
    getValueText( double );




private slots:


signals:
    void
    showComment( QString& );

    void
    hideComment();

public:
    CubeTestWidget( PerformanceTest* _t );

    virtual
    ~CubeTestWidget()
    {
    }

    void
    apply();

    void
    calculating();

    void
    setCalculating( const bool v )
    {
        calculated = v;
        updateCalculation();
    }

    void
    updateCalculation();

    QLabel*
    getName()
    {
        return name;
    }



    QLabel*
    getValueText()
    {
        return value_text;
    }

    QLabel*
    getValue()
    {
        return value_as_number;
    }


    Bar*
    getProgressBar()
    {
        return value;
    }

    inline
    bool
    isActive()
    {
        return test->isActive();
    }

    inline
    PerformanceTest*
    getPerformanceTest() const
    {
        return test;
    }

    inline
    void
    disable()
    {
        QColor vc_reg( 0, 0, 0, 255 );
        value_text->setStyleSheet( QString( "QLabel {color: %1; }" ).arg( vc_reg.name() ) );
        if ( value != nullptr )
        {
            value->setColor( vc_reg );
        }
        name->setEnabled( false );
        value_as_number->setEnabled( false );
        value_as_number->setText( tr( "" ) );
        if ( test->isActive() )
        {
            value_text->setText( tr( "calculating..." ) );
        }
        else
        {
            value_text->setText( tr( "" ) );
        }
        value_text->setEnabled( false );
        if ( value != nullptr )
        {
            value->setEnabled( false );
            value->setValue( 0 );
        }
    }

    inline
    void
    enable()
    {
        name->setEnabled( true );
        value_as_number->setEnabled( true );
        value_text->setEnabled( true );
        if ( value != nullptr )
        {
            value->setEnabled( true );
        }
    }
};
};

#endif // ADVISER_RATING_WIDGET_H
