#pragma once

#include "nc_connect.hpp"
#include "mk_request.hpp"



int main( int ac, char** av )
{
    char user[] = "root";
    // char host[] = "192.168.33.3";
    // char host[] = "192.168.22.2";
    char host[] = "192.168.11.3";
    char* xreply;
    
    init_auth();
    nc_connect* netconf = new nc_connect;
    mk_request* request = new mk_request;
    
    struct ly_ctx* junos_ctx = netconf->mk_context();
    uint16_t session_ident = netconf->mk_session( user, host, junos_ctx );
    struct nc_rpc* rp_request;
    
    char interface[] = "ge-0/0/0";

/* lock datastore */ 
    std::cout << "lock datastore\n";
    netconf->nc_send_receive_xml( netconf->get_session( session_ident ), nc_rpc_lock( NC_DATASTORE_CANDIDATE ), &xreply );
    std::cout << "xreply:\n" << xreply << std::endl;
    // nc_rpc_free( rp_request );
/* copy config */ 
    std::cout << "copy config\n";
    netconf->nc_send_receive_xml( netconf->get_session( session_ident ), request->run_to_prepare(), &xreply );
    std::cout << "xreply:\n" << xreply << std::endl;
    
/* config_add 
    char query[] = "<configuration><interfaces><interface><name>ge-0/0/1</name><mtu>1417</mtu></interface></interfaces></configuration>";
    // char query[] = "<configuration><version>any_text</version></configuration>";
    rp_request = request->config_add( query );
    netconf->nc_send_receive_xml( netconf->get_session( session_ident ), rp_request, &xreply );
    std::cout << "xreply:\n" << xreply << std::endl;
    nc_rpc_free( rp_request ); 
*/
    
/* config_rm */
    char query[] = "<configuration><interfaces><interface><name>ge-0/0/1</name><mtu>1417</mtu></interface></interfaces></configuration>";
    rp_request = request->config_rm( query );
    netconf->nc_send_receive_xml( netconf->get_session( session_ident ), rp_request, &xreply );
    std::cout << "xreply:\n" << xreply << std::endl;
    nc_rpc_free( rp_request ); 
    
/* commit */
    std::cout << "mk commit\n";
    rp_request = request->commit();
    netconf->nc_send_receive_xml( netconf->get_session( session_ident ), rp_request, &xreply );
    std::cout << "xreply:\n" << xreply << std::endl;
    nc_rpc_free( rp_request );
    
/* unlock datastore */    
    std::cout << "unlock datastore\n";
    netconf->nc_send_receive_xml( netconf->get_session( session_ident ), nc_rpc_unlock( NC_DATASTORE_CANDIDATE ), &xreply );
    std::cout << "xreply:\n" << xreply << std::endl;
    
/* get_interfaces */
    rp_request = request->get_interfaces( junos_ctx, interface );
    netconf->nc_send_receive_xml( netconf->get_session( session_ident ), rp_request, &xreply );
    std::cout << "xreply:\n" << xreply << std::endl;
    nc_rpc_free( rp_request ); 
    
    std::cout << "connection done \n";
    return 0;
}
