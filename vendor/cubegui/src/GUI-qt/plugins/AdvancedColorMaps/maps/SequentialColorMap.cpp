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

#include <cmath>
#include <cassert>
#include <iterator>
#include <set>

#include <QMessageBox>
#include <QInputDialog>

#include "SequentialColorMap.h"

using std::set;
using std::max;
using std::min;
using std::distance;
using std::make_pair;

const PSMap SequentialColorMap::SEQUENTIAL_PREDEFINED_SCHEMES =
    SequentialColorMap::sequentialInitializePredefinedSchemes();

const QString SequentialColorMap::SEQUENTIAL_COLOR_MAP_NAME =
    tr( "Sequential Color Map" );
const QString SequentialColorMap::SEQUENTIAL_COLOR_MAP_DESCRIPTION =
    tr( "Sequential color maps are configured by two colors - start and end - with an interpolation"
        " method, which fully describe the distribution of colors between minimum and maximum. <br>"
        "Predefined schemes provide exemplary usage of color map, with an interpolation from selected colour"
        " to pure white. One may select any other RGB color for start or end by double click on "
        "the plot. <br>"
        "This kind of color map is usually used for representation of ordered data, with "
        "the colour indicating the localization of value between minimum and maximum values." );

PSMap
SequentialColorMap::sequentialInitializePredefinedSchemes()
{
    PSMap ret;

    /**
     * Black
     */
    QColor   rgb           = QColor::fromRgb( 1, 1, 1 );
    MSHColor startingPoint = MSHColor::fromRGB( rgb );
    MSHColor endingPoint   = MSHColor( 100, 0, 0 );
    ret.insert( std::make_pair( tr( "Greyscale" ).toUtf8().data(), std::make_pair( startingPoint, endingPoint ) ) );

    /**
     * Blue
     */
    rgb           = QColor::fromRgb( 85, 170, 255 );
    startingPoint = MSHColor::fromRGB( rgb );
    endingPoint   = MSHColor( 100, 0, 0 );
    ret.insert( std::make_pair( tr( "Blues" ).toUtf8().data(), std::make_pair( startingPoint, endingPoint ) ) );

    /**
     * Orange
     */
    rgb           = QColor::fromRgb( 255, 85, 0 );
    startingPoint = MSHColor::fromRGB( rgb );
    endingPoint   = MSHColor( 100, 0, 0 );
    ret.insert( std::make_pair( tr( "Oranges" ).toUtf8().data(), std::make_pair( startingPoint, endingPoint ) ) );

    /**
     * Red
     */
    rgb           = QColor::fromRgb( 255, 0, 0 );
    startingPoint = MSHColor::fromRGB( rgb );
    endingPoint   = MSHColor( 100, 0, 0 );
    ret.insert( std::make_pair( tr( "Reds" ).toUtf8().data(), std::make_pair( startingPoint, endingPoint ) ) );

    /**
     * Green
     */
    rgb           = QColor::fromRgb( 0, 68, 26 );
    startingPoint = MSHColor::fromRGB( rgb );
    endingPoint   = MSHColor( 100, 0, 0 );
    ret.insert( std::make_pair( tr( "Greens" ).toUtf8().data(), std::make_pair( startingPoint, endingPoint ) ) );

    /**
     * Purple
     */
    rgb           = QColor::fromRgb( 63, 0, 125 );
    startingPoint = MSHColor::fromRGB( rgb );
    endingPoint   = MSHColor( 100, 0, 0 );
    ret.insert( std::make_pair( tr( "Purples" ).toUtf8().data(), std::make_pair( startingPoint, endingPoint ) ) );

    return ret;
}

SequentialColorMap::SequentialColorMap(
    SequentialColorMapWidget* _widget,
    const PSMap&              predefinedSchemes,
    const QString&            mapName,
    const QString&            mapDescription ) :
    PREDEFINED_SCHEMES( predefinedSchemes ),
    currentScheme( PREDEFINED_SCHEMES.begin()->second ),
    interpolationParameter( 0.5 ),
    interMethod( LINEAR ),
    COLOR_MAP_NAME( mapName ),
    COLOR_MAP_DESCRIPTION( mapDescription )
{
    if ( _widget == NULL )
    {
        widget = new SequentialColorMapWidget( *this );
        getWidget().constructAndInitializeData();
    }
    else
    {
        widget = _widget;
    }
}

SequentialColorMap::~SequentialColorMap()
{
}

const QString&
SequentialColorMap::getColorMapDescription() const
{
    return COLOR_MAP_DESCRIPTION;
}

ColorMapWidget*
SequentialColorMap::getConfigurationPanel()
{
    return widget;
}

QColor
SequentialColorMap::getColor( double value, double minValue,
                              double maxValue, bool whiteForZero ) const
{
    pair<unsigned short, QColor> preprocess = ColorMapExtended::getColorBasicProcessing( value,
                                                                                         minValue, maxValue, whiteForZero );
    // preprocess returns value
    if ( preprocess.first == 2 )
    {
        return preprocess.second;
    }
    double               position;
    pair<double, double> minMax = adjustFilteringBorders( minValue, maxValue );
    minValue = minMax.first;
    maxValue = minMax.second;

    double middleMarkPosition = ( markersPositions->getMiddleMarker( minValue, maxValue ) - minValue ) /
                                ( maxValue - minValue );                                        // change "speed"

    // usual interpolation
    if ( preprocess.first == 0 )
    {
        position = ( value - minValue ) / ( maxValue - minValue );

        if ( invertedCM )
        {
            position = 1.0 - position;
        }

        if ( interMethod == EXPONENTIAL )
        {
            // exponential interpolation
            return MSHColor::interpolateExp( currentScheme.first, currentScheme.second,
                                             position, middleMarkPosition ).toRGB();
        }
        else
        {
            // linear interpolation
            return MSHColor::interpolate( currentScheme.first, currentScheme.second,
                                          position, middleMarkPosition ).toRGB();
        }
    }
    // case when maxValue == minValue
    else
    {
        position = preprocess.second.red() / 255.0;
        return MSHColor::interpolate( currentScheme.first, currentScheme.second, position ).toRGB();
    }
}

QString
SequentialColorMap::getMapName() const
{
    return COLOR_MAP_NAME;
}

void
SequentialColorMap::saveGlobalSettings( QSettings& settings )
{
    ColorMapExtended::saveGlobalSettings( settings );
    getWidget().getCurrentPlot()->saveMiddleMarker( settings, "Sequential" );
    saveUDSchemes( "Sequential", settings );
    saveCurrentColorMap( "Sequential", settings );
}

void
SequentialColorMap::loadGlobalSettings( QSettings& settings )
{
    ColorMapExtended::loadGlobalSettings( settings );
    getWidget().getCurrentPlot()->loadMiddleMarker( settings, "Sequential" );
    loadUDSchemes( "Sequential", settings );
    widget->parentUpdated();
    // current index MUST be loaded after all color maps has been loaded
    loadCurrentColorMap( "Sequential", settings );
}

void
SequentialColorMap::usePredefinedScheme( const std::string& name )
{
    PSMap::const_iterator it = PREDEFINED_SCHEMES.find( name );
    assert( it != PREDEFINED_SCHEMES.end() );
    currentScheme = it->second;
}

void
SequentialColorMap::useUDScheme( const std::string& name )
{
    PSMap::iterator it = userDefinedSchemes.find( name );
    assert( it != userDefinedSchemes.end() );
    currentScheme = it->second;
}

void
SequentialColorMap::useScheme( const MSHColor& first, const MSHColor& second )
{
    currentScheme = std::make_pair( first, second );
}

const ColorMapExtended::MSHColor&
SequentialColorMap::getBeginColor() const
{
    return currentScheme.first;
}

const ColorMapExtended::MSHColor&
SequentialColorMap::getEndColor() const
{
    return currentScheme.second;
}

SequentialColorMap::InterpolationMethod
SequentialColorMap::getInterpolationMethod() const
{
    return interMethod;
}

void
SequentialColorMap::useInterpolationMethod( SequentialColorMap::InterpolationMethod method )
{
    interMethod = method;
}

SequentialColorMapWidget&
SequentialColorMap::getWidget()
{
    return dynamic_cast<SequentialColorMapWidget&>( *widget );
}

const PSMap&
SequentialColorMap::getUDSchemes() const
{
    return userDefinedSchemes;
}

void
SequentialColorMap::addScheme( const string& name )
{
    userDefinedSchemes[ name ] = currentScheme;
}

void
SequentialColorMap::addScheme( const string& name, const ColorScheme& scheme )
{
    userDefinedSchemes[ name ] = scheme;
}

void
SequentialColorMap::removeScheme( const string& name )
{
    userDefinedSchemes.erase( name );
}

void
SequentialColorMap::loadUDSchemes( const string& prefix, QSettings& settings )
{
    // # of color maps
    int count = settings.value( QString( "%1_UDSchemes_Count" ).arg( QString::fromStdString( prefix ) ) ).toInt();

    for ( int i = 0; i < count; ++i )
    {
        // map name, map begin color, map end color
        QString name = settings.value( QString( "%1_UDSchemes_%2_name" ).arg( QString::fromStdString( prefix ) ).arg( i ), tr( "" ) ).toString();
        if ( name != "" )
        {
            QColor begin = settings.value( QString( "%1_UDSchemes_%2_begin" ).arg(
                                               QString::fromStdString( prefix ) ).arg( i ), QColor( Qt::white ) ).value<QColor>();
            QColor end = settings.value( QString( "%1_UDSchemes_%2_end" ).arg(
                                             QString::fromStdString( prefix ) ).arg( i ), QColor( Qt::white ) ).value<QColor>();
            addScheme( name.toStdString(), make_pair( MSHColor::fromRGB( begin ), MSHColor::fromRGB( end ) ) );
        }
    }
}

void
SequentialColorMap::saveUDSchemes( const string& prefix, QSettings& settings )
{
    settings.setValue( QString( "%1_UDSchemes_Count" ).arg( QString::fromStdString( prefix ) ),
                       ( unsigned int )userDefinedSchemes.size() );

    PSMap::const_iterator it    = userDefinedSchemes.begin();
    int                   count = 0;

    while ( it != userDefinedSchemes.end() )
    {
        settings.setValue( QString( "%1_UDSchemes_%2_name" ).arg( QString::fromStdString( prefix ) ).arg( count ),
                           QString::fromStdString( it->first ) );
        settings.setValue( QString( "%1_UDSchemes_%2_begin" ).arg( QString::fromStdString( prefix ) ).arg( count ),
                           it->second.first.toRGB() );
        settings.setValue( QString( "%1_UDSchemes_%2_end" ).arg( QString::fromStdString( prefix ) ).arg( count ),
                           it->second.second.toRGB() );

        ++it;
        ++count;
    }
}

void
SequentialColorMap::loadCurrentColorMap( const string& prefix, QSettings& settings )
{
    int index = settings.value( QString( "%1_CurrentColorMap" ).arg( QString::fromStdString( prefix ) ), 0 ).toInt();
    getWidget().setCurrentCMIndex( index );
}

void
SequentialColorMap::saveCurrentColorMap( const string& prefix, QSettings& settings )
{
    settings.setValue( QString( "%1_CurrentColorMap" ).arg( QString::fromStdString( prefix ) ),
                       getWidget().currentCMIndex() );
}

/**
 * Widget class
 */

const QString SequentialColorMapWidget::INTERPOLATION_METHODS[ 2 ] = {
    tr( "Linear" ), tr( "Exponential" )
};

const QString SequentialColorMapWidget::SCHEME_MODIFIER_BUTTON_LABELS[ 3 ] = {
    QString::fromUtf8( "\u2795" ), QString::fromUtf8( "\u2796" ), QString::fromUtf8( "\u27f2" )
};

const int SequentialColorMapWidget::SCHEME_MODIFIER_BUTTON_SIZE = 40;

const int SequentialColorMapWidget::SCHEME_MODIFIER_BUTTON_FONT_SIZE = 20;

SequentialColorMapWidget::SequentialColorMapWidget( SequentialColorMap& _parent ) :
    ColorMapWidget( _parent ),
    cachedParentScheme( _parent.getBeginColor(), _parent.getEndColor() ),
    cachedInterMethod( _parent.getInterpolationMethod() ),
    cachedComboBoxIndex( 0 ),
    cachedSchemeChanged( false ),
    schemeChanged( false ),
    interpolationMethodLabel( tr( "Interpolation method: " ) )
{
    setMinimumHeight( 200 );
}

SequentialColorMapWidget::~SequentialColorMapWidget()
{
}

void
SequentialColorMapWidget::constructAndInitializeData()
{
    for ( int i = 0; i < 2; ++i )
    {
        interpolationMethod.addItem( INTERPOLATION_METHODS[ i ] );
    }
    interpolationMethodLayout.addWidget( &interpolationMethodLabel );
    interpolationMethodLayout.addWidget( &interpolationMethod );
    internalLayout.addLayout( &interpolationMethodLayout );
    QObject::connect( &interpolationMethod, SIGNAL( activated( int ) ),
                      this, SLOT( interpolationMethodChanged( int ) ) );

    /**
     * Selected
     */
    QString name;
    for ( PSMap::const_iterator it = getParent().PREDEFINED_SCHEMES.begin();
          it != getParent().PREDEFINED_SCHEMES.end(); ++it )
    {
        name = QString::fromStdString( ( *it ).first );
        schemeChoose.addItem( name, name );
    }
    configureColoringSchemeAutomatic( 0 );
    internalLayout.addWidget( &schemeChoose );

    for ( int i = 0; i < 3; ++i )
    {
        schemeModifiers[ i ].setText( SCHEME_MODIFIER_BUTTON_LABELS[ i ] );
        // symbols can be too small
        QFont font = schemeModifiers[ i ].font();
        font.setPointSize( SCHEME_MODIFIER_BUTTON_FONT_SIZE );
        schemeModifiers[ i ].setFont( font );
        schemeModifiers[ i ].setMaximumSize( SCHEME_MODIFIER_BUTTON_SIZE, SCHEME_MODIFIER_BUTTON_SIZE );
        schemeModifierLayout.addWidget( &schemeModifiers[ i ] );
    }
    QObject::connect( &schemeModifiers[ 0 ], SIGNAL( clicked( bool ) ), this, SLOT( addColorMapButton( bool ) ) );
    QObject::connect( &schemeModifiers[ 1 ], SIGNAL( clicked( bool ) ), this, SLOT( removeColorMapButton( bool ) ) );
    QObject::connect( &schemeModifiers[ 2 ], SIGNAL( clicked( bool ) ), this, SLOT( reloadColorMapButton( bool ) ) );

    internalLayout.addItem( &schemeModifierLayout );

    /**
     * Scheme changed
     */
    QObject::connect( &schemeChoose, SIGNAL( activated( int ) ),
                      this, SLOT( definitionMethodUpdate( int ) ) );

    addPlotToGUI( internalLayout );

    this->setLayout( &internalLayout );

    /**
     * Prepare cache.
     */
    applyChanges();
}

void
SequentialColorMapWidget::definitionMethodUpdate( int index )
{
    configureColoringSchemeAutomatic( index );
}

void
SequentialColorMapWidget::interpolationMethodChanged( int index )
{
    getParent().useInterpolationMethod(     static_cast<SequentialColorMap::InterpolationMethod>( index ) );
    colorMapUpdated();
}
/*
   void
   SequentialColorMapWidget::manual_firstColorUpdate( const QColor& color )
   {
        ColorMapExtended::MSHColor msh = ColorMapExtended::MSHColor::fromRGB( color );
        if( !schemeChanged && !getParent().getBeginColor().compare( msh ) ) {
                schemeChanged = true;
        }
    getParent().useScheme( msh, getParent().getEndColor() );
    colorMapUpdated();
   }

   void
   SequentialColorMapWidget::manual_secondColorUpdate( const QColor& color )
   {
        ColorMapExtended::MSHColor msh = ColorMapExtended::MSHColor::fromRGB( color );
        if( !schemeChanged && !getParent().getEndColor().compare( msh ) ) {
                schemeChanged = true;
        }
    getParent().useScheme( getParent().getBeginColor(), msh );
    colorMapUpdated();
   }*/

void
SequentialColorMapWidget::configureColoringSchemeAutomatic( int index )
{
    // predefined scheme
    if ( ( unsigned int )index < getParent().PREDEFINED_SCHEMES.size() )
    {
        getParent().usePredefinedScheme( schemeChoose.itemData( index ).toString().toStdString() );
    }
    else
    {
        getParent().useUDScheme( schemeChoose.itemData( index ).toString().toStdString() );
    }
    schemeChanged = false;
    // update GUI
    colorMapUpdated();
}

SequentialColorMap&
SequentialColorMapWidget::getParent() const
{
    return dynamic_cast<SequentialColorMap&>( parent );
}

void
SequentialColorMapWidget::processColorChanged( ColorMapPlot::Color pos, const QColor& col )
{
    ColorMapExtended::MSHColor msh = ColorMapExtended::MSHColor::fromRGB( col );
    if ( pos == ColorMapPlot::COLOR_FIRST )
    {
        if ( !schemeChanged && !getParent().getBeginColor().compare( msh ) )
        {
            schemeChanged = true;
        }
        getParent().useScheme( msh,
                               getParent().getEndColor() );
    }
    else if ( pos == ColorMapPlot::COLOR_SECOND )
    {
        if ( !schemeChanged && !getParent().getEndColor().compare( msh ) )
        {
            schemeChanged = true;
        }
        getParent().useScheme( getParent().getBeginColor(),
                               msh );
    }
    else
    {
        ColorMapWidget::processColorChanged( pos, col );
    }
    colorMapUpdated();
}

void
SequentialColorMapWidget::applyChanges()
{
    ColorMapWidget::applyChanges();

    cachedParentScheme  = std::make_pair( getParent().getBeginColor(), getParent().getEndColor() );
    cachedInterMethod   = getParent().getInterpolationMethod();
    cachedComboBoxIndex = schemeChoose.currentIndex();
    cachedSchemeChanged = schemeChanged;
}

void
SequentialColorMapWidget::revertChanges()
{
    ColorMapWidget::revertChanges();

    getParent().useScheme( cachedParentScheme.first, cachedParentScheme.second );
    getParent().useInterpolationMethod( cachedInterMethod );
    /**
     * Revert GUI
     */
    interpolationMethod.setCurrentIndex( cachedInterMethod );
    schemeChoose.setCurrentIndex( cachedComboBoxIndex );
    schemeChanged = cachedSchemeChanged;
    enableSchemeButtons();
}


ColorMapPlot*
SequentialColorMapWidget::getCurrentPlot()
{
    return currentPlot;
}

void
SequentialColorMapWidget::enableSchemeButtons()
{
    // button 'Save'
    schemeModifiers[ 0 ].setEnabled( schemeChanged );
    // button 'Delete'
    schemeModifiers[ 1 ].setEnabled( ( unsigned int )schemeChoose.currentIndex() >=
                                     getParent().PREDEFINED_SCHEMES.size() );
    // button 'Refresh'
    schemeModifiers[ 2 ].setEnabled( schemeChanged );
}

void
SequentialColorMapWidget::colorMapUpdated()
{
    ColorMapWidget::colorMapUpdated();
    // update all buttons
    enableSchemeButtons();
}

void
SequentialColorMapWidget::addColorMapButton( bool )
{
    bool    ok;
    QString text = QInputDialog::getText( this, tr( "Add new color map" ), tr( "Color map name:" ),
                                          QLineEdit::Normal, schemeChoose.currentText(), &ok );

    if ( ok && !text.isEmpty() )
    {
        // find whether it overwrites something
        PSMap::const_iterator it = getParent().getUDSchemes().find( text.toStdString() );
        // overwrite UDscheme
        if ( it != getParent().getUDSchemes().end() )
        {
            QMessageBox msgBox;
            msgBox.setText( QString( tr( "Do you want to overwrite scheme %1?" ) ).arg( text ) );
            msgBox.setStandardButtons( QMessageBox::Save | QMessageBox::Discard );
            msgBox.setDefaultButton( QMessageBox::Discard );
            int ret = msgBox.exec();

            if ( ret == QMessageBox::Save )
            {
                getParent().addScheme( text.toStdString() );
                schemeChoose.setCurrentIndex( getParent().PREDEFINED_SCHEMES.size() +
                                              distance( getParent().getUDSchemes().begin(), it ) );
                schemeChanged = false;
            }
        }
        // overwrite predefined scheme
        else if ( getParent().PREDEFINED_SCHEMES.end() !=
                  getParent().PREDEFINED_SCHEMES.find( text.toStdString() ) )
        {
            // can't overwrite predefined scheme
            QMessageBox msgBox;
            msgBox.setText( QString( tr( "Can't overwrite predefined scheme %1." ) ).arg( text ) );
            msgBox.exec();
        }
        else
        {
            // just write
            getParent().addScheme( text.toStdString() );
            // update chooser
            schemeChoose.addItem( text, text );
            schemeChoose.setCurrentIndex( schemeChoose.count() - 1 );
            schemeChanged = false;
        }
    }

    enableSchemeButtons();
}

void
SequentialColorMapWidget::removeColorMapButton( bool )
{
    getParent().removeScheme( schemeChoose.currentText().toStdString() );
    // change index
    int index = schemeChoose.currentIndex();
    schemeChoose.setCurrentIndex( index - 1 );
    // remove item
    schemeChoose.removeItem( index );

    // update GUI
    configureColoringSchemeAutomatic( index - 1 );
}

void
SequentialColorMapWidget::reloadColorMapButton( bool )
{
    unsigned int mapCount = schemeChoose.currentIndex();
    string       mapName  = schemeChoose.currentText().toStdString();

    // predefined scheme
    if ( mapCount < getParent().PREDEFINED_SCHEMES.size() )
    {
        ColorScheme scheme = getParent().PREDEFINED_SCHEMES.find( mapName )->second;
        getParent().useScheme( scheme.first, scheme.second );
    }
    // user-defined scheme
    else
    {
        ColorScheme scheme = getParent().getUDSchemes().find( mapName )->second;
        getParent().useScheme( scheme.first, scheme.second );
    }

    // scheme is reverted
    schemeChanged = false;
    colorMapUpdated();
}

void
SequentialColorMapWidget::parentUpdated()
{
    // recheck if we have all items in scheme chooser
    // first N items will be predefined schemes
    const int    offset    = getParent().PREDEFINED_SCHEMES.size();
    const PSMap& UDSchemes = getParent().getUDSchemes();
    const int    UDCount   = UDSchemes.size();

    if ( schemeChoose.count() - offset != UDCount )
    {
        set<string> existing;

        // we could do this much nicer with a lambda an transform, but C++11
        for ( PSMap::const_iterator it = UDSchemes.begin(); it != UDSchemes.end(); ++it )
        {
            existing.insert( it->first );
        }

        for ( int i = 0; i < schemeChoose.count() - offset; ++i )
        {
            string name = schemeChoose.itemText( i ).toStdString();
            existing.erase( name );
        }

        // add elements which aren't in the combobox
        for ( set<string>::const_iterator it = existing.begin(); it != existing.end(); ++it )
        {
            QString qstr = QString::fromStdString( *it );
            schemeChoose.addItem( qstr, qstr );
        }
    }
}

int
SequentialColorMapWidget::currentCMIndex()
{
    return schemeChoose.currentIndex();
}

void
SequentialColorMapWidget::setCurrentCMIndex( int index )
{
    index = index >= schemeChoose.count() ? 0 : index;
    schemeChoose.setCurrentIndex( index );
    configureColoringSchemeAutomatic( index );
}
