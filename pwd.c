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

#include "pwd.h"
#include "config.h"

#include <unistd.h>
#include <string.h>
#include <stdlib.h>

void pwd()
{
    char wd[MAX_LINE_SIZE];
    getcwd(wd, MAX_LINE_SIZE);
    write(STDOUT_FILENO, wd, strlen(wd));
    write(STDOUT_FILENO, "\n", 1);
    exit(0);
}