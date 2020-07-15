#ifndef __ROUTES_H__
#define __ROUTES_H__

#include "routing.h"

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

#endif // __ROUTES_H__