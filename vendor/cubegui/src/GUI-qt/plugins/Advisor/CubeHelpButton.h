/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2015-2019                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef HELP_BUTTON_WIDGET_H
#define HELP_BUTTON_WIDGET_H
#include <QPushButton>
#include <QUrl>
#include <QString>
#include "PerformanceTest.h"



namespace advisor
{
class HelpButton : public QPushButton
{
    Q_OBJECT
private:

    QString helpUrl;


private slots:

    void
    showHelp( bool );

public:
    HelpButton( const QString& help,
                bool           active = true );

    virtual
    ~HelpButton()
    {
    }
};
};

#endif // ADVISER_RATING_WIDGET_H
