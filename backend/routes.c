#include "routes.h"

// *********************************************** USER ROUTES *******************************************//

Route route_root = {.request=GET , .addr="/", .fnc_ptr=root_response};
Route route_js = {.request=GET , .addr="/script.js", .fnc_ptr=js_response};
Route route_css = {.request=GET , .addr="/style.css", .fnc_ptr=css_response};
Route route_png = {.request=GET ,.addr="/water.png", .fnc_ptr=png_response};
Route route_post_data = {.request=GET , .addr="/ReadDev", .fnc_ptr=get_data};
Route route_get_data = {.request=POST , .addr="/ReadDev", .fnc_ptr=post_data};

void root_response(uint8_t sockfd,uint8_t request, uint8_t* request_content,size_t length_data){
    uint8_t fd = open("../www/index.html",O_RDONLY,S_IRUSR | S_IWUSR);
    struct stat sb;

    fstat(fd,&sb);
    char* rfile = mmap(NULL,sb.st_size,PROT_READ,MAP_PRIVATE,fd,0);
    response(sockfd,"200 OK",rfile,sb.st_size,"text/html"); 
    munmap(rfile,sb.st_size);
    close(fd);
};

void js_response(uint8_t sockfd,uint8_t request, uint8_t* request_content,size_t length_data){
    uint8_t fd = open("../www/script.js",O_RDONLY,S_IRUSR | S_IWUSR);
    struct stat sb;

    fstat(fd,&sb);
    
    char* rfile = mmap(NULL,sb.st_size,PROT_READ,MAP_PRIVATE,fd,0);
    response(sockfd,"200 OK",rfile,sb.st_size,"text/javascript"); 
    munmap(rfile,sb.st_size);
    close(fd);
};

void css_response(uint8_t sockfd,uint8_t request, uint8_t* request_content,size_t length_data){
    uint8_t fd = open("../www/style.css",O_RDONLY,S_IRUSR | S_IWUSR);
    struct stat sb;

    fstat(fd,&sb);
    
    char* rfile = mmap(NULL,sb.st_size,PROT_READ,MAP_PRIVATE,fd,0);
    response(sockfd,"200 OK",rfile,sb.st_size,"text/css"); 
    munmap(rfile,sb.st_size);
    close(fd);
};

void png_response(uint8_t sockfd,uint8_t request, uint8_t* request_content,size_t length_data){
    // uint8_t fd = open("../www/water.png",O_RDONLY,S_IRUSR | S_IWUSR);
    FILE *fpng = fopen("../www/water.png","rb");


    fseek(fpng, 0, SEEK_END);
    long fsize = ftell(fpng);
    fseek(fpng, 0, SEEK_SET);  /* same as rewind(f); */

    uint8_t *rpng = malloc(fsize + 1);
    fread(rpng, 1, fsize, fpng);
    fclose(fpng);

    // struct stat sb;

    // fstat(fd,&sb);
    
    // uint8_t* rfile = mmap(NULL,sb.st_size,PROT_READ,MAP_PRIVATE,fd,0);
    // response(sockfd,"200 OK",rfile,sb.st_size,"image/png"); 
    response(sockfd,"200 OK",rpng,fsize,"image/png"); 
    // munmap(rfile,sb.st_size);
    
};


void get_data(uint8_t sockfd,uint8_t request,uint8_t* request_content,size_t length_data){
    //*** TO DO ***//
    response(sockfd,"200 OK","",0,"text");
    
}

void post_data(uint8_t sockfd,uint8_t request,uint8_t* request_content,size_t length_data){
    //*** TO DO ***//

    response(sockfd,"200 OK","",0,"text");
}

