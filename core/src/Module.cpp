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

#include "Module.h"
#include "Debug.h"

Module::Module(nlohmann::json config, std::string socket_dir){
  this->config = config;
  std::string mod_name = this->config["title"];

  this->socket_path = socket_dir + mod_name;

  return;
}

void Module::start(){
  std::string mod_name = this->config["title"];
  Debug::println("starting module --> " + mod_name,debug_type::INTERNAL);

  return;
}

void Module::stop(){
  std::string mod_name = this->config["title"];
  Debug::println("stopping module --> " + mod_name,debug_type::INTERNAL);

  return;
}
