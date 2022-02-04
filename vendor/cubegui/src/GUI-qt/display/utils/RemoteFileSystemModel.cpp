/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2020                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include "config.h"
#include <QDebug>
#include <QImage>
#include <QDateTime>
#include <algorithm>

#include "RemoteFileSystemModel.h"
#include "CubeError.h"
#include "CubeClientCallbackData.h"
#include "CubeClientConnection.h"
#include "CubeDisconnectRequest.h"
#include "CubeNetworkRequest.h"
#include "CubeSocket.h"
#include "Globals.h"

using namespace cubegui;
using namespace cube;
using namespace std;

/** creates a client connection to a cube server, which is closed if this object is deleted
 */
RemoteFileSystemModel::RemoteFileSystemModel( const QString& serverUrl ) :
    clientConnection( cube::ClientConnection::create( Socket::create(), serverUrl.toStdString() ) )
{
}

void
RemoteFileSystemModel::setDirectory( QString directoryPath )
{
    std::vector<FileInfo> oldFiles = files;

    try
    {
        files = getDirectoryContents( directoryPath );
        if ( files.size() > 0 )
        {
            QString directory = files.at( 0 ).name().c_str();
            files.erase( files.begin() );
            emit( directorySelected( directory ) );
            emit beginResetModel();
            emit endResetModel();
        }
        else
        {
            files = oldFiles;
            Globals::setStatusMessage( QString( tr( "Cannot read directory " ) ) + directoryPath );
        }
    }
    catch ( const cube::NetworkError& e )
    {
        files.clear();
        emit beginResetModel();
        emit endResetModel();
        Globals::setStatusMessage( e.what(), Error );
    }
}

void
RemoteFileSystemModel::itemActivated( QModelIndex index )
{
    FileInfo* file = getFile( index );

    if ( !file->isDirectory() )
    {
        emit fileSelected(  file->name().c_str() );
    }
    else
    {
        setDirectory( file->name().c_str() );
    }
}

/** reads directory contents from cube server
 * @param path server side directory
 */
vector<FileInfo> RemoteFileSystemModel::getDirectoryContents( QString path )
{
    NetworkRequest::Ptr request = FileSystemRequest::create( path.toStdString() );
    FileSystemRequest*  fr      = dynamic_cast<FileSystemRequest*>( request.get() );
    request->sendRequest( *clientConnection, NULL );
    request->receiveResponse( *clientConnection, NULL );

    std::vector<FileInfo> allFiles = fr->getFiles(); // all files
    std::vector<FileInfo> files;                     // non-hidden files
    for ( FileInfo file : allFiles )
    {
        QString filename = QFileInfo( file.name().c_str() ).fileName();
        if ( ( filename.at( 0 ) != '.' ) || ( filename == ".." ) )
        {
            files.push_back( file );
        }
    }
    return files;
}

FileInfo*
RemoteFileSystemModel::getFile( const QModelIndex& idx ) const
{
    if ( !idx.isValid() ) // invalid index is used for tree root
    {
        return NULL;
    }
    else
    {
        return static_cast<FileInfo*>( idx.internalPointer() );
    }
}

QModelIndex
RemoteFileSystemModel::index( int row, int column, const QModelIndex& parent ) const
{
    if ( !hasIndex( row, column, parent ) )
    {
        return QModelIndex();
    }

    return createIndex( row, column, ( void* )&files.at( row ) );
}

QModelIndex
RemoteFileSystemModel::parent( const QModelIndex& ) const
{
    return QModelIndex();
}

int
RemoteFileSystemModel::rowCount( const QModelIndex& index ) const
{
    if ( index.isValid() )
    {
        return 0;                  // don't show contents of subdirectories
    }
    return files.size();
}

int
RemoteFileSystemModel::columnCount( const QModelIndex& ) const
{
    return COLUMNS;
}

QString
fileSize( long size )
{
    double      num = size;
    QStringList list;
    list << QObject::tr( "KB" ) << QObject::tr( "MB" ) << QObject::tr( "GB" ) << QObject::tr( "TB" );

    QStringListIterator i( list );
    QString             unit( QObject::tr( "bytes" ) );

    while ( num >= 1024.0 && i.hasNext() )
    {
        unit = i.next();
        num /= 1024.0;
    }
    return QString().setNum( num, 'f', 2 ) + " " + unit;
}

QVariant
RemoteFileSystemModel::data( const QModelIndex& index, int role ) const
{
    if ( !index.isValid() || index.model() != this )
    {
        return QVariant();
    }
    FileInfo* file = getFile( index );
    if ( !file )
    {
        return QVariant();
    }
    switch ( role )
    {
        case Qt::EditRole:
        case Qt::DisplayRole:
        {
            switch ( index.column() )
            {
                case NAME:
                    return QFileInfo( file->name().c_str() ).fileName();
                case SIZE:
                    return file->isDirectory() ? "" : fileSize( file->size() );
                case DATE:
#if QT_VERSION >= 0x050800
                    return QDateTime::fromSecsSinceEpoch( file->time() ).toString( "dd.MM.yyyy HH:mm" );
#else
                    return QDateTime::fromTime_t( file->time() ).toString( "dd.MM.yyyy HH:mm" );
#endif
            }
            break;
        }
        case Qt::DecorationRole:
            if ( index.column() == 0 )
            {
                QIcon icon;

                if ( file->isDirectory() )
                {
                    icon = iconProvider.icon( QFileIconProvider::Folder );
                }
                else
                {
                    icon = iconProvider.icon( QFileIconProvider::File );
                }
                return icon;
            }
            break;
        case Qt::TextAlignmentRole:
            if ( index.column() == 1 )
            {
                return ( int )( Qt::AlignVCenter | Qt::AlignRight ); // align file size right and vertically centered
            }
            break;
        case LSIZE:
            return QVariant( static_cast<qlonglong> ( file->size() ) );
        case LDATE:
            return QVariant( static_cast<qlonglong> ( file->time() ) );
        case IS_DIR:
            return QVariant( file->isDirectory() );
        default:
            return QVariant();
    }


    return QVariant();
}

Qt::ItemFlags
RemoteFileSystemModel::flags( const QModelIndex& ) const
{
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

bool
RemoteFileSystemModel::hasChildren( const QModelIndex& parent ) const
{
    return QAbstractItemModel::hasChildren( parent );
}

QVariant
RemoteFileSystemModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    switch ( role )
    {
        case Qt::DecorationRole:
            break;
        case Qt::TextAlignmentRole:
            return Qt::AlignLeft;
    }
    if ( role != Qt::DisplayRole )
    {
        return QAbstractItemModel::headerData( section, orientation, role );
    }

    QString returnValue;
    switch ( section )
    {
        case NAME:
            returnValue = tr( "Name" );
            break;
        case SIZE:
            returnValue = tr( "Size" );
            break;
        case DATE:
            returnValue = tr( "Date modified" );
            break;
    }
    return returnValue;
}

// --- FileSortFilterProxyModel ---

bool
FileSortFilterProxyModel::lessThan( const QModelIndex& left, const QModelIndex& right ) const
{
    if ( left.column() == NAME )
    {
        QString first     = sourceModel()->data( left ).toString();
        QString second    = sourceModel()->data( right ).toString();
        bool    firstDir  = sourceModel()->data( left, IS_DIR ).toBool();
        bool    secondDir = sourceModel()->data( right, IS_DIR ).toBool();
        if ( firstDir && !secondDir )
        {
            return true;
        }
        else if ( !firstDir && secondDir )
        {
            return false;
        }
        else
        {
            return first < second;
        }
    }
    else if ( left.column() == SIZE )
    {
        return sourceModel()->data( left, LSIZE ).toLongLong() < sourceModel()->data( right, LSIZE ).toLongLong();
    }
    else if ( left.column() == DATE )
    {
        return sourceModel()->data( left, LDATE ).toLongLong() < sourceModel()->data( right, LDATE ).toLongLong();
    }
    return true;
}
