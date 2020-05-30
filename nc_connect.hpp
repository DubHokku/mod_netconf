#pragma once

#include <iostream>
#include <string.h>
#include <map>

#include <libyang/libyang.h>
#include <nc_client.h>


char* secret, *edit_config;
std::map<std::string, std::string> auth_db;

void init_auth()
{
    // char login[] = "root";
    // char address[] = "192.168.33.3";
    // char address[] = "192.168.22.2";
    // char address[] = "192.168.11.3";
    
    // char* user = ( char* )malloc( sizeof( login ) + 1 );
    // char* host = ( char* )malloc( sizeof( address ) + 1 );
    // strcpy( user, login );
    // strcpy( host, address );
    
    std::string account_1( "root@192.168.11.3" );
    std::string account_nc( "root@192.168.33.3" );
    std::string account_2( "root@192.168.22.2" );
    std::string password( "Qwerty1234" );
    
    auth_db.insert( std::pair<std::string, std::string>( account_1, password ));
    auth_db.insert( std::pair<std::string, std::string>( account_nc, password ));
    auth_db.insert( std::pair<std::string, std::string>( account_2, password ));
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
    timeout = 17;
}

nc_connect::~nc_connect()
{
    nc_client_destroy();   
}

struct ly_ctx* nc_connect::mk_context()
{
/* mk context   */
    struct ly_ctx* junos_ctx;
    const char ly_mod_path[] = "/home/fdmp/client_nc/yang";
    junos_ctx = ly_ctx_new( ly_mod_path, LY_CTX_ALLIMPLEMENTED );
    
/* add yang module ietf */    
    char path_ietf_netconf[] = "/home/fdmp/client_nc/yang/nc-modules/ietf-netconf@2013-09-29.yang";
    const struct lys_module* parse_ietf = lys_parse_path( junos_ctx, path_ietf_netconf, LYS_IN_YANG );
    
    char path_ietf_monitoring[] = "/home/fdmp/client_nc/yang/nc-modules/ietf-netconf-monitoring@2010-10-04.yang";
    const struct lys_module* parse_ietf_monitoring = lys_parse_path( junos_ctx, path_ietf_monitoring, LYS_IN_YANG );
    
    char path_ietf_defaults[] = "/home/fdmp/client_nc/yang/ly-modules/ietf-netconf-with-defaults.yang";
    const struct lys_module* parse_ietf_defaults = lys_parse_path( junos_ctx, path_ietf_defaults, LYS_IN_YANG );

/* feature set */    
    std::cout << "yang set features \n";
    uint8_t usr_feature_filter = 0;
    uint8_t* state = &usr_feature_filter;
    char **feature_list = ( char** )lys_features_list( parse_ietf, &state );
    
    for( int i = 0; feature_list[i] > 0; i++ )
        std::cout << "feature[" << i << "] " << feature_list[i] << std::endl;
    lys_features_enable( parse_ietf, "*" );

/* add yang module junos common */
    char path_junos_common[] = "/home/fdmp/client_nc/yang/common/junos-common-types@2019-01-01.yang";
    const struct lys_module* parse_junos_common = lys_parse_path( junos_ctx, path_junos_common, LYS_IN_YANG );
    
/* add yang module junos conf */
    char path_junos_root[] = "/home/fdmp/client_nc/yang/junos/conf/junos-conf-root@2019-01-01.yang";
    const struct lys_module* parse_junos_root = lys_parse_path( junos_ctx, path_junos_root, LYS_IN_YANG );
    
    char path_junos_conf[] = "/home/fdmp/client_nc/yang/junos/conf/junos-conf-interfaces@2019-01-01.yang";
    const struct lys_module* parse_junos = lys_parse_path( junos_ctx, path_junos_conf, LYS_IN_YANG );
    
/* add yang module junos rpc */
    char path_rpc_interfaces[] = "/home/fdmp/client_nc/yang/junos/rpc/junos-rpc-interfaces@2019-01-01.yang";
    const struct lys_module* parse_rpc_interfaces = lys_parse_path( junos_ctx, path_rpc_interfaces, LYS_IN_YANG );
    
    char path_rpc_lldp[] = "/home/fdmp/client_nc/yang/junos-rpc-lldp.yang";
    const struct lys_module* parse_rpc_lldp = lys_parse_path( junos_ctx, path_rpc_lldp, LYS_IN_YANG );
    
    char path_rpc_vlan[] = "/home/fdmp/client_nc/yang/junos/rpc/junos-rpc-vlans@2019-01-01.yang";
    const struct lys_module* parse_rpc_vlaN = lys_parse_path( junos_ctx, path_rpc_vlan, LYS_IN_YANG );    
    
    return junos_ctx;
}

uint16_t nc_connect::mk_session( char* user, char* host, struct ly_ctx* junos_ctx )
{
    uint16_t session_ident = 1;
    
    nc_client_ssh_set_username( user );
    nc_client_ssh_set_auth_pref( NC_SSH_AUTH_PASSWORD, 1 );
    nc_client_ssh_set_auth_pref( NC_SSH_AUTH_PUBLICKEY, -1 );
    nc_client_ssh_set_auth_pref( NC_SSH_AUTH_INTERACTIVE, -1 );

    char* (*func)( const char* user, const char* host, void *priv ) = &get_secret;
    nc_client_ssh_set_auth_password_clb( func, NULL );

    rp_session = nc_connect_ssh( host, 830, junos_ctx );

    auto session_ptr = sessions.cend();
    if( session_ptr != sessions.crbegin())
        session_ident = session_ptr->first + 1;
    
    sessions.insert( std::pair<uint16_t, struct nc_session*>( session_ident, rp_session ));
    
    return session_ident;
}

struct nc_session* nc_connect::get_session( uint16_t session_ident )
{
    struct nc_session* rp_session = NULL;
    
    auto session_ptr = sessions.find( session_ident );
    if( session_ptr != sessions.end())
        rp_session = session_ptr->second;
    else
        std::cout << "session id " << session_ident << " not found!\n";
    
    return rp_session;
}

int nc_connect::nc_send_receive_xml( struct nc_session* rp_session, struct nc_rpc* rp_request, char** xreply )
{
    NC_MSG_TYPE msgtype;
    struct lyxml_elem *xmsg;
   
    msgtype = nc_send_rpc( rp_session, rp_request, timeout, &msg_id );
    if( msgtype == NC_MSG_ERROR )
    {
        std::cout << "Failed to send the RPC.\n";
        return -1;
    }
    else
        if( msgtype == NC_MSG_WOULDBLOCK )
        {
            std::cout << "Timeout for sending the RPC expired.\n";
            return -1;
        }
        
recv_reply:        
        
    msgtype = nc_recv_xml( rp_session, timeout, msg_id, &xmsg );
    
    if( msgtype == NC_MSG_RPC )
    {
        std::cout << "Failed to receive a reply \n";
        if( nc_session_get_status( rp_session ) != NC_STATUS_RUNNING ) 
        {
            std::cout << "session status fault \n";
            // cmd_disconnect( NULL, NULL );
        }
        return -1;
    } 
    else 
        if( msgtype == NC_MSG_WOULDBLOCK ) 
        {
            std::cout << "Timeout for receiving a reply expired \n";
            return -1;
        } 
        else 
            if( msgtype == NC_MSG_NOTIF ) 
            {
                std::cout << "NC_MSG_NOTIF \n";
                /* read again */
                goto recv_reply;
            } 
            else 
                if( msgtype == NC_MSG_REPLY_ERR_MSGID ) 
                {
                    std::cout << "NC_MSG_REPLY_ERR_MSGID \n";
                    /* unexpected message, try reading again to get the correct reply */
                    std::cout << "Unexpected reply received - ignoring and waiting for the correct reply \n";
                    // nc_reply_free( rp_reply );
                    goto recv_reply;
                }
    
    switch( msgtype )
    {
        case NC_MSG_ERROR:
            std::cout << "msgtype NC_MSG_ERROR\n";
        break;
        case NC_MSG_WOULDBLOCK:
            std::cout << "msgtype NC_MSG_WOULDBLOCK\n";
        break;
        case NC_MSG_NONE:
            std::cout << "msgtype NC_MSG_NONE\n";
        break;
        case NC_MSG_HELLO:
            std::cout << "msgtype NC_MSG_HELLO\n";
        break;
        case NC_MSG_BAD_HELLO:
            std::cout << "msgtype NC_MSG_BAD_HELLO\n";
        break;
        case NC_MSG_RPC:
            std::cout << "msgtype NC_MSG_RPC\n";
        break;
        case NC_MSG_REPLY:
            std::cout << "msgtype NC_MSG_REPLY\n";
            lyxml_print_mem( xreply, xmsg, LYXML_PRINT_SIBLINGS );
        break;
        case NC_MSG_REPLY_ERR_MSGID:
            std::cout << "msgtype NC_MSG_REPLY_ERR_MSGID\n";
        break;
        case NC_MSG_NOTIF:
            std::cout << "msgtype NC_MSG_NOTIF\n";
        break;
        default:
            std::cout << "msgtype default\n";
        break;
    }
    msg_id++;
    
    return 0;
}


