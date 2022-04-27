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

#ifndef RESOLVER_H_
#define RESOLVER_H_

#define ITEMSOF(arr) (int)(sizeof(arr) / sizeof(arr[0]))

typedef struct
{
	unsigned int nidsha1;
	unsigned int nid5xx;
	unsigned int nid570;
	unsigned int nid620;
	unsigned int nid63x;
	unsigned int nid660;
	unsigned int stub;
} nid;

typedef struct
{
	const char *prxname;
	const char *name;
	unsigned int count;
} libname;

extern unsigned int moduleprobe_nid;

void zeroCtrlResolveNids(void);

#endif