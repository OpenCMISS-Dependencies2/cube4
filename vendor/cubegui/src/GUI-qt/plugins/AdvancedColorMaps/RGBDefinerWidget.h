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




#ifndef SRC_GUI_QT_PLUGINS_ADVANCEDCOLORMAPS_RGBDEFINERWIDGET_H_
#define SRC_GUI_QT_PLUGINS_ADVANCEDCOLORMAPS_RGBDEFINERWIDGET_H_

#include <QWidget>
#include <QColorDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QIntValidator>

/**
 * @class RGBDefinerWidget
 * @author Marcin Copik (m.copik@fz-juelich.de)
 * @date January 2015
 * @brief Small and convenient GUI widget used to define RGB color:
 * explicitly or with a Qt color dialog.
 */
class RGBDefinerWidget : public QWidget
{
    Q_OBJECT
    static const QString rgbLabels[ 3 ];
    QLabel               name;
    QLineEdit            rgb[ 3 ];
    QIntValidator*       rgbValidators[ 3 ];
    QFormLayout          rgbLayout;
    QPushButton          pickColors;
    QColorDialog         colorPicker;
    QVBoxLayout          layout;
public:
    /**
     * Default constructor.
     * @param title widget title
     */
    RGBDefinerWidget( const QString& title );

    /**
     * Destructor.
     */
    ~RGBDefinerWidget();

    /**
     * Enable/disable the widget.
     * @param flag
     */
    void
    setEnabled( bool flag );

    /**
     * @return color selected in widget.
     */
    QColor
    getColor();

    /**
     * Change color represented in widget.
     * @param
     */
    void
    setColor( const QColor& );

signals:
    /**
     * Signal - color changed by user.
     */
    void
    colorChanged( const QColor& );

private slots:
    /**
     * Slot - color dialog opened.
     */
    void
    buttonClicked();

    /**
     * Slot - color selected in dialog.
     * @param
     */
    void
    selectedColor( const QColor& );

    /**
     * Slot - explicit representation(RGB values) changed.
     */
    void
    editedColor();
};

#endif /* SRC_GUI_QT_PLUGINS_ADVANCEDCOLORMAPS_RGBDEFINERWIDGET_H_ */
