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

#ifndef CORE_H
#define CORE_H

#include <vector>
#include <string>
#include <sstream>

#include "Module.h"

struct core_status{
  int exit_code;
  bool shutting_down;
  bool shut_down;
  long long int uptime;
};

class Core{

public:
  Core(int argc, char** argv);
  Core();

  bool startup();
  core_status get_status();
  void wait_for_shutdown();
  void async_shutdown(int code);

private:
  int argc;
  char** argv;

  int exit_code;
  bool shutting_down;
  bool shut_down;
  long long int startup_time;

  std::vector<Module*> modules;
  std::string socket_dir;

  static void signal_handler(int signal);
  static std::vector<Core*> cores;

  bool daemon;
  std::string config_file;

  void shutdown();

  void distributor();
  std::stringstream dist_stream;

};

#endif /* CORE_H */

