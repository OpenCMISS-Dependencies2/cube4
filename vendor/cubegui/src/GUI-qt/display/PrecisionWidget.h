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



#ifndef _PRECISIONWIDGET_H
#define _PRECISIONWIDGET_H

#include <QDialog>

#include "Constants.h"

class QSpinBox;
class QCheckBox;

namespace cubegui
{
class MainWidget;
class TreeItem;

// the PrecisionWidget class extends QDialog and is used for two purposes:
// 1. to allow one to open a dialog to get user-defined precision settings
// 2. to convert numbers to strings, using the current precision settings

class PrecisionWidget : public QDialog
{
    Q_OBJECT

public:
    PrecisionWidget( QWidget* parent = 0 );

    ~PrecisionWidget();

    QString
    numberToQString( double                   value,
                     bool                     integerType = false,
                     PrecisionFormat          format = FORMAT_TREES,
                     const cubegui::TreeItem* item = 0 );

    QString
    numberToQString( double          value,
                     double          referenceValue,
                     bool            scientificSmallValue = false,
                     bool            integerType = false,
                     PrecisionFormat format = FORMAT_TREES );

    /**
     * Returns representation of a number and modified unit, if "human readable" is on
     */
    QPair<QString, QString>
    numberToQStringAndUnit( double                   value,
                            const QString&           unit,
                            bool                     integerType = false,
                            PrecisionFormat          format = FORMAT_TREES,
                            const cubegui::TreeItem* item = 0 );

    // some get methods
    double
    getRoundThreshold( PrecisionFormat format );
    int
    getPrecision( PrecisionFormat format );
    int
    getRoundNr( PrecisionFormat format );
    int
    getUpperExpNr( PrecisionFormat format );

    // some set methods
    void
    setPrecision( int             value,
                  PrecisionFormat format );
    void
    setUpperExpNr( int             value,
                   PrecisionFormat format );
    void
    setRoundNr( int             value,
                PrecisionFormat format );


signals:

    void
    apply();

public slots:

    // executes the precision setting dialog
    int
    exec();

private slots:

    // sets the values as defined in the dialog;
    // called when the "ok" button is clicked
    void
    setValues();

    // forget the last changes since the dialog was opened,
    // and re-set the values;
    // this slot is called when the used clicks on "cancel"
    void
    resetValues();

    // apply the current values as shown in the dialog;
    // called when the user clocks on the "apply" button
    void
    applyValues();

private:

    // precision for the two supported formats
    int precision[ 3 ];
    // threshold for rounding to zero (in number of digits after the dot being zero)
    int roundNr[ 3 ];
    // threshold for rounding to zero (as double)
    double roundThreshold[ 3 ];
    // threshold for diplaying the number in scientific (exponent) format
    // (as number of leading digits before the dot)
    int upperExpNr[ 3 ];
    // threshold for diplaying the number in scientific (exponent) format
    // (as number)
    double upperExpThreshold[ 3 ];
    bool   humanReadable;

    // spins for the dialog
    QSpinBox*  prec0Spin, * prec1Spin;
    QSpinBox*  upper0Spin, * upper1Spin;
    QSpinBox*  round0Spin, * round1Spin;
    QCheckBox* humanFormat;

    // store values before starting a dialog for the case of "cancel"
    int v1, v2, v3, v4, v5, v6;
};
}
#endif
