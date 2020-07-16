# Cerver

Cerver is C backend Server dedicated to handle REST requests. This server allows to set custom REST resource routes with custom handles.

# How to setup the server - Creating custom routes 

To add custom route, go to file named ```routes.c``` and declare new structure of Route type like ones visible belowe. Example routes shows how to send a file to html/js client.


```
// *********************************************** USER ROUTES *******************************************//

Route route_root = {.request=GET , .addr="/", .fnc_ptr=root_response};
Route route_js = {.request=GET , .addr="/script.js", .fnc_ptr=js_response};
Route route_css = {.request=GET , .addr="/style.css", .fnc_ptr=css_response};
Route route_png = {.request=GET ,.addr="/water.png", .fnc_ptr=png_response};
Route route_post_data = {.request=GET , .addr="/ReadDev", .fnc_ptr=get_data};
Route route_get_data = {.request=POST , .addr="/ReadDev", .fnc_ptr=post_data};
```

The Route structure consists of three fields: ```request``` holding REST reuqest command, ```addr``` holding path to resource and callback to custom handler. 

```
struct Route{
    uint8_t request;
    char *addr;
    void (*fnc_ptr)(uint8_t sockfd,uint8_t request, uint8_t* request_content,size_t length_data); 
}typedef Route;
```

Custom callback can be created based on the one below. The arguments it is receiving are obligatory the same as ```response``` function. 

```
void example(uint8_t sockfd,uint8_t request, uint8_t* request_content,size_t length_data){

    response(sockfd,"200 OK",content,content_size,"text/html"); 
};
```

Response function should be filled with ```request_code``` like "200 OK", ```content_ptr``` content pointer, ```content_size``` content size, ```type``` MIME type.

```
response(sockfd,request_code,content_ptr,content_size,type); 
```

Next custom route and handler should be added to ```routes.h```. Route need to be added as extern to be passed during initialization. 

```
// *********************************************** USER ROUTES *******************************************//

void root_response(uint8_t sockfd, uint8_t request, uint8_t* request_content,size_t length_data);
void js_response(uint8_t sockfd, uint8_t request, uint8_t* request_content,size_t length_data);
void css_response(uint8_t sockfd, uint8_t request, uint8_t* request_content,size_t length_data);
void png_response(uint8_t sockfd, uint8_t request, uint8_t* request_content,size_t length_data);
void get_data(uint8_t sockfd,uint8_t request,uint8_t* request_content,size_t length_data);
void post_data(uint8_t sockfd,uint8_t request,uint8_t* request_content,size_t length_data);
Queue* data_queue;

extern Route route_root;
extern Route route_js;
extern Route route_css;
extern Route route_png;
extern Route route_post_data;
extern Route route_get_data;

```

The last step is to pass route to routes table and init routes table. It can be done like below:  

```
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
```
Server is starting when the ```websock``` is invoked.

# Server

![alt text](http://url/to/img.png)

Server is listening on port 8080 for incoming traffic. It can handle multiple connections, and for robustness purposes multithreading capabilities were added. 

Every incomming connection is accepted in main thread in ```websock``` function in ```server.c```. After accepting connection, socket id is pushed into mutexed queue and signal is send to inform (also in ```server.c```) threads in pool waiting for tasks. Every thread is reading connection from socket it got, next threads are parsing received message to get route, request command, and other necessary informations. Those information are passed to ```call_route``` in ```routing.c```. This allow to get information if route is in routes table (if not 404 is sent). If route is in routes table, the callback is invoked, and connection is closed and thread is returning to waiting mode.     
