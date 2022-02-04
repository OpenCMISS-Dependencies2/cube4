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

#include <QDebug>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QComboBox>

#include "ValueViewConfig.h"
#include "ValueView.h"
#include "CubeRegion.h"
#include "PluginManager.h"
#include "Tree.h"
#include "TreeView.h"

using namespace cube;
using namespace cubegui;
using namespace cubepluginapi;

ValueViewConfig::ValueViewConfig( const QList<cube::DataType> usedTypes, cube::DataType currentType, QWidget* parent )
    : QDialog( parent ), typeList( usedTypes ), previousValueViews( Globals::valueViews )
{
    setWindowTitle( tr( "Configure tree item values" ) );

    //add ok and cancel buttons to the dialog
    QDialogButtonBox* buttonBox = new QDialogButtonBox( this );
    buttonBox->addButton( QDialogButtonBox::Ok );
    QPushButton* applyButton = buttonBox->addButton( QDialogButtonBox::Apply );
    buttonBox->addButton( QDialogButtonBox::Cancel );
    connect( buttonBox, SIGNAL( accepted() ), this, SLOT( accept() ) );
    connect( buttonBox, SIGNAL( rejected() ), this, SLOT( reject() ) );
    connect( applyButton, SIGNAL( clicked() ), this, SLOT( apply() ) );
    QVBoxLayout* vbox     = new QVBoxLayout;
    QGroupBox*   groupBox = new QGroupBox( tr( "Select Data Type" ), this );
    vbox->addStretch( 1 );
    groupBox->setLayout( vbox );

    configWidget = new QWidget();
    configWidget->setLayout( new QVBoxLayout() );

    typeCombo = new QComboBox();
    foreach( DataType type, usedTypes )
    {
        QString label = QString::fromStdString( getDataTypeAsString( type ) );
        typeCombo->addItem( label );
    }
    viewCombo = new QComboBox();
    vbox->addWidget( typeCombo );
    vbox->addWidget( viewCombo );

    typeCombo->setCurrentIndex( -1 );
    connect( typeCombo, SIGNAL( currentIndexChanged( int ) ), this, SLOT( typeChanged( int ) ) );
    connect( viewCombo, SIGNAL( currentIndexChanged( int ) ), this, SLOT( viewChanged( int ) ) );
    typeCombo->setCurrentIndex( usedTypes.indexOf( currentType ) );

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget( groupBox );
    layout->addWidget( configWidget );
    layout->addSpacerItem( new QSpacerItem( 1, 1, QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );
    layout->addWidget( buttonBox );
    setLayout( layout );

    TreeView* view = Globals::getTabManager()->getActiveView( METRIC );
    connect( view, SIGNAL( selectionChanged( Tree* ) ), this, SLOT( metricChanged( Tree* ) ) );

    this->setVisible( true );
}

void
ValueViewConfig::metricChanged( Tree* tree )
{
    DataType type = tree->getLastSelection()->getValueObject()->myDataType();
    typeCombo->setCurrentIndex( typeList.indexOf( type ) );
}

void
ValueViewConfig::typeChanged( int index )
{
    int      viewIndex = 0;
    DataType type      = typeList[ index ];
    viewList = PluginManager::getInstance()->getValueViews();
    ValueView* current = Globals::getValueView( type );

    viewCombo->blockSignals( true );
    viewCombo->clear();
    foreach( ValueView * view, viewList )
    {
        if ( view->isAvailable( type ) )
        {
            viewCombo->addItem( view->getName() );
            if ( view == current )
            {
                viewIndex = viewList.indexOf( view );
            }
        }
    }
    viewCombo->blockSignals( false );
    viewCombo->setCurrentIndex( viewIndex );
    viewChanged( viewIndex );
}

void
ValueViewConfig::viewChanged( int index )
{
    ValueView* view = viewList.at( index );
    DataType   type = typeList[ typeCombo->currentIndex() ];

    QLayoutItem* lItem = configWidget->layout()->takeAt( 0 );
    if ( lItem )
    {
        delete lItem->widget();
        delete lItem;
    }

    QWidget* config = view->getConfigWidget( type );
    if ( config )
    {
        configWidget->layout()->addWidget( config );
    }
}

void
ValueViewConfig::close()
{
    this->setVisible( false );
    this->deleteLater();
}

void
ValueViewConfig::accept()
{
    apply();
    setVisible( false ); // reject gets called, if setVisible(false) isn't called before close
    close();
}

void
ValueViewConfig::apply()
{
    ValueView* view = viewList.at( viewCombo->currentIndex() );
    view->applyConfig();
    DataType type = typeList[ typeCombo->currentIndex() ];
    Globals::setValueView( type, view ); // todo add update method
}

void
ValueViewConfig::reject() // todo restore view setting
{
    ValueView* view = viewList.at( viewCombo->currentIndex() );
    view->cancelConfig();
    DataType type = typeList[ typeCombo->currentIndex() ];
    Globals::valueViews = previousValueViews;
    Globals::setValueView( type, Globals::valueViews.value( type ) ); // update display
    close();
}
