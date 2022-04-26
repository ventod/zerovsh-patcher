/*
 * This file is part of ZeroVSH Patcher.

 * ZeroVSH Patcher is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * ZeroVSH Patcher is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ZeroVSH Patcher. If not, see <http://www.gnu.org/licenses/ .
 */

#include <pspsdk.h>

PSP_MODULE_INFO("ZeroVSH_Patcher_User", 0x0007, 0, 1);

#define UNUSED __attribute__((unused))

//OK
int module_start(SceSize args UNUSED, void *argp UNUSED) {     
	return 0;
}
//OK
int module_stop(SceSize args UNUSED, void *argp UNUSED) {
    return 0;
}
