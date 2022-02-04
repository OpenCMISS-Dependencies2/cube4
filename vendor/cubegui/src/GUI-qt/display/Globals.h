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


#ifndef GLOBALS_H
#define GLOBALS_H

#include <QHash>
#include <QColor>
#include <QMainWindow>
#include <QTextStream>
#include <QVariant>
#include "CubeTypes.h"
#include "CubeValues.h"
#include "Constants.h"

namespace cubepluginapi
{
class ValueView;
}

namespace cubegui
{
enum MessageType { AppendInformation, Verbose, DelayedInformation, Information, Warning, Error, Critical };
enum UserOption { DisablePlugins, ExpertMode, VerboseMode, ExperimentalMode, Single, ManualCalculation, DocPath };

class ColorMap;
class PrecisionWidget;
class MainWidget;
class TabManager;
class SettingsHandler;
class InternalSettingsHandler;
class ValueViewConfig;
class TreeItem;
class Settings;

class Globals
{
    friend class MainWidget;
    friend class PluginManager;
    friend class ValueViewConfig;
public:
    /** returns the corresponding color of the currently selected colormap */
    static QColor
    getColor( double value,
              double minValue,
              double maxValue,
              bool   whiteForZero = true );

    static QPair<QString, QString>
    formatNumberAndUnit( double                   value,
                         const QString&           unit,
                         bool                     integerType = false,
                         PrecisionFormat          format = FORMAT_TREES,
                         const cubegui::TreeItem* item = 0 );

    static QString
    formatNumber( double                   value,
                  bool                     integerType = false,
                  PrecisionFormat          format = FORMAT_TREES,
                  const cubegui::TreeItem* item = 0 );

    static QString
    formatNumber( double          value,
                  double          referenceValue,
                  bool            scientificSmall = false,
                  bool            integerType = false,
                  PrecisionFormat format = FORMAT_TREES );

    static double
    getRoundNumber( PrecisionFormat format );

    static double
    getRoundThreshold( PrecisionFormat format );

    /** prints the given text into the status line below the main widget */
    static void
    setStatusMessage( const QString& msg,
                      MessageType    type = Information,
                      bool           isLogged = true );

    static QMainWindow*
    getMainWindow();

    static TabManager*
    getTabManager();

    static void
    setVerbose( bool value );

    static QTextStream&
    debug( const QString& sender );

    static cubepluginapi::ValueView*
    getValueView( cube::DataType type );

    static Settings*
    getSettings()
    {
        return settings;
    }

    static QString
    getOption( UserOption option )
    {
        return userOptions.value( option );
    }
    static bool
    optionIsSet( UserOption option )
    {
        return !userOptions.value( option ).isEmpty();
    }
    static void
    setOption( UserOption option, QString value = "enabled" )
    {
        userOptions.insert( option, value );
    }

private:
    static PrecisionWidget* precisionWidget;
    static ColorMap*        defaultColorMap;
    static ColorMap*        colorMap;
    static MainWidget*      mainWidget;
    static TabManager*      tabManager;
    static QTextStream*     outStream;
    static Settings*        settings;

    static QHash<UserOption, QString>                       userOptions;
    static cubepluginapi::ValueView*                        defaultValueView;
    static QHash<cube::DataType, cubepluginapi::ValueView*> valueViews;

    static void
    initialize( MainWidget* m,
                TabManager* t );
    static PrecisionWidget*
    getPrecisionWidget();
    static void
    setColorMap( ColorMap* map = 0 );
    static ColorMap*
    getColorMap();
    static void
    setValueView( cube::DataType            type,
                  cubepluginapi::ValueView* view = 0 );
    static void
    removeValueView( cubepluginapi::ValueView* view );
};
}

// Qt::endl doesn't exist in Qt < 5.14.0
#if QT_VERSION < QT_VERSION_CHECK( 5, 14, 0 )
namespace Qt
{
using TextStreamFunction = QTextStream & ( * )( QTextStream & );
constexpr TextStreamFunction endl = ::endl;
}
#endif

#endif // GLOBALS_H
