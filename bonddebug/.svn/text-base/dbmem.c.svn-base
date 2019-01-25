#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <glib.h>

#include "dbmem.h"

/* #define MEM_DONT_FREE */

#define MAGIC_PRE_HASH  0xdeadbeef
#define MAGIC_POST_HASH 0xabcdef12

#define mem_error(fmt,args...) fprintf (stderr, "MEM: " fmt "\n" , ##args)
#define min(a,b) (((a) > (b)) ? (b) : (a))

#ifdef MEM_TEST

int globalmemoryid;

typedef struct
	{
	int pre_id;
#ifdef MEM_DONT_FREE

	int freed;
#endif

	char name[64];
	int size;
	int line;
	int id;
	char file[32];
	int post_pre_id;
	}
mem_pre_info;

typedef struct
	{
	int post_id;
	}
mem_post_info;

static GList *mem_list = NULL;

static void *
mem_get_post(mem_pre_info * pre)
	{
	return ((char *)pre) + sizeof(mem_pre_info) + pre->size;
	}

static void *
mem_get_data(mem_pre_info * pre)
	{
	return ((char *)pre) + sizeof(mem_pre_info);
	}

static mem_pre_info *
mem_get_pre(void *data)
	{
	return (void *)(((char *)data) - sizeof(mem_pre_info));
	}

void
mem_checkboundaries()
	{
	gint i;
	void *mem, *buf;
	GList *walk;
	mem_pre_info *pre;
	mem_post_info *post;

	for (walk = g_list_first(mem_list); walk != NULL; walk = walk->next)
		{
		pre = walk->data;
		mem = mem_get_data(pre);

		if (pre->pre_id != MAGIC_PRE_HASH || pre->post_pre_id != MAGIC_PRE_HASH)
			{
			mem_error("pre hash doesn't match on: %s", pre->name);
			if (pre->post_pre_id != MAGIC_PRE_HASH)
				mem_error("Error happened at post. %s.%d", pre->file, pre->line);
			buf = mem_get_data(pre);
			if (pre->size > 512)
				;
			else
				{
				for (i = 0; i < pre->size; i++)
					{
					if (*((char *)buf) >= ' ' && *((char *)buf) <= 'z')
						fprintf(stderr, "%c", *((char *)buf));
					(char *)buf = (char *)buf + 1;
					}
				fprintf(stderr, "\n");
				}
			g_assert(NULL);
			}
		post = mem_get_post(pre);
		if (post->post_id != MAGIC_POST_HASH)
			{
			mem_error("post hash doesn't match on: %s", pre->name);
			buf = mem_get_data(pre);
			if (pre->size > 512)
				;
			else
				{
				for (i = 0; i < pre->size; i++)
					{
					if (*((char *)buf) >= ' ' && *((char *)buf) <= 'z')
						fprintf(stderr, "%c", *((char *)buf));
					(char *)buf = (char *)buf + 1;
					}
				fprintf(stderr, "\n");
				}
			g_assert(NULL);
			}
		}
	}

char *
mem_strdup_raw(char *funcname, int linenum, char *file, char *string)
	{
	char *data;

	if (!string)
		return NULL;

#ifdef MEM_TEST

	data = mem_alloc_raw(funcname, linenum, file, strlen(string) + 1);
	if (data)
		strcpy(data, string);
#else

	data = g_strdup(string);
#endif

	return data;
	}

char *
mem_strappend_raw(char *funcname, int linenum, char *file, char *string1, char *string2)
	{
	int len = 0;
	char *retval;

	/* mem_error ("appending: %s, %s", string1, string2); */

	if (!string1 && !string2)
		return NULL;

	if (string1)
		len += strlen(string1);
	if (string2)
		len += strlen(string2);

#ifdef MEM_TEST

	retval = mem_alloc_raw(funcname, linenum, file, len + 1);
#else

	retval = g_malloc(len + 1);
#endif

	retval[0] = '\0';

	if (string1)
		strcpy(retval, string1);
	if (string2)
		strcat(retval, string2);

	retval[len] = '\0';

	return retval;
	}

char *
mem_strappend_free_raw(char *funcname, int linenum, char *file, char *str1, char *str2)
	{
	char *retval;

	retval = mem_strappend_raw(funcname, linenum, file, str1, str2);

	if (str1)
		mem_free_raw(str1);
	if (str2)
		mem_free_raw(str2);

	return retval;
	}

void *
mem_strdup_printf_raw(char *funcname, int linenum, char *file, const char *fmt, ...)
	{
	void *retptr;
	gchar *tmpstr;
	va_list args;

	va_start(args, fmt);
	tmpstr = g_strdup_vprintf(fmt, args);
	retptr = mem_strdup_raw(funcname, linenum, file, tmpstr);
	g_free(tmpstr);
	return retptr;
	}

void *
mem_alloc_raw(char *funcname, int linenum, char *file, int count)
	{
#ifndef MEM_TEST
	fprintf(stderr, "Using memory allocation, there is a linking issue here with mem_* func\n");
	return g_malloc(count);
#endif

	int real_size;
	mem_pre_info *pre;
	mem_post_info *post;

#ifdef MEM_CHECKBOUNDARY
	/* This code is slow */
	mem_checkboundaries();
#endif

	real_size = count + sizeof(mem_pre_info) + sizeof(mem_post_info);

	pre = malloc(real_size);

	if (!pre)
		{
		mem_error("Memory full. I think we are all doomed");
		exit( -1);
		}

	strncpy(pre->name, funcname, 63);
	pre->name[63] = '\0';
	strncpy(pre->file, file, 31);
	pre->file[31] = '\0';
	pre->line = linenum;
	pre->size = count;
	pre->id = globalmemoryid++;
	   
#ifdef MEM_DONT_FREE

	pre->freed = 0;
#endif

	post = mem_get_post(pre);

	pre->pre_id = MAGIC_PRE_HASH;
	pre->post_pre_id = MAGIC_PRE_HASH;
	post->post_id = MAGIC_POST_HASH;

	mem_list = g_list_prepend(mem_list, pre);

	return mem_get_data(pre);
	}

void *
mem_calloc_raw(char *funcname, int linenum, char *file, int count)
	{
	void *ptr;

	ptr = mem_alloc_raw(funcname, linenum, file, count);
	if (ptr)
		memset(ptr, 0, count);

	return ptr;
	}

void *
mem_realloc_raw(char *funcname, int linenum, char *file, void *mem, int count)
	{
#ifndef MEM_TEST
	return g_realloc(mem, count);
#endif

	void *new_mem;
	mem_pre_info *pre;

	new_mem = mem_alloc_raw(funcname, linenum, file, count);
	pre = mem_get_pre(new_mem);
	if (mem != NULL)
		memcpy(new_mem, mem, min(count, pre->size));

	mem_free_raw(mem);

	return new_mem;
	}

int
mem_free_raw(void *mem)
	{
#ifndef MEM_TEST
	return g_free(mem);
#endif

	void *buf;
	gint i;
	mem_pre_info *pre;
	mem_post_info *post;

#ifdef MEM_CHECKBOUNDARY

	mem_checkboundaries();
#endif

	if (!mem)
		return 0;

	pre = mem_get_pre(mem);

	if (pre->pre_id != MAGIC_PRE_HASH || pre->post_pre_id != MAGIC_PRE_HASH)
		{
		mem_error("pre hash doesn't match on: %s", pre->name);
		buf = mem_get_data(pre);
		if (pre->size > 256)
			;
		else
			{
			for (i = 0; i < pre->size; i++)
				{
				if (*((char *)buf) >= ' ' && *((char *)buf) <= 'z')
					fprintf(stderr, "%c", *((char *)buf));
				(char *)buf = (char *)buf + 1;
				}
			fprintf(stderr, "\n");
			}
		g_assert(NULL);
		}
	post = mem_get_post(pre);
	if (post->post_id != MAGIC_POST_HASH)
		{
		mem_error("post hash doesn't match on: %s", pre->name);
		buf = mem_get_data(pre);
		if (pre->size > 256)
			;
		else
			{
			for (i = 0; i < pre->size; i++)
				{
				if (*((char *)buf) >= ' ' && *((char *)buf) <= 'z')
					fprintf(stderr, "%c", *((char *)buf));
				(char *)buf = (char *)buf + 1;
				}
			fprintf(stderr, "\n");
			}
		g_assert(NULL);
		}

#ifdef MEM_DONT_FREE
	if (pre->freed)
		{
		mem_error("freeing memory block `%s' for a second time", pre->name);
		mem_error("%s: %d: %s(), %d %d bytes is orginal allocation", pre->file, pre->line, pre->name, pre->id,
		          pre->size);
		buf = mem_get_data(pre);
		if (pre->size > 256)
			;
		else
			{
			for (i = 0; i < pre->size; i++)
				{
				if (*((char *)buf) >= ' ' && *((char *)buf) <= 'z')
					fprintf(stderr, "%c", *((char *)buf));
				(char *)buf++;
				}
			fprintf(stderr, "\n");
			}
		g_assert(NULL);
		}

	pre->freed = 1;
#else

	mem_list = g_list_remove(mem_list, pre);
	free(pre);
#endif

	return 0;
	}

int
mem_display(void)
	{
#ifndef MEM_TEST
	return 0;
#endif

	void *buf;
	GList *cur;
	mem_pre_info *pre;
	int count = 0;
	int bytes = 0, i;

	for (cur = mem_list; cur != NULL; cur = cur->next)
		{
		pre = cur->data;
#ifdef MEM_DONT_FREE

		if (!pre->freed)
			{
#endif
			mem_error("%s: %d: %s(), %d %dbytes still alloced", pre->file, pre->line, pre->name, pre->id, pre->size);
			/* Uncomment this if you want to display the memory buffer */
			buf = mem_get_data(pre);
			if (pre->size > 256)
				;
			else
				{
				for (i = 0; i < pre->size; i++)
					{
					if (*((char *)buf) >= ' ' && *((char *)buf) <= 'z')
						fprintf(stderr, "%c", *((char *)buf));
					(char *)buf = (char *)buf + 1;
					}
				fprintf(stderr, "\n");
				}
			count++;
			bytes += pre->size;
#ifdef MEM_DONT_FREE

			}
#endif

		}

	if (count)
		mem_error("%d memory leaks, totaling %d bytes", count, bytes);

	return 0;
	}

#endif
