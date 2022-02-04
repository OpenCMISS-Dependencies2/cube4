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



#ifndef SRC_GUI_QT_PLUGINS_ADVANCEDCOLORMAPSSETTINGS_H_
#define SRC_GUI_QT_PLUGINS_ADVANCEDCOLORMAPSSETTINGS_H_
#include <string>
#include <set>

#include <QDialog>
#include <QCheckBox>
#include <QGroupBox>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QStackedLayout>
#include <QString>
#include <QDialogButtonBox>
#include <QSettings>

#include "ColorMapPlot.h"
#include "ColorMapsFactory.h"
#include "RGBDefinerWidget.h"
#include "PluginServices.h"

using std::set;
using std::string;
using namespace cubepluginapi;

/**
 * @class AdvancedColorMapsSettings
 * @author Marcin Copik (m.copik@fz-juelich.de)
 * @date January 2015
 * @brief Implementation of GUI dialog with selection and configuration of
 * new color maps.
 */
class AdvancedColorMapsSettings : public QDialog, public cubepluginapi::SettingsHandler
{
    Q_OBJECT
public:
    /**
     * Default constructor
     */
    AdvancedColorMapsSettings();
    /**
     * Destructor.
     */
    virtual
    ~AdvancedColorMapsSettings();
    /**
     * @return currently selected color map
     */
    cubepluginapi::ColorMap*
    getCurrentColorMap();

    /**
     * SettingsHandler interface methods.
     */
    virtual void
    loadGlobalSettings( QSettings& );
    virtual void
    saveGlobalSettings( QSettings& );
    virtual void
    loadExperimentSettings( QSettings& );
    virtual void
    saveExperimentSettings( QSettings& );

    virtual QString
    settingName();

signals:
    /**
     * Signal - changed color map.
     */
    void
    changeColorMap( cubepluginapi::ColorMap* );

    /**
     * Signal - updated settings of current color map.
     */
    void
    colorMapChanged();

private:
    /**
     * GUI consists of three parts:
     * - left panel, containing selection and description of color map
     * - right panel for configuration of color map
     * - left bottom panel used for common settings
     */
    QVBoxLayout mainLayout;
    QGroupBox   rightPanel;
    /**
     * CSS configuration for groupbox.
     */
    static const string GROUPBOX_BORDER_CONFIGURATION;
    static const string GROUPBOX_TITLE_POSITION;
    /**
     * Names of all three groupboxes
     */
    static const string GROUPBOX_NAMES[ 3 ];

    /**
     * Left panel - selection of color map and general buttons
     */
    static const QString LEFT_PANEL_SELECTION_TITLE;
    static const QString LEFT_PANEL_COMMON_TITLE;
    static const QString RIGHT_PANEL_TITLE;
    // QVBoxLayout leftPanel_Layout;
    QGroupBox   leftPanel_Selection;
    QVBoxLayout leftPanel_LayoutSelection;
    QComboBox   leftPanel_colorMapSelection;
    QLabel      leftPanel_colorMapDescription;
    // QWidget leftPanel_buttonsPanel;
    // QHBoxLayout leftPanel_buttonsLayout;
    // QPushButton leftPanel_okButton;
    // QPushButton leftPanel_cancelButton;
    // QPushButton leftPanel_applyButton;
    QDialogButtonBox buttonBox;
    QPushButton*     buttonBox_applyButton;
    /**
     * Right panel
     */
    static const int PLOT_FRAME_WIDTH;
    // QVBoxLayout rightPanel_Layout;
    ColorMapWidget*      rightPanel_colorMapEdit;
    QGroupBox            rightPanel_groupBox;
    set<ColorMapWidget*> rightPanel_widgets;
    QStackedLayout       rightPanel_groupBoxLayout;

    /**QWidget
     * Factory which provides color maps.
     */
    ColorMapsFactory factory;

    int currentlySelectedColorMapIndex;

    /**
     * Names for color maps.
     */
    const ColorMapsFactory::ColorMapsNames& mapNames;
    /**
     * Current
     */
    ColorMapExtended* currentColorMap;
    ColorMapExtended* currentlySelectedColorMap;
    /**
     * Initialize GUI components.
     */
    void
    initComponents();

    /**
     * Configure signal - slot connections.
     */
    void
    setConnections();

    /**
     * Update GUI after selecting new color map.
     */
    void
    updateGUI();

private slots:
    /**
     * Slot - click() method for accept and close.
     */
    void
    handleOKButton();

    /**
     * Slot - click() method for close with rejection of recent changes.
     */
    void
    handleCancelButton();

    /**
     * Slot - click() method for just apply new changes.
     * @param button clicked button
     */
    void
    handleApplyButton( QAbstractButton* button );

    /**
     * Slot - user changed color map
     * @param number of position in combobox
     */
    void
    colorMapChanged( int );

    /**
     * Slot - user changed settings of color map
     * Currently not used.
     */
    void
    colorMapUpdated();

    /**
     * Slot - changed color for values out of range.
     * @param new color
     */
    void
    colorExcludedValuesChanged( const QColor& );

private:
};

#endif
