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


#ifndef HEATMAPSETTINGSWIDGET_H
#define HEATMAPSETTINGSWIDGET_H

#include <QDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QLineEdit>
#include <QGroupBox>
#include <QComboBox>

class HeatmapCustomizationzation: public QDialog {
    Q_OBJECT
public:
    /**
     * Creates a new HeatmapCustomizationzation object
     * @param isHCount indicates whether the second parameter is the count of horizontal major notches or the
     * number of processes between two horizontal major notches
     * @param majorHNotches The number of the horizontal major notches (should be positive), or the number of processes
     * between two horizontal major notches
     * @param minorHNotchesCount The number of the horizontal minor notches (should be positive)
     * @param isVCount indicates whether the second parameter is the count of vertical major notches or the
     * number of processes between two vertical major notches
     * @param majorVNotches The number of the vertical major notches (should be positive), or the number of processes
     * between two vertical major notches
     * @param minorVNotchesCount The number of the vertical minor notches (should be positive)
     * @param parent The parent (could be null)
     */
    explicit HeatmapCustomizationzation(bool isHCount, int majorHNotches,
            int minorHNotchesCount, bool isVCount, int majorVNotches,
            int minorVNotchesCount, QWidget *parent = 0);
    ~HeatmapCustomizationzation();
    void setInitialHMajorIntervalAndMinorCount(int major_interval, int minor_count);
    void setInitialHMajorCountAndMinorCount(int major_interval, int minor_count);
    void setInitialVMajorIntervalAndMinorCount(int major_interval, int minor_count);
    void setInitialVMajorCountAndMinorCount(int major_interval, int minor_count);

signals:
    void setHMajorFixedInterval(int stepsCount);
    void setHMajorStepsCount(int stepsCount);
    void setHMinorStepsCount(int stepsCount);
    void setVMajorFixedInterval(int stepsCount);
    void setVMajorStepsCount(int stepsCount);
    void setVMinorStepsCount(int stepsCount);
    void resetAllValues();
 //   void setMinColor(QColor);
 //   void setMaxColor(QColor);

private:
    QPushButton* btn_ok;
    QPushButton* btn_cancel;
    QPushButton* btn_reset;

    QLabel* lbl_h_major;
    QLabel* lbl_h_major_desc_1_0;
    QLabel* lbl_h_major_desc_1_1;
    QLabel* lbl_h_major_desc_2;

    QLabel* lbl_h_minor;
    QLabel* lbl_h_minor_desc;

    QRadioButton* radio_h_major_1_interval;
    QRadioButton* radio_h_major_2_count;

    QHBoxLayout* h_h_major_1;
    QHBoxLayout* h_h_major_2;
    QHBoxLayout* h_h_minor;
    QHBoxLayout* h_buttons;

    QSpinBox* spin_h_major_1;
    QSpinBox* spin_h_major_2;
    QSpinBox* spin_h_minor;

    QLabel* lbl_v_major;
    QLabel* lbl_v_major_desc_1_0;
    QLabel* lbl_v_major_desc_1_1;
    QLabel* lbl_v_major_desc_2;

    QLabel* lbl_v_minor;
    QLabel* lbl_v_minor_desc;

    QRadioButton* radio_v_major_1_interval;
    QRadioButton* radio_v_major_2_count;

    QHBoxLayout* h_v_major_1;
    QHBoxLayout* h_v_major_2;
    QHBoxLayout* h_v_minor;

    QSpinBox* spin_v_major_1;
    QSpinBox* spin_v_major_2;
    QSpinBox* spin_v_minor;

    QVBoxLayout* vertical_layout_h;
    QVBoxLayout* vertical_layout_v;

    void initComponents();
    void setConnections();

    QGroupBox* h_grp;
    QGroupBox* v_grp;
    QGroupBox* col_grp;

    QVBoxLayout* main_layout;

  //  QComboBox* minColorList;
//    QComboBox* maxColorList;

private slots:
    void handleOKButton();
    void handleCancelButton();
    void handleResetToDefaultsButton();

private:
    int oldHMajorInterval;
    int oldHMajorCount;
    int oldHMinorCount;
    int initHMinorCount;
    int initHMajorInterval;
    int initHMajorCount;
    int oldVMajorInterval;
    int oldVMajorCount;
    int oldVMinorCount;
    int initVMinorCount;
    int initVMajorInterval;
    int initVMajorCount;
  //  int oldMinColorIndex;
   // int oldMaxColorIndex;
};


#endif // HEATMAPSETTINGSWIDGET_H
