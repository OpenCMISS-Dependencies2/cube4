/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2017                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef VALUEVIEWCONFIG_H
#define VALUEVIEWCONFIG_H

#include <QDialog>
#include <QComboBox>
#include "CubeTypes.h"

namespace cubepluginapi
{
class ValueView;
}

namespace cubegui
{
class Tree;

class ValueViewConfig : public QDialog
{
public:
    Q_OBJECT
public:
    ValueViewConfig( const QList<cube::DataType> usedTypes,
                     cube::DataType              currentType,
                     QWidget*                    parent );
public slots:
    void
    close();

private slots:
    void
    accept();
    void
    apply();
    void
    reject();

    void
    typeChanged( int index );
    void
    viewChanged( int index );
    void
    metricChanged( Tree* );

private:
    QList<cube::DataType>            typeList;
    QList<cubepluginapi::ValueView*> viewList;
    QComboBox*                       viewCombo;
    QComboBox*                       typeCombo;
    QWidget*                         configWidget;

    QHash<cube::DataType, cubepluginapi::ValueView*> previousValueViews;
};
}

#endif // VALUEVIEWCONFIG_H
