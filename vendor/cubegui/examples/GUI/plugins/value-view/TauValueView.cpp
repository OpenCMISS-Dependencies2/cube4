/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2017                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include <QVBoxLayout>
#include <QtPlugin>
#include "TauValueView.h"
#include "PluginServices.h"

/**
 * If the plugin doesn't load, start cube with -verbose to get detailed information.
 * If the error message "Plugin verification data mismatch" is printed, check if the
 * same compiler and Qt version have been used.
 */

using namespace cubepluginapi;
using namespace tauvalueplugin;
using namespace cube;

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2( TauValueViewPlugin, TauValueViewPlugin ); // ( PluginName, ClassName )
#endif

bool
TauValueViewPlugin::cubeOpened( PluginServices* service )
{
    this->service = service;
    view          = new TauValueView();
    service->addValueView( view );
    service->addSettingsHandler( view );
    return true;
}

void
TauValueViewPlugin::cubeClosed()
{
    delete view;
}

/** set a version number, the plugin with the highest version number will be loaded */
void
TauValueViewPlugin::version( int& major, int& minor, int& bugfix ) const
{
    major  = 1;
    minor  = 0;
    bugfix = 0;
}

/** unique plugin name */
QString
TauValueViewPlugin::name() const
{
    return "Tau ValueView Example";
}

QString
TauValueViewPlugin::getHelpText() const
{
    return "Tau Value View Plugin shows a boxplot with minimum, average and maximum value.";
}

//======================================================================================================
//======================================================================================================
TauValueView::TauValueView() : configWidget( 0 )
{
}

bool
TauValueView::isAvailable( DataType type ) const
{
    return type == CUBE_DATA_TYPE_TAU_ATOMIC;
}

QString
TauValueView::getName() const
{
    return "Tau Value View";
}

double
TauValueView::getDoubleValue( Value* value ) const
{
    return value->getDouble();
}

QString
TauValueView::toString( const TreeItem* item ) const
{
    return item->getValueObject()->getString().c_str();
}

QSize
TauValueView::getIconSize( const TreeItem* ) const
{
    return iconSize;
}

QPixmap
TauValueView::getIcon( const TreeItem* item, bool grayOut ) const
{
    QPixmap pix( getIconSize( item ) );
    QColor  borderColor = Qt::black;
    pix.fill( Qt::white );
    int      w = pix.width();
    int      h = pix.height();
    QPainter painter( &pix );
    painter.setPen( Qt::gray );
    painter.drawRect( 0, 0, w - 1, h - 1 ); // border

    if ( grayOut )
    {
    }

    TauAtomicValue*       tau  = dynamic_cast<TauAtomicValue*> ( item->getValueObject() );
    TauAtomicValueConfig& conf = tau->getConfig();

    if ( tau->getN().getUnsignedInt() > 0 )
    {
        double min = tau->getMinValue().getDouble();
        double max = tau->getMaxValue().getDouble();
        conf.pivot = TAU_AVG;
        double avg = tau->getDouble();
        conf.pivot = TAU_DEV;
        double dev = tau->getDouble();

        double factor = w / ( max - min );
        min  = 0;
        max  = w;
        avg  = ( avg - min ) * factor;
        dev *= factor;

        painter.setPen( borderColor );

        int pad = h / 4;
        painter.drawRect( avg - dev, h / 2 - pad, 2 * dev, 2 * pad );
        painter.drawLine( avg, h / 2 - pad, avg, h / 2 + pad );
        painter.drawLine( 1, h / 2, avg - dev, h / 2 );
        painter.drawLine( avg + dev, h / 2, w - 1, h / 2 );
    }
    return pix;
}

QWidget*
TauValueView::getConfigWidget( DataType type )
{
    iconSizeOld = iconSize;

    configWidget = 0;

    if ( type == CUBE_DATA_TYPE_TAU_ATOMIC )
    {
        configWidget = new QWidget();
        QLabel* label = new QLabel( tr( "Set icon size" ) );
        QLabel* wl    = new QLabel( tr( "width:" ) );
        QLabel* hl    = new QLabel( tr( "height:" ) );
        wbox = new QSpinBox();
        wbox->setMinimum( 5 );
        wbox->setMaximum( 100 );
        wbox->setValue( iconSize.width() );
        hbox = new QSpinBox();
        hbox->setMinimum( 5 );
        hbox->setMaximum( 100 );
        hbox->setValue( iconSize.height() );

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

/* This method is called, if this plugin has been selected for the datatype CUBE_DATA_TYPE_TAU_ATOMIC and the config dialog
 * has been opened and the Apply- or Ok-button has been pressed */
void
TauValueView::applyConfig()
{
    if ( configWidget )
    {
        iconSize.setHeight( hbox->value() );
        iconSize.setWidth( wbox->value() );
    }
}

/* This method is called, if this plugin has been selected for the datatype CUBE_DATA_TYPE_TAU_ATOMIC and the config dialog
 * has been opened and the Cancel-button has been pressed */
void
TauValueView::cancelConfig()
{
    iconSize = iconSizeOld;
}

void
TauValueView::loadGlobalSettings( QSettings& settings )
{
    iconSize = settings.value( "iconSize", QSize( 30, 20 ) ).toSize();
}

void
TauValueView::saveGlobalSettings( QSettings& settings )
{
    settings.setValue( "iconSize", iconSize );
}
