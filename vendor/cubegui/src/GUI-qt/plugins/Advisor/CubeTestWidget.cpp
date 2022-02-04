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


#include <config.h>
#include <QString>
#include "PluginServices.h"
#include "CubeTestWidget.h"

using namespace advisor;

extern cubepluginapi::PluginServices* advisor_services;


CubeTestWidget::CubeTestWidget( PerformanceTest* _t )
{
    calculated = false;
    test       = _t;
    if ( test->isNormalized() )
    {
        value = new Bar();
        value->setMinimum( 0 );
        value->setMaximum( 1 );
        value->setAttribute( Qt::WA_Hover );
        value->setMinimumWidth( 80 );
        value->setValue( 0 );
    }
    else
    {
        value = NULL;
    }


    name       = new QLabel( QString::fromStdString( test->name() ) );
    value_text = new QLabel( "" );
    value_text->setAttribute( Qt::WA_Hover );
    value_text->setWordWrap( true );

    value_as_number = new QLabel( "" );
    value_as_number->setAttribute( Qt::WA_Hover );
    value_as_number->setWordWrap( true );
    value_as_number->setAlignment( Qt::AlignRight );
    value_as_number->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );


    name->setEnabled( test->isActive() );
    if ( value != NULL )
    {
        value->setEnabled( test->isActive() );
    }
    value_text->setEnabled( test->isActive() );
    value_as_number->setEnabled( test->isActive() );

    if ( value != NULL )
    {
        value->installEventFilter( this );
    }
    value_text->installEventFilter( this );
    value_as_number->installEventFilter( this );
}


void
CubeTestWidget::updateCalculation()
{
    if ( calculated )
    {
        calculating();
    }
    else
    {
        apply();
    }
}


void
CubeTestWidget::calculating()
{
    if ( test->isActive() )
    {
        value_text->setEnabled( false );
        value_as_number->setEnabled( false );
        name->setEnabled( false );
        value_text->setText( tr( "calculating..." ) );
        value_as_number->setText( tr( "" ) );
        if ( value != NULL )
        {
            value->setEnabled( false );
            value->setValue( 0 );
        }
    }
}


void
CubeTestWidget::apply()
{
    if ( test->isActive() )
    {
        if ( value != NULL )
        {
            value->setEnabled( true );
        }
        value_text->setEnabled( true );
        value_as_number->setEnabled( true );
        name->setEnabled( true );
        double v     = test->value();     // ( test->isPercent() ) ? test->value() : ( test->value() / ( test->getMaximum() - test->getMinimum() ) );
        double min_v = test->min_value(); // ( test->isPercent() ) ? test->min_value() : test->value();
        double max_v = test->max_value(); //( test->isPercent() ) ? test->max_value() : test->value();



        if ( value != NULL )
        {
            if ( test->isRegular() )
            {
                QColor vc_reg( 255 * ( 1 - v ), ( v ) * 255, 0, 255 );
                value_text->setStyleSheet( QString( "QLabel {color: %1; }" ).arg( vc_reg.name() ) );
                value->setColor( vc_reg );
                if ( test->isSingleValue() )
                {
                    value->setValue( v );
                }
                else
                {
                    value->setValues( min_v, v, max_v );
                }
            }
            else
            {
                QColor vc_irreg( 255 * ( v ), ( 1 - v ) * 255, 0, 255 );
                value_text->setStyleSheet( QString( "QLabel {color: %1; }" ).arg( vc_irreg.name() ) );
                value->setColor( vc_irreg );
                if ( test->isSingleValue() )
                {
                    value->setValue( 1 - v );
                }
                else
                {
                    value->setValues( min_v, 1 - v, max_v );
                }
            }
        }
        QPair<QString, QString> value_unit = advisor_services->formatNumberAndUnit(
            ( test->isPercent() ) ?
            ( double )( ( int )( v * 100 + .5 ) ) :
            v,
            test->units(),
            cubegui::FORMAT_TREES,
            false );
        value_as_number->setText( value_unit.first + " " +
                                  ( ( test->isPercent() ) ?
                                    "%" :
                                    value_unit.second ) );


        value_text->setText( getValueText( v ) );
        value_text->setWordWrap( true );
        comment = QString::fromStdString( test->comment() );
    }
    else
    {
        if ( value != NULL )
        {
            value->setValue( 0 );
        }
    }
}

QString
CubeTestWidget::getValueText( double v )
{
    if ( !test->isNormalized() )
    {
        return tr( "Value" );
    }
    int range = ( test->isRegular() ) ? v * 5. : ( 1 - v ) * 5;
    switch ( range )
    {
        case 0:
            return tr( "Very poor" );
        case 1:
            return tr( "Poor" );
        case 2:
            return tr( "Fair" );
        case 3:
            return tr( "Good" );
        case 4:
            return tr( "Very good" );
        case 5:
            return tr( "Excellent" );
    }
    return tr( "" );
}
