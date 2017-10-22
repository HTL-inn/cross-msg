/*
 * Copyright 2017 tyrolyean.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * File:   Debug.cpp
 * Author: tyrolyean
 *
 * Created on May 12, 2017, 9:14 PM
 *
 * This file and it's header have been moved to this project with the creators
 * consensus. It seems as if the original creator was very bad at coding.
 * ~Tyrolyean
 */

#include "Debug.h"
#include "Tools.h"
#include <iostream>
#include <fstream>
#include <string>

/* This is the main lock for the text output, which was introduced because
 * it often occured, that messages were just printed into eachother...
 */
bool Debug::output_lock = false;
// The debug output threshold
unsigned short int Debug::debug_threshold = debug_type::INFO;

void Debug::println(std::string output, unsigned short int code){

    if(code < debug_threshold){
      // Ignore everything under the threshold
      return;
    }
    std::string out = generate_prefix(code);
    out.append(output);
    Debug::output(out);
    return;
}



std::string Debug::generate_prefix(unsigned short int code){

    std::string prefix ="[ "+Tools::get_time_printable()+" ";

    switch(code){

        case debug_type::INTERNAL :
            prefix = prefix + COLOR_BLUE +"internal\t    "+COLOR_RESET+"] ";
            break;

        case debug_type::INFO :
            prefix = prefix + COLOR_GREEN +"information\t    "+COLOR_RESET+"] ";
            break;
        case debug_type::WARNING :
            prefix = prefix + COLOR_YELLOW +"warning\t    "+COLOR_RESET+"] ";
            break;
        case debug_type::ERROR :
            prefix = prefix + COLOR_RED +"error\t    "+COLOR_RESET+"] ";
            break;
        case debug_type::CRITICAL_ERROR :
            prefix = prefix + COLOR_BOLDRED +"critical error     "+COLOR_RESET+"] ";
            break;
        case debug_type::END_CALL :
            prefix = prefix + COLOR_BOLDBLUE +"end call\t      "+COLOR_RESET+"] ";
            break;
        case debug_type::PENETRATION :
            prefix = prefix + COLOR_BOLDBLACK +"penetration   "+COLOR_RESET+"] ";
            break;
        default: // I'm probably going to face idiotic programmers.
            prefix = prefix + COLOR_MAGENTA + "unknown\t    " + COLOR_RESET + "] ";
            break;
    }

    return prefix;
}

void Debug::output(std::string output){

    while(output_lock){
        Tools::wait_milliseconds(10);
    } // Just to be safe :D
    output_lock = true;

    std::cout << output.c_str() << std::endl;

    output_lock = false;
    return;
}
