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
 * File:   Tools.h
 * Author: tyrolyean
 *
 * Created on May 12, 2017, 9:50 PM
 *
 * See the .cpp file in order to get more information on the use of this file.
 */

#ifndef TOOLS_H
#define TOOLS_H
#include <string>
#include <vector>

class Tools {
public:

    static std::string get_time_printable();
    static std::string from_c_str(const char* input);
    static std::string* rtrim(std::string *s);
    static std::string int_to_hex_str(long int number);
    static bool append_to_file(std::string file, std::string line);
    static bool overwrite_file(std::string file, std::string line);
    static std::string read_file(std::string file);
    static bool check_for_directory(std::string dir);
    static bool check_for_file(const std::string& file);
    static void wait_milliseconds(int milliseconds);
    static long long int get_unix_time_millis();
    static std::string to_upper_case(std::string in);
    static std::string to_lower_case(std::string in);
    static std::vector<std::string> split_by_string(char delimiter, std::string input);

};

#endif /* TOOLS_H */
