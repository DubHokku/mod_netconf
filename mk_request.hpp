#include <iostream>
#include <libyang/libyang.h>
#include <nc_client.h>

class mk_request
{
    public:
    mk_request();
    ~mk_request();
    
    struct nc_rpc* config_add( char* );
    struct nc_rpc* config_rm( char* );
    
    
    struct nc_rpc* get_interfaces( struct ly_ctx*, char* );
    struct nc_rpc* get_interface_info( struct ly_ctx*, char* );
    struct nc_rpc* get_neighbors( struct ly_ctx*, char* );
    
    struct nc_rpc* get_vlans( struct ly_ctx*, char* );
    struct nc_rpc* get_subscribe( uint16_t, char*, char*, char*, char* );
    
    struct nc_rpc* run_to_prepare();
    // struct nc_rpc* prepare_to_run();
    struct nc_rpc* commit();
    
    struct nc_rpc* lock_running();
    struct nc_rpc* unlock_running();
    struct nc_rpc* lock_candidate();
    struct nc_rpc* unlock_candidate();
    
    
    
    private:
};

mk_request::mk_request(){}
mk_request::~mk_request(){}

struct nc_rpc* mk_request::config_add( char* config )
{ 
    struct nc_rpc* rp_request;
    edit_config = ( char* )malloc( strlen( config ));
    sprintf( edit_config, "%s", config );
    
    rp_request = nc_rpc_edit( NC_DATASTORE_CANDIDATE, NC_RPC_EDIT_DFLTOP_MERGE, NC_RPC_EDIT_TESTOPT_TESTSET, 
        NC_RPC_EDIT_ERROPT_STOP, edit_config, NC_PARAMTYPE_FREE );
    
    return rp_request;
}

struct nc_rpc* mk_request::config_rm( char* config )
{
    struct nc_rpc* rp_request;
    edit_config = ( char* )malloc( strlen( config ));
    sprintf( edit_config, "%s", config );
    
    rp_request = nc_rpc_edit( NC_DATASTORE_CANDIDATE, NC_RPC_EDIT_DFLTOP_REPLACE, NC_RPC_EDIT_TESTOPT_TESTSET, 
        NC_RPC_EDIT_ERROPT_STOP, edit_config, NC_PARAMTYPE_FREE );
    
    return rp_request;
}

struct nc_rpc* mk_request::get_interfaces( struct ly_ctx* junos_ctx, char* interface )
{
    struct nc_rpc* rp_request;
    
    // rp_request = nc_rpc_getconfig( NC_DATASTORE_RUNNING, "<configuration><interfaces><interface><name>ge-0/0/0</name></interface></interfaces></configuration>", NC_WD_UNKNOWN, NC_PARAMTYPE_CONST );
    rp_request = nc_rpc_getconfig( NC_DATASTORE_RUNNING, "<configuration></configuration>", NC_WD_UNKNOWN, NC_PARAMTYPE_CONST );

    
    if( rp_request == NULL )
        std::cout << "nc_rpc_act_generic() ret. null \n";
    else
        std::cout << "nc_rpc_act_generic() ret. RPC \n";
    
    return rp_request;
}

struct nc_rpc* mk_request::get_interface_info( struct ly_ctx* junos_ctx, char* interface )
{
    struct nc_rpc* rp_request;
    
    char path[] = "/junos-rpc-interfaces:get-interface-information/interface-name";
    // char value[] = "ge-0/0/0.0";
    
    struct lyd_node *path_conf = lyd_new_path( NULL, junos_ctx, path, interface, ( LYD_ANYDATA_VALUETYPE )0, 0 );
    lyd_schema_sort( path_conf, 0 );
    
/* lyd print mem */
    void* xml_dump = malloc( 1024 );
    lyd_print_mem(( char** )&xml_dump, path_conf, LYD_XML, LYP_NETCONF );
    std::cout << "xml dump\n" << ( char* )xml_dump << std::endl;
/* /lyd print mem */
    
    rp_request = nc_rpc_act_generic(( const struct lyd_node* )path_conf, NC_PARAMTYPE_FREE );
    if( rp_request == NULL )
        std::cout << "nc_rpc_act_generic() ret. null \n";
    else
        std::cout << "nc_rpc_act_generic() ret. RPC \n";
    
    return rp_request;
}

struct nc_rpc* mk_request::get_neighbors( struct ly_ctx* junos_ctx, char* interface )
{
    struct nc_rpc* rp_request;
    
    // char path[] = "/junos-rpc-interfaces:get-interface-information/interface-name";
    char path[] = "/junos-rpc-lldp:get-lldp-interface-neighbors/interface-device";
    // char value[] = "ge-0/0/0.0";
    
    struct lyd_node *path_conf = lyd_new_path( NULL, junos_ctx, path, interface, ( LYD_ANYDATA_VALUETYPE )0, 0 );
    lyd_schema_sort( path_conf, 0 );
    
/* lyd print mem */
    void* xml_dump = malloc( 1024 );
    lyd_print_mem(( char** )&xml_dump, path_conf, LYD_XML, LYP_NETCONF );
    std::cout << "xml dump\n" << ( char* )xml_dump << std::endl;
/* /lyd print mem */
    
    rp_request = nc_rpc_act_generic(( const struct lyd_node* )path_conf, NC_PARAMTYPE_FREE );
    if( rp_request == NULL )
        std::cout << "nc_rpc_act_generic() ret. null \n";
    else
        std::cout << "nc_rpc_act_generic() ret. RPC \n";
    
    return rp_request;
}

struct nc_rpc* mk_request::get_vlans( struct ly_ctx* junos_ctx, char* interface )
{
    struct nc_rpc* rp_request;
    
    char path[] = "/junos-rpc-vlans:get-vlan-information/vlan-name";
    struct lyd_node *path_conf = lyd_new_path( NULL, junos_ctx, path, NULL, ( LYD_ANYDATA_VALUETYPE )0, 0 );
    lyd_schema_sort( path_conf, 0 );
    
/* lyd print mem */
    void* xml_dump = malloc( 1024 );
    lyd_print_mem(( char** )&xml_dump, path_conf, LYD_XML, LYP_NETCONF );
    std::cout << "xml dump\n" << ( char* )xml_dump << std::endl;
/* /lyd print mem */

    rp_request =  nc_rpc_act_generic(( const struct lyd_node* )path_conf, NC_PARAMTYPE_FREE );
    if( rp_request == NULL )
        std::cout << "nc_rpc_act_generic() ret. null \n";
    else
        std::cout << "nc_rpc_act_generic() ret. RPC \n";
    
    return rp_request;
}

struct nc_rpc* mk_request::run_to_prepare()
{
    struct nc_rpc* rp_request;
    rp_request = nc_rpc_copy( NC_DATASTORE_CANDIDATE, NULL, NC_DATASTORE_RUNNING, NULL, NC_WD_UNKNOWN, NC_PARAMTYPE_CONST );
    
    return rp_request;
}

struct nc_rpc* mk_request::prepare_to_run()
{
    struct nc_rpc* rp_request;
    rp_request = nc_rpc_copy( NC_DATASTORE_RUNNING, NULL, NC_DATASTORE_CANDIDATE, NULL, NC_WD_UNKNOWN, NC_PARAMTYPE_CONST );
    
    return rp_request;
}

struct nc_rpc* mk_request::lock_running()
{
    return nc_rpc_lock( NC_DATASTORE_RUNNING );
}

struct nc_rpc* mk_request::unlock_running()
{
    return nc_rpc_unlock( NC_DATASTORE_RUNNING );
}

struct nc_rpc* mk_request::lock_candidate()
{
    return nc_rpc_lock( NC_DATASTORE_CANDIDATE );
}

struct nc_rpc* mk_request::unlock_candidate()
{
    return nc_rpc_unlock( NC_DATASTORE_CANDIDATE );
}

struct nc_rpc* mk_request::commit()
{
    struct nc_rpc* rp_request;
    
    rp_request = nc_rpc_commit( 1, 100, NULL, NULL, NC_PARAMTYPE_CONST );
    
    return rp_request;
}

