#ifndef dbmem_h
#define dbmem_h

#define GET_FUNC_DETAILS __FUNCTION__, __LINE__, __FILE__

/* IF MEM_TEST is decleared then regiours (very slow) memory testing will
 *	occour, if it isn't then no testing will occur. 
 */
#define MEM_TEST

/* This is for hardcore testing of every boundary, on memory structure. It will
 * have a sharp performance hit but will ensure you are perfect with your
 * memory excution.
 */
/*#define MEM_CHECKBOUNDARY */

#ifdef MEM_TEST

#define mem_strdup_printf(fmt,args...) mem_strdup_printf_raw(GET_FUNC_DETAILS,fmt,##args)
#define mem_strdup(a) mem_strdup_raw(GET_FUNC_DETAILS, a)
#define mem_alloc(a) mem_alloc_raw(GET_FUNC_DETAILS, a)
#define mem_calloc(a) mem_calloc_raw(GET_FUNC_DETAILS, a)
#define mem_realloc(a,b) mem_realloc_raw(GET_FUNC_DETAILS, a, b)
#define mem_free(a) mem_free_raw(a)

#else

#define mem_strdup_printf(fmt,args...) g_strdup_printf(fmt,##args)
#define mem_strdup(a) g_strdup(a)
#define mem_alloc(a) g_malloc(a)
#define mem_calloc(a) g_malloc0(a)
#define mem_realloc(a,b) g_realloc(a, b)
#define mem_free(a) g_free(a)

#endif

char *mem_strdup_raw(char *funcname, int linenum, char *file, char *string);
void *mem_alloc_raw(char *funcname, int linenum, char *file, int count);
void *mem_calloc_raw(char *funcname, int linenum, char *file, int count);
void *mem_realloc_raw(char *funcname, int linenum, char *file, void *mem, int count);
char *mem_strappend_raw(char *funcname, int linenum, char *file, char *string1, char *string2);
char *mem_strappend_free_raw(char *funcname, int linenum, char *file, char *string1, char *string2);
void *mem_strdup_printf_raw(char *funcname, int linenum, char *file, const char *fmt, ...);

int mem_free_raw(void *mem);

/* memory checking/debugging functions */
int mem_display(void);
void mem_checkboundaries(void);

#endif /* MEM_H */
