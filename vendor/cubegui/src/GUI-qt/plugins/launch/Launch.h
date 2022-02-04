/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2016                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef LAUNCH_H
#define LAUNCH_H

#include <QObject>
#include <QProcess>
#include <QPair>
#include <QDebug>

class Launch : public QProcess
{
    Q_OBJECT

public:
    Launch(QObject *parent) : QProcess(parent) {};
    void launch( const QString& cmd );

signals:
    void
    receivedVar( QPair<QString, QString>);

private slots:
    void
    readStdout();
    void
    launchFinished( int exitCode );
    void
    error( QProcess::ProcessError err );

private:
    void
    writeStdout();
};

#endif // LAUNCH_H
