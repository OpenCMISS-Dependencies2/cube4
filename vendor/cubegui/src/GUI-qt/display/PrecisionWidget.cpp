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

#include <cassert>
#include <iostream>
#include <sstream>
#include <cmath>

#include "CubeMetric.h"
#include "PrecisionWidget.h"
#include "MainWidget.h"
#include "TreeItem.h"
#include "Tree.h"
#include "TabManager.h"

#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QSpinBox>
#include <QAbstractButton>
#include <QCheckBox>

using namespace std;
using namespace cubegui;

PrecisionWidget::PrecisionWidget( QWidget* parent ) : QDialog( parent )
{
    setWindowTitle( tr( "Precision settings" ) );

    // define layout of the dialog

    QVBoxLayout* layout = new QVBoxLayout();
    setLayout( layout );

    QGroupBox*   groupBox0       = new QGroupBox( tr( "Display in trees:" ), this );
    QGridLayout* groupBox0Layout = new QGridLayout();
    groupBox0->setLayout( groupBox0Layout );
    layout->addWidget( groupBox0 );

    QGroupBox*   groupBox1       = new QGroupBox( tr( "Display in the value widget under the tree widgets:" ), this );
    QGridLayout* groupBox1Layout = new QGridLayout();
    groupBox1->setLayout( groupBox1Layout );
    layout->addWidget( groupBox1 );


    QLabel* prec0Label = new QLabel( this );
    prec0Label->setText( tr( "Number of digits after decimal point: " ) );
    groupBox0Layout->addWidget( prec0Label, 1, 1 );

    prec0Spin = new QSpinBox( this );
    prec0Spin->setRange( 1, 20 );
    prec0Spin->setSingleStep( 1 );
    prec0Spin->setValue( 1 );
    groupBox0Layout->addWidget( prec0Spin, 1, 2 );

    QLabel* upper0Label = new QLabel( this );
    upper0Label->setText( tr( "Exponent representation above 10^x with x: " ) );
    groupBox0Layout->addWidget( upper0Label, 2, 1 );

    upper0Spin = new QSpinBox( this );
    upper0Spin->setRange( 0, 20 );
    upper0Spin->setSingleStep( 1 );
    upper0Spin->setValue( 0 );
    groupBox0Layout->addWidget( upper0Spin, 2, 2 );

    QLabel* round0Label = new QLabel( this );
    round0Label->setText( tr( "Display zero for values below 10^(-x) with x: " ) );
    groupBox0Layout->addWidget( round0Label, 3, 1 );

    round0Spin = new QSpinBox( this );
    round0Spin->setRange( 1, 20 );
    round0Spin->setSingleStep( 1 );
    round0Spin->setValue( 1 );
    groupBox0Layout->addWidget( round0Spin, 3, 2 );
    humanFormat = new QCheckBox( tr( "Use human readable units for bytes and occ" ) );
    groupBox0Layout->addWidget( humanFormat, 4, 1 );

    QLabel* prec1Label = new QLabel( this );
    prec1Label->setText( tr( "Number of digits after decimal point: " ) );
    groupBox1Layout->addWidget( prec1Label, 1, 1 );

    prec1Spin = new QSpinBox( this );
    prec1Spin->setRange( 1, 20 );
    prec1Spin->setSingleStep( 1 );
    prec1Spin->setValue( 1 );
    groupBox1Layout->addWidget( prec1Spin, 1, 2 );

    QLabel* upper1Label = new QLabel( this );
    upper1Label->setText( tr( "Exponent representation above 10^x with x: " ) );
    groupBox1Layout->addWidget( upper1Label, 2, 1 );

    upper1Spin = new QSpinBox( this );
    upper1Spin->setRange( 0, 20 );
    upper1Spin->setSingleStep( 1 );
    upper1Spin->setValue( 0 );
    groupBox1Layout->addWidget( upper1Spin, 2, 2 );

    QLabel* round1Label = new QLabel( this );
    round1Label->setText( tr( "Display zero for values below 10^(-x) with x: " ) );
    groupBox1Layout->addWidget( round1Label, 3, 1 );

    round1Spin = new QSpinBox( this );
    round1Spin->setRange( 1, 20 );
    round1Spin->setSingleStep( 1 );
    round1Spin->setValue( 1 );
    groupBox1Layout->addWidget( round1Spin, 3, 2 );

    QDialogButtonBox* buttonBox    = new QDialogButtonBox();
    QPushButton*      applyButton  = buttonBox->addButton( QDialogButtonBox::Apply );
    QPushButton*      okButton     = buttonBox->addButton( QDialogButtonBox::Ok );
    QPushButton*      cancelButton = buttonBox->addButton( QDialogButtonBox::Cancel );
    connect( applyButton, SIGNAL( clicked() ), this, SLOT( applyValues() ) );
    connect( okButton, SIGNAL( clicked() ), this, SLOT( setValues() ) );
    connect( cancelButton, SIGNAL( clicked() ), this, SLOT( resetValues() ) );
    layout->addWidget( buttonBox );

    // initialize values

    setPrecision( 2, FORMAT_TREES );
    setRoundNr( 7, FORMAT_TREES );
    setUpperExpNr( 4, FORMAT_TREES );

    setPrecision( 2, FORMAT_DEFAULT );
    setRoundNr( 7, FORMAT_DEFAULT );
    setUpperExpNr( 4, FORMAT_DEFAULT );

    v1 = prec0Spin->value();
    v2 = prec1Spin->value();
    v3 = upper0Spin->value();
    v4 = upper1Spin->value();
    v5 = round0Spin->value();
    v6 = round1Spin->value();

    setWhatsThis( tr( "This is a dialog for precision settings. Besides \"Ok\" and \"Cancel\", the dialog offers an \"Apply\" button, that applies the current dialog settings to the display. Pressing \"Cancel\" undoes all changes due to the dialog, even if you already pressed \"Apply\" previously.\n\nThe dialog consists of two parts: precision settings for the tree displays, and precision settings for the selected value info widgets and the topology displays. For both formats, three values can be defined:\n\n - Number of digits after the decimal point: As the name suggests, you can specify the precision for the fraction part of the values. E.g., the number 1.234 is displayed as 1.2 if you set this precision to 1, as 1.234 if you set it to 3, and as 1.2340 if you set it to 4.\n\n - Exponent representation above 10^x with x: Here you can define above which threshold we should use scientific notation. E.g., the value 1000 is displayed as 1000 if this value is larger then 3 and as 1e3 otherwise.\n\n - Display zero values below 10^(-x) with x: Due to inexact floating point representation it often happens that the users wish to round down values near by zero to zero. Here you can define the threshold below which this rounding should take place. E.g., the value 0.0001 is displayed as 0.0001 if this value is larger than 3 and as zero otherwise." ) );

    // define format for precisely displayed valued
    precision[ FORMAT_DOUBLE ]         = 8;
    roundNr[ FORMAT_DOUBLE ]           = 8;
    roundThreshold[ FORMAT_DOUBLE ]    = pow( 10.0, -roundNr[ FORMAT_DOUBLE ] );
    upperExpNr[ FORMAT_DOUBLE ]        = 7;
    upperExpThreshold[ FORMAT_DOUBLE ] = pow( 10.0, upperExpNr[ FORMAT_DOUBLE ] );

    humanReadable = humanFormat->isChecked();
}

PrecisionWidget::~PrecisionWidget()
{
}

int
PrecisionWidget::exec()
{
    // store values for the case that the user clicks on "cancel", in which case
    // the values at dialog start get re-set
    v1 = prec0Spin->value();
    v2 = prec1Spin->value();
    v3 = upper0Spin->value();
    v4 = upper1Spin->value();
    v5 = round0Spin->value();
    v6 = round1Spin->value();

    return QDialog::exec();
}

// some get methods

double
PrecisionWidget::getRoundThreshold( PrecisionFormat format )
{
    return roundThreshold[ format ];
}
int
PrecisionWidget::getPrecision( PrecisionFormat format )
{
    return precision[ format ];
}
int
PrecisionWidget::getRoundNr( PrecisionFormat format )
{
    return roundNr[ format ];
}
int
PrecisionWidget::getUpperExpNr( PrecisionFormat format )
{
    return upperExpNr[ format ];
}

// some set methods

void
PrecisionWidget::setPrecision( int value, PrecisionFormat format )
{
    if ( value < 1 || value > 20 )
    {
        std::cout << tr( "Precision out of range\n" ).toUtf8().data();
        return;
    }
    precision[ format ] = value;
    if ( format == FORMAT_TREES )
    {
        prec0Spin->setValue( value );
    }
    else
    {
        prec1Spin->setValue( value );
    }
}

void
PrecisionWidget::setUpperExpNr( int value, PrecisionFormat format )
{
    if ( value < 0 || value > 20 )
    {
        std::cout << tr( "Upper exponent threshold out of range\n" ).toUtf8().data();
        return;
    }
    upperExpNr[ format ]        = value;
    upperExpThreshold[ format ] = pow( 10.0, value );
    if ( format == FORMAT_TREES )
    {
        upper0Spin->setValue( value );
    }
    else
    {
        upper1Spin->setValue( value );
    }
}

void
PrecisionWidget::setRoundNr( int value, PrecisionFormat format )
{
    if ( value < 1 || value > 20 )
    {
        std::cout << tr( "Rounding threshold out of range\n" ).toUtf8().data();
        return;
    }
    roundNr[ format ]        = value;
    roundThreshold[ format ] = pow( 10.0, -value );
    if ( format == FORMAT_TREES )
    {
        round0Spin->setValue( value );
    }
    else
    {
        round1Spin->setValue( value );
    }
}

// save the dialog values as current values
// (user pressed "ok")
void
PrecisionWidget::setValues()
{
    setPrecision( prec0Spin->value(), FORMAT_TREES );
    setPrecision( prec1Spin->value(), FORMAT_DEFAULT );
    setUpperExpNr( upper0Spin->value(), FORMAT_TREES );
    setUpperExpNr( upper1Spin->value(), FORMAT_DEFAULT );
    setRoundNr( round0Spin->value(), FORMAT_TREES );
    setRoundNr( round1Spin->value(), FORMAT_DEFAULT );
    humanReadable = humanFormat->isChecked();

    emit apply();

    accept();
}

// save the dialog values as current values
// and apply them to the display
// (user pressed "apply"
void
PrecisionWidget::applyValues()
{
    setPrecision( prec0Spin->value(), FORMAT_TREES );
    setPrecision( prec1Spin->value(), FORMAT_DEFAULT );
    setUpperExpNr( upper0Spin->value(), FORMAT_TREES );
    setUpperExpNr( upper1Spin->value(), FORMAT_DEFAULT );
    setRoundNr( round0Spin->value(), FORMAT_TREES );
    setRoundNr( round1Spin->value(), FORMAT_DEFAULT );
    humanReadable = humanFormat->isChecked();

    emit apply();
}

// re-set the values upon dialog start
// (user pressed "cancel")
void
PrecisionWidget::resetValues()
{
    setPrecision( v1, FORMAT_TREES );
    setPrecision( v2, FORMAT_DEFAULT );
    setUpperExpNr( v3, FORMAT_TREES );
    setUpperExpNr( v4, FORMAT_DEFAULT );
    setRoundNr( v5, FORMAT_TREES );
    setRoundNr( v6, FORMAT_DEFAULT );
    humanReadable = humanFormat->isChecked();

    emit apply();

    reject();
}


QPair<QString, QString>
PrecisionWidget::numberToQStringAndUnit( double                   value,
                                         const QString&           unit,
                                         bool                     integerType,
                                         PrecisionFormat          format,
                                         const cubegui::TreeItem* item )
{
    assert( precision[ format ] > 0 );

    int precisionV = precision[ format ];

    QString prefix = "";
    if ( humanReadable && ( item == 0 || item->getTree()->getValueModus() == ABSOLUTE_VALUES ) )
    {
        if ( QString::compare( unit, tr( "bytes/s" ) ) == 0 )
        {
            QStringList prefixes = QStringList() << tr( "KB/s" ) << tr( "MB/s" ) << tr( "GB/s" ) << tr( "TB/s" ) << tr( "PB/s" ) << tr( "EB/s" ) << tr( "ZB/s" ) << tr( "YB/s" );
            foreach( QString pre, prefixes )
            {
                if ( value > 1024 )
                {
                    value /= 1024;
                    prefix = pre;
                }
            }
        }
        if ( QString::compare( unit, tr( "bytes" ) ) == 0 )
        {
            QStringList prefixes = QStringList() << tr( "KB" ) << tr( "MB" ) << tr( "GB" ) << tr( "TB" ) << tr( "PB" ) << tr( "EB" ) << tr( "ZB" ) << tr( "YB" );
            foreach( QString pre, prefixes )
            {
                if ( value > 1024 )
                {
                    value /= 1024;
                    prefix = pre;
                }
            }
        }
        if ( QString::compare( unit, "occ" ) == 0 || QString::compare( unit, "#" ) == 0 )
        {
            QStringList prefixes = QStringList() << tr( "k" ) << tr( "M" ) << tr( "G" ) << tr( "T" ) << tr( "P" ) << tr( "E" ) << tr( "Z" ) << tr( "Y" ); //prefixes for kilo=1_000, mega=1_000_000, etc
            foreach( QString pre, prefixes )
            {
                if ( value > 1000 )
                {
                    value /= 1000;
                    prefix = pre;
                }
            }
        }
        if ( QString::compare( unit, "sec" ) == 0  )
        {
            QStringList prefixes = QStringList() << tr( "msec" ) << tr( "mksec" ) << tr( "nsec" ) << tr( "psec" ) << tr( "fsec" ) << tr( "asec" ) << tr( "zsec" ) << tr( "ysec" ); //abbreviations for millisecond, microsend, etc
            foreach( QString pre, prefixes )
            {
                if ( value >  0. && value < 1e-1  )
                {
                    value *= 1000;
                    prefix = pre;
                }
            }
            //integerType = false; // todo : show exact value?
        }
    }

    // show value in scientific notation with negative exponent
    bool scientificSmall = ( format == FORMAT_DOUBLE ) && ( value < 0.1 ) && ( value > roundThreshold[ format ] );

    QString text;
    if ( scientificSmall || value >= upperExpThreshold[ format ] )
    {
        text = QString::number( value, 'e', precisionV );
        // 10e+09 = > 10e9
        int plus = text.indexOf( '+' );
        if ( plus > 0 )
        {
            int idx  = text.lastIndexOf( '0', -2 );
            int zero = std::max( idx - plus + 1, 1 );
            text.remove( plus, zero );
        }
    }
    else
    {
        if ( integerType )
        {
            precisionV = 0;
        }
        text = QString::number( value, 'f', precisionV );
    }

    QPair<QString, QString> pair = qMakePair( text, prefix );
    return pair;
}

/** converts the number using the given precision format
 * @param value the number to convert
 * @param integerType the type of the number is integer if "integerType" is true, otherwise the type is double
 * @param format FORMAT_TREES or FORMAT_DEFAULT
 */
QString
PrecisionWidget::numberToQString( double value, bool integerType, PrecisionFormat format, const TreeItem* item )
{
    QString prefix = "";
    QString unit   = "";
    if ( item )
    {
        if ( humanReadable && item && item->getTree()->getValueModus() == ABSOLUTE_VALUES )
        {
            cube::Metric* metric;
            if ( item->getDisplayType() == METRIC ) // metric tree -> get unit of the tree item
            {
                metric = static_cast<cube::Metric*> ( item->getCubeObject() );
            }
            else // other trees -> get unit of the selected metric tree item
            {
                Tree* tree = Globals::getTabManager()->getActiveTree( METRIC );
                metric = static_cast<cube::Metric*> ( tree->getSelectionList().first()->getCubeObject() );
            }
            unit = QString( metric->get_uom().c_str() );
        }
    }

    QPair<QString, QString> pair = numberToQStringAndUnit( value, unit, integerType, format );
    QString                 text = pair.first;

    if ( humanReadable )
    {
        text += " " + pair.second;
    }

    return text;
}

/** converts the number using the given precision format; in scientific format, the same exponent as
 * the reference value is used
 * @param value the number to convert
 * @param referenceValue the number that is used to determine the format
 * @param integerType the type of the number is integer if "integerType" is true, otherwise the type is double
 * @param format FORMAT_TREES or FORMAT_DEFAULT
 * @param scientificSmallValue use scientific notations for values below 0.1
 */
QString
PrecisionWidget::numberToQString( double value, double referenceValue, bool scientificSmallValue,
                                  bool integerType, PrecisionFormat format )
{
    assert( precision[ format ] > 0 );

    int  precisionV = precision[ format ];
    bool scientific = ( referenceValue >= upperExpThreshold[ format ] ) ||
                      ( ( scientificSmallValue || ( format == FORMAT_DOUBLE ) ) && referenceValue <= .1 );

    QString text;
    if ( scientific )
    {
        double exp = ( int )log10( referenceValue );
        if ( exp <= -roundNr[ format ] ) // treat as zero
        {
            value = 0.;
            exp   = 0;
        }
        else if ( exp <= 0 )
        {
            exp--;
        }

        value /= pow( 10.0, exp );
        text   = QString::number( value, 'f', precisionV );
        text  += "e" + QString::number( exp );
    }
    else
    {
        if ( integerType )
        {
            precisionV = 0;
        }
        text = QString::number( value, 'f', precisionV );
    }

    return text;
}
