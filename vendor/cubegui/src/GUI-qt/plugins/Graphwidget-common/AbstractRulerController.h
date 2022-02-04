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


#ifndef ABSTRACTRULERCONTROLLER_H
#define ABSTRACTRULERCONTROLLER_H


#include <QObject>
#include "AbstractRuler.h"
#include "DataProvider.h"
#include <QMap>
#include <QList>

class AbstractRulerController : public QObject
{
    static const int BEST_CUT_LENGTH_DIVIDER;
    static const int MINOR_STEPS_DEFAULT_COUNT;

    Q_OBJECT
public:
    virtual ~AbstractRulerController();
    explicit AbstractRulerController(QWidget *parent = 0);
    virtual AbstractRuler* getView();
    virtual void doPaint(QRect viewport, QPainter& painter) = 0;
    virtual int pixelCoordToRulerCoord(int pixelCoord, bool oppositeDirection = false) = 0;
    virtual double pixelCoordToRulerCoordF(int pixelCoord, bool oppositeDirection = false) = 0;

public slots:
    void setLimits(int minimumLimit, int maximumLimit);
    void setLimitsF(double minimumLimit, double maximumLimit);
    void setCalibrateStatus(bool tempCalibrateStatus);
    void setRealValuedNotches(bool isRealValuedNotches);
    void setMajorNotchesCount(int majorNotchesCustomCount);
    void setMinorNotchesCount(int minorNotchesCount);
    void setMajorNotchesInterval(int intervalSize);
    void setMajorNotchesIntervalF(double intervalSize);
    void setMajorAndMinorNotchesToDefault();
    bool isMajorNotchesUsingIntervals();
    int getMajorNotchesCount();
    int getMinorNotchesCount();
    int getMajorNotchesInterval();
    void setTitle(QString title, plotWidget::Alignment align=plotWidget::Center);
    QString getTitle();
    plotWidget::Alignment getTitleAlignment();
    void setTitleAlignmnt(plotWidget::Alignment);
    void setNotchesAlignment(plotWidget::Alignment);
    plotWidget::Alignment getNotchesAligment();
    void setTitleVisible(bool isVisible);
    void forceIndexAtMousePositionChangedSignal(QMouseEvent*);

protected:
    QString title;
    plotWidget::Alignment titleAlignment;
    bool drawTitle;
    double minimumLimit, maximumLimit;
    AbstractRuler* view;
    void initComponents();
    void setConnections();
    void calibrateAllNotches(double rulerLength);

private:
    int minorNotchesCount;
    bool isMajorNotchesCountCustom;
    int majorNotchesCustomCount;
    double majorNotchesIntervalSizeInProcessesOrPixelsF;
    bool graphOnDisplay;
    int correctStepSize(int totalSize, int stepSize);
    /**
      Calculates the best cut length based of the interval limits.
      \param minimumLimit minimum limit value
      \param maximumLimit maximum limit value
      \param rulerLengthInPixels length of the ruler
      returns Best cut length in pixels
    */
    int bestCutLengthHeuristic(double minimumLimit, double maximumLimit, int rulerLengthInPixels);
    bool isRealValuedNotches;
protected slots:
    void calibrateAllNotches();
private slots:
    void roundNotchToInteger(double notchValueF, int notchLocation, int maxNotchLocationInPixels, int &newNotchValue, int& newNotchLocation);
    void sizeChangedHandler();

signals:
    void majorNotchesLocationsChanged(QList<int>);
    void minorNotchesLocationsChanged(QList<int>);
};

#endif // ABSTRACTRULERCONTROLLER_H
