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




#ifndef SRC_GUI_QT_PLUGINS_ADVANCEDCOLORMAPS_SEQUENTIALCOLORMAP_H_
#define SRC_GUI_QT_PLUGINS_ADVANCEDCOLORMAPS_SEQUENTIALCOLORMAP_H_
#include <utility>
#include <map>
#include <string>

#include <QString>
#include <QWidget>
#include <QColor>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QRadioButton>
#include <QLabel>

#include "../ColorMapExtended.h"
#include "../RGBDefinerWidget.h"

using std::map;
using std::pair;
using std::string;

/**
 * @typedef Map of defined schemes: scheme name -> [begin color, end color]
 */
typedef pair<ColorMapExtended::MSHColor, ColorMapExtended::MSHColor> ColorScheme;
typedef map<string, ColorScheme >                                    PSMap;

class SequentialColorMapWidget;

/**
 * @class SequentialColorMap
 * @author Marcin Copik (m.copik@fz-juelich.de)
 * @date January 2015
 * @brief Implementation of color map 'Sequential' - linear interpolation
 * between two colors in MSH color space.
 */
class SequentialColorMap : public ColorMapExtended
{
    Q_OBJECT
public:
    /**
     * Method of interpolating colours between start/end
     */
    enum InterpolationMethod
    {
        LINEAR = 0,  // !< LINEAR
        EXPONENTIAL  // !< EXPONENTIAL
    };

    /**
     * Sequential map uses default values provided in function declaration.
     * Different sets of parameters are used by child class DivergentColorMap
     * @param widget widget instance; if NULL then create new one
     * @param predefinedSchemes map with defined schemes
     * @param mapName map name
     * @param mapDescription map description
     */
    SequentialColorMap( SequentialColorMapWidget* widget = NULL,
                        const PSMap&              predefinedSchemes = SEQUENTIAL_PREDEFINED_SCHEMES,
                        const QString&            mapName = SEQUENTIAL_COLOR_MAP_NAME,
                        const QString&            mapDescription = SEQUENTIAL_COLOR_MAP_DESCRIPTION );

    /**
     * Destructor - delete widget.
     */
    virtual
    ~SequentialColorMap();

    /**
     * Use one of predefined schemes.
     * @param name scheme name
     */
    void
    usePredefinedScheme( const std::string& name );

    /**
     * Use one of user-defined schemes.
     * @param name scheme name
     */
    void
    useUDScheme( const std::string& name );

    /**
     * Use user-defined scheme.
     * @param first begin color
     * @param second end color
     */
    void
    useScheme( const MSHColor& first,
               const MSHColor& second );

    /**
     * @return begin color of currently used scheme
     */
    const MSHColor&
    getBeginColor() const;

    /**
     * @return end color of currently used scheme
     */
    const MSHColor&
    getEndColor() const;

    /**
     * @return currently used interpolation method
     */
    InterpolationMethod
    getInterpolationMethod() const;

    /**
     * Change currently used interpolation method.
     * @param method
     */
    void
    useInterpolationMethod( InterpolationMethod method );

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

    virtual void
    saveGlobalSettings( QSettings& settings );

    virtual void
    loadGlobalSettings( QSettings& settings );

    /**
     * Map with predefined schemes.
     */
    const PSMap& PREDEFINED_SCHEMES;

    const PSMap&
    getUDSchemes() const;

    /**
     * Adds currently used scheme as new user-defined scheme.
     * May overwrite previous entry by the same name.
     */
    void
    addScheme( const string& name );

    /**
     * Adds new user-defined scheme.
     * @param name
     */
    void
    addScheme( const string&      name,
               const ColorScheme& scheme );

    /**
     * Remove user-defined scheme.
     */
    void
    removeScheme( const string& name );

protected:

    /**
     * Map with schemes defined by user.
     */
    PSMap userDefinedSchemes;

    /**
     * Current scheme.
     */
    ColorScheme currentScheme;

    /**
     * User defined schemes.
     */
    PSMap userSchemes;

    /**
     * Interpolation parameter - middle marker position.
     */
    double interpolationParameter;

    /**
     * Interpolation method.
     */
    InterpolationMethod interMethod;

    /**
     * Color map name (changed in inherited classes).
     */
    const QString COLOR_MAP_NAME;

    /**
     * Color map description (changed in inherited classes).
     */
    const QString COLOR_MAP_DESCRIPTION;

    /**
     * Inherited from ColorMapExtended.
     */
    SequentialColorMapWidget&
    getWidget();

    /**
     * Load/save user-defined schemes from Qt settings.
     */
    void
    loadUDSchemes( const string&,
                   QSettings& );
    void
    saveUDSchemes( const string&,
                   QSettings& );
    void
    loadCurrentColorMap( const string&,
                         QSettings& );
    void
    saveCurrentColorMap( const string&,
                         QSettings& );

private:
    /**
     * Predefined sequential color map.
     * Each element of tree contains name and pair of colors - starting and ending point.
     */
    static const PSMap SEQUENTIAL_PREDEFINED_SCHEMES;

    /**
     * Color map name.
     */
    static const QString SEQUENTIAL_COLOR_MAP_NAME;

    /**
     * Color map description.
     */
    static const QString SEQUENTIAL_COLOR_MAP_DESCRIPTION;

    /**
     * @return initialized map with predefined schemes
     */
    static PSMap
    sequentialInitializePredefinedSchemes();
};

/**
 * @class SequentialColorMapWidget
 * @author Marcin Copik (m.copik@fz-juelich.de)
 * @date January 2015
 * @brief GUI widget for SequentialColorMap - select one of predefined schemes or
 * manually define new scheme.
 */
class SequentialColorMapWidget : public ColorMapWidget
{
    Q_OBJECT
public:
    /**
     * This constructor does not initialize all data!
     * One has to call method constructAndInitializeData, when the class
     * SequentialColorMap is built and all fields are initialized.
     * @param map which will be 'served' by this widget
     */
    SequentialColorMapWidget( SequentialColorMap& );

    /**
     * Construct and initialize all GUI fields.
     * Call only when the object of class SequentialColorMap, which has been provided in
     * constructor, has been built (use predefined schemes field from this class).
     * @param plot
     */
    virtual void
    constructAndInitializeData();

    /**
     * Currently doesn't clean anything.
     */
    virtual
    ~SequentialColorMapWidget();

    /**
     * Overridden from parent.
     */
    SequentialColorMap&
    getParent() const;
    virtual void
    colorMapUpdated();
    virtual void
    parentUpdated();

    void
    revertChanges();

    void
    applyChanges();

    ColorMapPlot*
    getCurrentPlot();

    /**
     * Manipulate selection in ComboBox.
     */
    int
    currentCMIndex();
    void
    setCurrentCMIndex( int );

protected:

    pair<ColorMapExtended::MSHColor, ColorMapExtended::MSHColor> cachedParentScheme;
    SequentialColorMap::InterpolationMethod                      cachedInterMethod;
    // revert any actions in combobox
    int  cachedComboBoxIndex;
    bool cachedSchemeChanged;

    bool schemeChanged;

    static const QString INTERPOLATION_METHODS[ 2 ];
    static const int     SCHEME_MODIFIER_BUTTON_SIZE;
    static const int     SCHEME_MODIFIER_BUTTON_FONT_SIZE;
    static const QString SCHEME_MODIFIER_BUTTON_LABELS[ 3 ];

    QVBoxLayout internalLayout;
    QComboBox   schemeChoose;
    QHBoxLayout schemeModifierLayout;
    QPushButton schemeModifiers[ 3 ];
    QHBoxLayout interpolationMethodLayout;
    QLabel      interpolationMethodLabel;
    QComboBox   interpolationMethod;

    /**
     * Update GUI to reflect selected predefined scheme.
     * @param index
     */
    void
    configureColoringSchemeAutomatic( int index );

    /**
     * Enable/disable buttons according to current selection of scheme and changes.
     */
    void
    enableSchemeButtons();

protected slots:
    /**
     * Slot - selected one of predefined schemes.
     */
    void
    definitionMethodUpdate( int );

    /**
     * Slot - changed interpolation method.
     */
    void
    interpolationMethodChanged( int );

    /**
     * Slot - changed begin color in manual scheme.
     * @param color
     */
    // void manual_firstColorUpdate(const QColor & color);

    /**
     * Slot - changed end color in manual scheme.
     * @param color
     */
    // void manual_secondColorUpdate(const QColor & color);

    /**
     * Parent slot.
     */
    void
    processColorChanged( ColorMapPlot::Color pos,
                         const QColor&       col );

    /**
     * Slots for management of three buttons modifying color maps:
     * - add new map (activated when some changes have been made)
     * - remove color map (only for user-defined schemes)
     * - reload color map (revert any changes)
     */
    void
    addColorMapButton( bool );
    void
    removeColorMapButton( bool );
    void
    reloadColorMapButton( bool );
};

#endif /* SRC_GUI_QT_PLUGINS_ADVANCEDCOLORMAPS_SEQUENTIALCOLORMAP_H_ */
