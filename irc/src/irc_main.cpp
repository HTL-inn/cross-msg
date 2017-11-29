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


std::string unix_socket_main_path;

int main(int argc, char* argv[]) {
        if(argc < 3){
                std::string s1 = "Missing start parameter\n\n";
                std::string s2 = "Usage:\n";
                std::string s3 = "   -s 'path to socket'\n";
                std::string s4 = s1 + s2 + s3;
                std::cout << s4;
                return 0;
        }

        if(std::string(argv[1]) == "-s"){
                //socket_main_path = argv[2];
                unix_socket_main_path = std::string(argv[2]);
        }

        IRC_RX *irc_rx = new IRC_RX();

        irc_rx->unix_create(unix_socket_main_path);
        irc_rx->unix_handle();

        irc_rx->unix_close();
        
   return 0;
}
