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

#include <QObject>
#include "VampirConnecter.h"

#include <iostream>
#include <cassert>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>

using namespace std;

bool VampirConnecter::busNameRegistered = false;

VampirConnecter::VampirConnecter( std::string const& theBusName,
                                  std::string const& serverName,
                                  dbus_uint32_t      portNumber,
                                  std::string const& fileName,
                                  bool               verb )
    : connection( 0 ), pending( 0 ), busName( theBusName ),
    objectName( "/com/gwt/vampir" ), interfaceName( "com.gwt.vampir" ),
    server( serverName ), port( portNumber ), file( fileName ),
    isActive( false ), verbose( verb )
{
    {
        size_t const pointPosition = file.find_last_of( '.' );
        string const extension     = file.substr( pointPosition );
        if ( extension == ".elg" || extension == ".esd" )
        {
            fileType = ELG;
        }
        else if ( extension == ".otf" )
        {
            fileType = OTF;
        }
        else
        {
            //should never happen
            fileType = undefined;
        }
    }
    DBusError error;
    dbus_error_init( &error );
    //connect to the bus and check for errors
    connection = dbus_bus_get( DBUS_BUS_SESSION, &error );
    //Prevent the application from being closed when the connection closes
    dbus_connection_set_exit_on_disconnect( connection,  false );
    CheckError( error );
    if ( connection == 0 )
    {
        throw VampirConnecterException( QObject::tr( "Connection failed" ).toUtf8().data() );
    }
    if ( !busNameRegistered )
    {
        int ret = dbus_bus_request_name( connection,
                                         "com.gwt.CUBE-VampirConnecter",
                                         DBUS_NAME_FLAG_REPLACE_EXISTING, &error );
        CheckError( error );
        if ( ret != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER )
        {
            throw VampirConnecterException( QObject::tr( "No primary owner!" ).toUtf8().data() );
        }
        busNameRegistered = true;
    }
    dbus_error_free( &error );
}

string
VampirConnecter::InitiateAndOpenTrace()
{
    //tell dbus that we are interested in signals from vampir
    //(in order to check for the 'traceFileLoaded' signal below)
    string    rule = QObject::tr( "type='signal',interface='" ).toUtf8().data() + interfaceName + "'";
    DBusError err;
    dbus_error_init( &err );
    dbus_bus_add_match( connection, rule.c_str(), &err );
    if ( dbus_error_is_set( &err ) )
    {
        dbus_error_free( &err );
        return QObject::tr( "DBUSerror while waiting for vampir." ).toUtf8().data();
    }
    else
    {
        dbus_error_free( &err );
    }

    //check the opening method
    //if no servername and port was set, we open a local file
    if ( server == "" && port == 0 )
    {
        if ( !OpenLocalTraceFile( file ) )
        {
            return QObject::tr( "Local trace file could not be opened: " ).toStdString() + file;
        }
    }
    else if ( !OpenRemoteTraceFile( file, server, port ) )
    {
        return QObject::tr( "Remote trace file could not be opened." ).toUtf8().data();
    }

    //wait a second to give vampir some time for initializing all default start displays
    sleep( 1 );

    if ( !OpenDisplay( Timeline ) )
    {
        return QObject::tr( "Timeline display could not be opened." ).toUtf8().data();
    }

    isActive = true;
    return "";
}

string
VampirConnecter::ZoomIntervall( double start, double end, int step )
{
    //TODO - we do not need the step parameter anymore! vampir has a own undo zoom feature.
    //so we must not save the steps to undo any zoom intervall

    if ( verbose )
    {
        cout << QObject::tr( "Zooming vampir on bus name " ).toUtf8().data() << busName << QObject::tr( " to intervall [" ).toUtf8().data()
             << start << QObject::tr( " seconds, " ).toUtf8().data() << end << QObject::tr( " seconds]" ).toUtf8().data() << endl;
    }
    if ( !ExistsVampirWithBusName( busName ) )
    {
        //Ignore closed instances of vampir
        if ( verbose )
        {
            cout << QObject::tr( "Ignoring closed instance of vampir on bus name " ).toUtf8().data()
                 << busName << endl;
        }
        return "";
    }
    if ( step == 1 )
    {
        if ( !OpenDisplay( Timeline ) )
        {
            return QObject::tr( "Timeline display could not be opened." ).toUtf8().data();
        }
    }
    if ( !ZoomDisplay( Timeline, start, end ) )
    {
        return QObject::tr( "Remote zoom failed." ).toUtf8().data();
    }
    return "";
}

void
VampirConnecter::CheckError( DBusError& error )
{
    if ( dbus_error_is_set( &error ) )
    {
        string message = error.message;
        dbus_error_free( &error );
        throw VampirConnecterException( message );
    }
}

void
VampirConnecter::InitiateCommunication( string const& methodName )
{
    message = dbus_message_new_method_call( busName.c_str(), objectName.c_str(),
                                            interfaceName.c_str(), methodName.c_str() );
    if ( message == 0 )
    {
        throw VampirConnecterException(
                  QObject::tr( "Connect message null while initiating communication for method " ).toUtf8().data()
                  + methodName );
    }
    messageIterator = DBusMessageIter();
    dbus_message_iter_init_append( message, &messageIterator );
}

void
VampirConnecter::CompleteCommunicationGeneric( bool block )
{
    if ( !dbus_connection_send_with_reply( connection, message, &pending, -1 ) )
    {
        throw VampirConnecterException(
                  QObject::tr( "Out of memory while trying to send DBus message" ).toUtf8().data() );
    }
    if ( pending == 0 )
    {
        throw VampirConnecterException(
                  QObject::tr( "Pending call null while trying to send DBus message" ).toUtf8().data() );
    }
    dbus_connection_flush( connection );
    dbus_message_unref( message );
    message = 0;

    //block until reply is received
    if ( block )
    {
        dbus_pending_call_block( pending );
        message = dbus_pending_call_steal_reply( pending );
        if ( message == 0 )
        {
            throw VampirConnecterException( QObject::tr( "Connection reply is null" ).toUtf8().data() );
        }
        dbus_pending_call_unref( pending );
        pending = 0;
    }
}

bool
VampirConnecter::CompleteCommunication( bool block, bool )
{
    CompleteCommunicationGeneric( block );

    if ( block )
    {
        bool reply;
        if ( !dbus_message_iter_init( message, &messageIterator ) )
        {
            dbus_message_unref( message );
            if ( verbose )
            {
                cout << QObject::tr( "In CompleteCommunication: " ).toUtf8().data()
                     << QObject::tr( "dbus_message_iter_init failed. Returning false." ).toUtf8().data() << endl;
            }
            return false;
        }
        else if ( dbus_message_iter_get_arg_type( &messageIterator ) != DBUS_TYPE_BOOLEAN )
        {
            dbus_message_unref( message );
            if ( verbose )
            {
                cout << QObject::tr( "In CompleteCommunication: Message argument " ).toUtf8().data()
                     << QObject::tr( "type is not bool. Returning false." ).toUtf8().data() << endl;
            }
            return false;
        }
        else
        {
            dbus_message_iter_get_basic( &messageIterator, &reply );
            if ( verbose )
            {
                cout << QObject::tr( "In CompleteCommunication: reply = " ).toUtf8().data()
                     << boolalpha << reply << endl;
            }
            if ( reply )
            {
                dbus_message_unref( message );
                if ( verbose )
                {
                    cout << QObject::tr( "In CompleteCommunication (bool, bool) : " ).toUtf8().data()
                         << QObject::tr( "returning true" ).toUtf8().data() << endl;
                }
                return true;
            }
            else
            {
                dbus_message_unref( message );
                if ( verbose )
                {
                    cout << QObject::tr( "In CompleteCommunication (bool, bool) : " ).toUtf8().data()
                         << QObject::tr( "returning false" ).toUtf8().data() << endl;
                }
                return false;
            }
        }
    }
    //If we dont wait for the answer
    return true;
}

bool
VampirConnecter::CompleteCommunication( bool block, dbus_uint32_t_list** successDBusUint32List )
{
    CompleteCommunicationGeneric( block );

    if ( block )
    {
        DBusMessageIter index;
        if ( !dbus_message_iter_init( message, &messageIterator ) )
        {
            dbus_message_unref( message );
            if ( verbose )
            {
                cout << QObject::tr( "In CompleteCommunication (bool uint32_list):" ).toUtf8().data()
                     << QObject::tr( "dbus_message_iter_init failed. Returning false." ).toUtf8().data() << endl;
            }
            return false;
        }
        else if ( dbus_message_iter_get_arg_type( &messageIterator ) != DBUS_TYPE_ARRAY )
        {
            dbus_message_unref( message );
            if ( verbose )
            {
                cout << QObject::tr( "In CompleteCommunication (bool uint32_list): Message argument " ).toUtf8().data()
                     << QObject::tr( "type is not array. Returning false." ).toUtf8().data() << endl;
            }
            return false;
        }
        else
        {
            int counter = 0;

            dbus_message_iter_recurse( &messageIterator, &index );
            if ( DBUS_TYPE_UINT32 == dbus_message_iter_get_arg_type( &index ) )
            {
                ( *successDBusUint32List )       = new dbus_uint32_t_list;
                ( *successDBusUint32List )->next = NULL;
                dbus_message_iter_get_basic( &index, &( *successDBusUint32List )->data );

                while ( dbus_message_iter_next( &index ) )
                {
                    if ( DBUS_TYPE_UINT32 != dbus_message_iter_get_arg_type( &index ) )
                    {
                        dbus_message_unref( message );
                        if ( verbose )
                        {
                            cout << QObject::tr( "In CompleteCommunication (bool uint32_list):" ).toUtf8().data()
                                 << QObject::tr( "wrong argument in list at position " ).toUtf8().data() << counter <<  QObject::tr( ". Returning false." ).toUtf8().data() << endl;
                        }
                        return false;
                    }
                    else
                    {
                        successDBusUint32List = &( *successDBusUint32List )->next;

                        ( *successDBusUint32List )       = new dbus_uint32_t_list;
                        ( *successDBusUint32List )->next = NULL;
                        dbus_message_iter_get_basic( &index, &( *successDBusUint32List )->data );
                    }
                    counter++;
                }
            }
            else
            {
                dbus_message_unref( message );
                if ( verbose )
                {
                    cout << QObject::tr( "In CompleteCommunication (bool uint32_list):" ).toUtf8().data()
                         << QObject::tr( "wrong argument in list at position " ).toUtf8().data() << counter <<  QObject::tr( ". Returning false." ).toUtf8().data() << endl;
                }
                return false;
            }
        }
    }

    return true;
}

bool
VampirConnecter::CompleteCommunication( bool block, char** successCharStringValue )
{
    CompleteCommunicationGeneric( block );
    if ( block )
    {
        if ( !dbus_message_iter_init( message, &messageIterator ) )
        {
            dbus_message_unref( message );
            if ( verbose )
            {
                cout << QObject::tr( "In CompleteCommunication (bool char*): " ).toUtf8().data()
                     << QObject::tr( "dbus_message_iter_init failed. Returning false." ).toUtf8().data() << endl;
            }
            return false;
        }
        else if ( dbus_message_iter_get_arg_type( &messageIterator ) != DBUS_TYPE_STRING )
        {
            dbus_message_unref( message );
            if ( verbose )
            {
                cout << QObject::tr( "In CompleteCommunication (bool char*): Message argument " ).toUtf8().data()
                     << QObject::tr( "type is not char*." ).toUtf8().data() << endl;
            }
            return false;
        }
        else
        {
            char* buffer;
            dbus_message_iter_get_basic( &messageIterator, &buffer );
            *successCharStringValue = ( char* )calloc( strlen( buffer ) + 1, sizeof( char ) );
            strcpy( *successCharStringValue, buffer );

            if ( verbose )
            {
                cout << QObject::tr( "In CompleteCommunication (bool char*): reply = " ).toUtf8().data()
                     << *successCharStringValue << endl;
            }

            dbus_message_unref( message );
            return true;
        }
    }
    //If we dont wait for the answer
    return true;
}

bool
VampirConnecter::CompleteCommunication( bool          block,
                                        string const& successMessageBeginning )
{
    CompleteCommunicationGeneric( block );
    if ( block )
    {
        if ( !dbus_message_iter_init( message, &messageIterator ) )
        {
            dbus_message_unref( message );
            if ( verbose )
            {
                cout << QObject::tr( "In CompleteCommunication (bool string): " ).toUtf8().data()
                     << QObject::tr( "dbus_message_iter_init failed. Returning false." ).toUtf8().data() << endl;
            }
            return false;
        }
        else if ( dbus_message_iter_get_arg_type( &messageIterator )
                  != DBUS_TYPE_STRING )
        {
            dbus_message_unref( message );
            if ( verbose )
            {
                cout << QObject::tr( "In CompleteCommunication (bool string): Message argument " ).toUtf8().data()
                     << QObject::tr( "type is not string." ).toUtf8().data() << endl;
            }
            return false;
        }
        else
        {
            int const maxMessageLength = 128;
            char*     buffer           = new char[ maxMessageLength ];
            dbus_message_iter_get_basic( &messageIterator, &buffer );
            string receivedMessage = buffer;
            if ( verbose )
            {
                cout << QObject::tr( "In CompleteCommunication (bool string): receivedMessage = " ).toUtf8().data()
                     << receivedMessage << endl;
            }
            if ( receivedMessage.find( successMessageBeginning ) == 0 )
            {
                dbus_message_unref( message );
                if ( verbose )
                {
                    cout << QObject::tr( "In CompleteCommunication (bool, string) : " ).toUtf8().data()
                         << QObject::tr( "returning true" ).toUtf8().data() << endl;
                }
                return true;
            }
            else
            {
                dbus_message_unref( message );
                if ( verbose )
                {
                    cout << QObject::tr( "In CompleteCommunication (bool, string) : " ).toUtf8().data()
                         << QObject::tr( "returning false" ).toUtf8().data() << endl;
                }
                return false;
            }
        }
    }
    //If we dont wait for the answer
    return true;
}

bool
VampirConnecter::CompleteCommunication( bool block, dbus_uint32_t* successUint32Value )
{
    CompleteCommunicationGeneric( block );
    if ( block )
    {
        if ( !dbus_message_iter_init( message, &messageIterator ) )
        {
            dbus_message_unref( message );
            if ( verbose )
            {
                cout << QObject::tr( "In CompleteCommunication (bool uint32): " ).toUtf8().data()
                     << QObject::tr( "dbus_message_iter_init failed. Returning false." ).toUtf8().data() << endl;
            }
            return false;
        }
        else if ( dbus_message_iter_get_arg_type( &messageIterator ) != DBUS_TYPE_UINT32 )
        {
            dbus_message_unref( message );
            if ( verbose )
            {
                cout << QObject::tr( "In CompleteCommunication (bool uint32): Message argument " ).toUtf8().data()
                     << QObject::tr( "type is not uint32." ).toUtf8().data() << endl;
            }
            return false;
        }
        else
        {
            dbus_message_iter_get_basic( &messageIterator, successUint32Value );
            if ( verbose )
            {
                cout << QObject::tr( "In CompleteCommunication (bool uint32): reply = " ).toUtf8().data()
                     << *successUint32Value << endl;
            }
            if ( *successUint32Value > 0 )
            {
                dbus_message_unref( message );
                if ( verbose )
                {
                    cout << QObject::tr( "In CompleteCommunication (bool uint32): " ).toUtf8().data()
                         << QObject::tr( "returning true" ).toUtf8().data() << endl;
                }
                return true;
            }
            else
            {
                dbus_message_unref( message );
                if ( verbose )
                {
                    cout << QObject::tr( "In CompleteCommunication (bool uint32): " ).toUtf8().data()
                         << QObject::tr( "returning false" ).toUtf8().data() << endl;
                }
                return false;
            }
        }
    }
    //If we dont wait for the answer
    return true;
}

bool
VampirConnecter::CompleteFileOpening( std::string const& fileName )
{
    trace_file_session session;

    //for the future, you can open more than one file with qtvampir
    if ( CompleteCommunication( true, ( dbus_uint32_t* )&session.sessionId ) )
    {
        openedFiles[ fileName ] = session;

        //check for the 'traceFileLoaded' signal from vampir
        //before opening the timeline
        dbus_uint32_t   statusId     = 1;
        DBusMessage*    replyMessage = 0;
        DBusMessageIter replyArgs;
        while ( true )
        {
            dbus_connection_read_write( connection, 50 );
            replyMessage = dbus_connection_pop_message( connection );

            //check signal for successfully loading
            if ( replyMessage != 0 &&
                 dbus_message_is_signal( replyMessage, interfaceName.c_str(),
                                         "traceFileLoaded" ) )
            {
                if ( verbose )
                {
                    cout << QObject::tr( "Vampir instance at bus name " ).toUtf8().data() << busName
                         << QObject::tr( " has loaded file " ).toUtf8().data() << fileName
                         << QObject::tr( " in session id " ).toUtf8().data() << session.sessionId << endl;
                }
                //FIXME:
                sleep( 1 );
                return true;
            }
            //check signal for loading errors
            else if ( replyMessage != 0 &&
                      dbus_message_is_signal( replyMessage, interfaceName.c_str(),
                                              "status" ) )
            {
                //check status
                //a status id which is greater than 0 is not ok and was error is occurred
                if ( dbus_message_iter_init( replyMessage, &replyArgs ) )
                {
                    if ( DBUS_TYPE_UINT32 == dbus_message_iter_get_arg_type( &replyArgs ) )
                    {
                        dbus_message_iter_get_basic( &replyArgs, &statusId );
                        if ( statusId > 0 )
                        {
                            if ( verbose )
                            {
                                cout << QObject::tr( "Vampir instance at bus name " ).toUtf8().data() << busName
                                     << QObject::tr( " threw error status id " ).toUtf8().data() << statusId << endl;
                            }
                            return false;
                        }
                    }
                }
            }

            //do nothing and wait for right signal
            if ( replyMessage != 0 )
            {
                dbus_message_unref( replyMessage );
            }
            replyMessage = 0;
        }
    }

    return false;
}

void
VampirConnecter::AddMessage( string const& messageString )
{
    char* buffer = new char[ messageString.length() + 1 ];
    strcpy( buffer, messageString.c_str() );
    if ( !dbus_message_iter_append_basic( &messageIterator,
                                          DBUS_TYPE_STRING, &buffer ) )
    {
        throw VampirConnecterException(
                  QObject::tr( "Out of memory while trying to append a string" ).toUtf8().data() );
    }
    delete[] buffer;
}

void
VampirConnecter::AddMessage( dbus_uint32_t messageNumber )
{
    if ( !dbus_message_iter_append_basic( &messageIterator,
                                          DBUS_TYPE_UINT32, &messageNumber ) )
    {
        throw VampirConnecterException(
                  QObject::tr( "Out of memory while trying to append a dbus_uint32_t" ).toUtf8().data() );
    }
}

void
VampirConnecter::AddMessage( double messageNumber )
{
    if ( !dbus_message_iter_append_basic( &messageIterator,
                                          DBUS_TYPE_DOUBLE, &messageNumber ) )
    {
        throw VampirConnecterException(
                  QObject::tr( "Out of memory while trying to append a double" ).toUtf8().data() );
    }
}

bool
VampirConnecter::OpenLocalTraceFile( std::string const& fileName )
{
    if ( verbose )
    {
        cout << QObject::tr( "Vampir instance at bus name " ).toUtf8().data() << busName
             << QObject::tr( " tries to open local file " ).toUtf8().data() << fileName << endl;
    }
    InitiateCommunication( "openLocalTrace" );
    AddMessage( fileName );

    return CompleteFileOpening( fileName );
}

bool
VampirConnecter::OpenRemoteTraceFile( string const& fileName, string const& serverName, dbus_uint32_t portNumber )
{
    if ( verbose )
    {
        cout << QObject::tr( "Vampir instance at bus name " ).toUtf8().data() << busName
             << QObject::tr( " tries to open remote file " ).toUtf8().data() << fileName << QObject::tr( " on " ).toUtf8().data() << serverName << ":" << portNumber << endl;
    }
    InitiateCommunication( "openRemoteTrace" );
    AddMessage( fileName );
    AddMessage( serverName );
    AddMessage( portNumber );

    return CompleteFileOpening( fileName );
}

string
VampirConnecter::DisplayTypeToString( DisplayType type )
{
    switch ( type )
    {
        case Timeline:
            return QObject::tr( "Master Timeline" ).toUtf8().data();
        case CounterTimeline:
            return QObject::tr( "Counter Data Timeline" ).toUtf8().data();
        case SummaryChart:
            return QObject::tr( "Summary Chart" ).toUtf8().data();
        case ProcessProfile:
            return QObject::tr( "Process Summary" ).toUtf8().data();
    }
    throw VampirConnecterException( QObject::tr( "Unsupported display type" ).toUtf8().data() );
}

bool
VampirConnecter::OpenDisplay( DisplayType type )
{
    dbus_uint32_t displayId = 0;
    //we have only one file, so we need not search the right map key
    std::map<std::string const, trace_file_session>::iterator it;
    it = openedFiles.begin();

    //at first, we will check whether a display type was already opened
    if ( isDisplayOpen( type, &displayId ) )
    {
        it->second.displayIds[ displayId ] = type;

        if ( verbose )
        {
            cout << QObject::tr( "Vampir instance at bus name " ).toUtf8().data() << busName
                 << QObject::tr( " has already a opened display " ).toUtf8().data() << DisplayTypeToString( type )
                 << QObject::tr( "with id " ).toUtf8().data() << displayId
                 << QObject::tr( " for session " ).toUtf8().data() << it->second.sessionId << endl;
        }
        return true;
    }
    //if the type not open, we can try to open it
    else
    {
        if ( verbose )
        {
            cout << QObject::tr( "Vampir instance at bus name " ).toUtf8().data() << busName
                 << QObject::tr( " is opening display " ).toUtf8().data() << DisplayTypeToString( type )
                 << QObject::tr( " for session " ).toUtf8().data() << it->second.sessionId << endl;
        }
        InitiateCommunication( "openDisplay" );
        AddMessage( DisplayTypeToString( type ) );
        AddMessage( it->second.sessionId );

        if ( CompleteCommunication( true, ( dbus_uint32_t* )&displayId ) )
        {
            it->second.displayIds[ displayId ] = type;

            //give time to initialize the opened vampir window and wait
            //for 'displayLoaded'
            //check the loaded displayId with the set display id
            dbus_uint32_t   loadedDisplayId = 0, statusId = 1;
            DBusMessage*    replyMessage    = 0;
            DBusMessageIter replyArgs;
            while ( true )
            {
                dbus_connection_read_write( connection, 50 );
                replyMessage = dbus_connection_pop_message( connection );

                if ( replyMessage != 0 &&
                     dbus_message_is_signal( replyMessage, interfaceName.c_str(),
                                             "displayLoaded" ) )
                {
                    //check loaded displayId
                    if ( dbus_message_iter_init( replyMessage, &replyArgs ) )
                    {
                        if ( DBUS_TYPE_UINT32 == dbus_message_iter_get_arg_type( &replyArgs ) )
                        {
                            dbus_message_iter_get_basic( &replyArgs, &loadedDisplayId );
                            if ( loadedDisplayId == displayId )
                            {
                                if ( verbose )
                                {
                                    cout << QObject::tr( "Vampir instance at bus name " ).toUtf8().data() << busName
                                         << QObject::tr( " loaded display " ).toUtf8().data() << DisplayTypeToString( type )
                                         << QObject::tr( " with display id " ).toUtf8().data() << displayId
                                         << QObject::tr( " for session " ).toUtf8().data() << it->second.sessionId << endl;
                                }

                                //FIXME:
                                sleep( 1 );
                                return true;
                            }
                        }
                    }
                }
                //check signal for loading errors
                else if ( replyMessage != 0 &&
                          dbus_message_is_signal( replyMessage, interfaceName.c_str(),
                                                  "status" ) )
                {
                    //check status
                    //a status id which is greater than 0 is not ok and was error is occurred
                    if ( dbus_message_iter_init( replyMessage, &replyArgs ) )
                    {
                        if ( DBUS_TYPE_UINT32 == dbus_message_iter_get_arg_type( &replyArgs ) )
                        {
                            dbus_message_iter_get_basic( &replyArgs, &statusId );
                            if ( statusId > 0 )
                            {
                                if ( verbose )
                                {
                                    cout << QObject::tr( "Vampir instance at bus name " ).toUtf8().data() << busName
                                         << QObject::tr( " threw error status id " ).toUtf8().data() << statusId << endl;
                                }
                                return false;
                            }
                        }
                    }
                }

                //do nothing and wait for right signal
                if ( replyMessage != 0 )
                {
                    dbus_message_unref( replyMessage );
                }
                replyMessage = 0;
            }
        }
    }

    return false;
}

bool
VampirConnecter::ZoomDisplay( DisplayType type,
                              double startTick, double endTick )
{
    char buffer[ 128 ];
    ( void )type;

    //we can only have one opened file at the monent, so we take the first from map
    map<std::string const, trace_file_session>::iterator it_sessions;
    it_sessions = openedFiles.begin();
    trace_file_session session = it_sessions->second;

    //we open only one display at the moment, so we can take the first display from display map
    map<dbus_uint32_t, DisplayType>::iterator it_displays;
    it_displays = session.displayIds.begin();


    string tickunit       = QObject::tr( "seconds" ).toUtf8().data();
    double tickMultiplier = 1;
//     switch ( fileType )
//     {
// //         case ELG:
// //             tickMultiplier = 1e9;
// //              tickunit = "ticks"
// //             break;
// //         case OTF:
// //             tickMultiplier = 1e10;
// //              tickunit = "ticks"
// //             break;
// //         default: // defualt are seconds
// //             tickMultiplier = 1;
//         break;
//     }

    InitiateCommunication( "setZoom" );
    sprintf( buffer, "%f", tickMultiplier * startTick );
    AddMessage( buffer );
    sprintf( buffer, "%f", tickMultiplier * endTick );
    AddMessage( buffer );
    AddMessage( tickunit );
    AddMessage( it_displays->first );
    AddMessage( session.sessionId );

    if ( verbose )
    {
        cout << QObject::tr( "Vampir instance at bus name " ).toUtf8().data() << busName
             << QObject::tr( " start: " ).toUtf8().data() << tickMultiplier * startTick
             << QObject::tr( " end: " ).toUtf8().data() << tickMultiplier * endTick
             << QObject::tr( "numberFormat: " ).toUtf8().data() << tickunit
             << QObject::tr( " zoom display: " ).toUtf8().data() << it_displays->first
             << QObject::tr( " session id: " ).toUtf8().data() << session.sessionId << endl;
    }

    return CompleteCommunication( true, true );
}

bool
VampirConnecter::isDisplayOpen( DisplayType type, dbus_uint32_t* displayId )
{
    dbus_uint32_t_list* openDisplayIdList = NULL;
    char*               buffer            = NULL;

    //we can only have one opened file at the monent, so we take the first from map
    map<std::string const, trace_file_session>::iterator it_sessions;
    it_sessions = openedFiles.begin();
    trace_file_session session = it_sessions->second;

    InitiateCommunication( "listDisplays" );
    AddMessage( session.sessionId );

    if ( verbose )
    {
        cout << QObject::tr( "In isDisplayOpen: We will check all opened display, now" ).toUtf8().data()  << endl;
    }

    //get all opened display ids and check the display name
    if ( CompleteCommunication( true, &openDisplayIdList ) )
    {
        //check display id step by step
        while ( openDisplayIdList )
        {
            //now we must ask vampir after the name for current display id
            //after that, we must check the display name with the name, which we want to open
            InitiateCommunication( "displayIdToText" );
            AddMessage( openDisplayIdList->data );
            AddMessage( session.sessionId );
            CompleteCommunication( true, &buffer );

            if ( verbose )
            {
                cout << QObject::tr( "In isDisplayOpen: Display id " ).toUtf8().data() << openDisplayIdList->data << QObject::tr( " is a " ).toUtf8().data() <<  buffer << QObject::tr( " type" ).toUtf8().data()  << endl;
            }

            if ( strcmp( DisplayTypeToString( type ).c_str(), buffer ) == 0 )
            {
                if ( verbose )
                {
                    cout << QObject::tr( "In isDisplayOpen: Display is open " ).toUtf8().data() << ( char* )buffer << " == " << DisplayTypeToString( type ) << endl;
                    cout << QObject::tr( "In isDisplayOpen: We have found an already opened display!!!" ).toUtf8().data()  << endl;
                }

                *displayId = openDisplayIdList->data;
                free( buffer );
                destroyUint32_t_list( openDisplayIdList );
                return true;
            }
            else
            {
                if ( verbose )
                {
                    cout << QObject::tr( "In isDisplayOpen: " ).toUtf8().data() << ( char* )buffer << " != " << DisplayTypeToString( type ) << endl;
                }
                free( buffer );
            }

            openDisplayIdList = openDisplayIdList->next;
        }
    }

    if ( verbose )
    {
        cout << QObject::tr( "In isDisplayOpen: Display is NOT open." ).toUtf8().data()  << endl;
    }

    destroyUint32_t_list( openDisplayIdList );
    return false;
}

void
VampirConnecter::destroyUint32_t_list( dbus_uint32_t_list* list )
{
    while ( list )
    {
        dbus_uint32_t_list* next = list->next;
        delete list;
        list = next;
    }
}

void
VampirConnecter::Exit()
{
    if ( verbose )
    {
        cout << QObject::tr( "Calling exit on vampir client at bus name " ).toUtf8().data() << busName << endl;
    }
    InitiateCommunication( "quit" );
    CompleteCommunicationGeneric( true );
}

bool
VampirConnecter::IsActive() const
{
    return isActive;
}

VampirConnecter::~VampirConnecter()
{
    if ( ExistsVampirWithBusName( busName ) )
    {
        Exit();
    }
}

bool
VampirConnecter::ExistsVampirWithBusName( string const& name )
{
    DBusError error;
    dbus_error_init( &error );
    //connect to the bus and check for errors
    DBusConnection* connection = dbus_bus_get( DBUS_BUS_SESSION, &error );
    //Prevent the application from being closed when the connection closes
    dbus_connection_set_exit_on_disconnect( connection,  false );
    CheckError( error );
    if ( connection == 0 )
    {
        throw VampirConnecterException( QObject::tr( "Connection failed" ).toUtf8().data() );
    }
    bool res = dbus_bus_name_has_owner( connection, name.c_str(), &error );
    dbus_error_free( &error );
    return res;
}

string
VampirConnecter::GetVampirBusName( int index )
{
    assert( index < GetMaxVampirNumber() );
    switch ( index )
    {
        case 0:
            return "com.gwt.vampir";
        case 1:
            return "com.gwt.vampir.slave";
        default:
            return "";
    }
}

int
VampirConnecter::GetMaxVampirNumber()
{
    return 2;
}
