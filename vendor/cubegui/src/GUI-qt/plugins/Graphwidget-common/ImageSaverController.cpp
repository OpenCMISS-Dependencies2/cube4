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


#include "config.h"

#include "ImageSaverController.h"
#include <QFileDialog>
#include <QImage>
#include <QPainter>
#include <QTemporaryFile>
#include <QPrinter>


ImageSaverController::ImageSaverController( AbstractPlotAreaController* abstractPlotAreaController,
                                            HorizontalRulerController*  horizontalRulerController,
                                            VerticalRulerController*    verticalRulerController,
                                            QWidget*                    parent ) : QObject( parent )
{
    this->abstractPlotAreaController = abstractPlotAreaController;
    this->horizontalRulerController  = horizontalRulerController;
    this->verticalRulerController    = verticalRulerController;
}

ImageSaverController::~ImageSaverController()
{
    this->abstractPlotAreaController = NULL;
    this->horizontalRulerController  = NULL;
    this->verticalRulerController    = NULL;
}

void
ImageSaverController::saveImage()
{
    QFileDialog fileDialog( ( QWidget* )this->parent(), tr( "Export Graph ..." ) );
    fileDialog.setAcceptMode( QFileDialog::AcceptSave );
    QStringList filters;
#if QT_VERSION < 0x050000
    filters << "PNG (*.png)" << "JPEG (*.jpg)" << "EPS (*.eps)";
#else
    filters << "PNG (*.png)" << "JPEG (*.jpg)";
#endif
    fileDialog.setDefaultSuffix( "png" );
    fileDialog.setNameFilters( filters );
    QString filePath;
    if ( fileDialog.exec() == QDialog::Rejected )
    {
        return;
    }
    QString selectedFilter = fileDialog.selectedNameFilter();
    QString fileName       = fileDialog.selectedFiles()[ 0 ];
    // don't add suffix
    filePath = fileName;     // + selectedFilter.right(5).left(4);
    if ( filePath.endsWith( ".png" ) || filePath.endsWith( ".jpg" ) )
    {
        int    totalWidth  = verticalRulerController->getView()->width() + abstractPlotAreaController->getView()->width();
        int    totalHeight = horizontalRulerController->getView()->height() + abstractPlotAreaController->getView()->height();
        QImage image( QSize( totalWidth, totalHeight ), QImage::Format_RGB32 );
        image.fill( Qt::white );
        QPainter painter( &image );
        painter.setOpacity( 0.0 );
        QRect viewport;
        viewport.setTop( 0 );
        viewport.setLeft( verticalRulerController->getView()->width() );
        viewport.setWidth( horizontalRulerController->getView()->width() );
        viewport.setHeight( horizontalRulerController->getView()->height() );
        horizontalRulerController->getView()->doPaint( viewport, &painter );
        painter.resetTransform();
        viewport.setTop( horizontalRulerController->getView()->height() );
        viewport.setLeft( 0 );
        viewport.setWidth( verticalRulerController->getView()->width() );
        viewport.setHeight( verticalRulerController->getView()->height() );
        verticalRulerController->doPaint( viewport, painter );
        painter.resetTransform();
        viewport.setTop( horizontalRulerController->getView()->height() );
        viewport.setLeft( verticalRulerController->getView()->width() );
        viewport.setWidth( abstractPlotAreaController->getView()->width() );
        viewport.setHeight( abstractPlotAreaController->getView()->height() );
        painter.setRenderHint( QPainter::Antialiasing, false );
        abstractPlotAreaController->getView()->doPaint( viewport, &painter );

        image.save( filePath );
    }
#if QT_VERSION < 0x050000
    else if ( filePath.endsWith( ".eps" ) )
    {
        // 1. Create a printer
        QPrinter epsPrinter( QPrinter::PrinterResolution );
        QPainter painter;
        QFile    imageFile( filePath );
        imageFile.open( QIODevice::WriteOnly );
        epsPrinter.setOutputFileName( imageFile.fileName() );
        epsPrinter.setOutputFormat( QPrinter::PostScriptFormat );
        epsPrinter.setFullPage( false );
        // although the size is fixed, but it does not matter actually since an EPS image is actually lossless
        epsPrinter.setPaperSize( QSize( 190, 190 ), QPrinter::Millimeter );
        // 2. Print!;
        QSize abstractPlotAreaControllerOldSize = abstractPlotAreaController->getView()->size();
        QSize verticalRulerControllerOldSize    = verticalRulerController->getView()->size();
        QSize horizontalRulerControllerOldSize  = horizontalRulerController->getView()->size();
        abstractPlotAreaController->getView()->resize( 400, 400 );
        verticalRulerController->getView()->resize( verticalRulerController->getView()->width(), 400 );
        horizontalRulerController->getView()->resize( 400, horizontalRulerController->getView()->height() );
        painter.begin( &epsPrinter );
        QRect viewport;
        viewport.setTop( 0 );
        viewport.setLeft( verticalRulerController->getView()->width() );
        viewport.setWidth( horizontalRulerController->getView()->width() );
        viewport.setHeight( horizontalRulerController->getView()->height() );
        horizontalRulerController->getView()->doPaint( viewport, &painter );
        painter.resetTransform();
        viewport.setTop( horizontalRulerController->getView()->height() );
        viewport.setLeft( 0 );
        viewport.setWidth( verticalRulerController->getView()->width() );
        viewport.setHeight( verticalRulerController->getView()->height() );
        verticalRulerController->doPaint( viewport, painter );
        painter.resetTransform();

        viewport.setTop( horizontalRulerController->getView()->height() );
        viewport.setLeft( verticalRulerController->getView()->width() );

        viewport.setWidth( abstractPlotAreaController->getView()->width() );
        viewport.setHeight( abstractPlotAreaController->getView()->height() );
        //qDebug() << "SAVE:";
        abstractPlotAreaController->getView()->doPaint( viewport, &painter );
        painter.end();
        ( void )abstractPlotAreaControllerOldSize;
//		((BarsPlotArea*)(abstractPlotAreaController->getView()))->resize(abstractPlotAreaControllerOldSize);
        verticalRulerController->getView()->resize( verticalRulerControllerOldSize );
        horizontalRulerController->getView()->resize( horizontalRulerControllerOldSize );
        imageFile.close();
    }
#endif
}
