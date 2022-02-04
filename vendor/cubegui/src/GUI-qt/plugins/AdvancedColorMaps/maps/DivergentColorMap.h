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




#ifndef SRC_GUI_QT_PLUGINS_ADVANCEDCOLORMAPS_MAPS_DIVERGENTCOLORMAP_H_
#define SRC_GUI_QT_PLUGINS_ADVANCEDCOLORMAPS_MAPS_DIVERGENTCOLORMAP_H_
#include <vector>

#include "../ColorMapExtended.h"
#include "../ColorMapPlot.h"
#include "SequentialColorMap.h"

using std::vector;

/**
 * @class DivergentColorMap
 * @author Marcin Copik (m.copik@fz-juelich.de)
 * @date January 2015
 * @brief Implementation of color map 'Divergent' - linear interpolation
 * between two colors in MSH color space with a white gap in the middle.
 */
class DivergentColorMap : public SequentialColorMap
{
    Q_OBJECT
public:
    /**
     * Calls widget's constructAndInitializeData!
     */
    DivergentColorMap();

    ~DivergentColorMap();

    QColor
    getColor( double value,
              double minValue,
              double maxValue,
              bool   whiteForZero ) const;

    virtual void
    saveGlobalSettings( QSettings& settings );

    virtual void
    loadGlobalSettings( QSettings& settings );

private:

    /**
     * Predefined sequential color map.
     * Each element of tree contains name and pair of colors - starting and ending point.
     */
    static const PSMap DIVERGENT_PREDEFINED_SCHEMES;

    /**
     * Color map name.
     */
    static const QString DIVERGENT_COLOR_MAP_NAME;

    /**
     * Color map name.
     */
    static const QString DIVERGENT_COLOR_MAP_DESCRIPTION;


    static PSMap
    divergentInitializePredefinedSchemes();
};

/**
 * @class DivergentColorMapWidget
 * @author Marcin Copik (m.copik@fz-juelich.de)
 * @date January 2015
 * @brief GUI Widget for DivergentColorMapWidget. Just widget from Sequential map,
 * only with new plot.
 */
class DivergentColorMapWidget : public SequentialColorMapWidget
{
    Q_OBJECT
public:
    DivergentColorMapWidget( DivergentColorMap& );

    ~DivergentColorMapWidget();

    DivergentColorMap&
    getParent() const;
};

#endif /* SRC_GUI_QT_PLUGINS_ADVANCEDCOLORMAPS_MAPS_DIVERGENTCOLORMAP_H_ */
