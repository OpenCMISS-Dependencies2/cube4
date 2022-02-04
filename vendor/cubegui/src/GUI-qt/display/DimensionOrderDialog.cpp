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

#include "DimensionOrderDialog.h"

using namespace cubegui;

DimensionOrderDialog::DimensionOrderDialog( QWidget* parent, const QList<DisplayType>& initOrder ) :
    QDialog( parent )
{
    setWindowTitle( tr( "Dimension order" ) );

    QStringList labels = QStringList() << tr( "Metric" ) << tr( "Call" ) << tr( "System" );
    orders.append( QList<DisplayType>() << METRIC << CALL << SYSTEM );
    orders.append( QList<DisplayType>() << METRIC << SYSTEM << CALL );
    orders.append( QList<DisplayType>() << CALL << METRIC << SYSTEM );
    orders.append( QList<DisplayType>() << CALL << SYSTEM << METRIC );
    orders.append( QList<DisplayType>() << SYSTEM << METRIC << CALL );
    orders.append( QList<DisplayType>() << SYSTEM << CALL << METRIC );

    //add ok and cancel buttons to the dialog
    QDialogButtonBox* buttonBox = new QDialogButtonBox( this );
    buttonBox->addButton( QDialogButtonBox::Ok );
    buttonBox->addButton( QDialogButtonBox::Cancel );
    connect( buttonBox, SIGNAL( accepted() ), this, SLOT( accept() ) );
    connect( buttonBox, SIGNAL( rejected() ), this, SLOT( reject() ) );

    QVBoxLayout* vbox = new QVBoxLayout;
    //define the buttons for the possible orders
    groupBox = new QGroupBox( tr( "Possible dimension orders" ), this );
    foreach( QList<DisplayType> order, orders )
    {
        QString       label = labels[ order[ 0 ] ] + "\t" + labels[ order[ 1 ] ] + "\t" + labels[ order[ 2 ] ];
        QRadioButton* but   = new QRadioButton( label );
        vbox->addWidget( but );
        buttons.append( but );
        if ( order == initOrder )
        {
            but->setChecked( true );
        }
    }
    vbox->addStretch( 1 );
    groupBox->setLayout( vbox );

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget( groupBox );
    layout->addWidget( buttonBox );
    setLayout( layout );
}

const QList<DisplayType>&
DimensionOrderDialog::getOrder()
{
    for ( int i = 0; i < buttons.size(); i++ )
    {
        QRadioButton* but = buttons.at( i );
        if ( but->isChecked() )
        {
            return orders.at( i );
        }
    }
    return orders[ 0 ];
}
