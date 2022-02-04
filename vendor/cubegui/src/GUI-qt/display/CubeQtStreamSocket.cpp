/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2020                                                     **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


/*-------------------------------------------------------------------------*/
/**
 *  @file
 *  @ingroup CUBE_gui.network
 *  @brief   Definition of the class cube::QtStreamSocket.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include "CubeQtStreamSocket.h"

#include <QApplication>
#include <QEventLoop>
#include <QMetaType>
#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>
#include <QTimer>
#include <QtGlobal>
#include <QDebug>
#include <unistd.h>

#include "CubeError.h"
#include "CubeNetworkProxy.h"

using namespace std;
using namespace cube;

#if QT_VERSION < 0x050000 // already implemented in Qt5
Q_DECLARE_METATYPE( QAbstractSocket::SocketError );
Q_DECLARE_METATYPE( QAbstractSocket::SocketState );
#endif

#define SETUP_EVENT_LOOP_FOR( object, signal )                          \
    QTimer timer;                                                     \
    timer.setSingleShot( true );                                        \
    QEventLoop loop;                                                  \
    loop.connect( &timer, SIGNAL( timeout() ), SLOT( quit() ) );            \
    loop.connect( object, SIGNAL( signal ), SLOT( quit() ) );               \
    loop.connect( object, SIGNAL( error( QAbstractSocket::SocketError ) ), \
                  SLOT( quit() ) );

#define PROCESS_EVENT_LOOP() \
    loop.exec();

#define PROCESS_EVENT_LOOP_WITH_TIMEOUT( timeout ) \
    timer.start( timeout );                        \
    loop.exec();

#define ASSERT_STATE_OF( socket, socket_state )                     \
    if ( socket->state() != socket_state )                          \
    {                                                             \
        throw UnrecoverableNetworkError( QObject::tr( "Connection timed out." ).toUtf8().data() ); \
    }

namespace
{
// default socket timeout of 1 second
int DEFAULT_TIMEOUT = 1000;
}

QtStreamSocket* QtStreamSocket::single = nullptr;

SocketPtr
QtStreamSocket::create()
{
    qRegisterMetaType< QAbstractSocket::SocketError >(
        "QAbstractSocket::SocketError" );
    qRegisterMetaType< QAbstractSocket::SocketState >(
        "QAbstractSocket::SocketState" );

    CubeNetworkProxy::exceptionPtr = nullptr;
    /*
     * A QTcpSocket cannot be accessed from different threads. When QtStreamSocket is created, a new thread (socketIOThread) is created
     * and it's thread affinity moved to the new thread. All read/write operations will be processed in socketIOThread.
     * If methods of CubeProxy are called from other threads, signals are used to replace direct calls.
     */
    QtStreamSocket* qss = new QtStreamSocket();
    single              = qss;
    qss->socketIOThread = new QThread();
    QObject::connect( qss->socketIOThread, SIGNAL( finished() ), qss->socketIOThread, SLOT( deleteLater() ) );
    qss->moveToThread( qss->socketIOThread );
    qss->socketIOThread->start();
    QObject::connect( qss, SIGNAL( connectSignal( const QString &, int ) ), qss, SLOT( socketConnect( const QString &, int ) ),
                      Qt::BlockingQueuedConnection );
    QObject::connect( qss, SIGNAL( receiveSignal( void*, size_t ) ), qss, SLOT( socketReceive( void*, size_t ) ),
                      Qt::BlockingQueuedConnection );
    QObject::connect( qss, SIGNAL( sendSignal( const void*, size_t ) ), qss, SLOT( socketSend( const void*, size_t ) ),
                      Qt::BlockingQueuedConnection );
    QObject::connect( qss, SIGNAL( disconnectSignal() ), qss, SLOT( disconnectSlot() ),
                      Qt::BlockingQueuedConnection );

    return SocketPtr( qss  );
}


void
QtStreamSocket::connect( const std::string& address,
                         int                port )
{
    // calls socketConnect() in main thread, waits for slot to be finished (Qt::BlockingQueuedConnection)
    emit( connectSignal( address.c_str(), port ) );
}

void
QtStreamSocket::socketConnect( const QString& address,
                               int            port )
{
    if ( mSocket )
    {
        if ( mSocket->isOpen() )
        {
            disconnect();
        }
    }
    else
    {
        mSocket = new QTcpSocket();
    }

    if ( !mSocket )
    {
        throw UnrecoverableNetworkError( QObject::tr( "Cannot connect on invalid socket." ).toUtf8().data() );
    }

    mSocket->connectToHost( address, port );
    mSocket->waitForConnected( 1000 );

    try
    {
        if ( mSocket->state() != QAbstractSocket::ConnectedState )
        {
            throw UnrecoverableNetworkError( QObject::tr( "Connection timed out." ).toUtf8().data() );
        }
        if ( !mSocket->isValid() )
        {
            throw UnrecoverableNetworkError( mSocket->errorString().toStdString() );
        }
    }
    catch ( const NetworkError& )
    {
        // exceptions have to be handled in a different thread
        CubeNetworkProxy::exceptionPtr = std::current_exception();
    }
}


void
QtStreamSocket::disconnect()
{
    emit disconnectSignal();
}


void
QtStreamSocket::disconnectSlot()
{
    if ( !mSocket || !mSocket->isValid() || !mSocket->isOpen() )
    {
        throw UnrecoverableNetworkError(
                  QObject::tr( "Attempting to disconnect invalid socket." ).toUtf8().data() );
    }

    SETUP_EVENT_LOOP_FOR( mSocket, disconnected() );

    mSocket->disconnectFromHost();

    PROCESS_EVENT_LOOP_WITH_TIMEOUT( 50 );
}

void
QtStreamSocket::listen( int port )
{
    if ( mServer )
    {
        shutdown();
    }
    else
    {
        mServer = new QTcpServer();
        mServer->moveToThread( QApplication::instance()->thread() );
    }

    if ( !mServer )
    {
        throw UnrecoverableNetworkError( QObject::tr( "Cannot listen on invalid socket." ).toUtf8().data() );
    }

    SETUP_EVENT_LOOP_FOR( mServer, stateChanged() );

    mServer->listen( QHostAddress::Any, port );

    PROCESS_EVENT_LOOP_WITH_TIMEOUT( ::DEFAULT_TIMEOUT );

    if ( !mServer->isListening() )
    {
        throw UnrecoverableNetworkError( mServer->errorString().toStdString() );
    }
}


void
QtStreamSocket::accept()
{
    if ( !mServer )
    {
        throw UnrecoverableNetworkError(
                  QObject::tr( "Cannot accept connections on invalid socket." ).toUtf8().data() );
    }

    SETUP_EVENT_LOOP_FOR( mSocket, connected() );

    mSocket = mServer->nextPendingConnection();

    PROCESS_EVENT_LOOP_WITH_TIMEOUT( ::DEFAULT_TIMEOUT );

    if ( !mSocket->isOpen() )
    {
        throw UnrecoverableNetworkError( mSocket->errorString().toStdString() );
    }
}


void
QtStreamSocket::shutdown()
{
    if ( !mServer )
    {
        throw UnrecoverableNetworkError( QObject::tr( "Cannot shutdown invalid socket." ).toUtf8().data() );
    }
    mServer->close();
}


/**
   QtStreamSocket::send() blocks calling thread and processes socketSend() in the thread socketIOThread.
 */
size_t
QtStreamSocket::send( const void* buffer,
                      size_t      num_bytes )
{
    emit sendSignal( buffer, num_bytes );

    if ( cube::CubeNetworkProxy::exceptionPtr != nullptr )
    {
        std::rethrow_exception( cube::CubeNetworkProxy::exceptionPtr );
    }
    return num_bytes;
}

/**
   QtStreamSocket::receive() blocks calling thread and processes socketReceive() in the thread socketIOThread.
 */
size_t
QtStreamSocket::receive( void*  buffer,
                         size_t num_bytes )
{
    emit receiveSignal( buffer, num_bytes ); // blocks caller till slot socketReceive() has been finished
    return num_bytes;
}


void
QtStreamSocket::socketSend( const void* buffer,
                            size_t      num_bytes )
{
    if ( !mSocket || !mSocket->isValid() || !mSocket->isOpen() )
    {
        throw UnrecoverableNetworkError( QObject::tr( "Unable to write to invalid socket." ).toUtf8().data() );
    }

    SETUP_EVENT_LOOP_FOR( mSocket, bytesWritten( qint64 ) );

    qint64 bytesSent  = 0;
    qint64 totalBytes = num_bytes;

    try
    {
        while ( bytesSent < totalBytes )
        {
            qint64 res =
                mSocket->write( &static_cast< const char* >( buffer )[ bytesSent ],
                                totalBytes - bytesSent );
            mSocket->flush();

            if ( res < 0 )
            {
                throw UnrecoverableNetworkError( mSocket->errorString().toStdString() );
            }
            else
            {
                bytesSent += res;
            }

            while ( mSocket->bytesToWrite() > 0 )
            {
                PROCESS_EVENT_LOOP_WITH_TIMEOUT( 100 );

                ASSERT_STATE_OF( mSocket, QAbstractSocket::ConnectedState );

                if ( !mSocket->isWritable() )
                {
                    throw UnrecoverableNetworkError(
                              mSocket->errorString().toStdString() );
                }
            }
        }
    }
    catch ( const NetworkError& )
    {
        // exception has to be handled in a different thread
        CubeNetworkProxy::exceptionPtr = std::current_exception();
    }
}

void
QtStreamSocket::socketReceive( void*  buffer,
                               size_t num_bytes )
{
    if ( !mSocket || !mSocket->isValid() || !mSocket->isOpen() )
    {
        throw UnrecoverableNetworkError( QObject::tr( "Unable to read from invalid socket." ).toUtf8().data() );
    }

    try
    {
        qint64 bytesRead  = 0;
        qint64 totalBytes = num_bytes;

        /** new event loop is required, otherwise the following loop prevents that readyRead() signal
            is processed and bytesAvailable() only return zero
         */
        SETUP_EVENT_LOOP_FOR( mSocket, readyRead() );

        while ( ( bytesRead < totalBytes ) && ( mSocket->state() != QAbstractSocket::UnconnectedState ) )
        {
            if ( mSocket->bytesAvailable() > 0 )
            {
                qint64 res =
                    mSocket->read( &static_cast< char* >( buffer )[ bytesRead ],
                                   qMin( totalBytes - bytesRead,
                                         mSocket->bytesAvailable() ) );
                if ( res < 0 )
                {
                    throw UnrecoverableNetworkError(
                              mSocket->errorString().toStdString() );
                }
                else
                {
                    bytesRead += res;
                }
            }
            else
            {
                PROCESS_EVENT_LOOP_WITH_TIMEOUT( 100 );

                ASSERT_STATE_OF( mSocket, QAbstractSocket::ConnectedState );

                if ( !mSocket->isReadable() )
                {
                    throw UnrecoverableNetworkError(
                              mSocket->errorString().toStdString() );
                }
            }
        }
    }
    catch ( const std::exception& e )
    {
        CubeNetworkProxy::exceptionPtr = std::current_exception();
    }
}


bool
QtStreamSocket::isConnected()
{
    if ( !mSocket )
    {
        throw UnrecoverableNetworkError( QObject::tr( "Invalid socket." ).toUtf8().data() );
    }

    return mSocket->isOpen();
}


bool
QtStreamSocket::isListening()
{
    if ( !mServer )
    {
        throw UnrecoverableNetworkError( QObject::tr( "Invalid socket." ).toUtf8().data() );
    }

    return mServer->isListening();
}


std::string
QtStreamSocket::getHostname()
{
    return QObject::tr( "Unknown host" ).toUtf8().data();
}


std::string
QtStreamSocket::getInfoString()
{
    return "Qt";
}


QtStreamSocket::QtStreamSocket()
    : mSocket( NULL ),
    mServer( NULL )
{
}


QtStreamSocket::~QtStreamSocket()
{
    socketIOThread->quit();
    if ( mSocket )
    {
        if ( isConnected() )
        {
            disconnect();
        }
        delete mSocket;
    }
    if ( mServer )
    {
        if ( isListening() )
        {
            shutdown();
        }
        delete mServer;
    }
}
