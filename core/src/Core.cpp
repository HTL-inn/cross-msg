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
  author: tyrolyean
  This class will be used to run the feakin module?
*/

#include <cstddef>
#include <string>
#include <signal.h>
#include <iostream>
#include <thread>
#include <unistd.h>
#include <pwd.h>
#include <fstream>
#include <sys/stat.h>

#include "Core.h"
#include "Tools.h"
#include "Debug.h"
#include "data.h"
#include "json.hpp"
#include "Module.h"

/* Static variables */
std::vector<Core*> Core::cores;

Core::Core() : Core(0, NULL){}
Core::Core(int argc, char** argv){

  this->argc = argc;
  this->argv = argv;

  this->startup_time = Tools::get_unix_time_millis();
  this->shutting_down = false;
  this->shut_down = false;
  this->exit_code = 0;

  this->config_file = "";
  this->socket_dir = "";

  this->daemon = false;

  // Register this core instance
  Core::cores.push_back(this);

  return;
}

bool Core::startup(){

  bool success = true;

  // Hook signals
  signal(SIGTERM, &this->signal_handler);
  signal(SIGKILL, &this->signal_handler);
  signal(SIGINT, &this->signal_handler);
  signal(SIGPIPE, &this->signal_handler);

  //
  // Prepare
  //

  // Parse arguments
  for (int i = 0; i < this->argc; i++) {
      // Parse all arguments given to the program
      std::string row = Tools::from_c_str(this->argv[i]);

      try {
          if (row[0] == '-') {
              // config
              if(row == "-h" || row == "--help"){
                // Display help and exit
                // disable any further output
                Debug::debug_threshold = debug_type::PENETRATION + 1;

                std::cout << "Usage: core [options]\n\
  -h, --help                Display this message\n\
  -D, --debug [threshold]   Set the debug threshold\n\
  -d, --daemon              Start the module in daemon mode\n\
  -c, --config              Set the config file\n\
\n\
Report bugs to tyrolyean@gmx.at" << std::endl;
                this->shutting_down = true;
                this->shut_down = true;
                return true;


              } else if (row == "--daemon" || row == "-d") {
                  daemon = true;
                  Debug::println("starting core in daemon mode due to option " + row,debug_type::INTERNAL);

              } else if(row == "-D" || row == "--debug"){
                // Set the output threshold
                Debug::debug_threshold = atoi(argv[++i]);

              }else if(row == "-c" || row == "--config"){

                this->config_file = Tools::from_c_str(argv[++i]);

              }else {
                  Debug::println("Unknown parameter: " + row, debug_type::INFO);
              }

          }
      } catch (std::exception& e) {
          Debug::println("parameter crash at argument "+std::to_string(i)+" : " + Tools::from_c_str(e.what()), debug_type::CRITICAL_ERROR);
          return false;
      }
  }
  //
  // DEBUG OUTPUT
  //

  Debug::println("Program started in directory: " + Tools::from_c_str(get_current_dir_name()) ,debug_type::INTERNAL);

  Debug::println("using character as separator: 0x" + Tools::int_to_hex_str(DATA_SEPARATOR) ,debug_type::INTERNAL);


  //
  // Real init
  //

  if(this->config_file == ""){
    //  no config file has been set
    if(Tools::check_for_file(Tools::from_c_str(get_current_dir_name()) + "/config.json")){
      // Found a config file! wohoo
      this->config_file = Tools::from_c_str(get_current_dir_name()) + "/config.json";
    }else{
      if(Tools::check_for_file("/etc/cross-msg/config.json")){
        // Found a config file! wohoo
        this->config_file = "/etc/cross-msg/config.json";
      }else{

        struct passwd *pw = getpwuid(getuid());
        const char *homedir = pw->pw_dir;

        if(Tools::check_for_file(Tools::from_c_str(homedir) + "/.config/cross-msg/config.json")){
          // Found a config file! wohoo
          this->config_file = Tools::from_c_str(homedir) + "/.config/cross-msg/config.json";
        }else{
          Debug::println("No config file was found!", debug_type::CRITICAL_ERROR);
          return false;
        }
      }
    }

  }else{
    // check if it's really there
    if(!Tools::check_for_file(this->config_file)){
      Debug::println("You specified a file as config file which doesn't exist... wow", debug_type::CRITICAL_ERROR);
      return false;
    }

  }
  Debug::println("Using " + this->config_file +" as config file", debug_type::INTERNAL);

  std::ifstream config_stream(this->config_file);
  std::string config_raw((std::istreambuf_iterator<char>(config_stream)),
                   std::istreambuf_iterator<char>());

  nlohmann::json config;
  try{
    config = nlohmann::json::parse(config_raw);
  }catch(std::exception& e){
    Debug::println("failed to parse your config file at "+this->config_file+" : " + Tools::from_c_str(e.what()), debug_type::CRITICAL_ERROR);
    return false;
  }

  if (config.empty()){
    // Really? An empty config?
    Debug::println("empty config detected! thanks for testing this program! you may now configure it correctly!", debug_type::CRITICAL_ERROR);
    return false;
  }

  // the config file has been successfully parsed. congratulations! you found a
  // only partially stupid user!

  try{

    this->socket_dir = config["socketdir"];

  }catch(std::exception& e){
    Debug::println("Failed to get socket directory: " + Tools::from_c_str(e.what()), debug_type::CRITICAL_ERROR);
    return false;
  }


  if(!Tools::check_for_directory(this->socket_dir)){
    const int dir_err = mkdir(this->socket_dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if (dir_err < 0)
    {
      Debug::println("Failed to create missing socket directory at " + this->socket_dir, debug_type::CRITICAL_ERROR);
      return false;
    }else{
      Debug::println("created missing socket directory at " + this->socket_dir,debug_type::INTERNAL);
    }
  }

  if(this->socket_dir.back() != '/'){
    this->socket_dir.append("/");
  }

  Debug::println("Using " +  this->socket_dir + " as socket directory",debug_type::INTERNAL);

  for (nlohmann::json module : config["modules"]) {
    std::string mod_name = module["title"];
    Debug::println( "initializing module --> " + mod_name, debug_type::INTERNAL);

    Module* mod = new Module(module,this->socket_dir);

    this->modules.push_back(mod);

    mod->start();

  }


  return success;
}

core_status Core::get_status(){
  struct core_status tmp;
  tmp.exit_code = this->exit_code;
  tmp.shutting_down = this->shutting_down;
  tmp.shut_down = this->shut_down;
  tmp.uptime = (Tools::get_unix_time_millis() - this->startup_time);
  return tmp;
}

void Core::wait_for_shutdown(){

  while (!get_status().shutting_down) {
    Tools::wait_milliseconds(10);
  }

  return;
}

void Core::async_shutdown(int code){

  this->exit_code = code;
  this->shutting_down = true;

  std::thread tmp(&Core::shutdown, this);
  tmp.detach();

  return;
}

void Core::shutdown(){

  // Call all shutdown methods and stop all modules

  for(Module* m : this->modules){
    m->stop();
  }

  this->shut_down = true;

  return;
}

void Core::signal_handler(int signal) {
    int shut_down = -1;
    switch (signal) {
        case SIGINT:
            shut_down = EXIT_SUCCESS;
            // I don't see SIGINT that serious, because the user knew, what he
            // did. I ain't gotta have a hurry for a lazy idiot
            break;
        case SIGPIPE:
            Debug::println("REGISTERED SIGPIPE!!!!!!!!!!!!!!!!!! ignoring...",debug_type::INTERNAL);
            // Sig pipe has to be taken very seriously!
            break;
        case SIGTERM:
        case SIGKILL:
            shut_down = signal;
            break;
        default:
            shut_down = signal;

            break;
    }

    if(shut_down != -1){
      for(Core* c: Core::cores){
        c->async_shutdown(shut_down);
        Debug::println("signal enforced shutdown: " + std::to_string(signal), debug_type::END_CALL);
      }
    }

    return;
}
