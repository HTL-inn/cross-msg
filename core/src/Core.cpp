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

#include "Core.h"
#include "Tools.h"
#include "Debug.h"
#include "data.h"

/* Static variables */
std::vector<Core*> Core::cores;

Core::Core() : Core(0, NULL){}
Core::Core(int argc, char** argv){

  this->argc = argc;
  this->argv = argv;

  this->startup_time = Tools::get_unix_time_millis();
  this->shutting_down = false;
  this->exit_code = 0;

  // Register this core instance
  Core::cores.push_back(this);

  return;
}

bool Core::startup(){

  bool success = true;

  // Startup hooks
  signal(SIGTERM, &this->signal_handler);
  signal(SIGKILL, &this->signal_handler);
  signal(SIGINT, &this->signal_handler);
  signal(SIGPIPE, &this->signal_handler);

  //
  // DEBUG OUTPUT
  //
  Debug::println("using character as separator: 0x" + Tools::int_to_hex_str(DATA_SEPARATOR) ,debug_type::INTERNAL);

  return success;
}

core_status Core::get_status(){
  struct core_status tmp;
  tmp.exit_code = this->exit_code;
  tmp.shutting_down = this->shutting_down;
  tmp.uptime = (Tools::get_unix_time_millis() - this->startup_time);
  return tmp;
}

void Core::wait_for_shutdown(){

  while (!get_status().shutting_down) {
    Tools::wait_milliseconds(10);
  }

  return;
}

void Core::shutdown(int code){

  this->exit_code = code;
  this->shutting_down = true;
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
        c->shutdown(shut_down);
        Debug::println("signal enforced shutdown: " + std::to_string(signal), debug_type::END_CALL);
      }
    }

    return;
}
