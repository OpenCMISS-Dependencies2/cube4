/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2020                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2009-2015                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include "config.h"

#include "HeatmapSettingsWidget.h"

#include "GraphWidgetGlobals.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QSpacerItem>
#include <QDebug>
#include <QFormLayout>
#include <limits>

HeatmapCustomizationzation::HeatmapCustomizationzation( bool     isHCount,
                                                        int      majorHNotches,
                                                        int      minorHNotchesCount,
                                                        bool     isVCount,
                                                        int      majorVNotches,
                                                        int      minorVNotchesCount,
                                                        QWidget* parent ) :
    QDialog( parent )
{
    // setInitialValues (will be used for reset)
    initHMinorCount = minorHNotchesCount;
    if ( isHCount == false )
    {
        initHMajorInterval = majorHNotches;
        initHMajorCount    = -1;
    }
    else
    {
        initHMajorInterval = -1;
        initHMajorCount    = majorHNotches;
    }

    initVMinorCount = minorVNotchesCount;
    if ( isVCount == false )
    {
        initVMajorInterval = majorVNotches;
        initVMajorCount    = -1;
    }
    else
    {
        initVMajorInterval = -1;
        initVMajorCount    = majorVNotches;
    }

    setModal( true );
    setWindowTitle( tr( "Heat Map Customization" ) );

    //  oldMinColorIndex = -1;
    // oldMaxColorIndex = -1;

    initComponents();
    setConnections();

    if ( isHCount == true )
    {
        oldHMajorCount    = majorHNotches;
        oldHMajorInterval = -1; // i.e. not set
        radio_h_major_2_count->setChecked( true );
        spin_h_major_2->setEnabled( true );
        spin_h_major_1->setEnabled( false );
        spin_h_major_2->setValue( majorHNotches );
    }
    else
    {
        oldHMajorInterval = majorHNotches;
        oldHMajorCount    = -1; // i.e. not set
        radio_h_major_1_interval->setChecked( true );
        spin_h_major_2->setEnabled( false );
        spin_h_major_1->setEnabled( true );
        spin_h_major_1->setValue( majorHNotches );
    }
    oldHMinorCount = minorHNotchesCount;
    lbl_h_major_desc_1_0->setEnabled( false );
    lbl_h_major_desc_1_1->setEnabled( false );
    lbl_h_major_desc_2->setEnabled( true );
    spin_h_minor->setValue( minorHNotchesCount );

    if ( isVCount == true )
    {
        oldVMajorCount    = majorVNotches;
        oldVMajorInterval = -1; // i.e. not set
        radio_v_major_2_count->setChecked( true );
        spin_v_major_2->setEnabled( true );
        spin_v_major_1->setEnabled( false );
        spin_v_major_2->setValue( majorVNotches );
    }
    else
    {
        oldVMajorInterval = majorVNotches;
        oldVMajorCount    = -1; // i.e. not set
        radio_v_major_1_interval->setChecked( true );
        spin_v_major_2->setEnabled( false );
        spin_v_major_1->setEnabled( true );
    }
    oldVMinorCount = minorVNotchesCount;
    lbl_v_major_desc_1_0->setEnabled( false );
    lbl_v_major_desc_1_1->setEnabled( false );
    lbl_v_major_desc_2->setEnabled( true );
    spin_v_minor->setValue( minorVNotchesCount );
}

void
HeatmapCustomizationzation::initComponents()
{
    lbl_h_major = new QLabel( tr( "<b>Horizontal Major Ticks</b>" ) );
    lbl_h_minor = new QLabel( tr( "<b>Horizontal Minor Ticks</b>" ) );

    lbl_h_major_desc_1_0 = new QLabel( tr( "Draw a major tick every: " ) );
    lbl_h_major_desc_1_0->setIndent( 20 );
    lbl_h_major_desc_1_1 = new QLabel( tr( "iterations." ) );

    lbl_h_major_desc_2 = new QLabel( tr( "Number of major ticks: " ) );
    lbl_h_major_desc_2->setIndent( 20 );

    lbl_h_minor_desc = new QLabel( tr( "Number of minor ticks: " ) );
    lbl_h_minor_desc->setIndent( 20 );

    radio_h_major_1_interval = new QRadioButton( tr( "Set major ticks by intervals" ) );
    radio_h_major_2_count    = new QRadioButton( tr( "Set a specific  number of major ticks:" ) );

    spin_h_major_1 = new QSpinBox();
    spin_h_major_1->setSingleStep( 10 );
    spin_h_major_2 = new QSpinBox();
    spin_h_minor   = new QSpinBox();

    h_h_major_1 = new QHBoxLayout();
    h_h_major_1->addWidget( lbl_h_major_desc_1_0 );
    h_h_major_1->addWidget( spin_h_major_1 );
    h_h_major_1->addWidget( lbl_h_major_desc_1_1 );
    h_h_major_1->addStretch();

    h_h_major_2 = new QHBoxLayout();
    h_h_major_2->addWidget( lbl_h_major_desc_2 );
    h_h_major_2->addWidget( spin_h_major_2 );
    h_h_major_2->addStretch();

    h_h_minor = new QHBoxLayout();
    h_h_minor->addWidget( lbl_h_minor_desc );
    h_h_minor->addWidget( spin_h_minor );
    h_h_minor->addStretch();

    lbl_v_major = new QLabel( tr( "<b>Vertical Major Ticks</b>" ) );
    lbl_v_minor = new QLabel( tr( "<b>Vertical Minor Ticks</b>" ) );

    lbl_v_major_desc_1_0 = new QLabel( tr( "Draw a major tick every: " ) );
    lbl_v_major_desc_1_0->setIndent( 20 );
    lbl_v_major_desc_1_1 = new QLabel( tr( "threads." ) );

    lbl_v_major_desc_2 = new QLabel( tr( "Number of major ticks: " ) );
    lbl_v_major_desc_2->setIndent( 20 );

    lbl_v_minor_desc = new QLabel( tr( "Number of minor ticks: " ) );
    lbl_v_minor_desc->setIndent( 20 );

    radio_v_major_1_interval = new QRadioButton( tr( "Set major ticks by intervals" ) );
    radio_v_major_2_count    = new QRadioButton( tr( "Set a specific  number of major ticks:" ) );

    spin_v_major_1 = new QSpinBox();
    spin_v_major_1->setSingleStep( 10 );
    spin_v_major_2 = new QSpinBox();
    spin_v_minor   = new QSpinBox();

    h_v_major_1 = new QHBoxLayout();
    h_v_major_1->addWidget( lbl_v_major_desc_1_0 );
    h_v_major_1->addWidget( spin_v_major_1 );
    h_v_major_1->addWidget( lbl_v_major_desc_1_1 );
    h_v_major_1->addStretch();

    h_v_major_2 = new QHBoxLayout();
    h_v_major_2->addWidget( lbl_v_major_desc_2 );
    h_v_major_2->addWidget( spin_v_major_2 );
    h_v_major_2->addStretch();

    h_v_minor = new QHBoxLayout();
    h_v_minor->addWidget( lbl_v_minor_desc );
    h_v_minor->addWidget( spin_v_minor );
    h_v_minor->addStretch();

    btn_ok     = new QPushButton( tr( "OK" ) );
    btn_cancel = new QPushButton( tr( "Cancel" ) );
    btn_reset  = new QPushButton( tr( "Reset" ) );

    h_buttons = new QHBoxLayout();
    h_buttons->addWidget( btn_ok );
    h_buttons->addWidget( btn_cancel );
    h_buttons->addSpacerItem( new QSpacerItem( 20, 0 ) );
    h_buttons->addWidget( btn_reset, 0, Qt::AlignRight );

    vertical_layout_h = new QVBoxLayout();

    vertical_layout_h->addWidget( lbl_h_major );
    vertical_layout_h->addWidget( radio_h_major_1_interval );
    vertical_layout_h->addLayout( h_h_major_1 );
    vertical_layout_h->addWidget( radio_h_major_2_count );
    vertical_layout_h->addLayout( h_h_major_2 );

    vertical_layout_h->addStretch();

    vertical_layout_h->addWidget( lbl_h_minor );
    vertical_layout_h->addLayout( h_h_minor );

    vertical_layout_h->addStretch();
    h_grp = new QGroupBox();
    h_grp->setLayout( vertical_layout_h );

    vertical_layout_v = new QVBoxLayout();

    vertical_layout_v->addWidget( lbl_v_major );
    vertical_layout_v->addWidget( radio_v_major_1_interval );
    vertical_layout_v->addLayout( h_v_major_1 );
    vertical_layout_v->addWidget( radio_v_major_2_count );
    vertical_layout_v->addLayout( h_v_major_2 );

    vertical_layout_v->addStretch();

    vertical_layout_v->addWidget( lbl_v_minor );
    vertical_layout_v->addLayout( h_v_minor );

    vertical_layout_v->addStretch();
    v_grp = new QGroupBox();
    v_grp->setLayout( vertical_layout_v );

    /*minColorList = new QComboBox();
       maxColorList = new QComboBox();
       QStringList colorNames = QColor::colorNames();
       short size = maxColorList->style()->pixelMetric(QStyle::PM_SmallIconSize);
       QPixmap icon(size, size);
       unsigned int i = 0;
       foreach(QString colorName, colorNames){
        minColorList->addItem(colorName, QColor(i));
        maxColorList->addItem(colorName, QColor(i));
        icon.fill(QColor(colorName));
        minColorList->setItemData(i, icon, Qt::DecorationRole);
        maxColorList->setItemData(i, icon, Qt::DecorationRole);
        if(oldMinColorIndex == -1){
            if(QColor(colorName) == plotWidget::DEFAULT_HEAT_MAP_MIN_COLOR)
                oldMinColorIndex = i;
        }
        if(oldMaxColorIndex == -1){
            if(QColor(colorName) == plotWidget::DEFAULT_HEAT_MAP_MAX_COLOR)
                oldMaxColorIndex = i;
        }
        i++;
       }
       if(oldMinColorIndex != -1){
        minColorList->setCurrentIndex(oldMinColorIndex);
       }
       if(oldMaxColorIndex != -1){
        maxColorList->setCurrentIndex(oldMaxColorIndex);
       }
       minColorList->setMaximumWidth(170);
       maxColorList->setMaximumWidth(170);

       col_grp = new QGroupBox("Heat Map Colors");
       QFormLayout* colLayout = new QFormLayout(col_grp);
       colLayout->addRow("Min Color:", minColorList);
       colLayout->addRow("Max Color:", maxColorList);*/

    main_layout = new QVBoxLayout();
    main_layout->addWidget( h_grp );
    main_layout->addWidget( v_grp );
    // main_layout->addWidget(col_grp);
    main_layout->addLayout( h_buttons );
    // FIXME fix the margins to make the dialog a littled smaller, main_layout should be fixed
    this->setLayout( main_layout );
}


void
HeatmapCustomizationzation::setConnections()
{
    // TODO: clean out a SpinBox widget when another RadioBox widget is activated
    // Horizontal controls connections
    QObject::connect( radio_h_major_1_interval, SIGNAL( clicked( bool ) ), spin_h_major_1, SLOT( setEnabled( bool ) ) );
    QObject::connect( radio_h_major_1_interval, SIGNAL( clicked( bool ) ), spin_h_major_2, SLOT( setDisabled( bool ) ) );
    QObject::connect( radio_h_major_1_interval, SIGNAL( clicked( bool ) ), lbl_h_major_desc_1_0, SLOT( setEnabled( bool ) ) );
    QObject::connect( radio_h_major_1_interval, SIGNAL( clicked( bool ) ), lbl_h_major_desc_1_1, SLOT( setEnabled( bool ) ) );
    QObject::connect( radio_h_major_1_interval, SIGNAL( clicked( bool ) ), lbl_h_major_desc_2, SLOT( setDisabled( bool ) ) );
    QObject::connect( radio_h_major_1_interval, SIGNAL( clicked( bool ) ), spin_h_major_2, SLOT( setDisabled( bool ) ) );
    QObject::connect( radio_h_major_1_interval, SIGNAL( clicked() ), spin_h_major_1, SLOT( setFocus() ) );
    QObject::connect( radio_h_major_2_count, SIGNAL( clicked( bool ) ), spin_h_major_2, SLOT( setEnabled( bool ) ) );
    QObject::connect( radio_h_major_2_count, SIGNAL( clicked( bool ) ), spin_h_major_1, SLOT( setDisabled( bool ) ) );
    QObject::connect( radio_h_major_2_count, SIGNAL( clicked( bool ) ), lbl_h_major_desc_1_0, SLOT( setDisabled( bool ) ) );
    QObject::connect( radio_h_major_2_count, SIGNAL( clicked( bool ) ), lbl_h_major_desc_1_1, SLOT( setDisabled( bool ) ) );
    QObject::connect( radio_h_major_2_count, SIGNAL( clicked( bool ) ), lbl_h_major_desc_2, SLOT( setEnabled( bool ) ) );
    QObject::connect( radio_h_major_2_count, SIGNAL( clicked() ), spin_h_major_2, SLOT( setFocus() ) );

    // Vertical controls connections
    QObject::connect( radio_v_major_1_interval, SIGNAL( clicked( bool ) ), spin_v_major_1, SLOT( setEnabled( bool ) ) );
    QObject::connect( radio_v_major_1_interval, SIGNAL( clicked( bool ) ), spin_v_major_2, SLOT( setDisabled( bool ) ) );
    QObject::connect( radio_v_major_1_interval, SIGNAL( clicked( bool ) ), lbl_v_major_desc_1_0, SLOT( setEnabled( bool ) ) );
    QObject::connect( radio_v_major_1_interval, SIGNAL( clicked( bool ) ), lbl_v_major_desc_1_1, SLOT( setEnabled( bool ) ) );
    QObject::connect( radio_v_major_1_interval, SIGNAL( clicked( bool ) ), lbl_v_major_desc_2, SLOT( setDisabled( bool ) ) );
    QObject::connect( radio_v_major_1_interval, SIGNAL( clicked( bool ) ), spin_v_major_2, SLOT( setDisabled( bool ) ) );
    QObject::connect( radio_v_major_1_interval, SIGNAL( clicked() ), spin_v_major_1, SLOT( setFocus() ) );
    QObject::connect( radio_v_major_2_count, SIGNAL( clicked( bool ) ), spin_v_major_2, SLOT( setEnabled( bool ) ) );
    QObject::connect( radio_v_major_2_count, SIGNAL( clicked( bool ) ), spin_v_major_1, SLOT( setDisabled( bool ) ) );
    QObject::connect( radio_v_major_2_count, SIGNAL( clicked( bool ) ), lbl_v_major_desc_1_0, SLOT( setDisabled( bool ) ) );
    QObject::connect( radio_v_major_2_count, SIGNAL( clicked( bool ) ), lbl_v_major_desc_1_1, SLOT( setDisabled( bool ) ) );
    QObject::connect( radio_v_major_2_count, SIGNAL( clicked( bool ) ), lbl_v_major_desc_2, SLOT( setEnabled( bool ) ) );
    QObject::connect( radio_v_major_2_count, SIGNAL( clicked() ), spin_v_major_2, SLOT( setFocus() ) );

    QObject::connect( btn_ok, SIGNAL( clicked() ), this, SLOT( handleOKButton() ) );
    QObject::connect( btn_cancel, SIGNAL( clicked() ), this, SLOT( handleCancelButton() ) );
    QObject::connect( btn_reset, SIGNAL( clicked() ), this, SLOT( handleResetToDefaultsButton() ) );
}

void
HeatmapCustomizationzation::handleOKButton()
{
    // 1. Handle horizontal major and minor notches
    if ( radio_h_major_1_interval->isChecked() )
    {
        if ( spin_h_major_1->value() - 1 != oldHMajorInterval ) // do we really need to update?
        {
            oldHMajorInterval = spin_h_major_1->value();
            oldHMajorCount    = -1;
            // emit setHMajorFixedInterval(spin_h_major_1->value() - 1);
            emit setHMajorFixedInterval( spin_h_major_1->value() );
        }
    }
    else
    {
        if ( spin_h_major_2->value() != oldHMajorCount )
        {
            oldHMajorCount    = spin_h_major_2->value();
            oldHMajorInterval = -1;
            emit setHMajorStepsCount( spin_h_major_2->value() );
        }
    }
    if ( oldHMinorCount != spin_h_minor->value() )
    {
        oldHMinorCount = spin_h_minor->value();
        emit setHMinorStepsCount( spin_h_minor->value() );
    }

    // 2. Handle vertical major and minor notches
    if ( radio_v_major_1_interval->isChecked() )
    {
        if ( spin_v_major_1->value() - 1 != oldVMajorInterval ) // do we really need to update?
        {
            oldVMajorInterval = spin_v_major_1->value();
            oldVMajorCount    = -1;
            // emit setVMajorFixedInterval(spin_v_major_1->value() - 1);
            emit setVMajorFixedInterval( spin_v_major_1->value() );
        }
    }
    else
    {
        if ( spin_v_major_2->value() != oldVMajorCount )
        {
            oldVMajorCount    = spin_v_major_2->value();
            oldVMajorInterval = -1;
            emit setVMajorStepsCount( spin_v_major_2->value() );
        }
    }
    if ( oldVMinorCount != spin_v_minor->value() )
    {
        oldVMinorCount = spin_v_minor->value();
        emit setVMinorStepsCount( spin_v_minor->value() );
    }

    // 3. Handle colors
/*   if(minColorList->currentIndex() != oldMinColorIndex){
       oldMinColorIndex = minColorList->currentIndex();
       emit setMinColor(QColor(minColorList->currentText()));
   }
   if(maxColorList->currentIndex() != oldMaxColorIndex){
       oldMaxColorIndex = maxColorList->currentIndex();
       emit setMaxColor(QColor(maxColorList->currentText()));
   }*/

    this->hide();
}

void
HeatmapCustomizationzation::handleCancelButton()
{
    if ( oldHMajorInterval == -1 )
    {
        radio_h_major_2_count->setChecked( true );
        spin_h_major_2->setValue( oldHMajorCount );
    }
    else
    {
        radio_h_major_1_interval->setChecked( true );
        spin_h_major_1->setValue( oldHMajorInterval );
    }
    spin_h_minor->setValue( oldVMinorCount );
    if ( oldVMajorInterval == -1 )
    {
        radio_v_major_2_count->setChecked( true );
        spin_v_major_2->setValue( oldVMajorCount );
    }
    else
    {
        radio_v_major_1_interval->setChecked( true );
        spin_v_major_1->setValue( oldVMajorInterval );
    }
    spin_v_minor->setValue( oldVMinorCount );
//   if(oldMinColorIndex >= 0)
//	   minColorList->setCurrentIndex(oldMinColorIndex);
//   if(oldMaxColorIndex >= 0)
//	   maxColorList->setCurrentIndex(oldMaxColorIndex);
    this->hide();
}

HeatmapCustomizationzation::~HeatmapCustomizationzation()
{
}

void
HeatmapCustomizationzation::setInitialHMajorIntervalAndMinorCount( int major_interval, int minor_count )
{
    // set major default values
    if ( radio_h_major_1_interval )
    {
        radio_h_major_1_interval->setChecked( true );
    }
    if ( spin_h_major_1 )
    {
        spin_h_major_1->setEnabled( true );
        lbl_h_major_desc_1_0->setEnabled( true );
        lbl_h_major_desc_1_1->setEnabled( true );
        lbl_h_major_desc_2->setEnabled( false );
        spin_h_major_2->setEnabled( false );
        spin_h_major_1->setValue( major_interval );
        spin_h_major_1->setFocus();
    }
    // set minor default values
    if ( spin_h_minor )
    {
        spin_h_minor->setValue( minor_count );
    }
}

void
HeatmapCustomizationzation::setInitialHMajorCountAndMinorCount( int major_count, int minor_count )
{
    // set major default values
    if ( radio_h_major_2_count )
    {
        radio_h_major_2_count->setChecked( true );
    }
    if ( spin_h_major_2 )
    {
        lbl_h_major_desc_1_0->setEnabled( false );
        lbl_h_major_desc_1_1->setEnabled( false );
        lbl_h_major_desc_2->setEnabled( true );
        spin_h_major_1->setEnabled( false );
        spin_h_major_2->setEnabled( true );
        spin_h_major_2->setValue( major_count );
        spin_h_major_2->setFocus();
    }
    // set minor default values
    if ( spin_h_minor )
    {
        spin_h_minor->setValue( minor_count );
    }
}

void
HeatmapCustomizationzation::setInitialVMajorIntervalAndMinorCount( int major_interval, int minor_count )
{
    // set major default values
    if ( radio_v_major_1_interval )
    {
        radio_v_major_1_interval->setChecked( true );
    }
    if ( spin_v_major_1 )
    {
        spin_v_major_1->setEnabled( true );
        lbl_v_major_desc_1_0->setEnabled( true );
        lbl_v_major_desc_1_1->setEnabled( true );
        lbl_v_major_desc_2->setEnabled( false );
        spin_v_major_2->setEnabled( false );
        spin_v_major_1->setValue( major_interval );
        spin_v_major_1->setFocus();
    }
    // set minor default values
    if ( spin_v_minor )
    {
        spin_v_minor->setValue( minor_count );
    }
}

void
HeatmapCustomizationzation::setInitialVMajorCountAndMinorCount( int major_count, int minor_count )
{
    // set major default values
    if ( radio_v_major_2_count )
    {
        radio_v_major_2_count->setChecked( true );
    }
    if ( spin_v_major_2 )
    {
        lbl_v_major_desc_1_0->setEnabled( false );
        lbl_v_major_desc_1_1->setEnabled( false );
        lbl_v_major_desc_2->setEnabled( true );
        spin_v_major_1->setEnabled( false );
        spin_v_major_2->setEnabled( true );
        spin_v_major_2->setValue( major_count );
        spin_v_major_2->setFocus();
    }
    // set minor default values
    if ( spin_v_minor )
    {
        spin_v_minor->setValue( minor_count );
    }
}

void
HeatmapCustomizationzation::handleResetToDefaultsButton()
{
    if ( initHMajorInterval == -1 )
    {
        radio_h_major_2_count->setChecked( true );
        spin_h_major_2->setValue( initHMajorCount );
    }
    else
    {
        radio_h_major_1_interval->setChecked( true );
        spin_h_major_1->setValue( initHMajorInterval );
    }
    spin_h_minor->setValue( initHMinorCount );

    if ( initVMajorInterval == -1 )
    {
        radio_v_major_2_count->setChecked( true );
        spin_v_major_2->setValue( initVMajorCount );
    }
    else
    {
        radio_v_major_1_interval->setChecked( true );
        spin_v_major_1->setValue( initVMajorInterval );
    }
    spin_v_minor->setValue( initVMinorCount );
}
