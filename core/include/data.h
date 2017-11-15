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
  This file may be used to store globally used data and preprocessor
  definitions in order to have a central place for those.
*/

#ifndef DATA_H
#define DATA_H

#define LINE_SEPARATOR '\u0004'
#define DATA_SEPARATOR '\u001f'

/*  This is the definition of the status codes used in the protocol.
    All codes may be used as appropriately.
    BREACH may only be used in case of a security breach or worse...*/
enum status : unsigned short int {CONFIGURATION = 0,SYSTEM_COMMAND, USER_COMMAND,
  SYSTEM_MESSAGE, USER_MESSAGE, BREACH};

#endif /* DATA_H */
