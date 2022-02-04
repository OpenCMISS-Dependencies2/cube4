/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2018                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef REMOTEFILEDIALOG_H
#define REMOTEFILEDIALOG_H

#include <QDialog>
#include <QTreeView>
#include "RemoteFileSystemModel.h"

namespace cubegui
{
class RemoteFileDialog : public QDialog
{
    Q_OBJECT
public:
    /**
     * @brief getFileName opens a connection to a cube server and
     * @param directory initial directory to show on the file dialog. If empty, the directory where the cube-server is started is choosen.
     * @param parent parent widget of the file dialog
     * @return filename, that the user has choosen or empty string if cancelled
     */
    static QString
    getFileName( const QString& directory = ".",
                 QWidget*       parent = NULL );

private slots:
    void
    fileSelected( QString filename );
    void
    setDirectory();
    void
    reconnect();

private:
    RemoteFileDialog( const QString& server,
                      const QString& directory = ".",
                      QWidget*       parent = NULL );
    QLineEdit*                pathLine;
    QLineEdit*                serverLine;
    QTreeView*                tree;
    RemoteFileSystemModel*    model;
    FileSortFilterProxyModel* proxy;
    QString                   filename;
    void
    disconnect();
};
}
#endif // REMOTEFILEDIALOG_H
