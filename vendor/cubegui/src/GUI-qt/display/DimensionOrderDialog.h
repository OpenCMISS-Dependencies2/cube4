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


#ifndef DIMENSIONORDERDIALOG_H
#define DIMENSIONORDERDIALOG_H

#include <QDialog>
#include <QGroupBox>
#include <QDialogButtonBox>
#include <QRadioButton>
#include <QVBoxLayout>

#include "Constants.h"

namespace cubegui
{
/**
   Creates a dialog which allows the user to choose the order of the widgets. The default order
   is METRIC, CALL, SYSTEM
 */
class DimensionOrderDialog : public QDialog
{
    Q_OBJECT
public:
    explicit
    DimensionOrderDialog( QWidget*                  parent,
                          const QList<DisplayType>& initOrder );

    const QList<DisplayType>&
    getOrder();

private:
    QList<QList<DisplayType> > orders;
    QList<QRadioButton*>       buttons;
    QGroupBox*                 groupBox;
};
}
#endif // DIMENSIONORDERDIALOG_H
