/*
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Author: tyrolyean
 *
 * The class for a module.
 */

#include <vector>
#include <signal.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <iterator>
#include <numeric>
#include <thread>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>

#include "Module.h"
#include "Debug.h"
#include "Tools.h"

Module::Module(nlohmann::json config, std::string socket_dir, std::stringstream* out){
  this->config = config;
  this->output = out;
  std::string mod_name = this->config["title"];

  try{
    std::string sockn;
    sockn = this->config["socket"];
    this->socket_path = socket_dir + sockn;
  }catch(std::exception& e){
    this->socket_path = "";
    Debug::println("Failed to initialize module due to missing config entry socket --> " + mod_name + " " + Tools::from_c_str(e.what()), debug_type::CRITICAL_ERROR);
    return;
  }

  this->started = false;

  return;
}

void Module::start(){

  std::string mod_name = this->config["title"];

  Debug::println("starting module --> " + mod_name,debug_type::INTERNAL);

  if(std::string(this->socket_path) == "" || this->started){
    // Illegal state!
    return;
  }

  // start subprocess according to the config

  this->child = fork();

  if(this->child == -1){
    // I am a failure
    // Hi
    Debug::println("failed to for process for module --> " + mod_name,debug_type::ERROR);
    return;
  }else if(this->child == 0){
    // I am the child!
    // Hi
    // Parse config to arguments

    std::string cmd;
    try{
      cmd = this->config["command"];
    }catch(std::exception& e){
      Debug::println("Failed to start module due to missing config entry command --> " + mod_name + " " + Tools::from_c_str(e.what()), debug_type::CRITICAL_ERROR);
      return;
    }

    std::vector<std::string> args = Tools::split_by_string(' ',cmd);

    // The first one should be

    args.push_back("-s");
    args.push_back(this->socket_path);

    char *cmdln[args.size()+1];

    for(unsigned int i = 0; i < args.size(); i++){

      char* tmp = (char*) malloc(args[i].length()+1);
      bzero(tmp, args[i].length()+1);
      std::copy(args[i].begin(), args[i].end(), tmp);
      cmdln[i] = tmp;

    }


    cmdln[args.size()] = NULL;

    int n;

    try{
      // Handing over to subprocess

      n = execvp(cmdln[0], (char* const*) &cmdln);
    }catch(std::exception& e){
      Debug::println("Failed to start module with command --> " + mod_name + " " + Tools::from_c_str(e.what()), debug_type::CRITICAL_ERROR);
    }

    this->started = false;
    Debug::println("child process ended with code "+std::to_string(n)+" --> " + mod_name, debug_type::INTERNAL);

    exit(0);

  }else{
    // I am a parent
    // Hi

    this->started = true;

    // Start looping around
    std::thread tmp(&Module::handle, this);
    tmp.detach();
    Debug::println("master handing over to new thread --> " + mod_name,debug_type::INTERNAL);

  }

  return;
}

void Module::stop(){

  std::string mod_name = this->config["title"];
  Debug::println("stopping module --> " + mod_name,debug_type::INTERNAL);
  kill(this->child, SIGTERM);
  Tools::wait_milliseconds(10);

  for(int i = 0; !((i > 6) || kill(this->child, 0) == 0); i++ ){

    kill(this->child, SIGTERM);

    Tools::wait_milliseconds(100);

  }

  if(kill(this->child, 0) != 0){
    Debug::println("too slow at stopping module, forceing child shutdown --> " + mod_name,debug_type::INTERNAL);
  }

  kill(this->child, SIGKILL);

  return;
}

void Module::handle(){
	
  std::string mod_name = this->config["title"];

  for(int i = 0; ((i < 500) && !Tools::check_for_file(this->socket_path));i++){
    Tools::wait_milliseconds(100);
    // 50 seconds enough to open a socket?
  }
  
  std::cout << "socket found" << Tools::check_for_file(this->socket_path) << std::endl;
  Tools::wait_milliseconds(1000);
    // Open Socket and initialize module by writing it's config

  if ( (this->sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
    Debug::println("failed to get socket --> " + mod_name, debug_type::ERROR);
    this->stop();
  }

  struct sockaddr_un addr;
  // memset(&addr, 0, sizeof(&addr));

  addr.sun_family = AF_UNIX;

  //memcpy(&addr.sun_path, this->socket_path.c_str(),sizeof(this->socket_path.c_str()));
  strcpy(addr.sun_path, this->socket_path.c_str());
  std::cout << addr.sun_path << std::endl;
  std::cout << connect(this->sockfd,(const sockaddr*) &addr, sizeof(addr)) << std::endl;
  
  Debug::println("module entering regular operation --> " + mod_name,debug_type::INTERNAL);
  //this->listen();
  std::thread tmpthr(&Module::listener,this);
  tmpthr.detach();

  while((kill(this->child, 0) == 0) /*&& !this->core->get_status().shutting_down*/){
    // Handle input from the unix socket... I should start to implement things..
	
  }

  Debug::println("Module leaving regular operation --> " + mod_name,debug_type::INTERNAL);

}

void Module::listener(){
  std::string mod_name = this->config["title"];
  Debug::println("listener started --> " + mod_name,debug_type::INTERNAL);

  char buf[1000];
  std::string buffer;
  int rc;
  std::stringstream streeeeeaaam;

  while((kill(this->child, 0) == 0) && (rc=read(this->sockfd,&buf,sizeof(buf) > 0))  /*&& !this->core->get_status().shutting_down*/){
      // Handle input from the unix socket...
      //*this->output << buf;
      std::cout << buf;
  }

  Debug::println("listener stopped --> " + mod_name,debug_type::INTERNAL);
  return;
}

