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


#ifndef CALLTREELABELDIALOG_H
#define CALLTREELABELDIALOG_H

#include <QDialog>
#include <QCheckBox>
#include <QSlider>
#include <QLabel>

#include "Globals.h"
#include "TreeConfig.h"

namespace cubegui
{
class CallTreeLabelDialog : public QDialog
{
    Q_OBJECT
public:
    CallTreeLabelDialog( const CallDisplayConfig& config,
                         QWidget*                 parent );

private slots:
    void
    onApply();
    void
    onOk();
    void
    onCancel();

    void
    enableSlider( bool ok );
    void
    setSliderLabel();

signals:
    void
    configChanged( const CallDisplayConfig& );

private:
    QCheckBox* arguments;
    QCheckBox* ret;
    QCheckBox* hideScope;
    QCheckBox* modules;
    QSlider*   slider;
    QLabel*    sliderValue;
    QWidget*   sliderRow;

    CallDisplayConfig oldConfig;
};
}
#endif // CALLTREELABELDIALOG_H
