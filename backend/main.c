#include "server.h"
#include "routing.h"
#include "routes.h"

int main(int argc, char **argv){
    
    init_routes();
    add_route(&route_root);
    add_route(&route_js);
    add_route(&route_css);  
    add_route(&route_png); 
    add_route(&route_post_data);
    add_route(&route_get_data);

    websock();      
    
    return 0; 
}