/*
    diecat
    A2L/HEX file reader.

    File: constants.hpp

    Copyright (C) 2013-2014 Artem Petrov <pa2311@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#define PROGNAME "diecat"
#define PROGVER  "0.3.0"

#define A2LCHARBLOCKMINSIZE 9
#define A2LCOMPUMETHODSIZE 6
#define A2LCOEFFNUM 6
#define A2LCOMPUVTABMINSIZE 4

enum {
    VARTYPE_SCALAR_NUM,
    VARTYPE_SCALAR_VTAB
};

#endif // CONSTANTS_HPP
