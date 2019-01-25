#ifndef bcmem_h
#define bcmem_h

#define GET_FUNC_DETAILS __FUNCTION__, __LINE__, __FILE__

/* IF MEM_TEST is decleared then regiours (very slow) memory testing will
 *	occour, if it isn't then no testing will occur. 
 */
/* This is handled in SConsctruct file */
/*#ifndef MEM_TEST
#define MEM_TEST
#endif */
/* #define MEM_DONT_FREE */

/* This is for hardcore testing of every boundary, on memory structure. It will
 * have a sharp performance hit but will ensure you are perfect with your
 * memory excution.
 */
/* #define MEM_CHECKBOUNDARY */

char * mem_concatenate_list(const GList* list,const char* prefix,const char *glue,const char *suffix);
void mem_free_list_and_data(GList* list);

#ifndef BUILD_BCMEM_C /* these macros are no use while building BCMEM */
#ifdef MEM_TEST

#define mem_strdup_printf(fmt,args...) mem_strdup_printf_raw(GET_FUNC_DETAILS,fmt,##args)
#define mem_strdup_vprintf(fmt,args)  mem_strdup_vprintf_raw(GET_FUNC_DETAILS,fmt,args)
#define mem_strdup_replace(a,fmt,args...) mem_strdup_replace_raw(GET_FUNC_DETAILS,a,fmt,##args)
#define mem_strdup(a) mem_strdup_raw(GET_FUNC_DETAILS, a)
#define mem_strndup(a,b) mem_strndup_raw(GET_FUNC_DETAILS, a,b)
#define mem_alloc(a) mem_alloc_raw(GET_FUNC_DETAILS, a)
#define mem_calloc(a) mem_calloc_raw(GET_FUNC_DETAILS, a)
#define mem_realloc(a,b) mem_realloc_raw(GET_FUNC_DETAILS, a, b)
#define mem_free(a) mem_free_raw_detailed(GET_FUNC_DETAILS,a) /*new function*/
#define mem_free_nn(a) if(a)mem_free(a)
#define mem_free_list_and_data(a) mem_free_list_and_data_raw(GET_FUNC_DETAILS,a) /*new function*/
#define mem_verify(a) mem_verify_raw(GET_FUNC_DETAILS,a)
#define mem_strappend(a,b) mem_strappend_raw(GET_FUNC_DETAILS, a, b)
#define mem_strappend_free(a,b) mem_strappend_free_raw(GET_FUNC_DETAILS, a, b)
#define mem_concatenate_list(list,p,g,s) mem_concatenate_list_raw(GET_FUNC_DETAILS,list,p,g,s)

#else

#ifndef __GNUC__
#define mem_strdup_printf g_strdup_printf
#define mem_strdup_replace mem_strdup_replace_raw1
#ifndef __attribute__ 
#define __attribute__(X) /* */
#endif
#else
#define mem_strdup_printf(fmt,args...) g_strdup_printf(fmt,##args)
#define mem_strdup_replace(a, fmt,args...) mem_strdup_replace_raw1(a, fmt,##args)
#endif
#define mem_strdup_vprintf(fmt,args) g_strdup_vprintf(fmt,args)
#define mem_strdup(a) g_strdup(a)
#define mem_strndup(a,b) g_strndup(a,b)
#define mem_alloc(a) g_malloc(a)
#define mem_calloc(a) g_malloc0(a)
#define mem_realloc(a,b) g_realloc(a, b)
#define mem_free(a) g_free(a)
#define mem_free_nn(a) g_free(a) /*g_free() already ignores NULL*/
#define mem_verify(a) g_assert(a)
#define mem_strappend(a,b) mem_strappend_raw(GET_FUNC_DETAILS, a, b)
#define mem_strappend_free(a,b) mem_strappend_free_raw(GET_FUNC_DETAILS, a, b)

#endif
#endif 

char *mem_strdup_raw(const char *funcname, int linenum, const char *file, const char *string);
char *mem_strndup_raw(const char *funcname, int linenum, const char *file, const char *string, size_t length);
void *mem_alloc_raw(const char *funcname, int linenum,const  char *file, int count);
void *mem_calloc_raw(const char *funcname, int linenum,const  char *file, int count);
void *mem_realloc_raw(const char *funcname, int linenum,const  char *file, void *mem, int count);
char *mem_strappend_raw(const char *funcname, int linenum,const  char *file,const char *string1,const  char *string2);
char *mem_strappend_free_raw(const char *funcname, int linenum, const char *file, char *string1, char *string2) ;

char *mem_strdup_printf_raw(const char *funcname, int linenum, const char *file, const char *fmt, ...) __attribute__ ((format (printf, 4, 5))); 
char *mem_strdup_vprintf_raw(const char*funcname, int linenum, const char*file, const char*fmt, va_list args);
char *mem_strdup_replace_raw(const char *funcname, int linenum,const  char *file, char **ptr, const char *fmt, ...) __attribute__ ((format (printf, 5, 6))); 
char *mem_strdup_replace_raw1(char **ptr, const char *fmt, ...) __attribute__ ((format (printf, 2, 3))); 
int mem_verify_raw(const char *funcname, int linenum,const char *file,void *ptr);
int mem_free_raw(void *mem);
int mem_free_raw_detailed(const char *funcname, int linenum,const char *file, void *mem);

/* memory checking/debugging functions */
int mem_display(void);
void mem_checkboundaries(void);
int mem_getid(void *ptr);
int mem_check(void *ptr);


/* handy string funcs */

int mem_IsEqual(const char *a, const char *b) __attribute__ ((pure));
int mem_IsEqualCI(const char *a, const char *b) __attribute__ ((pure));
int mem_IsTrue(const char *a ) __attribute__ ((pure));
int mem_IsFalse(const char *a) __attribute__ ((pure));

char * mem_concatenate_list_raw(const char*funcname, int linenum, const char*file, const GList* list,const char* prefix,const char *glue,const char *suffix);
void mem_free_list_and_data_raw(const char*funcname, int linenum, const char*file, GList* list);
#endif /* MEM_H */
