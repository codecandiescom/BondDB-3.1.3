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
 
#ifndef __dbmasteroftable_h
#define __dbmasteroftable_h

gint db_masteroftable_populate(DbObject * obj);
gint db_masteroftable_test(DbObject * obj);
gboolean db_masteroftable_check(DbObject * obj, gchar * fieldname, gchar * tablename);
GList* db_masteroftable_actionchange(DbObject *obj, DbField *field, gchar *newvalue);

#endif
