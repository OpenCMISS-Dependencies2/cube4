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

#include "AdvancedColorMapsSettings.h"
#include "DefaultColorMap.h"

using namespace cubepluginapi;

const QString AdvancedColorMapsSettings::LEFT_PANEL_SELECTION_TITLE =
    tr( "Color map selection" );
const QString AdvancedColorMapsSettings::LEFT_PANEL_COMMON_TITLE =
    tr( "Common settings" );
const QString AdvancedColorMapsSettings::RIGHT_PANEL_TITLE =
    tr( "Color map configuration" );

const string AdvancedColorMapsSettings::GROUPBOX_BORDER_CONFIGURATION =
    "{ border: 1px solid gray; border-radius: 9px; margin-top: 0.5em;}";
const string AdvancedColorMapsSettings::GROUPBOX_TITLE_POSITION =
    "::title { subcontrol-origin: margin; left: 10px; padding: 0 3px 0 3px;}";
const string AdvancedColorMapsSettings::GROUPBOX_NAMES[] = {
    "MapSelection", "CommonSettings", "MapConfiguration"
};

const int AdvancedColorMapsSettings::PLOT_FRAME_WIDTH = 1;

AdvancedColorMapsSettings::AdvancedColorMapsSettings() :
    mapNames( factory.mapNames )
{
    initComponents();
    setConnections();
}

AdvancedColorMapsSettings::~AdvancedColorMapsSettings()
{
}

void
AdvancedColorMapsSettings::initComponents()
{
    /**
     * Main GUI
     */
    // leftPanel.setLayout(&leftPanel_Layout);
    // mainLayout.addWidget(&leftPanel);

    /**
     * Left panel
     */
    leftPanel_Selection.setTitle( LEFT_PANEL_SELECTION_TITLE );
    leftPanel_Selection.setObjectName( QString::fromStdString( GROUPBOX_NAMES[ 0 ] ) );
    leftPanel_Selection.setLayout( &leftPanel_LayoutSelection );
    // populate
    for ( ColorMapsFactory::ColorMapsNames::const_iterator it = mapNames.begin(); it != mapNames.end(); ++it )
    {
        leftPanel_colorMapSelection.addItem( QString::fromStdString( ( *it ).second ), ( *it ).first );
    }

    leftPanel_colorMapDescription.setWordWrap( true );
    leftPanel_LayoutSelection.addWidget( &leftPanel_colorMapSelection );
    leftPanel_LayoutSelection.addWidget( &leftPanel_colorMapDescription );

    /**
     * Buttons
     */
    /*leftPanel_okButton.setText("OK");
       leftPanel_cancelButton.setText("Cancel");
       leftPanel_applyButton.setText("Apply");
     */

    buttonBox.addButton( QDialogButtonBox::Ok );
    buttonBox_applyButton = buttonBox.addButton( QDialogButtonBox::Apply );
    buttonBox.addButton( QDialogButtonBox::Cancel );

    /*leftPanel_buttonsLayout.addWidget(&leftPanel_okButton);
       leftPanel_buttonsLayout.addWidget(&leftPanel_cancelButton);
       leftPanel_buttonsLayout.addWidget(&leftPanel_applyButton);
       leftPanel_buttonsPanel.setLayout(&leftPanel_buttonsLayout);
     */

    /**
     * Right panel.
     */
    rightPanel_colorMapEdit = NULL;
    rightPanel_groupBox.setTitle( tr( "Color map configuration" ) );
    rightPanel_groupBox.setObjectName( QString::fromStdString( GROUPBOX_NAMES[ 2 ] ) );
    rightPanel_groupBox.setLayout( &rightPanel_groupBoxLayout );

    /**
     * Configure border and position of title
     * See: http://stackoverflow.com/questions/14582591/border-of-qgroupbox
     */
    string styleSheet = "";
    for ( int i = 0; i < 3; ++i )
    {
        styleSheet += "QGroupBox#" + GROUPBOX_NAMES[ i ] + GROUPBOX_BORDER_CONFIGURATION;
        styleSheet += "QGroupBox#" + GROUPBOX_NAMES[ i ] + GROUPBOX_TITLE_POSITION;
    }
    this->setStyleSheet( QString::fromStdString( styleSheet ) );

    this->setLayout( &mainLayout );
    mainLayout.addWidget( &leftPanel_Selection );
    mainLayout.addWidget( &rightPanel_groupBox );
    // mainLayout.addWidget(&leftPanel_buttonsPanel);
    mainLayout.addWidget( &buttonBox );

    // setMinimumWidth( 400 );
    // setMaximumWidth( 400 );
    // setFixedWidth( 400 );
    this->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Expanding );

    /**
     * Initialize internal data structures
     */
    colorMapChanged( ColorMapsFactory::SEQUENTIAL );
    currentColorMap = currentlySelectedColorMap;
}

void
AdvancedColorMapsSettings::setConnections()
{
    /*
       QObject::connect(&leftPanel_okButton, SIGNAL(clicked()), this, SLOT(handleOKButton()));
       QObject::connect(&leftPanel_cancelButton, SIGNAL(clicked()), this, SLOT(handleCancelButton()));
       QObject::connect(&leftPanel_applyButton, SIGNAL(clicked()), this, SLOT(handleApplyButton()));
     */

    QObject::connect( &buttonBox,  SIGNAL( accepted() ), this, SLOT( handleOKButton() ) );
    QObject::connect( &buttonBox, SIGNAL( clicked( QAbstractButton* ) ),  this,
                      SLOT( handleApplyButton( QAbstractButton* ) ) );
    QObject::connect( &buttonBox,  SIGNAL( rejected() ), this, SLOT( handleCancelButton() ) );
    QObject::connect( &leftPanel_colorMapSelection, SIGNAL( currentIndexChanged( int ) ), this, SLOT( colorMapChanged( int ) ) );
}

void
AdvancedColorMapsSettings::updateGUI()
{
    // description
    leftPanel_colorMapDescription.setText( tr( "<b>Description</b>\n" ) +
                                           currentlySelectedColorMap->getColorMapDescription() );
    // edit widget
    rightPanel_colorMapEdit = currentlySelectedColorMap->getConfigurationPanel();
    rightPanel_colorMapEdit->setParent( &rightPanel_groupBox );
    if ( rightPanel_widgets.find( rightPanel_colorMapEdit ) == rightPanel_widgets.end() )
    {
        rightPanel_widgets.insert( rightPanel_colorMapEdit );
        rightPanel_groupBoxLayout.addWidget( rightPanel_colorMapEdit );
    }

    rightPanel_groupBoxLayout.setCurrentWidget( rightPanel_colorMapEdit );


    rightPanel_groupBox.updateGeometry();
    rightPanel_groupBox.adjustSize();
    rightPanel_groupBox.setMinimumHeight( rightPanel_groupBoxLayout.sizeHint().height() );
    rightPanel_groupBox.setMinimumWidth( rightPanel_groupBoxLayout.sizeHint().width() );

    leftPanel_Selection.setMinimumHeight( leftPanel_LayoutSelection.sizeHint().height() );

    updateGeometry();
    adjustSize();

    this->setMinimumHeight( mainLayout.sizeHint().height() );
    this->setMinimumWidth( mainLayout.sizeHint().width() );
}

ColorMap*
AdvancedColorMapsSettings::getCurrentColorMap()
{
    return currentColorMap;
}

/**
 * Slots
 */

void
AdvancedColorMapsSettings::handleOKButton()
{
    // user may have modified every map
    for ( ColorMapsFactory::ColorMapsNames::const_iterator it = mapNames.begin(); it != mapNames.end(); ++it )
    {
        ColorMapExtended* map = factory.getColorMap( ( *it ).first );
        map->getConfigurationPanel()->applyChanges();
    }
    if ( currentColorMap != currentlySelectedColorMap )
    {
        currentColorMap = currentlySelectedColorMap;
        emit changeColorMap( currentColorMap );
    }
    emit colorMapChanged();
    this->close();
}

void
AdvancedColorMapsSettings::handleCancelButton()
{
    // user may have modified every map
    for ( ColorMapsFactory::ColorMapsNames::const_iterator it = mapNames.begin(); it != mapNames.end(); ++it )
    {
        ColorMapExtended* map = factory.getColorMap( ( *it ).first );
        map->getConfigurationPanel()->revertChanges();
    }
    this->close();
}

void
AdvancedColorMapsSettings::handleApplyButton( QAbstractButton* button )
{
    /**
     * Works only for apply()
     */
    if ( button->text().compare( buttonBox_applyButton->text() ) )
    {
        return;
    }
    // user may have modified every map
    for ( ColorMapsFactory::ColorMapsNames::const_iterator it = mapNames.begin(); it != mapNames.end(); ++it )
    {
        ColorMapExtended* map = factory.getColorMap( ( *it ).first );
        map->getConfigurationPanel()->applyChanges();
    }
    if ( currentColorMap != currentlySelectedColorMap )
    {
        currentColorMap = currentlySelectedColorMap;
        emit changeColorMap( currentColorMap );
    }
    emit colorMapChanged();
}

void
AdvancedColorMapsSettings::colorMapChanged( int index )
{
    // inserted enum was converted to int, so we can make safe cast
    currentlySelectedColorMap = factory.getColorMap(
        static_cast<ColorMapsFactory::ColorMaps>( leftPanel_colorMapSelection.itemData( index ).toInt() )
        );
    updateGUI();
}

void
AdvancedColorMapsSettings::colorMapUpdated()
{
    rightPanel_colorMapEdit->update();
}

void
AdvancedColorMapsSettings::colorExcludedValuesChanged( const QColor& color )
{
    ColorMapExtended::setColorForValuesOutOfRange( color );
    colorMapUpdated();
}

/**
 * Implementation of SettingsHandler interface.
 */

/**
 * Nothing is stored in experiment settings!
 */
void
AdvancedColorMapsSettings::loadExperimentSettings( QSettings& )
{
}

void
AdvancedColorMapsSettings::saveExperimentSettings( QSettings& )
{
}

void
AdvancedColorMapsSettings::loadGlobalSettings( QSettings& settings )
{
    int colorMap = settings.value( QString( "Selected_Color_Map" ), 0 ).toInt();
    // incorrect value
    if ( colorMap < 0 || colorMap >= ColorMapsFactory::COLORMAPS_END )
    {
        colorMap = 0;
    }

    // static fields!:

    // color out-of-range
    ColorMapExtended::setColorForValuesOutOfRange(
        settings.value( "colorValuesOutOfRange", ColorMapExtended::DEFAULT_COLOR_VALUES_OUT_OF_RANGE ).value<QColor>() );
    // filtering values
    ColorMapPlot::loadSettings( settings );

    for ( ColorMapsFactory::ColorMapsNames::const_iterator it = mapNames.begin(); it != mapNames.end(); ++it )
    {
        ColorMapExtended* map = factory.getColorMap( ( *it ).first );
        map->loadGlobalSettings( settings );
    }
    // it will send a signal to GUI
    leftPanel_colorMapSelection.setCurrentIndex( colorMap );
    currentColorMap = currentlySelectedColorMap;
    emit changeColorMap( currentColorMap );
}

void
AdvancedColorMapsSettings::saveGlobalSettings( QSettings& settings )
{
    // static fields!:

    // color out-of-range

    settings.setValue( "colorValuesOutOfRange", ColorMapExtended::getColorForValuesOutOfRange() );
    // filtering values
    ColorMapPlot::saveSettings( settings );

    for ( ColorMapsFactory::ColorMapsNames::const_iterator it = mapNames.begin(); it != mapNames.end(); ++it )
    {
        ColorMapExtended* map = factory.getColorMap( ( *it ).first );
        map->saveGlobalSettings( settings );
    }
    settings.setValue( QString( "Selected_Color_Map" ), factory.getColorMapIndex( currentColorMap ) );
}

QString
AdvancedColorMapsSettings::settingName()
{
    return "AdvancedColorMaps Plugin";
}
