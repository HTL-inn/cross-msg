#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netdb.h>
#include <cstring>
#include <sys/un.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

std::string socket_main_path;
int socket_main;
int client_sock;
struct sockaddr_un socket_main_addr;
char client_message[128];


void unix_socket_create(){
        
        puts("socket created");

        /*set socket family (ex. UNIX-socket)*/
        socket_main_addr.sun_family = AF_UNIX;

        /*copy the socket address to the socket*/
        strcpy(socket_main_addr.sun_path, socket_main_path.c_str());

        /*unlink old socket if it exists*/
        unlink(socket_main_path.c_str());

        /*create socket*/
        socket_main = socket(AF_UNIX, SOCK_STREAM, 0);

        /*check if creation was succesful*/
        if(socket_main == -1){
                puts("can't create socket");
                return;
        }

        /*bind the socket to the address*/
        int sock_bind = bind(socket_main, (sockaddr*) &socket_main_addr,
                             sizeof(socket_main_addr)); // check for error

        /*chmod so that everyone can access it*/
        chmod(socket_main_path.c_str(), 0777);

        /*listen socket*/
        int sock_listen = listen(socket_main, 1);

        /*wait for client*/
        client_sock = accept(socket_main, nullptr, nullptr);
	puts("client connected");        
}


void unix_socket_close(){

        /*close client socket*/
        close(client_sock);

        /*close unix-socket*/
        close(socket_main);

        /*unlink unix-socket file*/
        unlink(socket_main_path.c_str());
}


void unix_socket_write(const char message[]){
        write(client_sock, message, strlen(message));
}


void unix_com_handle(){
        char buf[64];
        int size;
        while(size = read(client_sock, &buf, sizeof(buf))){
                std::cout << buf;


                bzero(&buf, sizeof(buf));
        }
}


int main(int argc, char* argv[]) {
        if(argc < 3){
                string s1 = "Missing start parameter\n\n";
                string s2 = "Usage:\n";
                string s3 = "   -s 'path to socket'\n";
                string s4 = s1 + s2 + s3;
                std:cout << s4;
                return 0;
        }

        if(std::string(argv[1]) == "-s"){
                //socket_main_path = argv[2];
                socket_main_path = std::string(argv[2]);
        }

        unix_socket_create();
        unix_socket_write("Mamma hab mich lieb!\n\r\u0004");
        unix_com_handle();
        unix_socket_close();
        unix_socket_write("Test\n\r");
   return 0;
}
