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




#ifndef SRC_GUI_QT_PLUGINS_ADVANCEDCOLORMAPS_MAPS_IMPROVEDRAINBOWCOLORMAP_H_
#define SRC_GUI_QT_PLUGINS_ADVANCEDCOLORMAPS_MAPS_IMPROVEDRAINBOWCOLORMAP_H_
#include <string>

#include <QLabel>
#include <QComboBox>
#include <QVBoxLayout>

#include "../ColorMapExtended.h"

using std::string;

class ImprovedRainbowColorMapWidget;

/**
 * @class ImprovedRainbowColorMap
 * @author Marcin Copik (m.copik@fz-juelich.de)
 * @date January 2015
 * @brief Implementation of color map 'Improved Rainbow' - schemes presented as
 * an addition to MATLAB.
 * See: http://www.mathworks.com/matlabcentral/fileexchange/28982-perceptually-improved-colormaps
 */
class ImprovedRainbowColorMap : public ColorMapExtended
{
    Q_OBJECT
public:
    ImprovedRainbowColorMap();

    /**
     * Delete widget.
     */
    ~ImprovedRainbowColorMap();

    /**
     * Overridden methods from parent class.
     */
    const QString&
    getColorMapDescription() const;

    ColorMapWidget*
    getConfigurationPanel();

    QColor
    getColor( double value,
              double minValue,
              double maxValue,
              bool   whiteForZero ) const;

    QString
    getMapName() const;

    void
    saveGlobalSettings( QSettings& settings );

    void
    loadGlobalSettings( QSettings& settings );

    /**
     *	Implemented schemes.
     */
    enum Schemes
    {
        ISO_L = 0,      // !< ISO_L
        ISO_AZ,         // !< ISO_AZ
        ISO_AZ180,      // !< ISO_AZ180
        LINEAR_L,       // !< LINEAR_L
        LIN_L_HOT,      // !< LIN_L_HOT
        CUBIC_YF,       // !< CUBIC_YF
        CUBIC_L,        // !< CUBIC_L
        SWTTH,          // !< SWTTH
        EDGE,           // !< EDGE
        SCHEMES_END     // !< marks enum end
    };

    /**
     * @return currently used scheme
     */
    Schemes
    getScheme();

    /**
     * Change currently used scheme.
     * @param scheme
     */
    void
    setScheme( Schemes scheme );

    /**
     * Number of schemes
     */
    static const int SCHEMES_COUNT = 9;

    /**
     * Size of each scheme - number of points
     */
    static const int SCHEME_SIZE = 256;

    /**
     * Interpolation data.
     */
    static const double SCHEMES_DATA[ SCHEMES_COUNT ][ SCHEME_SIZE ][ 3 ];

    /**
     * Schemes names.
     */
    static const QString SCHEMES_NAMES[ SCHEMES_COUNT ];

    /**
     * Schemes descriptions.
     */
    static const QString SCHEMES_DESCRIPTION[ SCHEMES_COUNT ];
private:
    /**
     * Currently used scheme.
     */
    Schemes currentScheme;

    /**
     * Color map description.
     */
    static const QString COLOR_MAP_DESCRIPTION;

    /**
     * Default, starting scheme.
     */
    static const Schemes DEFAULT_SCHEME;
private slots:
    /**
     * Slot - scheme changed in GUI.
     */
    void
    schemeChanged( ImprovedRainbowColorMap::Schemes );
};

/**
 * @class ImprovedRainbowColorMapWidget
 * @author Marcin Copik (m.copik@fz-juelich.de)
 * @date January 2015
 * @brief GUI widget for ImprovedRainbowColorMap - select one schemes.
 */
class ImprovedRainbowColorMapWidget : public ColorMapWidget
{
    Q_OBJECT
public:
    /**
     * @param map which will be 'served' by this widget
     * @param scheme initial scheme
     */
    ImprovedRainbowColorMapWidget( ImprovedRainbowColorMap&         map,
                                   ImprovedRainbowColorMap::Schemes scheme );

    /**
     * Currently doesn't clean anything.
     */
    virtual
    ~ImprovedRainbowColorMapWidget();

    /**
     * Overridden from parent.
     */
    ImprovedRainbowColorMap&
    getParent() const;

    void
    parentUpdated();

    void
    revertChanges();

    void
    applyChanges();

signals:
    /**
     * Signal - selected new scheme.
     */
    void
    schemeChanged( ImprovedRainbowColorMap::Schemes );

protected:
    QVBoxLayout layout;

    QComboBox schemeChoose;

    QLabel schemeDescription;

    /**
     * Currently used scheme.
     */
    ImprovedRainbowColorMap::Schemes currentScheme;

    /**
     * Cached previous scheme.
     */
    ImprovedRainbowColorMap::Schemes cachedSchemeChoose;

    /**
     * Change description to reflect change in scheme.
     * @param scheme new scheme
     */
    void
    updateDescription( ImprovedRainbowColorMap::Schemes scheme );

protected slots:
    /**
     * Slot - changed selection of scheme.
     */
    void
    schemeChanged( int );
};

#endif /* SRC_GUI_QT_PLUGINS_ADVANCEDCOLORMAPS_MAPS_IMPROVEDRAINBOWCOLORMAP_H_ */
