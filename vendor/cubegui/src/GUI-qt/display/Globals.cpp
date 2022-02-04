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

#include <iostream>
#include "Globals.h"
#include "DefaultColorMap.h"
#include "ColorMap.h"
#include "PrecisionWidget.h"
#include "MainWidget.h"
#include "ColorMap.h"
#include "TabManager.h"
#include "DefaultValueView.h"
#include "TreeItem.h"
#include "Settings.h"
#if QT_VERSION >= QT_VERSION_CHECK( 5, 0, 0 )
#include <QRegularExpression>
#else
#include <QRegExp>
#endif

using namespace cubegui;
using namespace cubepluginapi;

MainWidget*      Globals::mainWidget       = 0;
TabManager*      Globals::tabManager       = 0;
ColorMap*        Globals::defaultColorMap  = 0;
ColorMap*        Globals::colorMap         = 0;
PrecisionWidget* Globals::precisionWidget  = 0;
QTextStream*     Globals::outStream        = 0;
Settings*        Globals::settings         = 0;
ValueView*       Globals::defaultValueView = new DefaultValueView();

QHash<UserOption, QString>        Globals::userOptions;
QHash<cube::DataType, ValueView*> Globals::valueViews = QHash<cube::DataType, ValueView*>();

class DebugIODevice : public QIODevice
{
public:
    DebugIODevice()
    {
        open( QIODevice::WriteOnly | QIODevice::Text );
        enabled = true;
    }

    void
    setEnabled( bool enabled )
    {
        this->enabled = enabled;
    }
protected:
    qint64
    readData( char*, qint64 )
    {
        return 0;
    }
    qint64
    writeData( const char* data, qint64 maxSize )
    {
        if ( enabled )
        {
            QString msg = QString( data );
#if QT_VERSION >= QT_VERSION_CHECK( 5, 0, 0 )
            msg = msg.remove( QRegularExpression( "\\s+$" ) );
#else
            msg = msg.remove( QRegExp( "\\s+$" ) );
#endif
            Globals::setStatusMessage( msg, Verbose );
        }
        return maxSize;
    }
private:
    bool enabled;
};

QTextStream&
Globals::debug( const QString& sender )
{
    if ( outStream == 0 )
    {
        DebugIODevice* device = new DebugIODevice();
        device->setEnabled( optionIsSet( VerboseMode ) );
        outStream = new QTextStream( device );
    }
    *outStream << sender << ": ";
    return *outStream;
}

PrecisionWidget*
Globals::getPrecisionWidget()
{
    return precisionWidget;
}

QColor
Globals::getColor( double value, double minValue, double maxValue, bool whiteForZero )
{
    return colorMap->getColor( value, minValue, maxValue, whiteForZero );
}

void
Globals::setStatusMessage( const QString& msg, MessageType type, bool isLogged )
{
    if ( mainWidget )
    {
        mainWidget->setMessage( msg, type, isLogged );
    }
    else
    {
        std::cerr << msg.toStdString() << std::endl;
    }
}

/** converts the number using the given precision format; in scientific format, the same exponent as
 * the reference value is used
 * @param value the number to convert
 * @param integerType the type of the number is integer if "integerType" is true, otherwise the type is double
 * @param format FORMAT_TREES or FORMAT_DEFAULT
 */
QPair<QString, QString>
Globals::formatNumberAndUnit( double value, const QString& unit, bool integerType, PrecisionFormat format, const cubegui::TreeItem* item )
{
    return precisionWidget->numberToQStringAndUnit( value, unit, integerType, format, item );
}


/** converts the number using the given precision format; in scientific format, the same exponent as
 * the reference value is used
 * @param value the number to convert
 * @param integerType the type of the number is integer if "integerType" is true, otherwise the type is double
 * @param format FORMAT_TREES or FORMAT_DEFAULT
 */
QString
Globals::formatNumber( double value, bool integerType, PrecisionFormat format, const cubegui::TreeItem* item )
{
    return precisionWidget->numberToQString( value, integerType, format, item );
}

/** converts the number using the given precision format; in scientific format, the same exponent as
 * the reference value is used
 * @param value the number to convert
 * @param referenceValue the number that is used to determine the format
 * @param integerType the type of the number is integer if "integerType" is true, otherwise the type is double
 * @param format FORMAT_TREES or FORMAT_DEFAULT
 * @param scientificSmallValue use scientificSmallValue use scientific notations for values below 0.1
 */
QString
Globals::formatNumber( double value, double referenceValue, bool integerType, bool scientificSmall, PrecisionFormat format )
{
    return precisionWidget->numberToQString( value, referenceValue, integerType, scientificSmall, format );
}

double
Globals::getRoundThreshold( PrecisionFormat format )
{
    return precisionWidget->getRoundThreshold( format );
}

double
Globals::getRoundNumber( PrecisionFormat format )
{
    return precisionWidget->getRoundNr( format );
}

QMainWindow*
Globals::getMainWindow()
{
    return mainWidget;
}

TabManager*
Globals::getTabManager()
{
    return tabManager;
}

// -------------- private ---------------------

ColorMap*
Globals::getColorMap()
{
    return colorMap;
}

void
Globals::setColorMap( ColorMap* map )
{
    if ( map )
    {
        colorMap = map;
    }
    else
    {
        colorMap = defaultColorMap;
    }
    tabManager->updateTreeItemProperties(); // update trees
    mainWidget->update();                   // update colormap widget
}

void
Globals::initialize( MainWidget* m, TabManager* t )
{
    mainWidget      = m;
    tabManager      = t;
    precisionWidget = new PrecisionWidget();
    defaultColorMap = new DefaultColorMap();
    colorMap        = defaultColorMap;
}

void
Globals::setValueView( cube::DataType type, ValueView* view )
{
    if ( !view )
    {
        view = defaultValueView;
    }
    valueViews.insert( type, view );

    tabManager->reinit(); // can be optimized, complete recalculation isn't required but min/maximum value may have changed
    tabManager->updateValueViews();
}

void
Globals::removeValueView( ValueView* view )
{
    foreach( cube::DataType type, valueViews.keys( view ) )
    {
        valueViews.remove( type );
    }
}

ValueView*
Globals::getValueView( cube::DataType type )
{
    ValueView* view = valueViews.value( type );
    return view ? view : defaultValueView;
}
