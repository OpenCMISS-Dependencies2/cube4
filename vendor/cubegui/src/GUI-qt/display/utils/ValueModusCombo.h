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


#ifndef VALUEMODUSCOMBO_H
#define VALUEMODUSCOMBO_H

#include <QComboBox>
#include "Constants.h"

namespace cubegui
{
class ValueModusCombo : public QComboBox
{
    Q_OBJECT
public:
    ValueModusCombo( DisplayType display );
    void
    setOrder( const QList<DisplayType>& order );
    void
    setValueModus( ValueModus modus );

signals:
    void valueModusChanged( ValueModus );
    void
    externalValueModusSelected();

public slots:
    void
    updateLabel();

private slots:
    void
    setValueModus( int );

private:
    QList<DisplayType> order;
    ValueModus         valueModus;
    DisplayType        displayType;

    bool
    isOrder( DisplayType left,
             DisplayType right );
};
}

#endif // VALUEMODUSCOMBO_H
