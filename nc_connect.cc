// #pragma once

#ifndef NC_CONNECT_HPP
#define NC_CONNECT_HPP

#include "nc_connect.hpp"
#include "mk_request.hpp"
#endif /* MY_HEADER_H */

struct ly_ctx* nc_connect::mk_context()
{
/* mk context   */
    struct ly_ctx* junos_ctx;
    const char ly_mod_path[] = "/home/fdmp/client_nc/yang";
    junos_ctx = ly_ctx_new( ly_mod_path, LY_CTX_ALLIMPLEMENTED );
    
/* add yang module ietf */    
    char path_ietf_netconf[] = "/home/fdmp/client_nc/yang/nc-modules/ietf-netconf@2013-09-29.yang";
    const struct lys_module* parse_ietf = lys_parse_path( junos_ctx, path_ietf_netconf, LYS_IN_YANG );
    
    // char path_ietf_monitoring[] = "/home/fdmp/client_nc/yang/nc-modules/ietf-netconf-monitoring@2010-10-04.yang";
    // const struct lys_module* parse_ietf_monitoring = lys_parse_path( junos_ctx, path_ietf_monitoring, LYS_IN_YANG );

/* feature set */    
    std::cout << "yang set features \n";
    uint8_t usr_feature_filter = 0;
    uint8_t* state = &usr_feature_filter;
    char **feature_list = ( char** )lys_features_list( parse_ietf, &state );
    
    for( int i = 0; feature_list[i] > 0; i++ )
        std::cout << "feature[" << i << "] " << feature_list[i] << std::endl;
    lys_features_enable( parse_ietf, "*" );

/* add yang module junos */
    char path_junos_common[] = "/home/fdmp/client_nc/yang/common/junos-common-types@2019-01-01.yang";
    const struct lys_module* parse_junos_common = lys_parse_path( junos_ctx, path_junos_common, LYS_IN_YANG );
    
/* add yang module rpc */
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
    if( session_ptr != sessions.cbegin())
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
/*    
    uint16_t session_ident
    auto session_ptr = sessions.find( session_ident );
    if( session_ptr != sessions.end())
        rp_session = session_ptr->second;
    else
        std::cout << "session id " << session_ident << " not found!\n";
*/    
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
            // std::cout << "reply:\n" << *xreply << std::endl;
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

struct nc_rpc* generic_rpc( struct ly_ctx* junos_ctx )
{
    struct nc_rpc* rp_request;
    //.char path[] = "/junos-conf-root:configuration/apply-groups[.='interfaces-group']";
    //.char value[] = "interfaces/interface-set[name='ge-0/0/0']/unit[name='0']/family/inet/address[name='192.168.33.3/24']";
    
    char path[] = "/junos-rpc-interfaces:get-interface-information/interface-name";
    char value[] = "ge-0/0/0.0";
    
    struct lyd_node *path_conf = lyd_new_path( NULL, junos_ctx, path, value, ( LYD_ANYDATA_VALUETYPE )0, 0 );
    lyd_schema_sort( path_conf, 0 );
    
/* lyd print mem */
    void* xml_dump = malloc( 1024 );
    lyd_print_mem(( char** )&xml_dump, path_conf, LYD_XML, LYP_NETCONF );
    std::cout << "xml dump\n" << ( char* )xml_dump << std::endl;
/* /lyd print mem */

    // rp_request = nc_rpc_act_generic(( const struct lyd_node* )path_conf, NC_PARAMTYPE_CONST );
    rp_request = nc_rpc_act_generic(( const struct lyd_node* )path_conf, NC_PARAMTYPE_FREE );
    if( rp_request == NULL )
        std::cout << "nc_rpc_act_generic() ret. null \n";
    else
        std::cout << "nc_rpc_act_generic() ret. RPC \n";
    
    return rp_request;
}


int main( int ac, char** av )
{
    char user[] = "root";
    char host[] = "192.168.33.3";
    char* xreply;
    
    init_auth();
    nc_connect* netconf = new nc_connect;
    mk_request* request = new mk_request;
    
    struct ly_ctx* junos_ctx = netconf->mk_context();
    uint16_t session_ident = netconf->mk_session( user, host, junos_ctx );
    struct nc_rpc* rp_request;
    // struct nc_rpc* rp_request = generic_rpc( junos_ctx );
    
    char interface[] = "ge-0/0/0";
    
/* get_interface_info
    rp_request = request->get_interface_info( junos_ctx, interface );
    netconf->nc_send_receive_xml( netconf->netconf->get_session( session_ident ), rp_request, &xreply );
    std::cout << "xreply:\n" << xreply << std::endl;
    nc_rpc_free( rp_request ); 
*/
/* get_neighbors
    rp_request = request->get_neighbors( junos_ctx, interface );
    netconf->nc_send_receive_xml( netconf->netconf->get_session( session_ident ), rp_request, &xreply );
    std::cout << "xreply:\n" << xreply << std::endl;
    nc_rpc_free( rp_request );
*/
/* get-vlan-information 
    rp_request = request->get_vlans( junos_ctx, interface );
    netconf->nc_send_receive_xml( netconf->get_session( session_ident ), rp_request, &xreply );
    std::cout << "xreply:\n" << xreply << std::endl;
    nc_rpc_free( rp_request ); */

/* mk_vlan */
    std::cout << "lock datastore\n";
    netconf->nc_send_receive_xml( netconf->get_session( session_ident ), nc_rpc_lock( NC_DATASTORE_CANDIDATE ), &xreply );
    std::cout << "xreply:\n" << xreply << std::endl;
    // nc_rpc_free( rp_request );
    
    std::cout << "copy config\n";
    netconf->nc_send_receive_xml( netconf->get_session( session_ident ), request->run_to_prepare(), &xreply );
    std::cout << "xreply:\n" << xreply << std::endl;
    // nc_rpc_free( rp_request );
    
    std::cout << "edit config\n";
    rp_request = request->mk_vlan();
    netconf->nc_send_receive_xml( netconf->get_session( session_ident ), rp_request, &xreply );
    std::cout << "xreply:\n" << xreply << std::endl;
    nc_rpc_free( rp_request );

    std::cout << "unlock datastore\n";
    netconf->nc_send_receive_xml( netconf->get_session( session_ident ), nc_rpc_unlock( NC_DATASTORE_CANDIDATE ), &xreply );
    std::cout << "xreply:\n" << xreply << std::endl;
    // nc_rpc_free( rp_request );    
    
    std::cout << "mk commit\n";
    rp_request = request->commit();
    netconf->nc_send_receive_xml( netconf->get_session( session_ident ), rp_request, &xreply );
    std::cout << "xreply:\n" << xreply << std::endl;
    nc_rpc_free( rp_request ); 

    
    
    // std::cout << "xreply:\n" << xreply << std::endl;
    std::cout << "connection done \n";
    
    return 0;
}
