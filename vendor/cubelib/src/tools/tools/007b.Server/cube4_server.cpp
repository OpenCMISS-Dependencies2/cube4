/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2016-2020                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2014-2015                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  Copyright (c) 2014-2015                                                **
**  RWTH Aachen University, JARA-HPC                                       **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


/*-------------------------------------------------------------------------*/
/**
 *  @file
 *  @ingroup CUBE_tools
 *  @brief   Server application for the client-server network layer.
 *
 *  The cube server will listen on a given port for incoming client
 *  connections to serve information read and computed from a Cube file
 *  local to the server.
 **/
/*-------------------------------------------------------------------------*/
#include "config.h"

#include <cerrno>
#include <csignal>
#include <cstdlib>
#include <getopt.h>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <thread>

#include "CubeError.h"
#include "CubeNetworkRequest.h"
#include "CubeServerCallbackData.h"
#include "CubeServerConnection.h"
#include "CubeSocket.h"
#include "CubeUrl.h"
#include "ThreadPool.h"

using namespace std;
using namespace cube;

/// @brief Signal handler to clean up child processes.
///
/// @param s
///     Signal number
///
void
sigchld_handler( int s )
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while ( waitpid( -1, NULL, WNOHANG ) > 0 )
    {
    }

    errno = saved_errno;
}


/// @brief Print command-line options and usage info
///
const string&
getUsage()
{
    stringstream portNo;
    portNo << Url::getDefaultPort();
    static const string USAGE =
        "Usage: cube_server [options]\n"
        " -h, -?    Print this help message.\n"
        " -p N      Bind socket on port N (default port: " + portNo.str()
        + ")\n\n"
        "Report bugs to <" PACKAGE_BUGREPORT ">\n";

    return USAGE;
}

/**
 * @brief The RequestTask class holds the data of a cube request and the method work() to execute it and to
 * send the result to the client.
 */
class RequestTask
{
public:
    RequestTask( ServerConnection::Ptr connection,
                 NetworkRequest::Ptr   request,
                 ServerCallbackData*   data
                 ) : connection_( connection ), request_( request ), data_( data )
    {
    }

    void
    work()
    {
        request_->processRequest( data_ ); // calculate result

        stringstream message;
        try
        {
            request_->sendResponse( *connection_, data_ ); // send result to client

#if defined( CUBE_NETWORK_DEBUG )
            message << "Sent response number #" << request_->getSequenceNumber();
            cerr << message.str() << endl;
            message.str( "" );
#endif
        }
        catch ( NetworkError& theError )
        {
            message << "cube_server[" << getpid() << "] ### Failed to send response.\n"
                    << "cube_server[" << getpid() << "] ### " << theError.what();
            cerr << message.str() << endl;
            message.str( "" );
            connection_->disconnect();
        }
        catch ( const std::exception& theError )
        {
            message << "cube_server[" << getpid() << "] ### Unexpected exception while handling request.\n"
                    << "cube_server[" << getpid() << "] ### " << theError.what();
            cerr << message.str() << endl;
            message.str( "" );
        }
    }
private:
    ServerConnection::Ptr connection_;
    NetworkRequest::Ptr   request_;
    ServerCallbackData*   data_;
};

int
main( int   argc,
      char* argv[] )
{
    size_t           portNo = Url::getDefaultPort();
    struct sigaction sigAction;
    stringstream     message;

    int option_arg;

    // check for command line parameters
    while ( ( option_arg = getopt( argc, argv, "h?p:" ) ) != -1 )
    {
        switch ( option_arg )
        {
            case 'h':
            case '?':
                cerr << getUsage() << endl;
                exit( EXIT_SUCCESS );
                break;

            case 'p':
                portNo = atoi( optarg );
        }
    }

    ServerConnection::Ptr connection;
    try
    {
        connection = ServerConnection::create( Socket::create(),
                                               portNo );
    }
    catch ( NetworkError& theError )
    {
        cerr << "cube_server: " << theError.what() << endl;
        exit( EXIT_FAILURE );
    }
    message << "[" << getpid() << "] Cube Server: "
            << connection->getInfoString();
    cerr << message.str() << endl;
    message.str( "" );

    // install signal handler
    sigAction.sa_handler = sigchld_handler;    // reap all dead processes
    sigemptyset( &sigAction.sa_mask );
    sigAction.sa_flags = SA_RESTART;
    if ( sigaction( SIGCHLD, &sigAction, NULL ) == -1 )
    {
        cerr << "cube_server: " << "Could not install signal handler." << endl;
        exit( EXIT_FAILURE );
    }

    while ( connection->isListening() )
    {
        // wait for incoming connection

#if defined( CUBE_NETWORK_DEBUG )
        message << "cube_server[" << getpid() << "] Waiting for connections on port " << portNo << ".";
        cerr << message.str() << endl;
        message.str( "" );
#endif

        try
        {
            connection->accept();
        }
        catch ( const exception& err )
        {
            message << "cube_server[" << getpid() << "] " << err.what();
            cerr << message.str() << endl;
            message.str( "" );
            continue;
        }
        cerr << "cube_server: Accepted connection" << endl;

        // fork process handling connection to client
        if ( !fork() )
        {
            // child does not need to listen to the server socket
            try
            {
                connection->stopListening();
            }
            catch ( const exception& err )
            {
                message << "cube_server[" << getpid() << "] " << err.what();
                cerr << message.str() << endl;
                message.str( "" );
                exit( EXIT_FAILURE );
            }
#if defined( CUBE_NETWORK_DEBUG )
            message << "cube_server[" << getpid() << "] Child stopped listening.";
            cerr << message.str() << endl;
            message.str( "" );
#endif
        }
        else
        {
            // parent can disconnect socket handled by child
            try
            {
                connection->disconnect();
            }
            catch ( const exception& err )
            {
                message << "cube_server[" << getpid() << "] " << err.what();
                cerr << message.str() << endl;
                message.str( "" );
                exit( EXIT_FAILURE );
            }
#if defined( CUBE_NETWORK_DEBUG )
            message << "cube_server[" << getpid() << "] Parent disconnected.";
            cerr << message.str() << endl;
            message.str( "" );
#endif
        }

        ThreadPool pool( std::thread::hardware_concurrency() );

        /* Allocated here and only one instance of ServerCallbackData, so that
         * server-side information is truly stored between distinct network
         * requests i.e. for the whole time of connection->isEstablished()
         */
        ServerCallbackData* data = new ServerCallbackData();
        while ( connection->isEstablished() )
        {
            // read request ID from stream
            NetworkRequest::Ptr request;
            try
            {
                // create request from input on connection stream
                request = connection->createRequestFromStream( data );

#if defined( CUBE_NETWORK_DEBUG )
                message << "cube_server[" << getpid() << "] -- Received request: "
                        << request->getName() << " --" << request.get()->getSequenceNumber();
                cerr << message.str() << endl;
                message.str( "" );
#endif

                if ( request->getName() == "OpenCube" )
                {
                    // client waits for cube to be opened, so this check is not required, but it dosn't hurt
                    // (and removes a lot of warning of helgrind)
                    RequestTask( connection, request, data ).work();
                }
                else if ( request->getName() == "Disconnect" )
                {
                    RequestTask( connection, request, data ).work();
                    connection->disconnect();
                }
                else
                {
                    pool.addTask( std::bind( &RequestTask::work, RequestTask( connection, request, data ) ) );
                }
            }
            catch ( const RecoverableNetworkError& theError )
            {
                message << "cube_server[" << getpid()
                        << "] ### Recoverable error while handling request.\n"
                        << "[" << getpid() << "] ### " << theError.what();
                cerr << message.str() << endl;
                message.str( "" );

                request->sendErrorResponse( *connection,
                                            REQUEST_ERROR_RECOVERABLE,
                                            theError.what() );
            }
            catch ( const UnrecoverableNetworkError& theError )
            {
                message << "cube_server[" << getpid()
                        << "] ### Unrecoverable error while handling request.\n"
                        << "[" << getpid() << "] ### " << theError.what();
                cerr << message.str() << endl;
                message.str( "" );

                request->sendErrorResponse( *connection,
                                            REQUEST_ERROR_UNRECOVERABLE,
                                            theError.what() );
                // unrecoverable error -> abort loop and disconnect
                connection->disconnect();
                break;
            }
            catch ( const std::exception& theError )
            {
                message << "cube_server[" << getpid()
                        <<
                    "] ### Unexpected exception while handling request.\n"
                        << "[" << getpid() << "] ### " << theError.what();
                cerr << message.str() << endl;
                message.str( "" );
            }
        }  // connection->isEstablished()

        cerr << "cube_server: Connection terminated." << endl << endl;

        // reset connection protocol
        connection->setProtocolVersion( 0 );
    }

#if defined( CUBE_NETWORK_DEBUG )
    message << "cube_server[" << getpid() << "] Done.";
    cerr << message.str() << endl;
    message.str( "" );
#endif
}
