/* simple VB bindings for some common bonddb operations */

#include <windows.h>
#include "bc.h"
#include "bonddb.h"
#include "bonddbwrite.h"

#define VB_BONDDB_EXPORTS
#include "vb_bonddb.h"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}


static BSTR make_bstr(char *c) // I'm not sure that this doesn't leak memory...
{return SysAllocStringByteLen(c?c:"",strlen(c?c:""));}

// Exported functions:

VB_BONDDB_API void* __stdcall system_init(char **name)
{
	static int beenherebefore = 0;
    {  	
	 // return cached value if already called //	
     char *c=bcconfig_getvalue("   MAGIC=MAGIC");
     if(c) return (void*) strtoul(c,NULL,16);
    }

	{
	char *myargv[2]=
		{
		*name,NULL
		};
	char** argv0=myargv;
		
	if( beenherebefore ) bc_cleanup();
	bc_init(0,argv0);
	beenherebefore =1;
	}

	{
    BonddbGlobal *bonddb;
    bonddb = bonddb_init(NULL,"pgsql", NULL);
		
	if(bonddb){
		char *b=mem_strdup_printf("%lx",(long) bonddb);
		bcconfig_setvalue("   MAGIC=MAGIC",b);
	}
    return bonddb;
    }
}

VB_BONDDB_API BSTR __stdcall config_getvalue(char **name)
{  
    char *c=bcconfig_getvalue((*name));
    return make_bstr(c);
}

VB_BONDDB_API BSTR __stdcall VB_save(BonddbClass *obj, int *result)
{
	BSTR errmsg;
	gchar *emsg=NULL;
	if(!obj) 
		{
		*result =-1;
		return make_bstr("no object to save!");
		}
	*result=bonddb_save(obj, &emsg);
	if(*result || emsg)
	{
		errmsg=make_bstr(emsg?emsg:"Unspecified Error #3");
		mem_free_nn(emsg);
		*result=*result?*result:-1;
		return errmsg;
	}
	return make_bstr("");
}

	
//VB_BONDDB_API BonddbClass __stdcall *VB_new_sql(BonddbGlobal *bonddb, char **sql, BSTR *errmsg,int write)
VB_BONDDB_API BSTR __stdcall VB_new_sql(BonddbGlobal *bonddb, char **sql, BonddbClass **obj,int write)
{
	char *emsg=NULL;
	int res;
	if (!bonddb) {
		*obj=NULL;
		return make_bstr("error: No bonndb handle!");
		}
	*obj = bonddb_new_sql(bonddb, *sql, FALSE,  &emsg);
	if(emsg || !*obj  ) 
	{
		BSTR errmsg=make_bstr(emsg?emsg:"Unspecified Error #1.1");
		mem_free_nn(emsg);
		*obj=NULL;
		return errmsg;
	}

	if(!write) res=bonddb_sql_runnow(*obj, &emsg);
	else res=bonddb_sql_runnow_write(*obj, &emsg);
	
	if(emsg || res) 
	{
		BSTR errmsg=make_bstr(emsg?emsg:"Unspecified Error #2");
		bonddb_free(*obj);
		mem_free_nn(emsg);
		*obj=NULL;
		return errmsg;
	}
	
	debuglog(88,"RES=%p",*obj);
	return make_bstr("");
}

VB_BONDDB_API long __stdcall VB_setvalue(BonddbClass *obj, gchar **table, gchar **field, 
		gchar **value, long mark){
	debuglog(88,"RES=%p table=%s field=%s nuv=%s",obj,*table,*field,*value);
	return obj?bonddb_setvalue(obj, *table, *field, *value,mark?TRUE:FALSE):-1;
		}

VB_BONDDB_API BSTR __stdcall VB_getvalue(BonddbClass *obj, gchar **table, gchar **field,long  *result)
{
	gchar *v=NULL;	
	*result=obj?bonddb_getvalue(obj, **table?*table:NULL, **field?*field:NULL, &v):-1;
	return make_bstr(v?v:"");
}

VB_BONDDB_API int __stdcall VB_numrecord(BonddbClass *obj)
{
	return obj?bonddb_numrecord(obj):-1;
}

VB_BONDDB_API int __stdcall VB_moveto(BonddbClass *obj, gint row){
	return obj?bonddb_moveto(obj,row):-1;
	}

VB_BONDDB_API BSTR __stdcall VB_notify_check(BonddbGlobal *bonddb)
{
	BonddbNotify *n=bonddb?bonddb_notify_check(bonddb):NULL;
	BSTR r=make_bstr(n && n->name?n->name:"");
	if(n)  bonddb_notify_free(n);
	return r;
}

VB_BONDDB_API int __stdcall VB_notify_listen(BonddbGlobal *bonddb,char** notify)
{
	return bonddb? bonddb_notify_listen(bonddb,*notify) : -1;
}

VB_BONDDB_API void __stdcall VB_free(BonddbClass **obj)
{
	if(*obj)
		bonddb_free(*obj);
	*obj=0;
}

VB_BONDDB_API int __stdcall VB_addrecord(BonddbClass *obj,char **tablename)
{
	return obj?bonddb_add(obj,*tablename):-1;
}


VB_BONDDB_API BSTR __stdcall VB_delete(BonddbClass *obj,gchar **tablename,long *retval)
{
	gchar* errormsg;
	if(!obj){ *retval=-1;  return make_bstr("No obj");}
	bonddb_write_delete(obj, *tablename, FALSE);
	*retval = bonddb_save(obj,&errormsg);
	return make_bstr(errormsg?errormsg:"");
}
