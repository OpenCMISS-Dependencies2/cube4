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


#include <config.h>
#include <QPainter>
#include <QDebug>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include <QFormLayout>

#include "CubeTypes.h"
#include "CubeValues.h"
#include "CubeTauAtomicValue.h"
#include "Globals.h"
#include "DefaultValueView.h"
#include "TreeItem.h"
#include "TabManager.h"

using namespace cube;
using namespace cubegui;
using namespace cubepluginapi;

const static QString TAU_STRING            = "VALUE:TAU_DEV";
const static int     VALUE_VIEW_ICONHEIGHT = 10;

DefaultValueView::DefaultValueView()
{
    tauVarString = "VALUE:TAU_DEV";                                                             //TAU_STRING;
    tauIconParts << TAU_MIN << TAU_AVG << TAU_MAX;
    tauValueOptions << TAU_MAX << TAU_AVG << TAU_DEV << TAU_N << TAU_MIN << TAU_MAX << TAU_SUM; // << TAU_SUM2
    configType       = CUBE_DATA_TYPE_NONE;
    tauValueIndex    = 0;
    tauValueIndexOld = tauValueIndex;
    histogramSize    = QSize( VALUE_VIEW_ICONHEIGHT, VALUE_VIEW_ICONHEIGHT );
    histogramSizeOld = histogramSize;
}

bool
DefaultValueView::isAvailable( DataType ) const
{
    return true; // default view handles all types
}

double
DefaultValueView::getDoubleValue( Value* value ) const
{
    switch ( value->myDataType() )
    {
        case CUBE_DATA_TYPE_TAU_ATOMIC:
        {
            TauAtomicValue*       tau  = dynamic_cast<TauAtomicValue*> ( value );
            TauAtomicValueConfig& conf = tau->getConfig();
            conf.pivot = tauValueOptions[ tauValueIndex ];
            return value->getDouble();
        }
        default:
            return value->getDouble();
    }
}

QString
DefaultValueView::toString( const TreeItem* item ) const
{
    switch ( item->getValueObject()->myDataType() )
    {
        case CUBE_DATA_TYPE_TAU_ATOMIC:
            return getTauString( item );
        default:
            return item->getValueObject()->getString().c_str();
    }
}

QSize
DefaultValueView::getIconSize( const TreeItem* item ) const
{
    QSize iconSize;
    QFont font       = Globals::getTabManager()->property( "font" ).value<QFont>();
    int   iconHeight = font.pointSize() + 2; // = fontheight + border

    if ( !item || !item->getValueObject() )
    {
        return QSize( iconHeight, iconHeight );
    }
    switch ( item->getValueObject()->myDataType() )
    {
        case CUBE_DATA_TYPE_TAU_ATOMIC:
            iconSize = tauValueIndex == 0 ? QSize( 3 * iconHeight, iconHeight ) : QSize( iconHeight, iconHeight );
            break;
        case CUBE_DATA_TYPE_HISTOGRAM:
        {
            int             width = histogramSize.width();
            HistogramValue* hist  = dynamic_cast<HistogramValue*> ( item->getValueObject() );
            if ( hist && hist->get_number_of_values() != 0 )
            {
                int n = hist->get_number_of_values();
                width = ( ( width - 1 ) / n + 1 ) * n; // enlarge width to fit multiples of value count
            }
            iconSize = QSize( width, histogramSize.height() );
        }
        break;
        default:
            iconSize = QSize( iconHeight, iconHeight );
            break;
    }
    return iconSize;
}

QPixmap
DefaultValueView::getIcon( const TreeItem* item, bool grayOut ) const
{
    if ( !item->getValueObject() )
    {
        return getColorIcon( item, grayOut );
    }
    switch ( item->getValueObject()->myDataType() )
    {
        case CUBE_DATA_TYPE_TAU_ATOMIC:
            return tauValueIndex == 0 ? getTauIcon( item, grayOut ) : getColorIcon( item, grayOut );
        case CUBE_DATA_TYPE_HISTOGRAM:
            return getHistogramIcon( item, grayOut );
        default:
            return getColorIcon( item, grayOut );
    }
}

QPixmap
DefaultValueView::getColorIcon( const TreeItem* item, bool grayOut ) const
{
    QPixmap pix( getIconSize( item ) );
    int     iconHeight  = pix.height();
    int     iconWidth   = pix.width();
    QColor  fillColor   = item->getColor();
    QColor  borderColor = Qt::black;
    if ( grayOut )
    {
        borderColor = Qt::gray;
        fillColor.setAlpha( fillColor.alpha() / 8 );
    }
    pix.fill( fillColor );

    QPainter painter( &pix );
    painter.setPen( borderColor );
    painter.drawRect( 0, 0, iconWidth - 1, iconHeight - 1 );

    double prec = Globals::getRoundThreshold( FORMAT_TREES );
    if ( item->getValue() >= prec )
    {
        // for positive values draw a raised color icon
        painter.setPen( borderColor );
        painter.drawLine( iconWidth - 2, 1, iconWidth - 2, iconHeight - 2 );
        painter.drawLine( 1, iconHeight - 2, iconWidth - 2, iconHeight - 2 );

        painter.setPen( Qt::white );
        painter.drawLine( 1, 1, 1, iconHeight - 2 );
        painter.drawLine( 1, 1, iconWidth - 2, 1 );
    }
    else if ( item->getValue() < -prec )
    {
        // for negative values draw a deepened color icon
        painter.setPen( borderColor );
        painter.drawLine( 1, 1, 1, iconHeight - 2 );
        painter.drawLine( 1, 1, iconWidth - 2, 1 );

        painter.setPen( Qt::white );
        painter.drawLine( 1, iconHeight - 2, iconWidth - 2, iconHeight - 2 );
        painter.drawLine( iconWidth - 2, 1, iconWidth - 2, iconHeight - 2 );
    }
    return pix;
}

/**
 * @brief DefaultValueView::getTauIcon displays min, max and average
 */
QPixmap
DefaultValueView::getTauIcon( const TreeItem* item, bool grayOut ) const
{
    QPixmap pix( getIconSize( item ) );
    QColor  borderColor = Qt::black;

    QList<double>   values;
    TauAtomicValue* tau = dynamic_cast<TauAtomicValue*> ( item->getValueObject() );
    if ( !tau || tau->getN().getSignedInt() == 0 )
    {
        values << 0 << 0 << 0;
    }
    else
    {
        TauAtomicValueConfig& conf = tau->getConfig();
        foreach( TauAtomicValuePivot type, tauIconParts )
        {
            conf.pivot = type;
            values << tau->getDouble();
        }
    }

    int iconHeight = pix.height();
    int iconWidth  = pix.width() / values.size();

    QPainter painter( &pix );
    foreach( double value, values )
    {
        QColor fillColor = item->calculateColor( value );
        if ( grayOut )
        {
            borderColor = Qt::gray;
            fillColor.setAlpha( fillColor.alpha() / 8 );
        }

        painter.setPen( borderColor );
        painter.fillRect( 0, 0, iconWidth, iconHeight, fillColor );
        painter.drawRect( 0, 0, iconWidth - 1, iconHeight - 1 );

        double prec = Globals::getRoundThreshold( FORMAT_TREES );
        if ( item->getValue() >= prec )
        {
            // for positive values draw a raised color icon
            painter.setPen( borderColor );
            painter.drawLine( iconWidth - 2, 1, iconWidth - 2, iconHeight - 2 );
            painter.drawLine( 1, iconHeight - 2, iconWidth - 2, iconHeight - 2 );

            painter.setPen( Qt::white );
            painter.drawLine( 1, 1, 1, iconHeight - 2 );
            painter.drawLine( 1, 1, iconWidth - 2, 1 );
        }
        else if ( item->getValue() < -prec )
        {
            // for negative values draw a deepened color icon
            painter.setPen( borderColor );
            painter.drawLine( 1, 1, 1, iconHeight - 2 );
            painter.drawLine( 1, 1, iconWidth - 2, 1 );

            painter.setPen( Qt::white );
            painter.drawLine( 1, iconHeight - 2, iconWidth - 2, iconHeight - 2 );
            painter.drawLine( iconWidth - 2, 1, iconWidth - 2, iconHeight - 2 );
        }
        painter.translate( iconWidth, 0 );
    }
    return pix;
}

/**
 * @brief DefaultValueView::getHistogramIcon displays min, max and average
 */
QPixmap
DefaultValueView::getHistogramIcon( const TreeItem* item, bool ) const
{
    QPixmap pix( getIconSize( item ) );
    pix.fill( Qt::white );

    HistogramValue* hist = dynamic_cast<HistogramValue*> ( item->getValueObject() );
    if ( !hist || ( hist->get_number_of_values() == 0 ) || ( hist->get_max() == -DBL_MAX ) )
    {
        QPainter painter( &pix );
        painter.drawRect( 0, 0, pix.width() - 1, pix.height() - 1 );
        return pix;
    }

    int           iconHeight = pix.height();
    int           iconWidth  = pix.width();
    const double* values     = hist->get_values();

    double max = 0;
    for ( uint i = 0; i < hist->get_number_of_values(); i++ )
    {
        if ( max < values[ i ] )
        {
            max = values[ i ];
        }
    }

    double   factor = iconHeight / max;
    double   w      = iconWidth * 1. / hist->get_number_of_values();
    QPainter painter( &pix );
    for ( uint i = 0; i < hist->get_number_of_values(); i++ )
    {
        int h = ( int )( values[ i ] * factor + .5 );
        painter.fillRect( i * w, iconHeight - h, w, h, Qt::black );
    }

    return pix;
}

QString
DefaultValueView::getTauString( const TreeItem* item ) const
{
    TauAtomicValue*       tau       = dynamic_cast<TauAtomicValue*> ( item->getValueObject() );
    TauAtomicValueConfig& conf      = tau->getConfig();
    QChar                 plusMinus = QChar( 177 ); // +-
    QString               currentValue;

    QString valueString = tauVarString;
    if ( valueString.contains( "VALUE" ) )
    {
        conf.pivot = tauValueOptions[ tauValueIndex ];
        if ( tauValueIndex == 0 )
        {
            cube::Value* value = item->getValueObject();
            conf.pivot    = TAU_MIN;
            currentValue += Globals::formatNumber( value->getDouble() ) + ", ";
            conf.pivot    = TAU_AVG;
            currentValue += Globals::formatNumber( value->getDouble() ) + ", ";
            conf.pivot    = TAU_MAX;
            currentValue += Globals::formatNumber( value->getDouble() );
        }
        else
        {
            currentValue = Globals::formatNumber( tau->getDouble() );
        }
        valueString.replace( "VALUE", currentValue );
    }

    QStringList tauStringOptions;
    tauStringOptions << "TAU_MAX" << "TAU_AVG" << "TAU_DEV" << "TAU_N" << "TAU_MIN" << "TAU_MAX" << "TAU_SUM"; // << TAU_SUM2
    for ( int i = 0; i < tauStringOptions.size(); i++ )
    {
        if ( valueString.contains( tauStringOptions.at( i ) ) )
        {
            conf.pivot    = tauValueOptions.at( i );
            currentValue  = ( tauValueOptions.at( i ) == TAU_DEV ) ? QString( plusMinus ) : "";
            currentValue += Globals::formatNumber( tau->getDouble() );
            valueString.replace( tauStringOptions.at( i ), currentValue );
        }
    }

    return valueString;
}


QString
DefaultValueView::getExtendedInfo( const TreeItem* item )
{
    if ( item->getValueObject()->myDataType() != CUBE_DATA_TYPE_TAU_ATOMIC || !item->getValueObject() )
    {
        return "";
    }

    QList<TauAtomicValuePivot> pivot;
    pivot << TAU_N << TAU_MIN << TAU_MAX << TAU_AVG << TAU_DEV << TAU_SUM << TAU_SUM2;
    QList<QString> labelList;
    labelList << tr( "count" ) << tr( "min" ) << tr( "max" ) << tr( "average" ) << tr( "standard deviation" ) << tr( "sum" ) << tr( "sum2" );
    QString str;

    TauAtomicValue*       tau  = dynamic_cast<TauAtomicValue*> ( item->getValueObject() );
    TauAtomicValueConfig& conf = tau->getConfig();
    for ( int i = 0; i < pivot.size(); i++ )
    {
        conf.pivot = pivot.at( i );
        cube::Value* value = item->getValueObject();
        if ( value )
        {
            bool isInteger = conf.pivot == TAU_N;
            str += labelList.at( i ) + " : " + Globals::formatNumber( value->getDouble(), isInteger ) + "\n";
        }
    }

    return str;
}

void
DefaultValueView::saveGlobalSettings( QSettings& settings )
{
    settings.setValue( "histogramSize", histogramSize );
    settings.setValue( "tauIndex", tauValueIndex );
    settings.setValue( "tauVarString", tauVarString );
}

void
DefaultValueView::loadGlobalSettings( QSettings& settings )
{
    histogramSize = settings.value( "histogramSize", QSize( VALUE_VIEW_ICONHEIGHT, VALUE_VIEW_ICONHEIGHT ) ).toSize();
    tauValueIndex = settings.value( "tauIndex", 0 ).toInt();
    tauVarString  = settings.value( "tauVarString", TAU_STRING ).toString();
}

QWidget*
DefaultValueView::getConfigWidget( DataType type )
{
    tauValueIndexOld = tauValueIndex;
    histogramSizeOld = histogramSize;

    configWidget = 0;
    configType   = type;

    if ( type == CUBE_DATA_TYPE_TAU_ATOMIC )
    {
        configWidget = new QWidget();

        QList<QString> labelList;
        labelList << tr( "min,average,max" ) << tr( "average" ) << tr( "standard deviation" ) << tr( "count" ) << tr( "min" ) << tr( "max" ) << tr( "sum" );
        tauValueBox = new QComboBox();
        foreach( QString str, labelList )
        {
            tauValueBox->addItem( str );
        }
        tauValueBox->setCurrentIndex( this->tauValueIndex );

        QVBoxLayout* layout = new QVBoxLayout();
        configWidget->setLayout( layout );
        layout->addWidget(  new QLabel( tr( "Tau icon shows" ) ) );
        layout->addWidget( tauValueBox );

        QString doc = tr( "The following terms will be replaced by the corresponding values:\n\
        VALUE\t the icon value\n\
        TAU_N\t the number of values\n\
        TAU_MIN\t the minimum value\n\
        TAU_MAX\t the maximum value\n\
        TAU_AVG\t the average of all values\n\
        TAU_DEV\t the standard deviation\n\
        TAU_SUM\t the sum of all values" );
        QLabel* label = new QLabel( tr( "Tau value string" ) );
        label->setToolTip( doc );
        layout->addWidget( label );
        tauInput = new QLineEdit();
        tauInput->setToolTip( doc );
        layout->addWidget( tauInput );
        tauInput->setText( tauVarString );
    }
    else if ( type == CUBE_DATA_TYPE_HISTOGRAM )
    {
        configWidget = new QWidget();

        QLabel* label = new QLabel( tr( "Set Histogram icon size" ) );
        QLabel* wl    = new QLabel( tr( "width:" ) );
        QLabel* hl    = new QLabel( tr( "height:" ) );
        wbox = new QSpinBox();
        wbox->setMinimum( 5 );
        wbox->setMaximum( 100 );
        wbox->setValue( histogramSize.width() );
        hbox = new QSpinBox();
        hbox->setMinimum( 5 );
        hbox->setMaximum( 100 );
        hbox->setValue( histogramSize.height() );

        QWidget*     form       = new QWidget();
        QFormLayout* formLayout = new QFormLayout( form );
        formLayout->setContentsMargins( 0, 0, 0, 0 );

        formLayout->setWidget( 0, QFormLayout::LabelRole, wl );
        formLayout->setWidget( 0, QFormLayout::FieldRole, wbox );
        formLayout->setWidget( 1, QFormLayout::LabelRole, hl );
        formLayout->setWidget( 1, QFormLayout::FieldRole, hbox );

        QVBoxLayout* layout = new QVBoxLayout();
        configWidget->setLayout( layout );
        layout->addWidget( label );
        layout->addWidget( form );
    }
    return configWidget;
}

void
DefaultValueView::cancelConfig()
{
    tauValueIndex = tauValueIndexOld;
    histogramSize = histogramSizeOld;
    tauVarString  = tauVarStringOld;
}

void
DefaultValueView::applyConfig()
{
    if ( configWidget )
    {
        if ( configType == CUBE_DATA_TYPE_TAU_ATOMIC )
        {
            tauValueIndex = tauValueBox->currentIndex();
            tauVarString  = tauInput->text();
        }
        else if ( configType == CUBE_DATA_TYPE_HISTOGRAM )
        {
            histogramSize.setHeight( hbox->value() );
            histogramSize.setWidth( wbox->value() );
        }
    }
}
