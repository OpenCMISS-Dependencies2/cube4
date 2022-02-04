/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2016                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2009-2015                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H


#include <QDialog>
#include <QRadioButton>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QComboBox>
#include <QFormLayout>


class SettingWidget : public QDialog
{
    Q_OBJECT
public:
    explicit SettingWidget(int processesMajorCountNumDefault, int processesMinorNotchesCountDefault,
                           int measurementsMajorCountNumDefault, int measurementsMinorNotchesCountDefault, QWidget *parent = 0);

private:
    void
    initComponents();
    void
    setConnections();
    void
    setDefaultValues();

    QRadioButton* pMajorNotchesByInterval;
    QRadioButton* pMajorNotchesByCount;
    QSpinBox* pMajorNotchesIntervalSize;
    QSpinBox* pMajorNotchesCount;
    QSpinBox* pMinorNotchesCount;

    QRadioButton* mTopNotchValueAuto;
    QRadioButton* mTopNotchValueManual;
    QSpinBox* mTopNotchManualValue;

    QRadioButton* mBottomNotchValueAuto;
    QRadioButton* mBottomNotchValueManual;
    QSpinBox* mBottomNotchManualValue;

    QRadioButton* mMajorNotchesByInterval;
    QRadioButton* mMajorNotchesByCount;
    QDoubleSpinBox* mMajorNotchesIntervalSize;
    QSpinBox* mMajorNotchesCount;
    QSpinBox* mMinorNotchesCount;

    QPushButton* okButton;
    QPushButton* cancelButton;
    QPushButton* resetButton;

    int processesMajorCountNumDefault;
    int processesMinorNotchesCountDefault;
    int measurementsMajorCountNumDefault;
    int measurementsMinorNotchesCountDefault;

    bool pMajorNotchesByIntervalWasChecked;
    int pMajorNotchesCountOldValue;
    int pMajorNotchesIntervalSizeOldValue;
    int pMinorNotchesOldCount;
    bool mMajorNotchesByIntervalWasChecked;
    int mMajorNotchesCountOldValue;
    double mMajorNotchesIntervalSizeOldValue;
    int mMinorNotchesOldCount;
    bool mTopNotchValueAutoWasChecked;
    double mTopNotchValueManualOldValue;
    bool mBottomNotchValueAutoWasChecked;
    double mBottomNotchValueManualOldValue;

public slots:
    void
    setTopNotchToAutomatic();
    void
    setBottomNotchToAutomatic();
    void
    resetButtonClickHandler();

private slots:
    void
    okButtonClickHandler();
    void
    cancelButtonClickHandler();

signals:
    void
    setProcessesRulerMajorNotchesInterval(int intervalSize);
    void
    setProcessesRulerMajorNotchesCount(int count);
    void
    setProcessesRulerMinorNotchesCount(int count);
    void
    setMeasurementRulerMajorNotchesInterval(double intervalSize);
    void
    setMeasurementRulerMajorNotchesCount(int count);
    void
    setMeasurementRulerMinorNotchesCount(int count);
    void
    resetMeasurementTopNotchValue();
    void
    resetMeasurementBottomNotchValue();
    void
    setMeasurementTopNotchValue(double newValue);
    void
    setMeasurementBottomNotchValue(double newValue);

};

#endif // SETTINGSWIDGET_H
