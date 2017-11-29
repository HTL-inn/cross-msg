#ifndef IRC_RX_H
#define IRC_RX_H


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

class IRC_RX {

 public:
        
        void unix_create(std::string socket_main_path);
        void unix_close();
        void unix_write(const char message[]);
        void unix_handle();

 private:
        
        std::string socket_main_path;
        int socket_main;
        int client_sock;
        struct sockaddr_un socket_main_addr;
        char client_message[128];
};



#endif
