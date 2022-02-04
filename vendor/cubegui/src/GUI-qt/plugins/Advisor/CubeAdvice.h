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


#ifndef ADVICE_H
#define ADVICE_H
#include "PerformanceTest.h"



namespace advisor
{
class AdvisorAdvice
{
private:


private slots:


public:
    AdvisorAdvice()
    {
    };

    cubegui::TreeItem* callpath;
    QList<QString>     comments;
};
};

#endif // ADVISER_RATING_WIDGET_H
