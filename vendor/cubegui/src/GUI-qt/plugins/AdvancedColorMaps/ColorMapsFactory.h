/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2016                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/



#ifndef SRC_GUI_QT_PLUGINS_ADVANCEDCOLORMAPS_COLORMAPSFACTORY_H_
#define SRC_GUI_QT_PLUGINS_ADVANCEDCOLORMAPS_COLORMAPSFACTORY_H_
#include <map>

#include "ColorMapExtended.h"

/**
 * @class ColorMapsFactory
 * @author Marcin Copik (m.copik@fz-juelich.de)
 * @date January 2015
 * @brief Create and manage instances of color maps.
 */
class ColorMapsFactory
{
public:
    ColorMapsFactory()
    {
    }
    // static ColorMapsFactory & getInstance();
    /**
     * Destructor: clean all instances.
     */
    virtual
    ~ColorMapsFactory();

    /**
     * Supported color maps.
     */
    enum ColorMaps
    {
        SEQUENTIAL = 0,         // !< Sequential color map
        DIVERGENT,              // !< Divergent CM
        CUBEHELIX,              // !< Cubehelix
        IMPROVED_RAINBOW,       // !< Improved Rainbow CM
        COLORMAPS_END           // !< Marks end of enum
    };

    /**
     * @typedef ColorMapsNames
     * ColorMap -> String
     */
    typedef std::map<ColorMaps, std::string> ColorMapsNames;

    /**
     * Assigns to each color map its name
     */
    static const ColorMapsNames mapNames;

    /**
     * @param map
     * @return instance of demanded color map; will be freed by factory
     */
    ColorMapExtended*
    getColorMap( ColorMaps map );

    /**
     * @param color map instance
     * @return enum value corresponding to this color map
     */
    ColorMaps
    getColorMapIndex( ColorMapExtended* );

private:
    /**
     * Default constructor.
     */
    ColorMapsFactory( const ColorMapsFactory& )
    {
    }
    void
    operator=( ColorMapsFactory& )
    {
    }

    /**
     * Initialize static member mapNames
     * @return
     */
    static ColorMapsNames
    initializeMap();

    /**
     * Contains instances of maps.
     */
    std::map<ColorMaps, ColorMapExtended*> mapObjects;
};

#endif /* SRC_GUI_QT_PLUGINS_ADVANCEDCOLORMAPS_COLORMAPSFACTORY_H_ */
