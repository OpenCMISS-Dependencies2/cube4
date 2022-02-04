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


#ifndef IMAGESAVERCONTROLLER_H
#define IMAGESAVERCONTROLLER_H

#include <QObject>
#include "AbstractPlotAreaController.h"
#include "HorizontalRulerController.h"
#include "VerticalRulerController.h"

/**
 * This class is responsible for saving an image of the plot. It can save the image in two formats: JPEG and EPS.
 * It needs three references to the three controllers that control how the horizontal ruler, vertical ruler, and plot area are displayed.
 * It calls each of the controllers and tells it to draw its view on a specific part of the image file.
 * @see BarsPlotImageSaver::saveImage method.
 */
class ImageSaverController : public QObject {
    Q_OBJECT
public:
    /**
     * Creates a new instance
     * @param barsPlotAreaController Reference to BarsPlotAreaController instance
     * @param horizontalRulerController Reference to HorizontalRulerController instance
     * @param verticalRulerController Reference to VerticalRulerController instance
     * @param parent Parent of the instance
     */
    explicit ImageSaverController(AbstractPlotAreaController* abstractPlotAreaController,
            HorizontalRulerController* horizontalRulerController,
            VerticalRulerController* verticalRulerController, QWidget *parent =
                    0);
    /**
     * Upon destruction, all controller references are set to NULL
     */
    virtual ~ImageSaverController();
public slots:
    /**
     * Pops up a save dialog with two kinds of images: JPEG (lossy) and EPS (lossless).It basically creates an image file and attaches a painter to it, and queries each of the controllers to draw on a specific region of the file.
     */
    void saveImage();
private:
    AbstractPlotAreaController* abstractPlotAreaController;
    HorizontalRulerController* horizontalRulerController;
    VerticalRulerController* verticalRulerController;
};

#endif // IMAGESAVERCONTROLLER_H
