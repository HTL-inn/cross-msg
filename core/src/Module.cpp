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

#include "Module.h"
#include "Debug.h"
#include "Tools.h"

Module::Module(nlohmann::json config, std::string socket_dir){
  this->config = config;
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

    std::vector<char*> argv;

    for(unsigned int i = 0; i < args.size();i++){

      std::vector<char> cstr(args[i].c_str(), args[i].c_str() + args[i].size() + 1);
      argv.push_back(&cstr[0]);
    }

    argv.push_back(NULL);

    char **cmdln = &argv[0];

    try{
      execvp(cmdln[0], cmdln);
    }catch(std::exception& e){
      Debug::println("Failed to start module with command --> " + mod_name + " " + Tools::from_c_str(e.what()), debug_type::CRITICAL_ERROR);
    }

    this->started = false;
    Debug::println("child process of module --> " + mod_name, debug_type::INTERNAL);

    exit(0);

  }else{
    // I am a parent
    // Hi
    Debug::println("master process signing off --> " + mod_name,debug_type::INTERNAL);

    this->started = true;

    // Start looping around
    Debug::println("Waiting for module to open socket --> " + mod_name,debug_type::INTERNAL);
    for(int j = 0; ((j < 6) && !Tools::check_for_file(this->socket_path));j++){

      // I'm loopin!
      for(int i = 0; ((i < 6) && !Tools::check_for_file(this->socket_path));i++){

        // Still loopin
        Tools::wait_milliseconds(10);
      }

      if(!Tools::check_for_file(this->socket_path)){
        Debug::println("Child missing target! --> " + mod_name, debug_type::WARNING);
      }
      Tools::wait_milliseconds(100);
    }

    if(!Tools::check_for_file(this->socket_path)){
      Debug::println("Failed to start child!! --> " + mod_name, debug_type::ERROR);
      kill(this->child, SIGKILL);
      this->started = false;
      return;
    }else{
      std::thread tmp(&Module::handle, this);
      tmp.detach();
      // Started module, now I'm going on
    }


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
  Debug::println("module entering regular operation --> " + mod_name,debug_type::INTERNAL);

  while(kill(this->child, 0) == 0){
    // Handle input from the unix socket... I should start to implement things..

  }

}
