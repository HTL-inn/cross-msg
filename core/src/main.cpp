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
 * Do I need to describe what a main file does?
 */

#include <cstdlib>
#include <iostream>
#include <string>
#include <signal.h>

#include "data.h"
#include "Debug.h"
#include "Core.h"
#include "Tools.h"

using namespace std;

int main(int argc, char** argv) {

	Core c(argc,argv);

	if(!c.startup()){
	Debug::println("startup failed! stopping program!",debug_type::CRITICAL_ERROR);
	return 1;

	}else{
		Debug::println("Startup finished! going on with regular business",debug_type::INFO);

	}

	Debug::println("main going to sleep",debug_type::INTERNAL);

	c.wait_for_shutdown();

	Debug::println("main woken up from sleep",debug_type::INTERNAL);

	int exit_code = c.get_status().exit_code;
	int i;

	if(exit_code == SIGKILL || exit_code == SIGTERM){
		// Hurry up you lazy bastard!
		i = 2;
	}else{
		// Give the server more shutdown time if it wasn't terminated via a
		// more important system call
		i = 10;
	}
	Tools::wait_milliseconds(10);

	// maybe? please?
	if(c.get_status().shut_down){
		i = 0;
	}

	for(; i > 0; i--){
		Debug::println("enforced process termination in "+std::to_string(i)+"s!",debug_type::INTERNAL);

		for(int j = 0; j < 1000; j++){
			if(c.get_status().shut_down){
				i = 0;
				continue;
			}

			 Tools::wait_milliseconds(1); // 1 millisecond delay

		}


	}

	Debug::println("bb",debug_type::INTERNAL);

	return exit_code;

}
