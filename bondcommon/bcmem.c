#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

/* any pointer below this is considered invalid */
#define LOWMARK ((void*)(NULL + 20000))

#ifdef UNIX
#include <unistd.h>
#endif /*UNIX*/

#include <glib.h>

#define BUILD_BCMEM_C
#include "bcmem.h"
#include "bcdebug.h"

/* #define MEM_DONT_FREE */

#define MEM_FREE_OVERWRITE

#define MAGIC_PRE_HASH  0xA5BE5705  /* asbestos */
#define MAGIC_POST_HASH 0x5EAC0A57  /* seacoast */
#define TRASH_HASH      0x5AB07A6E  /* sabotage */
/*#define mem_error(fmt,args...) fprintf (globaldebuginfo.debug_stream, "MEM: " fmt "\n" , ##args) */
#define min(a,b) (((a) > (b)) ? (b) : (a))

#ifdef MEM_TEST

int globalmemoryid=0;
int globalwatchmemoryid=0;
static GList *mem_list = NULL;

typedef struct
	{
	int pre_id;
#ifdef MEM_DONT_FREE
	int freed;
#endif /*MEM_DONT_FREE*/
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
mem_get_pre(const char*funcname, int linenum, const char*file, void *data)
	{
	if (!data || data<LOWMARK)
		{
		if (funcname)
			errormsg("%s: %d: %s(), NULL or Near NULL pointer passed in. => %p.",
			file,linenum,funcname,data);
		fflush(NULL);
		}
#ifndef WIN32
	     if (data>((void*)&data))
		{
		if (funcname)
			fprintf(stderr, "%s: %d: %s(), Pointer to stack address passed in. %p.\n",
			file,linenum,funcname, data);
		fflush(NULL);
		}
#endif	   
	return (void *)(((char *)data) - sizeof(mem_pre_info));
	}

#endif /* MEM_TEST */

void
mem_checkboundaries()
	{
#ifdef MEM_TEST
	gint i, j=0;
	void *mem;
	char*buf;
	GList *walk;
	mem_pre_info *pre;
	mem_post_info *post;
	for (walk = g_list_first(mem_list); walk != NULL; walk = walk->next)
		{
		pre = walk->data;
		mem = mem_get_data(pre);

		if (pre->pre_id != MAGIC_PRE_HASH
		        || pre->post_pre_id != MAGIC_PRE_HASH)
			{
			errormsg("%d items allocated. %d",g_list_length(mem_list), j);
			errormsg("pre hash doesn't match on id %d: %s():%d", 
				  pre->id,pre->name,pre->line);
			if (pre->post_pre_id != MAGIC_PRE_HASH)
				errormsg("Error happened at post. %s.%d", 
					pre->file, pre->line);
			buf = mem_get_data(pre);
			if (pre->size > 512)
				;
			else
				{
				for (i = 0; i < pre->size; i++)
					{
					if (*buf >= ' ' && *buf <= 'z')
						fprintf(stderr, "%c", *buf);
					buf = buf + 1;
					}
				fprintf(stderr, "\n");
				}
			if (walk->prev)
			{
				walk = walk->prev;
				pre = walk->data;
				errormsg("Prev alloc on id %d: %s():%d", 
				  pre->id,pre->name,pre->line);
				walk = walk->next;
				if (walk->next)
				{
					walk = walk->next;
					pre = walk->data;
					errormsg("Next alloc on id "
						"%d: %s():%d", pre->id,
						pre->name,pre->line);
				}
			}
			
			g_assert(NULL);
			}
		post = mem_get_post(pre);
		if (post->post_id != MAGIC_POST_HASH)
			{
			errormsg("post hash doesn't match on: %s", pre->name);
			buf = mem_get_data(pre);
			if (pre->size > 512)
				;
			else
				{
				for (i = 0; i < pre->size; i++)
					{
					if (*buf >= ' ' && *buf <= 'z')
						fprintf(stderr, "%c", *buf);
					buf = buf + 1;
					}
				fprintf(stderr, "\n");
				}
			g_assert(NULL);
			}
		j++;
		}
#endif /* MEM_TEST */
	}

char *
mem_strdup_raw(const char*funcname, int linenum, const char*file, const char*string)
	{
	char *data = NULL;

	if (!string)
		return NULL;

#ifdef MEM_TEST
	data = mem_alloc_raw(funcname, linenum, file, strlen(string) + 1);
	if (data)
		strcpy(data, string);
#else /* MEM_TEST */
	data = g_strdup(string);
#endif /*ifdef-else MEM_TEST */

	return data;
	}


char *
mem_strndup_raw(const char*funcname, int linenum, const char*file, const char*string, size_t len)
	{
	char *data;

	if (!string)
		return NULL;

#ifdef MEM_TEST
	len = min(strlen(string),len);
	data = mem_alloc_raw(funcname, linenum, file,len + 1);
	if (data)
		{strncpy(data, string,len);data[len]='\0';};
#else /* ifdef MEM_TEST */
	data = g_strndup(string,len);
#endif /* ifdef-else MEM_TEST */

	return data;
	}

char *
mem_strappend_raw(const char*funcname, int linenum, const char*file, const char*string1,
                  const char*string2)
	{
	int len = 0;
	char *retval;

	/* errormsg ("appending: %s, %s", string1, string2); */

	if (!string1 && !string2)
		return NULL;

	if (string1)
		len += strlen(string1);
	if (string2)
		len += strlen(string2);

#ifdef MEM_TEST

	retval = mem_alloc_raw(funcname, linenum, file, len + 1);
#else /* ifdef MEM_TEST */

	retval = g_malloc(len + 1);
#endif /* ifdef-else MEM_TEST */

	retval[0] = '\0';

	if (string1)
		strcpy(retval, string1);
	if (string2)
		strcat(retval, string2);

	retval[len] = '\0';

	return retval;
	}

char *
mem_strappend_free_raw(const char*funcname, int linenum, const char*file, char*str1,
                       char *str2)
	{
	char *retval;

	retval = mem_strappend_raw(funcname, linenum, file, str1, str2);

	if (str1)
		mem_free_raw_detailed(funcname, linenum, file,str1);
	if (str2)
		mem_free_raw_detailed(funcname, linenum, file,str2);

	return retval;
	}

char *
mem_strdup_printf_raw(const char*funcname, int linenum, const char*file, const char*fmt,
                      ...)
	{
	void *retptr;
	gchar *tmpstr;
	va_list args;

	va_start(args, fmt);
	tmpstr = g_strdup_vprintf(fmt, args);
	va_end(args);
	retptr = mem_strdup_raw(funcname, linenum, file, tmpstr);
	g_free(tmpstr);
	return retptr;
	}

char *
mem_strdup_vprintf_raw(const char*funcname, int linenum, const char*file, const char*fmt
		       ,va_list args)
	{
	void *retptr;
	gchar *tmpstr;
	tmpstr = g_strdup_vprintf(fmt, args);
	retptr = mem_strdup_raw(funcname, linenum, file, tmpstr);
	g_free(tmpstr);
	return retptr;
	}

char * /* this function is used for non-debug mode */
mem_strdup_replace_raw1(char **ptr, const char*fmt, ...)
{
/*	void *retptr; */
	gchar *tmpstr;
	va_list args;

	va_start(args, fmt);
	tmpstr = g_strdup_vprintf(fmt, args);
/*	retptr = g_strdup(tmpstr);*/
	va_end(args);
/*	g_free(tmpstr);*/
	if (*ptr)
		g_free(*ptr);
/*	return *ptr = retptr;*/
	return *ptr=tmpstr;
}


char * mem_concatenate_list(const GList* list,const char* prefix,const char *glue,const char *suffix) 
{
	GList *walk;
	char* retval;
	char* step;
	
	if(!list) return NULL;
	size_t glue_size=strlen(glue),
		len=1
		    +(prefix?strlen(prefix):0)
			+(suffix?strlen(suffix):0)
			-glue_size;		
		if( glue_size > (suffix?strlen(suffix):0))
		len += glue_size - (suffix?strlen(suffix):0);	
	
	for(walk=g_list_first((GList*)list);walk;walk=walk->next)
		len += glue_size + (walk->data?strlen(walk->data):4);
	
	step=retval = g_malloc(len+1);
	
	strcpy(retval,prefix);
	for(walk=g_list_first((GList*)list);walk;walk=walk->next)
	{
		step+=strlen(step);
		strcpy(step,walk->data?walk->data:"NULL");
		step+=strlen(step);
		strcpy(step,glue);
	}
	strcpy(step,suffix);
	debuglog(90,"OOPS! len=%d alloc=%d",strlen(retval),len+1);
	return retval;
}

char * mem_concatenate_list_raw(const char*funcname, int linenum, const char*file, const GList* list,const char* prefix,const char *glue,const char *suffix) 
{
	GList *walk;
	char* retval;
	char* step;
	size_t lenn;
	if(!list) return NULL;
	size_t glue_size=glue?strlen(glue):0
			,len=1
			+(prefix?strlen(prefix):0)
			+(suffix?strlen(suffix):0)
			-glue_size;
	
	if( glue_size > (suffix?strlen(suffix):0))
		len += glue_size - (suffix?strlen(suffix):0);
	
	for(walk=g_list_first((GList*)list);walk;walk=walk->next)
			len += glue_size + (walk->data?strlen(walk->data):4);
	
	step=retval = mem_alloc_raw(funcname,linenum,file,lenn=len+1);
	strcpy(retval,prefix);
	for(walk=g_list_first((GList*)list);walk;walk=walk->next)
	{
		step+=strlen(step);
		strcpy(step,walk->data?walk->data:"NULL");
		step+=strlen(step);
		strcpy(step,glue);
	}
	strcpy(step,suffix);
	debuglog(90,"len=%d alloc=%d",strlen(retval),lenn);
	return retval;
}

void mem_free_list_and_data_raw(const char*funcname, int linenum, const char*file, GList* list)
	{
	GList *walk;
	for(walk=g_list_first((GList*)list);walk;walk=walk->next)
		{
			mem_free_raw_detailed(funcname,linenum,file,walk->data);
		}
	g_list_free(list);
	};

void mem_free_list_and_data(GList* list)
	{
	GList *walk;
	for(walk=g_list_first((GList*)list);walk;walk=walk->next)
		{
			g_free(walk->data);
		}
	g_list_free(list);
	};	
	
char *
mem_strdup_replace_raw(const char*funcname, int linenum, const char*file, char **ptr, 
		const char*fmt, ...)
	{
	void *retptr;
	gchar *tmpstr;
	va_list args;

	va_start(args, fmt);
	tmpstr = g_strdup_vprintf(fmt, args);
	retptr = mem_strdup_raw(funcname, linenum, file, tmpstr);
	g_free(tmpstr);
	if (*ptr)
		mem_free_raw_detailed(funcname, linenum, file,*ptr);
	*ptr = retptr;
	return retptr;
	}

void *
mem_alloc_raw(const char*funcname, int linenum, const char*file, int count)
	{
#ifndef MEM_TEST
	return g_malloc(count);
#else /* MEM_TEST */

	int real_size;
	mem_pre_info *pre;
	mem_post_info *post;

#ifdef MEM_CHECKBOUNDARY
	/* This code is slow */
	mem_checkboundaries();
#endif /* MEM_CHECKBOUNDARY */
	if (count < 0)
		{
		errormsg("Trying to allocate %d bytes which is invalid", count);
		fflush(NULL);
		return NULL;
		}

	real_size = count + sizeof(mem_pre_info) + sizeof(mem_post_info);

	pre = g_malloc(real_size);

	if (pre == NULL)
		{
		errormsg("Memory full. I think we are all doomed. Was "
			"trying to allocate %d bytes", count);
		perror("Error");
		fflush(NULL);
		exit( -1);
		}
	
	strncpy(pre->name, funcname, 63);
	pre->name[63] = '\0';
	strncpy(pre->file, file, 31);
	pre->file[31] = '\0';
	pre->line = linenum;
	pre->size = count;
	pre->id = globalmemoryid++;
	/* printf("Alloc %d %s():%d\n",pre->id, pre->file, pre->line); */

	if (pre->id > 0 && pre->id == globalwatchmemoryid)
		{
		errormsg("Watch on memory found on %s:%s. id %d", pre->file,
		          pre->name, pre->id);
		g_assert(NULL);
		}

#ifdef MEM_DONT_FREE

	pre->freed = 0;
#endif /*MEM_DONT_FREE*/

	post = mem_get_post(pre);

	pre->pre_id = MAGIC_PRE_HASH;
	pre->post_pre_id = MAGIC_PRE_HASH;
	post->post_id = MAGIC_POST_HASH;
	mem_list = g_list_prepend(mem_list, pre);

	return mem_get_data(pre);
#endif
	}

void *
mem_calloc_raw(const char*funcname, int linenum, const char*file, int count)
	{
#ifdef MEM_TEST
	void *ptr;

	ptr = mem_alloc_raw(funcname, linenum, file, count);
	if (ptr)
		memset(ptr, 0, count);

	return ptr;
#else
	return g_malloc0(count);
#endif /* ifndef - else MEM_TEST */
	
	}

void *
mem_realloc_raw(const char*funcname, int linenum, const char*file, void *mem, int count)
	{
#ifndef MEM_TEST
	return g_realloc(mem, count);
#else /* MEM_TEST */
	void *new_mem;
	mem_pre_info *pre;
		
	new_mem = mem_alloc_raw(funcname, linenum, file, count);
	if(mem)
	{	/* copy the smaller of old and new */
		pre = mem_get_pre(funcname, linenum, file,mem); 
		memcpy(new_mem, mem, min(count, pre->size)); 
		mem_free_raw_detailed(funcname,linenum,file,mem);
	}
	
	return new_mem;
#endif /* ifndef-else MEM_TEST */
	}



int
mem_free_raw_detailed(const char*funcname, int linenum, const char*file, void *mem)
{
#ifndef MEM_TEST
	g_free(mem);
	return 0;
#else /* !MEM_TEST */
	char *buf;
	gint i;
	mem_pre_info *pre;
	mem_post_info *post;
   

#ifdef MEM_CHECKBOUNDARY
	mem_checkboundaries();
#endif /* MEM_CHECKBOUNDARY */

	if (!mem)
		return 0;

	pre = mem_get_pre(funcname, linenum, file,mem);

	if (pre->pre_id != MAGIC_PRE_HASH || pre->post_pre_id != MAGIC_PRE_HASH)
		{
		if(file)
			errormsg("%s: %d: %s(), pre hash doesn't match "
				"on: %s", file, linenum, funcname, pre->name);
		else
			errormsg("pre hash doesn't match on: %s",pre->name);
		buf = mem_get_data(pre);
		if (pre->size > 256)
			;
		else
			{
			for (i = 0; i < pre->size; i++)
				{
				if (*buf >= ' ' && *buf <= 'z')
					fprintf(stderr, "%c", *buf);
				buf = buf + 1;
				}
			fprintf(stderr, "\n");
			}
		fflush(NULL);
		g_assert(NULL);
		}
	post = mem_get_post(pre);
	if (post->post_id != MAGIC_POST_HASH)
		{
		if (file)
		errormsg("%s: %d: %s(),post hash doesn't match on: %s",file,linenum,funcname, pre->name);
		else
		errormsg("post hash doesn't match on: %s",pre->name);
		buf = mem_get_data(pre);
		if (pre->size > 256)
			;
		else
			{
			for (i = 0; i < pre->size; i++)
				{
				if (*buf >= ' ' && *buf <= 'z')
					fprintf(stderr, "%c", *buf);
				buf = buf + 1;
				}
			fprintf(stderr, "\n");
			}
		fflush(NULL);
		g_assert(NULL);
		}

#ifdef MEM_DONT_FREE
	if (pre->freed)
		{
		if( file)
		errormsg("%s: %d: %s(), freeing memory block `%s' for a second time",file,linenum,funcname,pre->name);
		else
		errormsg("freeing memory block `%s' for a second time",pre->name);
		errormsg("%s: %d: %s(), %d %d bytes is orginal allocation", pre->file,
		          pre->line, pre->name, pre->id, pre->size);
		buf = mem_get_data(pre);
		if (pre->size > 256)
			;
		else
			{
			for (i = 0; i < pre->size; i++)
				{
				if (*buf >= ' ' && *buf <= 'z')
					fprintf(stderr, "%c", *buf);
				buf++;
				}
			fprintf(stderr, "\n");
			}
		g_assert(NULL);
		}

	pre->freed = 1;
#else /* MEM - DO -FREE */
#ifdef MEM_FREE_OVERWRITE
   if(pre->size)memset(mem,0xA5,pre->size); 
#endif
   	pre->pre_id = TRASH_HASH; /* jump on the cookie so that it fails mem_valididate() */

	mem_list = g_list_remove(mem_list, pre);
	g_free(pre);
#endif /*ifdef-else MEM_DONT_FREE*/

	return 0;
#endif /*ifndef-else MEM_TEST */
}

int
mem_free_raw(void *mem)
	{
#ifndef MEM_TEST
	g_free(mem);
	return 0;
#else /* !MEM_TEST */
return mem_free_raw_detailed(NULL,0,NULL,mem);
#endif /* !MEM_TEST */
}


int
mem_display(void)
	{
#ifndef MEM_TEST
	return 0;
#else /*MEM_TEST*/

	char *buf;
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
#endif /* MEM_DONT_FREE */
#ifndef BC_INTERNAL_MEM_CHECK
			 if ( pre->name[0] == (unsigned char)'0')
				continue;

#endif
			 errormsg("%s: %d: %s(), %d %d bytes still alloced", 
				pre->file, pre->line, pre->name, pre->id, 
				pre->size);
			/* Uncomment this if you want to display the memory 
			 * buffer */
			buf = mem_get_data(pre);
			if (pre->size > 256)
				;
			else
				{
				for (i = 0; i < pre->size; i++)
					{
					if (*buf >= ' ' && *buf <= 'z')
						fprintf(stderr, "%c", *buf);
					buf = buf + 1;
					}
				fprintf(stderr, "\n");
				}
			count++;
			bytes += pre->size;
#ifdef MEM_DONT_FREE

			} 
#endif /*MEM_DONT_FREE*/

		}

	if (count)
		errormsg("%d memory leaks, totaling %d bytes", count, bytes);

	return 0;
#endif /*MEM_TEST*/
	}

int
mem_getid(void *ptr)
	{
#ifdef MEM_TEST
	mem_pre_info *pre;

	if (!ptr)
		return 0;

	pre = mem_get_pre("",-1,"",ptr);

	return pre->id;
#else /*MEM_TEST*/

	return 0;
#endif /*MEM_TEST*/
	}

int
mem_verify_raw(const char*funcname, int linenum, const char*file, void *ptr)
	{
#ifdef MEM_TEST
	mem_pre_info *pre;
	mem_post_info *post;

	if (!ptr)
		{
		fprintf(stderr, "%s:%d %s() Null pointer passed in.\n", 
			file, linenum, funcname);
		fflush(NULL);
#ifdef UNIX
		sleep(6);
#endif
		abort();
		}
	if (ptr<LOWMARK)
		{
		fprintf(stderr, "%s:%d %s() Near Null pointer passed "
			"in. %p.\n", file, linenum, funcname, ptr);
		fflush(NULL);
#ifdef UNIX
		sleep(5);
#endif /*UNIX*/

		abort();
		}

	pre = mem_get_pre(funcname, linenum, file,ptr);
	if (pre->pre_id != MAGIC_PRE_HASH || pre->post_pre_id != MAGIC_PRE_HASH)
		{
		fprintf(stderr, "%s:%d %s() Memory is invalid (corrupt pre hash) at %p.\n", file,
		        linenum, funcname,(void*) ptr);
		fflush(NULL);
#ifdef UNIX
		sleep(10);
#endif /*UNIX*/
		abort();
		}

	post = mem_get_post(pre);
	if (post->post_id != MAGIC_POST_HASH)
		{

		fprintf(stderr, "%s:%d %s() Memory is invalid on post hash at %p\n", 
		        file, linenum, funcname, (void*) ptr);
		fflush(NULL);
#ifdef UNIX		   
		sleep(10);
#endif /*UNIX*/
		abort();
		}
#ifdef MEM_DONT_FREE
	if( pre->freed)
#else 
	if(pre->pre_id == TRASH_HASH)
#endif
		{
		fprintf(stderr, "%s:%d %s() Memory was freed at %p\n", 
		        file, linenum, funcname, (void*) ptr);
		fflush(NULL);
#ifdef UNIX		   
		sleep(10);
#endif /*UNIX*/
		abort();
		}
	return 0;
#else /*MEM_TEST*/
	g_assert(ptr);
	return 0;
#endif /*MEM_TEST*/
	}

int 
mem_check(void *ptr)
{
#ifdef MEM_TEST
	mem_pre_info *pre;
	mem_post_info *post;

	if (!ptr)
		return -1;
	if (ptr<LOWMARK)
		return -2;
	pre = mem_get_pre(NULL, 0, NULL, ptr);
	if (pre->pre_id != MAGIC_PRE_HASH || pre->post_pre_id != MAGIC_PRE_HASH)
		return -3;
	post = mem_get_post(pre);
	if (post->post_id != MAGIC_POST_HASH)
		return -4;
#ifdef MEM_DONT_FREE
	if (pre->freed)
#else 
	if (pre->pre_id == TRASH_HASH)
#endif
		return -5;
#endif 
	return 0;
}


/* these string funcs are handy... */

int mem_IsEqual(const char *a, const char *b) 
{ 
 return  a == b  || ( a && b  && !strcmp(a,b)) ;
}

/* case insensitive version of the above */

int mem_IsEqualCI(const char *a, const char *b) 
{ 
 return  a == b  || ( a && b  && !strcasecmp(a,b)) ;
}

int mem_IsTrue(const char *s )
{
   if( !s ) return 0;
   switch (*s) 
   {
    case 't':
    case '1':
    case 'y':
      return 1;
   }
  return 0;
}

int mem_IsFalse(const char *s )
{
   if( !s ) return 0;
   switch (*s|32)
   {
    case 'f':
    case '0':
    case 'n':
      return 1;
   }
  return 0;
}
