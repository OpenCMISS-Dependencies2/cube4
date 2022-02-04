/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2017-2020                                                **
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
 *  @ingroup CUBE_lib.network
 *  @brief   Implementation of the class NetworkRequest.
 *
 *  Implementation of the abstract base class NetworkRequest.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include "CubeNetworkRequest.h"

#include <iostream>
#include <limits>

#include "CubeClientConnection.h"
#include "CubeError.h"
#include "CubeServerConnection.h"
#include "CubeNetworkProxy.h"

using namespace std;
using namespace cube;

//#define CUBE_NETWORK_DEBUG 1

namespace
{
/// @brief Marker for the start of the request header.
///
/// @note uint32_t representation of ASCII string 'CUBE'
///
static const NetworkRequest::marker_t REQUEST_HEADER_START = 0x43554245;

/// @brief Marker for the end of the request header and the beginning of
///     the payload.
///
/// @note uint32_t representation of ASCII string '-CC-'
///
static const NetworkRequest::marker_t REQUEST_HEADER_END = 0x2d43432d;

/// @brief Marker for the end of the request payload
///
/// @note uint32_t representation of ASCII string 'EBUC'
///
static const NetworkRequest::marker_t REQUEST_PAYLOAD_END = 0x45423543;
}

std::unordered_map<int, std::condition_variable*> NetworkRequest::conditions;

std::mutex              NetworkRequest::cmutex;
std::mutex              NetworkRequest::smutex;
std::mutex              NetworkRequest::rmutex;
std::mutex              NetworkRequest::responseFinishedMutex;
std::mutex              NetworkRequest::conditionHashMutex;
std::condition_variable NetworkRequest::responseFinishedCondition;
bool                    NetworkRequest::responseBodyFinished = false;
bool                    NetworkRequest::loopIsStopped        = false;

NetworkRequest::sequence_t
NetworkRequest::getSequenceNumber() const
{
    return mSequenceNo;
}


void
NetworkRequest::setSequenceNumber( sequence_t seqNo )
{
    mSequenceNo = seqNo;
}


void
NetworkRequest::resetRequestCounter()
{
    mNumRequests = 0;
}


bool
NetworkRequest::isRegistered()
{
    return mId != NetworkRequest::UNREGISTERED;
}


NetworkRequest::NetworkRequest()
    : mId( UNREGISTERED )
{
    cmutex.lock();
    mSequenceNo = ++mNumRequests;
    cmutex.unlock();
}


NetworkRequest::~NetworkRequest()
{
}

/**
 * client workflow:
 * 1. ClientConnection is created
 * 2. client starts loop ( @see receiveResponseLoop) in a separate thread, which waits for any response
 * - all client threads send requests (@see sendRequest) in arbitrary order
 * - all client threads wait for answer (@see receiveResponse)
 * - receiveResponseLoop reads a response header from server
 *   a) notifies receiveResponse()
 *   b) waits till receiveResponse() has been finished
 *   c) receiveResponse() calls virtual function receiveResponsePayload(), which reads the corresponding data
 *   d) receiveResponse() notifies receiveResponseLoop, that next response header can be
 * */

void
NetworkRequest::sendRequest( ClientConnection&   connection,
                             ClientCallbackData* data ) const
{
    if ( CubeNetworkProxy::exceptionPtr != nullptr )
    {
        std::rethrow_exception( CubeNetworkProxy::exceptionPtr );
    }

    // use lock_guard to ensure unlock, because exceptions may be thrown in the socket implementation
    std::lock_guard<std::mutex> lock( smutex );

    /// 1. Send 'StartOfHeader' marker
    connection << ::REQUEST_HEADER_START;

    /// 2. Send request ID
    connection << this->getId();

    /// 3. Send sequence Number
    connection << this->getSequenceNumber();

    /// 4. Send 'EndOfHeader' marker
    connection << ::REQUEST_HEADER_END;

    /// 5. Send the request payload
    this->sendRequestPayload( connection, data );

    /// 6. Send 'EndOfPayload' marker
    connection << ::REQUEST_PAYLOAD_END;

#if defined( CUBE_NETWORK_DEBUG )
    cerr << "---send request done sequence number #" << this->getSequenceNumber() << endl;
#endif
}

/**
 * NetworkRequest::createFromStream creates a request header and request body from stream
 */
NetworkRequest::Ptr
NetworkRequest::createFromStream( ServerConnection& connection, ServerCallbackData* data )
{
    std::lock_guard<std::mutex> lock( rmutex );
    marker_t                    marker;

    /// 1. Receive StartOfHeader marker
    connection >> marker;
#if defined( CUBE_NETWORK_DEBUG )
    //   cerr << "HeaderStart: " << marker << endl;
#endif
    if ( marker != ::REQUEST_HEADER_START )
    {
        throw UnrecoverableNetworkError( "Malformed request header: Incorrect start marker." );
    }

    /// 2. Receive request ID
    NetworkRequest::id_t reqId;
    connection >> reqId;

    /// 3. Receive sequence number
    NetworkRequest::sequence_t seqNo;
    connection >> seqNo;

    /// 4. Receive EndOfHeader marker
    connection >> marker;
#if defined( CUBE_NETWORK_DEBUG )
    //  cerr << "HeaderEnd: " << marker << endl;
#endif
    if ( marker != ::REQUEST_HEADER_END )
    {
        throw UnrecoverableNetworkError( "Malformed request header: Incorrect end marker." );
    }

    /// 5. create request object
    NetworkRequest::Ptr req = connection.createRequest( reqId, seqNo );
    /// 6. read request body
    req->receiveRequestBody( connection, data );
    return req;
}


void
NetworkRequest::receiveRequestBody( ServerConnection&   connection,
                                    ServerCallbackData* data )
{
    this->receiveRequestPayload( connection, data );

    /// Receive 'EndOfPayload' marker
    marker_t marker;
    connection >> marker;
#if defined( CUBE_NETWORK_DEBUG )
    cerr << "PayloadEnd: " << marker << endl;
#endif
    if ( marker != ::REQUEST_PAYLOAD_END )
    {
        throw UnrecoverableNetworkError( "Malformed request payload: Incorrect end marker." );
    }
}


void
NetworkRequest::processRequest( ServerCallbackData* )
{
}


void
NetworkRequest::sendResponse( ServerConnection&   connection,
                              ServerCallbackData* data ) const
{
    std::lock_guard<std::mutex> lock( smutex );

    /** @internal
     * ID is not needed in the response, as either in synchronous mode
     * the client knows the type is expects, and in asynchronous mode
     * the sequence number should be sufficient to retrieve the original
     * request object.
     */

    /// 1. Send StartOfHeader
    connection << ::REQUEST_HEADER_START;

    /// 2. Send ACK = REQUEST_OK
    connection << uint32_t( REQUEST_OK );

    /// 3. Send sequence number
    connection << this->getSequenceNumber();

    /// 4. Send EndOfHeader
    connection << ::REQUEST_HEADER_END;

    /// 5. Send response payload
    this->sendResponsePayload( connection, data );

    /// @todo Maybe think of a more general way to handle disconnect?
    if ( connection.isEstablished() )
    {
        /// 6. Send EndOfPayload
        connection << ::REQUEST_PAYLOAD_END;
    }
}


void
NetworkRequest::sendErrorResponse( ServerConnection&          connection,
                                   NetworkRequestResponseCode responseCode,
                                   const string&              errorMessage ) const
{
    std::lock_guard<std::mutex> lock( smutex );

    /** @internal
     * ID is not needed in the response, as either in synchronous mode
     * the client knows the type is expects, and in asynchronous mode
     * the sequence number should be sufficient to retrieve the original
     * request object.
     */

    /// 1. Send StartOfHeader
    connection << ::REQUEST_HEADER_START;

    /// 2. Send ACK = REQUEST_ERROR_RECOVERABLE
    connection << uint32_t( responseCode );

    /// 3. Send sequence number
    connection << this->getSequenceNumber();

    /// 4. Send EndOfHeader
    connection << ::REQUEST_HEADER_END;

    /// 5. Send response payload
    connection << errorMessage;

    /// @todo Maybe think of a more general way to handle disconnect?
    if ( connection.isEstablished() )
    {
        /// 6. Send EndOfPayload
        connection << ::REQUEST_PAYLOAD_END;
    }
}


/**
 * @brief NetworkRequest::receiveResponse waits till the response header for the current request
 * has been received by receiveResponseLoop() and reads the response body
 * @see receiveResponseLoop()
 */
void
NetworkRequest::receiveResponse( ClientConnection&   connection,
                                 ClientCallbackData* data )
{
    if ( CubeNetworkProxy::exceptionPtr != nullptr )
    {
        std::rethrow_exception( CubeNetworkProxy::exceptionPtr );
    }
#if defined( CUBE_NETWORK_DEBUG )
    cerr << "NetworkRequest::..... receive response start for sequence number #" << this->getSequenceNumber() << endl;
#endif

    // wait till response header with this sequence number has been received in receiveResponseLoop()
    std::unique_lock<std::mutex> mlock( responseConditionMutex );

    conditionHashMutex.lock();
    conditions[ this->getSequenceNumber() ] = &responseCondition;
    conditionHashMutex.unlock();

    responseCondition.wait( mlock, [ this ] {
        // handle spurious awakenings: the condition variable is deleted from conditiond after message
        // header has been read in receiveResponseLoop()
        std::lock_guard<std::mutex> lock( conditionHashMutex );
        return conditions.find( getSequenceNumber() ) == conditions.end();
    } );

    // header has been read -> read contents of the request
    if ( CubeNetworkProxy::exceptionPtr == nullptr )
    {
        receiveResponseData( connection, data );
    }
    // always notify receiveResponseLoop that reading has been finshed and next request can be read
    responseBodyFinished = true;
    responseFinishedCondition.notify_one();

    if ( CubeNetworkProxy::exceptionPtr != nullptr )
    {
        std::rethrow_exception( CubeNetworkProxy::exceptionPtr );
    }

#if defined( CUBE_NETWORK_DEBUG )
    cerr << "NetworkRequest::..... received data for sequence number #" << this->getSequenceNumber() << endl;
#endif
}

/**
 * @brief NetworkRequest::receiveRequestLoop reads request headers from server and notifies corresponding thread
 * in NetworkRequest::receiveResponse() to read the response body
 * @param connection
 */
void
NetworkRequest::receiveResponseLoop( ClientConnection& connection )
{
    loopIsStopped = false;

    try
    {
        while ( !loopIsStopped )
        {
            // read next available header (blocking)
            int sequenceNumber = receiveResponseHeader( connection );

#if defined( CUBE_NETWORK_DEBUG )
            cerr << "NetworkRequest::receiveResponseLoop: received sequence number #" << sequenceNumber << endl;
#endif
            // notify receiver thread with the corresponding sequence number that the request body can be read
            conditionHashMutex.lock();
            std::condition_variable* cond = conditions[ sequenceNumber ];
            conditionHashMutex.unlock();

            if ( cond )
            {
                conditionHashMutex.lock();
                conditions.erase( sequenceNumber ); // header finished, also see responseCondition.wait()
                responseBodyFinished = false;
                conditionHashMutex.unlock();
                cond->notify_one();
            }
            else
            {
                // todo: should not happen, abort?
                cerr << "NetworkRequest::receiveResponseLoop received invalid sequence number " << sequenceNumber << endl;
            }

            // wait till response body has been read by one of the client threads
            std::unique_lock<std::mutex> mlock( responseFinishedMutex );
            responseFinishedCondition.wait( mlock, [] {
                std::lock_guard<std::mutex> lock( conditionHashMutex );
                return responseBodyFinished;
            } );
        }
    }
    catch ( cube::UnrecoverableNetworkError& )
    {
        if ( !loopIsStopped )
        {
            // pass exception to caller thread
            CubeNetworkProxy::exceptionPtr = std::current_exception();
#if defined( CUBE_NETWORK_DEBUG )
            cerr << "NetworkRequest::receiveResponseLoop: exception has been thrown " << endl;
#endif
        }
        loopIsStopped = true;
    }

    // client isn't listening any more -> stop waiting for answer

    std::vector<std::condition_variable*> vconditions;
    for ( std::pair<int, std::condition_variable*> element : conditions )
    {
        vconditions.push_back( element.second );
    }
    conditions.clear(); // set the predicate of the wait() call
    // release locks of threads which wait for response
    for ( std::condition_variable* condition : vconditions )
    {
        condition->notify_one();
    }
}

void
NetworkRequest::stopLoop()
{
    loopIsStopped = true;
    // don't wait for responses
    responseBodyFinished = true;
    responseFinishedCondition.notify_one();
}

/** reads next available header and returns its sequence number */
int
NetworkRequest::receiveResponseHeader( ClientConnection& connection )
{
    marker_t marker;

    /// 1. Receive StartOfHeader marker
    connection >> marker;
    if ( marker != ::REQUEST_HEADER_START )
    {
        throw UnrecoverableNetworkError(
                  "Malformed response header. Incorrect start marker." );
    }

    /// 2. Receive request acknowledgment
    uint32_t ACK;
    connection >> ACK;

    /// 3. Receive sequence number
    NetworkRequest::sequence_t seqNo;
    connection >> seqNo;

    // cerr << "NetworkRequest::.....Received sequence number #" << seqNo << endl;

    /// 4. Receive EndOfHeader marker
    connection >> marker;
    if ( marker != ::REQUEST_HEADER_END )
    {
        throw UnrecoverableNetworkError(
                  "Malformed response header. Incorrect end marker." );
    }

    /// 5. Receive message body in case of errors
    if ( ACK == REQUEST_ERROR_RECOVERABLE )
    {
        try
        {
            string errorMessage;
            connection >> errorMessage;
            connection >> marker; // Receive EndOfPayload marker
            throw RecoverableNetworkError( errorMessage );
        }
        catch ( cube::RecoverableNetworkError& e )
        {
            CubeNetworkProxy::exceptionPtr = std::current_exception();
        }
    }
    else if ( ACK == REQUEST_ERROR_UNRECOVERABLE )
    {
        string errorMessage;
        connection >> errorMessage;
        throw UnrecoverableNetworkError( errorMessage );
    }

    return seqNo;
}

void
NetworkRequest::receiveResponseData( ClientConnection&   connection,
                                     ClientCallbackData* data )
{
    this->receiveResponsePayload( connection, data );
    //   cerr << "NetworkRequest::.....receive payload" << endl;

    /// @todo Maybe think of a more general way to handle disconnect?
    if ( connection.isEstablished() )
    {
        /// Receive EndOfPayload marker
        marker_t marker;
        connection >> marker;
        if ( marker != ::REQUEST_PAYLOAD_END )
        {
            throw UnrecoverableNetworkError(
                      "Malformed response payload. Incorrect end marker." );
        }
    }
}


void
NetworkRequest::sendRequestPayload( ClientConnection& /*connection*/,
                                    ClientCallbackData*    /*data*/ ) const
{
}


void
NetworkRequest::receiveRequestPayload( ServerConnection& /*connection*/,
                                       ServerCallbackData*    /*data*/ )
{
}


void
NetworkRequest::sendResponsePayload( ServerConnection& /*connection*/,
                                     ServerCallbackData*    /*data*/ ) const
{
}


void
NetworkRequest::receiveResponsePayload( ClientConnection& /*connection*/,
                                        ClientCallbackData*    /*data*/ )
{
}


NetworkRequest::sequence_t NetworkRequest::mNumRequests = 0;

const NetworkRequest::id_t NetworkRequest::UNREGISTERED =
    numeric_limits< NetworkRequest::id_t >::max();
