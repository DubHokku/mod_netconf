#pragma once

//#ifndef NC_CONNECT_HPP
//#define NC_CONNECT_HPP

#include <iostream>
#include <string.h>
#include <map>

#include <libyang/libyang.h>
#include <nc_client.h>


// namespace runos {
char* secret, *edit_config;
std::map<std::string, std::string> auth_db;

void init_auth()
{
    char login[] = "root";
    char address[] = "192.168.33.3";
    
    char* user = ( char* )malloc( sizeof( login ) + 1 );
    char* host = ( char* )malloc( sizeof( address ) + 1 );
    strcpy( user, login );
    strcpy( host, address );
    
    std::string account( "root@192.168.33.3" );
    std::string password( "Qwerty1234" );
    
    auth_db.insert( std::pair<std::string, std::string>( account, password ));
}

char* get_secret( const char* user, const char* host, void* pwd )
{
    std::map<std::string, std::string>::iterator auth_ptr;
    
    char* account = ( char*)malloc( strlen( user ) + strlen( host ) + 1 );
    sprintf( account, "%s@%s", user, host );
    
    auth_ptr = auth_db.find( account );
    if( auth_ptr != auth_db.end())
    {
        secret = ( char* )malloc( auth_ptr->second.size() + 1 );
        strcpy( secret, auth_ptr->second.c_str());
    }
    else
        secret = NULL;
    
    
    return ( char* )secret;
}

class nc_connect
{
    public:
    nc_connect();
    ~nc_connect();
    
    struct ly_ctx* mk_context();
    uint16_t mk_session( char*, char*, struct ly_ctx* );
    struct nc_session* get_session( uint16_t );
    // struct nc_reply* nc_send_receive( uint16_t, struct nc_rpc* );
    int nc_send_receive_xml( struct nc_session*, struct nc_rpc*, char** );
    void* notify_handler();
    
    uint64_t msg_id;
    uint64_t timeout;
    struct nc_session* rp_session;
    std::map<uint16_t, struct nc_session*> sessions;
    
    private:
};

nc_connect::nc_connect()
{
    nc_client_init();
    
    // NC_VERB_LEVEL lverbose = NC_VERB_ERROR; // Print only error messages. 
    // NC_VERB_LEVEL lverbose = NC_VERB_WARNING; // Print error and warning messages. 
    // NC_VERB_LEVEL lverbose = NC_VERB_VERBOSE; // Besides errors and warnings, print some other verbose messages. 
    NC_VERB_LEVEL lverbose = NC_VERB_DEBUG; // Print all messages including some development debug messages. 
    
    nc_verbosity( lverbose );
    
    msg_id = 1;
    // timeout = 17;
    timeout = 1217;
}

nc_connect::~nc_connect()
{
    nc_client_destroy();   
}

// } // namespace
//#endif /* MY_HEADER_H */
