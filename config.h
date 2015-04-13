/**
 * Copyright (C) 2015  Jun D. Ouyang, Isaac Baron
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef CONFIG_H
#define CONFIG_H

static const char SHELL_DELIMITER = ' ';
static const char PIPE_CHARACTER = '|';
static const char REDIRECT_IN_CHARACTER = '<';
static const char REDIRECT_OUT_CHARACTER = '>';
static const int MAX_LINE_SIZE = 100000;
static const int MAX_ARG_SIZE = 1000;
static const int MAX_ARG_COUNT = MAX_LINE_SIZE/MAX_ARG_SIZE;
static const int MAX_HISTORY_SIZE = 10;
static const char LINE_BREAK = 0x0A;
static const char ALERT = '\a';

#endif