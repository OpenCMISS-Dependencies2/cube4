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



#ifndef SRC_GUI_QT_PLUGINS_ADVANCEDCOLORMAPS_H_
#define SRC_GUI_QT_PLUGINS_ADVANCEDCOLORMAPS_H_

#include <QObject>
#include <QtPlugin>

#include "ColorMap.h"
#include "CubePlugin.h"
#include "PluginServices.h"
#include "AdvancedColorMapsSettings.h"

namespace advancedcolormaps
{
using namespace cubepluginapi;

/**
 * @class AdvancedColorMaps
 * @author Marcin Copik (m.copik@fz-juelich.de)
 * @date January 2015
 * @brief Provides new options for choosing the color map for Cube.
 * This class works as a proxy for internal implementations of color maps.
 */
class AdvancedColorMaps : public ColorMap, public CubePlugin
{
    Q_OBJECT
    Q_INTERFACES( cubepluginapi::CubePlugin )
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA( IID "AdvancedColorMapsPlugin" )
#endif
    /**
     * Main GUI class - editor for color maps.
     */
    AdvancedColorMapsSettings * colorMapSettings;

    /**
     * Currently chosen color map by user.
     */
    ColorMap* currentColorMap;
public:
    /**
     * Default constructor.
     */
    AdvancedColorMaps();

    /**
     * Default destructor.
     */
    virtual
    ~AdvancedColorMaps();

    /**
     * ColorMap interface methods.
     */

    /**
     * Implementation of abstract method from ColorMap class.
     * @param value current value
     * @param minValue
     * @param maxValue
     * @param whiteForZero true when zero value should become white (for topology widgets)
     * @return Qt color object for given value
     */
    virtual QColor
    getColor( double value,
              double minValue,
              double maxValue,
              bool   whiteForZero ) const;

    virtual QString
    getMapName() const;

    virtual bool
    dialogAvailable() const;

    virtual void
    showDialog();

    /**
     * CubePlugin interface methods
     */
    virtual bool
    cubeOpened( PluginServices* service );

    virtual void
    version( int& major,
             int& minor,
             int& bugfix ) const;

    virtual QString
    name() const;

    virtual QString
    getHelpText() const;

    virtual QString
    getHelpURL() const
    {
        return "AdvancedColorMapPlugin.html";
    }

private slots:
    /**
     * Slot function - color map changed in GUI.
     */
    void
    updateColorMap( cubepluginapi::ColorMap* );

    /**
     * Slot function - changed settings of currently selected color map.
     */
    void
    applyChanges();
};
}

#endif /* SRC_GUI_QT_PLUGINS_ADVANCEDCOLORMAPS_H_ */
