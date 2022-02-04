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


#ifndef REMOTEFILESYSTEMMODEL2_H
#define REMOTEFILESYSTEMMODEL2_H

#include <QAbstractItemModel>
#include <QFileIconProvider>
#include <QSortFilterProxyModel>
#include <QDebug>

#include "CubeFileSystemRequest.h"
#include "CubeClientConnection.h"


namespace cubegui
{
enum FSColumns { NAME, SIZE, DATE, COLUMNS };
enum UserRoles { LSIZE = Qt::UserRole, LDATE, IS_DIR };

/**
 * @class RemoteFileSystemModel
 * @brief The RemoteFileSystemModel class connects to a cube server to get directory contents of the remote file system
 */
class RemoteFileSystemModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    /**
     * @brief RemoteFileSystemModel
     * @param serverUrl url of the cube server, e.g. cube://localhost:3300
     * @param directory initial directory on the server
     */
    RemoteFileSystemModel( const QString& serverUrl );

public:
    // QAbstractItemModel interface
    virtual QModelIndex
    index( int                row,
           int                column,
           const QModelIndex& parent ) const;
    virtual QModelIndex
    parent( const QModelIndex& child ) const;
    virtual int
    rowCount( const QModelIndex& parent ) const;
    virtual int
    columnCount( const QModelIndex& parent ) const;
    virtual QVariant
    data( const QModelIndex& index,
          int                role ) const;
    virtual Qt::ItemFlags
    flags( const QModelIndex& index ) const;
    virtual bool
    hasChildren( const QModelIndex& parent ) const;
    virtual QVariant
    headerData( int             section,
                Qt::Orientation orientation,
                int             role ) const;

signals:
    /**
     * @brief fileSelected is emitted, if the item of @see itemActivated is a file
     * @param filename filename of the selected item
     */
    void
    fileSelected( QString filename );
    void
    directorySelected( QString dirname );

public slots:
    /**
     * @brief itemActivated handles user selection. If the selected item is a directory, the
     * model is filled with the directory contents, otherwise @see fileSelected is emiited.
     * @param index the selected item of the model
     */
    void
    itemActivated( QModelIndex index );

    /**
     * @brief setDirectory sets the model contents to the contents of the given directory
     * @param directoryPath
     */
    void
    setDirectory( QString directoryPath );

private:
    cube::ClientConnection::Ptr clientConnection; // client connection to cube server
    std::vector<cube::FileInfo> files;            // directory contents
    QFileIconProvider           iconProvider;

    cube::FileInfo*
    getFile( const QModelIndex& idx ) const;

    std::vector<cube::FileInfo>
    getDirectoryContents( QString path );
};

/**
 * @brief The FileSortFilterProxyModel class sorts the items of RemnoteFileSystemModel
 */
class FileSortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public slots:
    void
    itemActivated( QModelIndex index )    // calls itemActivated in source model
    {
        RemoteFileSystemModel* model = dynamic_cast<RemoteFileSystemModel*> ( sourceModel() );
        model->itemActivated( this->mapToSource( index ) );
    }

protected:
    bool
    lessThan( const QModelIndex& left,
              const QModelIndex& right ) const;
};
}
#endif // REMOTEFILESYSTEMMODEL_H
