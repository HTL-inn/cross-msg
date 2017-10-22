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

#ifndef MODULE_H
#define MODULE_H

#include <string>

#include "json.hpp"

class Module{
public:
  Module(nlohmann::json config, std::string socket_dir);

  void start();
  void stop();

private:
  nlohmann::json config;
  // The socket path is NOT the socket directory! It is the socket itself!
  std::string socket_path;
};

#endif /* MODULE_H */
