/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "bc.h"

MemGarbage * 
mem_addtogarbage(MemGarbage *garbage, int id, void (*freefunc) (void*), void *data)
	{
	MemGarbage *walk, *last = NULL;
	typedef void (*function) (void*);
	function torun;

	for (walk = garbage;walk!=NULL;walk=walk->next)
		{
		/* only overwrite on non-nero ids */
		if (id && walk->id == id)
			{
			torun = walk->freefunc;
			torun(walk->data);
			walk->data = data;
			walk->freefunc = freefunc;
			return garbage;
			}
		last = walk;
		}
	
	walk = mem_calloc(sizeof(MemGarbage));
	walk->id = id;
	walk->freefunc = freefunc;
	walk->data = data;
	walk->prev = last;
	if (last)
		last->next = walk;
	
	if (!garbage)
		return walk;
	return garbage;
	}

void
mem_freegarbage(MemGarbage *garbage)
	{
	MemGarbage *walk, *next;
	typedef void (*function) (void*);
	function torun;
	
	g_assert(garbage);
	
	walk = garbage;
	while (walk != NULL)
		{
		torun = walk->freefunc;
		if (torun)
			torun(walk->data);
		next = walk->next;
		mem_free(walk);
		walk = next;
		}
	}
