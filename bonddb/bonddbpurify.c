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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>

#include "bc.h"
#include "bonddbobj.h"
#include "bonddbpurify.h"


/** bonddb_purify(gchar *value) 
 * @param gchar* value
 * (pointer to) a NUL terminated string 
 * 
 * makes a string safe for use between sql single quotes. 
 * by escaping ' and \ symbols with \
 *
 * @return (pointer to) a NUL terminated SQL-safe string that should be
 * mem_free()d 
 **/

gchar *
bonddb_purify(gchar *value)  /* to escape strings for sql */
{
	gint aloc, slen, i, j;
	gchar *newstr = NULL;
	if (!value)
		return NULL;
	slen = strlen(value);
	aloc = slen+(slen>>5);
	j = 0;
	newstr = mem_alloc(aloc+2);
	for (i=0;i<slen;i++)
	{
		if (j >= aloc)
		{
			aloc = aloc + 16 + (aloc >> 5); 
			newstr = mem_realloc(newstr,(sizeof(gchar*)*(aloc+2)));
		}
		if (value[i] == '\'') /* ' */
		{
			newstr[j] = '\\';
			j++;
		}
		if (value[i] == '\\') /* \ */
		{
			newstr[j] = '\\';
			j++;
		}
		newstr[j] = value[i];
		j++;
	}
	newstr[j] = 0;
	return newstr;
}


/** bonddb_unpurify(gchar *value) 
 * @param gchar* value
 * (pointer to) a writeable NUL terminated string which will be modified
 * 
 * strip excess backslashes from a string such as produced by bonddb_purify() 
 *
 **/

void  /* modifies the string in-place removing excess slashes */
bonddb_unpurify(gchar *value)  /* to unescape strings */ 
{ 
	register gchar *head=value,a;
	if(value)
		{
		while((a=*head++))
			{
				if(a == '\\') if(!(a=*head++)){ a='\\'; --head;}
				*value++=a;	
			}	
		*value='\0';
		}
}
