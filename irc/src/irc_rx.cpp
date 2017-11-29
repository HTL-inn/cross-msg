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
#include "irc_rx.h"

/*
std::string socket_main_path;
int socket_main;
int client_sock;
struct sockaddr_un socket_main_addr;
char client_message[128];
*/

void IRC_RX::unix_create(std::string socket_main_path){

        this->socket_main_path = socket_main_path;
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


void IRC_RX::unix_close(){

        /*close client socket*/
        close(client_sock);

        /*close unix-socket*/
        close(socket_main);

        /*unlink unix-socket file*/
        unlink(socket_main_path.c_str());
}


void IRC_RX::unix_write(const char message[]){
        write(client_sock, message, strlen(message));
}



void IRC_RX::unix_handle(){
        char buf[64];
        int size;
        bool config = false;
        while(size = read(client_sock, &buf, sizeof(buf))){
                std::cout << buf;

                if(config == false){
                        std::string s = buf;
                        std::string delimiter = "\u001f";
                        std::string mode = s.substr(0, s.find(delimiter));
                        std::string addr = s.substr(1, s.find(delimiter));
                        std::cout << mode;
                        std::cout << addr;
                        config = true;
                }
                
                bzero(&buf, sizeof(buf));
        }
}


