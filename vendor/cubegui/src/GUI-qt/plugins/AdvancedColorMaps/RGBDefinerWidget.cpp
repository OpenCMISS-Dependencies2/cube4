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



#include "config.h"

#include <QIntValidator>
#include "RGBDefinerWidget.h"

const QString RGBDefinerWidget::rgbLabels[ 3 ] = {
    tr( "Red:" ), tr( "Green:" ),
    tr( "Blue:" )
};

RGBDefinerWidget::RGBDefinerWidget( const QString& title ) :
    name( title ), pickColors( tr( "Color picker" ) )
{
    layout.addWidget( &name );

    for ( int i = 0; i < 3; ++i )
    {
        rgbValidators[ i ] = new QIntValidator( 0, 255, NULL );
        rgb[ i ].setValidator( rgbValidators[ i ] );
        QObject::connect( &rgb[ i ], SIGNAL( editingFinished() ),
                          this, SLOT( editedColor() ) );
        rgb[ i ].setFixedWidth( 50 );
        rgbLayout.addRow( rgbLabels[ i ], &rgb[ i ] );
    }
    layout.addLayout( &rgbLayout );

    QObject::connect( &pickColors, SIGNAL( clicked() ), this, SLOT( buttonClicked() ) );
    layout.addWidget( &pickColors );

    QObject::connect( &colorPicker, SIGNAL( colorSelected( const QColor & ) ),
                      this, SLOT( selectedColor( const QColor & ) ) );
    colorPicker.setVisible( false );

    this->setLayout( &layout );
}

RGBDefinerWidget::~RGBDefinerWidget()
{
    for ( int i = 0; i < 3; ++i )
    {
        delete rgbValidators[ i ];
    }
}

void
RGBDefinerWidget::setEnabled( bool flag )
{
    pickColors.setEnabled( flag );
    for ( int i = 0; i < 3; ++i )
    {
        rgb[ i ].setEnabled( flag );
    }
}

void
RGBDefinerWidget::buttonClicked()
{
    colorPicker.setVisible( true );
}

void
RGBDefinerWidget::selectedColor( const QColor& color )
{
    /**
     * write new color from picker to edits
     */
    setColor( color );
    emit colorChanged( color );
}

void
RGBDefinerWidget::editedColor()
{
    /**
     * emit new color from edits
     */
    emit colorChanged( getColor() );
}

QColor
RGBDefinerWidget::getColor()
{
    int rgb_data[ 3 ];
    for ( int i = 0; i < 3; ++i )
    {
        rgb_data[ i ] = rgb[ i ].text().toInt();
    }
    return QColor( rgb_data[ 0 ], rgb_data[ 1 ], rgb_data[ 2 ] );
}

void
RGBDefinerWidget::setColor( const QColor& color )
{
    int rgb_data[ 3 ];
    color.getRgb( &rgb_data[ 0 ], &rgb_data[ 1 ], &rgb_data[ 2 ] );
    for ( int i = 0; i < 3; ++i )
    {
        rgb[ i ].setText( QString::number( rgb_data[ i ] ) );
    }
}
