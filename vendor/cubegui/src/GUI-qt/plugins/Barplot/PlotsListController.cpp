/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2020                                                **
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

#include "PlotsListController.h"
#include <QColor>
#include <QDebug>
#include <QMessageBox>

PlotsListController::PlotsListController( DataProvider* model,
                                          QWidget*      parent ) : QWidget( parent )
{
    this->model = model;
}
void
PlotsListController::setData( QList<CubeDataItem>& data )
{
    this->data = data;
    if ( data.size() == 0 )
    {
        //  qDebug()<<"No data";
        return;
    }
    // / determine the maximum zOrder of visible data items
    int maxZOrder = -1;
    for ( QList<CubeDataItem>::const_iterator i = data.constBegin(); i != data.constEnd(); i++ )
    {
        if ( ( *i ).isVisible() )
        {
            maxZOrder = ( *i ).getZOrder();
            break;
        }
    }
    // / while finding the max element, set the zOrder of any invisible data item to -1
    // / this guarantees that no two data items will get the same (positive) zOrder
    for ( int i = 0; i < data.size(); i++ )
    {
        if ( data[ i ].isVisible() )
        {
            maxZOrder = qMax( maxZOrder, data[ i ].getZOrder() );
        }
        else
        {
            data[ i ].setZOrder( -1 );
        }
    }
    // / clean up the view from any existing data items
    //  view->getListView()->removeAllItems();
    // / fix zOrder for any new visible item (the zOrder of any new item is maximum zOrder of exiting elements + 1)
    for ( int i = 0; i < data.size(); i++ )
    {
        if ( data[ i ].isVisible() )
        {
            if ( data[ i ].getZOrder() == -1 )
            {
                data[ i ].setZOrder( ++maxZOrder );
            }
            plotWidget::MathOp itemOpType = data[ i ].getMathOp();
            if ( itemOpType < plotWidget::Minimum || itemOpType >= plotWidget::MathOpUnknown )
            {
                qDebug() << tr( "Trying to add a plot to plot-list of unknown math operation type!" );
            }
            // view->getListView()->addItem(data[i].getName() + ", " + plotWidget::CALC_NAMES[static_cast<int>(itemOpType)].toLower(), data[i].getColor(), data[i].getStyle(), data[i].getZOrder());
        }
    }
}
void
PlotsListController::deleteItemHandler()
{
    model->removeCubeDataElement();
}
