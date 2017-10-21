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
 * File:   Tools.cpp
 * Original-Author: tyrolyean
 *
 * Created on May 12, 2017, 9:50 PM
 *
 * This file and it's header have been moved to this project with the creators
 * consensus. It seems as if the original creator was very bad at coding.
 * ~Tyrolyean
 */

#include "Tools.h"
#include <time.h>
#include <stdio.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <thread>
#include <chrono>
#include <sys/types.h>
#include <sys/stat.h>
#include <vector>
#include <sstream>

/* Convert C-style strings into c++ strings.
 * They will be trimmed from the right side as well.
 * The trim makes this function totally useful!
 */
std::string Tools::from_c_str(const char* input) {

    std::string tmp(input);

    return *rtrim(&tmp);

}

/* This trims a string from the right hand side.*/
inline std::string* Tools::rtrim(std::string *s) {
    s->erase(std::find_if(s->rbegin(), s->rend(),
            std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s->end());
    return s;
}

/* This will convert a int to a string with the int as hex */
std::string Tools::int_to_hex_str(long int number){
  std::stringstream ss;

  ss << std::hex << number << std::dec;
  return ss.str();
}

//
// File-system related functions
//

/* This appends a line to a file.No \n is added by the function*/
bool Tools::append_to_file(std::string file, std::string line) {
    std::ofstream outfile;
    outfile.open(file.c_str(), std::ios_base::app);
    outfile << line.c_str();
    bool success = !outfile.fail();
    outfile.close();
    return success;
}

/* This overwrites a complete file. No \n is added by the function.*/
bool Tools::overwrite_file(std::string file, std::string line) {

    std::ofstream outfile;
    outfile.open(file.c_str());
    outfile << line.c_str();
    outfile.flush();

    bool success = !outfile.fail();
    outfile.close();
    return success;
}

std::string Tools::read_file(std::string file) {

    std::ifstream ifs(file);

    std::string content((std::istreambuf_iterator<char>(ifs)),
            (std::istreambuf_iterator<char>()));
    return content;

}

bool Tools::check_for_directory(std::string dir) {

    struct stat info;

    if (stat(dir.c_str(), &info) != 0) {
        return false;
    } else if (info.st_mode & S_IFDIR) { // S_ISDIR() doesn't exist on my windows
        return true;
    } else {
        return false;
    }

}

//
// Time related functions.
//

void Tools::wait_milliseconds(int milliseconds) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    return;
}

long long int Tools::get_unix_time_millis() {
    std::chrono::milliseconds ms = std::chrono::duration_cast< std::chrono::milliseconds >(std::chrono::system_clock::now().time_since_epoch());
    return ms.count();
}

/* This returns a printable version of the actual time.
 * It is formatted like this: %Y-%m-%d %I:%M:%S
 */
std::string Tools::get_time_printable() {
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, 80, "%Y-%m-%dT%H:%M:%S.%s", timeinfo);
    return Tools::from_c_str(buffer);
}

//
// String transforming an so on...
//

std::string Tools::to_upper_case(std::string in) {

    std::transform(in.begin(), in.end(), in.begin(), ::toupper);

    return in;
}

std::string Tools::to_lower_case(std::string in) {

    std::transform(in.begin(), in.end(), in.begin(), ::tolower);

    return in;
}

std::vector<std::string> Tools::split_by_string(std::string delimiter, std::string input) {
    std::vector<std::string> output;
    size_t pos = 0;
    std::string token;
    while ((pos = input.find(delimiter)) != std::string::npos) {
        token = input.substr(0, pos);
        token.erase(std::find_if(token.rbegin(), token.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), token.end());
        output.push_back(token);

        input.erase(0, pos + delimiter.length());


    }

    input.erase(std::find_if(input.rbegin(), input.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), input.end());
    output.push_back(input);
    return output;
}
