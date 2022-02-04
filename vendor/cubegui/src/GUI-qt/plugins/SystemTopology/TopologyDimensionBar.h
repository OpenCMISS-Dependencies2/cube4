/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2019                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/




#ifndef TOPOLOGYDIMENSIONBAR_H
#define TOPOLOGYDIMENSIONBAR_H

#include "OrderWidget.h"
#include "DimensionSelectionWidget.h"
#include <QStackedLayout>
#include <QRadioButton>
#include <QRadioButton>
#include <QSettings>
#include <QGroupBox>
#include <QLineEdit>

/**
   creates a button bar for topologies with more than tree dimensions
   view1: create ValuePopSlider widgets to select the dimensions to display
   view2: merge selected dimensions into one
 */
class TopologyDimensionBar : public QWidget
{
    Q_OBJECT
public:
    explicit
    TopologyDimensionBar( const std::vector<long>&        dims,
                          const std::vector<std::string>& dimnames,
                          QWidget*                        parent = 0 );
    std::vector<std::vector<int> >
    getFoldingVector();

    bool
    loadSettings( QSettings& settings,
                  int        topologyId );
    void
    saveSettings( QSettings& settings,
                  int        topologyId );

signals:
    void
    foldingDimensionsChanged( std::vector<std::vector<int> >);
    void
    selectedDimensionsChanged( std::vector<long>);
    void
    splitLengthChanged( int length );
    void
    toggleAxis();

private slots:
    void
    selectedDimensionsChanged();
    void
    foldingDimensionsChanged();
    void
    handleSplitLengthChanged();
    void
    onToggleAxis();

private:
    void
    setAxisLabel( QString icon = QString( ":/images/folding_xyz_small.png" ) );

    std::vector<long>         dims_;
    OrderWidget*              order;         // used for folding mode
    DimensionSelectionWidget* selection;
    QStackedLayout*           modeSelection; // select view: selectionWidget or foldingWidget
    QRadioButton*             foldBut;
    QRadioButton*             sliceBut;
    QLabel*                   axisLabel;
    QPushButton*              toggleAxisButton;
    QLineEdit*                splitLengthInput;

    bool axisToggled;
};

#endif // TOPOLOGYDIMENSIONBAR_H
