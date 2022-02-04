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

#include "SettingsWidget.h"

#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QGridLayout>
#include <QDebug>

SettingWidget::SettingWidget( int      processesMajorCountNum,
                              int      processesMinorNotchesCount,
                              int      measurementsMajorCountNum,
                              int      measurementsMinorNotchesCount,
                              QWidget* parent ) :
    QDialog( parent ), processesMajorCountNumDefault( processesMajorCountNum ), processesMinorNotchesCountDefault( processesMinorNotchesCount ),
    measurementsMajorCountNumDefault( measurementsMajorCountNum ), measurementsMinorNotchesCountDefault( measurementsMinorNotchesCount )
{
    pMajorNotchesByIntervalWasChecked = false;
    pMajorNotchesCountOldValue        = processesMajorCountNum;
    pMajorNotchesIntervalSizeOldValue = -1;
    pMinorNotchesOldCount             = processesMinorNotchesCount;
    mMajorNotchesByIntervalWasChecked = false;
    mMajorNotchesCountOldValue        = measurementsMajorCountNum;
    mMajorNotchesIntervalSizeOldValue = -1;
    mMinorNotchesOldCount             = measurementsMinorNotchesCount;
    mTopNotchValueAutoWasChecked      = true;
    mTopNotchValueManualOldValue      = -1;
    mBottomNotchValueAutoWasChecked   = true;
    mBottomNotchValueManualOldValue   = -1;

    initComponents();
    setConnections();
    setDefaultValues();
}

void
SettingWidget::initComponents()
{
    pMajorNotchesByInterval   = new QRadioButton( tr( "Draw major tick every" ) );
    pMajorNotchesByCount      = new QRadioButton( tr( "Draw" ) );
    pMajorNotchesIntervalSize = new QSpinBox();
    pMajorNotchesCount        = new QSpinBox();
    pMinorNotchesCount        = new QSpinBox();

    mTopNotchValueAuto   = new QRadioButton( tr( "Set automatically" ) );
    mTopNotchValueManual = new QRadioButton( tr( "Set to" ) );
    mTopNotchManualValue = new QSpinBox();

    mBottomNotchValueAuto   = new QRadioButton( tr( "Set Automatically" ) );
    mBottomNotchValueManual = new QRadioButton( tr( "Set to" ) );
    mBottomNotchManualValue = new QSpinBox();

    mMajorNotchesByInterval   = new QRadioButton( tr( "Draw major tick every " ) );
    mMajorNotchesByCount      = new QRadioButton( tr( "Draw" ) );
    mMajorNotchesIntervalSize = new QDoubleSpinBox();
    mMajorNotchesIntervalSize->setDecimals( 8 );
    mMajorNotchesCount = new QSpinBox();
    mMinorNotchesCount = new QSpinBox();

    okButton     = new QPushButton( tr( "OK" ) );
    cancelButton = new QPushButton( tr( "Cancel" ) );
    resetButton  = new QPushButton( tr( "Reset" ) );

    QVBoxLayout* pTicksLayout = new QVBoxLayout();
    QHBoxLayout* pTicks1      = new QHBoxLayout();
    pTicks1->addWidget( pMajorNotchesByInterval );
    pTicks1->addWidget( pMajorNotchesIntervalSize );
    pTicks1->addWidget( new QLabel( tr( "iterations" ) ) );
    pTicks1->addStretch();
    QHBoxLayout* pTicks2 = new QHBoxLayout();
    pTicks2->addWidget( pMajorNotchesByCount );
    pTicks2->addWidget( pMajorNotchesCount );
    pTicks2->addWidget( new QLabel( tr( "major ticks" ) ) );
    pTicks2->addStretch();
    QHBoxLayout* pTicks3 = new QHBoxLayout();
    pTicks3->addWidget( new QLabel( tr( "Draw" ) ) );
    pTicks3->addWidget( pMinorNotchesCount );
    pTicks3->addWidget( new QLabel( tr( "minor ticks" ) ) );
    pTicks3->addStretch();
    pTicksLayout->addLayout( pTicks1 );
    pTicksLayout->addLayout( pTicks2 );
    pTicksLayout->addLayout( pTicks3 );
    QGroupBox* pTicksGB = new QGroupBox( tr( "Iterations Ruler Customization" ) );
    pTicksGB->setLayout( pTicksLayout );

    // /////////////////////////////////////////////////////
    QVBoxLayout* mTicksLayout = new QVBoxLayout();
    QHBoxLayout* mTicks1      = new QHBoxLayout();
    mTicks1->addWidget( mMajorNotchesByInterval );
    mTicks1->addWidget( mMajorNotchesIntervalSize );
    mTicks1->addStretch();
    QHBoxLayout* mTicks2 = new QHBoxLayout();
    mTicks2->addWidget( mMajorNotchesByCount );
    mTicks2->addWidget( mMajorNotchesCount );
    mTicks2->addWidget( new QLabel( tr( "major ticks" ) ) );
    mTicks2->addStretch();
    QHBoxLayout* mTicks3 = new QHBoxLayout();
    mTicks3->addWidget( new QLabel( tr( "Draw" ) ) );
    mTicks3->addWidget( mMinorNotchesCount );
    mTicks3->addWidget( new QLabel( tr( "minor ticks" ) ) );
    mTicks3->addStretch();
    mTicksLayout->addLayout( mTicks1 );
    mTicksLayout->addLayout( mTicks2 );
    mTicksLayout->addLayout( mTicks3 );
    QGroupBox* mTicksGB = new QGroupBox( tr( "Ruler Customization" ) );
    mTicksGB->setLayout( mTicksLayout );
    // /////////////////////////////////////////////////////
    QVBoxLayout* mTopNotchLayout = new QVBoxLayout();
    QHBoxLayout* mTop1           = new QHBoxLayout();
    mTop1->addWidget( mTopNotchValueAuto );
    mTop1->addStretch();
    QHBoxLayout* mTop2 = new QHBoxLayout();
    mTop2->addWidget( mTopNotchValueManual );
    mTop2->addWidget( mTopNotchManualValue );
    mTop2->addStretch();
    mTopNotchLayout->addLayout( mTop1 );
    mTopNotchLayout->addLayout( mTop2 );
    QGroupBox* mTopNotchGB = new QGroupBox( tr( "Top Notch Value" ) );
    mTopNotchGB->setLayout( mTopNotchLayout );
    // /////////////////////////////////////////////////////
    QVBoxLayout* mBottomNotchLayout = new QVBoxLayout();
    QHBoxLayout* mBottom1           = new QHBoxLayout();
    mBottom1->addWidget( mBottomNotchValueAuto );
    mBottom1->addStretch();
    QHBoxLayout* mBottom2 = new QHBoxLayout();
    mBottom2->addWidget( mBottomNotchValueManual );
    mBottom2->addWidget( mBottomNotchManualValue );
    mBottom2->addStretch();
    mBottomNotchLayout->addLayout( mBottom1 );
    mBottomNotchLayout->addLayout( mBottom2 );
    QGroupBox* mBottomNotchGB = new QGroupBox( tr( "Bottom Notch Value" ) );
    mBottomNotchGB->setLayout( mBottomNotchLayout );
    // /////////////////////////////////////////////////////
    QGroupBox*   mBG     = new QGroupBox( tr( "Measurements Customization" ) );
    QGridLayout* mLayout = new QGridLayout();
    mLayout->addWidget( mTicksGB, 0, 0, 1, 2 );
    mLayout->addWidget( mTopNotchGB, 1, 0 );
    mLayout->addWidget( mBottomNotchGB, 1, 1 );
    mBG->setLayout( mLayout );
    // /////////////////////////////////////////////////////
    QHBoxLayout* btnsLayout = new QHBoxLayout();
    btnsLayout->addWidget( okButton );
    btnsLayout->addWidget( cancelButton );
    btnsLayout->addStretch( 3 );
    btnsLayout->addWidget( resetButton );

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget( mBG );
    layout->addWidget( pTicksGB );
    layout->addLayout( btnsLayout );
    setLayout( layout );
}

void
SettingWidget::setConnections()
{
    QObject::connect( pMajorNotchesByInterval, SIGNAL( clicked( bool ) ), pMajorNotchesIntervalSize, SLOT( setEnabled( bool ) ) );
    QObject::connect( pMajorNotchesByInterval, SIGNAL( clicked( bool ) ), pMajorNotchesCount, SLOT( setDisabled( bool ) ) );
    QObject::connect( pMajorNotchesByCount, SIGNAL( clicked( bool ) ), pMajorNotchesCount, SLOT( setEnabled( bool ) ) );
    QObject::connect( pMajorNotchesByCount, SIGNAL( clicked( bool ) ), pMajorNotchesIntervalSize, SLOT( setDisabled( bool ) ) );
    QObject::connect( mMajorNotchesByInterval, SIGNAL( clicked( bool ) ), mMajorNotchesIntervalSize, SLOT( setEnabled( bool ) ) );
    QObject::connect( mMajorNotchesByInterval, SIGNAL( clicked( bool ) ), mMajorNotchesCount, SLOT( setDisabled( bool ) ) );
    QObject::connect( mMajorNotchesByCount, SIGNAL( clicked( bool ) ), mMajorNotchesCount, SLOT( setEnabled( bool ) ) );
    QObject::connect( mMajorNotchesByCount, SIGNAL( clicked( bool ) ), mMajorNotchesIntervalSize, SLOT( setDisabled( bool ) ) );
    QObject::connect( mTopNotchValueAuto, SIGNAL( clicked( bool ) ), mTopNotchManualValue, SLOT( setDisabled( bool ) ) );
    QObject::connect( mTopNotchValueManual, SIGNAL( clicked( bool ) ), mTopNotchManualValue, SLOT( setEnabled( bool ) ) );
    QObject::connect( mBottomNotchValueAuto, SIGNAL( clicked( bool ) ), mBottomNotchManualValue, SLOT( setDisabled( bool ) ) );
    QObject::connect( mBottomNotchValueManual, SIGNAL( clicked( bool ) ), mBottomNotchManualValue, SLOT( setEnabled( bool ) ) );

    QObject::connect( okButton, SIGNAL( clicked() ), this, SLOT( okButtonClickHandler() ) );
    QObject::connect( cancelButton, SIGNAL( clicked() ), this, SLOT( cancelButtonClickHandler() ) );
    QObject::connect( resetButton, SIGNAL( clicked() ), this, SLOT( resetButtonClickHandler() ) );
}

void
SettingWidget::setDefaultValues()
{
    pMajorNotchesByCount->setChecked( true );
    pMajorNotchesCount->setValue( processesMajorCountNumDefault );
    pMajorNotchesIntervalSize->setEnabled( false );
    pMinorNotchesCount->setValue( processesMinorNotchesCountDefault );
    mMajorNotchesByCount->setChecked( true );
    mMajorNotchesCount->setValue( measurementsMajorCountNumDefault );
    mMajorNotchesIntervalSize->setEnabled( false );
    mMinorNotchesCount->setValue( measurementsMinorNotchesCountDefault );
    mTopNotchValueAuto->setChecked( true );
    mBottomNotchValueAuto->setChecked( true );
}

void
SettingWidget::setTopNotchToAutomatic()
{
    if ( mTopNotchValueManual->isChecked() )
    {
        mTopNotchManualValue->setValue( 0 );
        mTopNotchManualValue->setEnabled( false );
        mTopNotchValueAuto->setChecked( true );
        mTopNotchValueAutoWasChecked = true;
        mTopNotchValueManualOldValue = -1;
    }
}

void
SettingWidget::setBottomNotchToAutomatic()
{
    if ( mBottomNotchValueManual->isChecked() )
    {
        mBottomNotchManualValue->setValue( 0 );
        mBottomNotchManualValue->setEnabled( false );
        mBottomNotchValueAuto->setChecked( true );
        mBottomNotchValueAutoWasChecked = true;
        mBottomNotchValueManualOldValue = -1;
    }
}

void
SettingWidget::okButtonClickHandler()
{
    if ( pMajorNotchesByInterval->isChecked() )
    {
        if ( pMajorNotchesByIntervalWasChecked == false )
        {
            emit setProcessesRulerMajorNotchesInterval( pMajorNotchesIntervalSize->value() );
        }
        else  // it was checked by the value might have changed by the user
        {
            if ( pMajorNotchesIntervalSizeOldValue != pMajorNotchesIntervalSize->value() )
            {
                emit setProcessesRulerMajorNotchesInterval( pMajorNotchesIntervalSize->value() );
            }
        }
        pMajorNotchesIntervalSizeOldValue = pMajorNotchesIntervalSize->value();
        pMajorNotchesByIntervalWasChecked = true;
    }
    else  // pMajorNotchesByCount->isChecked()
    {
        if ( pMajorNotchesByIntervalWasChecked == true )
        {
            emit setProcessesRulerMajorNotchesCount( pMajorNotchesCount->value() );
        }
        else  // it was checked by the value might have changed by the user
        {
            if ( pMajorNotchesCountOldValue != pMajorNotchesCount->value() )
            {
                emit setProcessesRulerMajorNotchesCount( pMajorNotchesCount->value() );
            }
        }
        pMajorNotchesCountOldValue        = pMajorNotchesCount->value();
        pMajorNotchesByIntervalWasChecked = false;
    }

    if ( pMinorNotchesCount->value() != pMinorNotchesOldCount )
    {
        emit setProcessesRulerMinorNotchesCount( pMinorNotchesCount->value() );
        pMinorNotchesOldCount = pMinorNotchesCount->value();
    }
    if ( mMajorNotchesByInterval->isChecked() )
    {
        if ( mMajorNotchesByIntervalWasChecked == false )
        {
            emit setMeasurementRulerMajorNotchesInterval( mMajorNotchesIntervalSize->value() );
        }
        else  // it was checked by the value might have changed by the user
        {
            if ( mMajorNotchesIntervalSizeOldValue != mMajorNotchesIntervalSize->value() )
            {
                emit setMeasurementRulerMajorNotchesInterval( mMajorNotchesIntervalSize->value() );
            }
        }
        mMajorNotchesIntervalSizeOldValue = mMajorNotchesIntervalSize->value();
        mMajorNotchesByIntervalWasChecked = true;
    }
    else  // mMajorNotchesByCount->isChecked()
    {
        if ( mMajorNotchesByIntervalWasChecked == true )
        {
            emit setMeasurementRulerMajorNotchesCount( mMajorNotchesCount->value() );
        }
        else  // it was checked by the value might have changed by the user
        {
            if ( mMajorNotchesCountOldValue != mMajorNotchesCount->value() )
            {
                emit setMeasurementRulerMajorNotchesCount( mMajorNotchesCount->value() );
            }
        }
        mMajorNotchesCountOldValue        = mMajorNotchesCount->value();
        mMajorNotchesByIntervalWasChecked = false;
    }
    if ( mMinorNotchesCount->value() != mMinorNotchesOldCount )
    {
        emit setMeasurementRulerMinorNotchesCount( mMinorNotchesCount->value() );
        mMinorNotchesOldCount = mMinorNotchesCount->value();
    }
    if ( mTopNotchValueAuto->isChecked() )
    {
        if ( mTopNotchValueAutoWasChecked == false )
        {
            emit resetMeasurementTopNotchValue();
            mTopNotchValueAutoWasChecked = true;
            mTopNotchValueManualOldValue = -1;
            mTopNotchManualValue->setValue( 0 );
        }
    }
    else  // mTopNotchValueManual->isChecked()
    {
        if ( mTopNotchValueManualOldValue != mTopNotchManualValue->value() )
        {
            emit setMeasurementTopNotchValue( mTopNotchManualValue->value() );
            mTopNotchValueManualOldValue = mTopNotchManualValue->value();
        }
        mTopNotchValueAutoWasChecked = false;
    }
    if ( mBottomNotchValueAuto->isChecked() )
    {
        if ( mBottomNotchValueAutoWasChecked == false )
        {
            emit resetMeasurementBottomNotchValue();
            mBottomNotchValueAutoWasChecked = true;
            mBottomNotchValueManualOldValue = -1;
            mBottomNotchManualValue->setValue( 0 );
        }
    }
    else  // mBottomNotchValueManual->isChecked()
    {
        if ( mBottomNotchValueManualOldValue != mBottomNotchManualValue->value() )
        {
            emit setMeasurementBottomNotchValue( mBottomNotchManualValue->value() );
            mBottomNotchValueManualOldValue = mBottomNotchManualValue->value();
        }
        mBottomNotchValueAutoWasChecked = false;
    }
    this->hide();
}

void
SettingWidget::cancelButtonClickHandler()
{
    if ( pMajorNotchesByIntervalWasChecked == true )
    {
        pMajorNotchesIntervalSize->setValue( pMajorNotchesIntervalSizeOldValue );
        pMajorNotchesByCount->setChecked( false );
        pMajorNotchesByInterval->setChecked( true );
        pMajorNotchesCount->setValue( 0 );
        pMajorNotchesCount->setEnabled( false );
    }
    else
    {
        pMajorNotchesIntervalSize->setValue( 0 );
        pMajorNotchesIntervalSize->setEnabled( false );
        pMajorNotchesByCount->setChecked( true );
        pMajorNotchesByInterval->setChecked( false );
        pMajorNotchesCount->setValue( pMajorNotchesCountOldValue );
        pMajorNotchesCount->setEnabled( true );
    }
    pMinorNotchesCount->setValue( pMinorNotchesOldCount );


    if ( mMajorNotchesByIntervalWasChecked == true )
    {
        mMajorNotchesIntervalSize->setValue( mMajorNotchesIntervalSizeOldValue );
        mMajorNotchesByCount->setChecked( false );
        mMajorNotchesByInterval->setChecked( true );
        mMajorNotchesCount->setValue( 0 );
        mMajorNotchesCount->setEnabled( false );
    }
    else
    {
        mMajorNotchesIntervalSize->setValue( 0 );
        mMajorNotchesIntervalSize->setEnabled( false );
        mMajorNotchesByCount->setChecked( true );
        mMajorNotchesByInterval->setChecked( false );
        mMajorNotchesCount->setValue( mMajorNotchesCountOldValue );
        mMajorNotchesCount->setEnabled( true );
    }
    mMinorNotchesCount->setValue( pMinorNotchesOldCount );

    if ( mTopNotchValueAutoWasChecked == true )
    {
        mTopNotchValueAuto->setChecked( true );
        mTopNotchValueManual->setChecked( false );
        mTopNotchManualValue->setValue( 0 );
        mTopNotchManualValue->setEnabled( false );
    }
    else
    {
        mTopNotchValueAuto->setChecked( false );
        mTopNotchValueManual->setChecked( true );
        mTopNotchManualValue->setValue( mTopNotchValueManualOldValue );
        mTopNotchManualValue->setEnabled( true );
    }

    if ( mBottomNotchValueAutoWasChecked == true )
    {
        mBottomNotchValueAuto->setChecked( true );
        mBottomNotchValueManual->setChecked( false );
        mBottomNotchManualValue->setValue( 0 );
        mBottomNotchManualValue->setEnabled( false );
    }
    else
    {
        mBottomNotchValueAuto->setChecked( false );
        mBottomNotchValueManual->setChecked( true );
        mBottomNotchManualValue->setValue( mBottomNotchValueManualOldValue );
        mBottomNotchManualValue->setEnabled( true );
    }
    this->reject();
}

void
SettingWidget::resetButtonClickHandler()
{
    setDefaultValues();
    okButtonClickHandler();
    if ( this->isHidden() == false )
    {
        this->hide();
    }
}
