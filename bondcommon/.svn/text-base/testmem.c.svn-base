/*
simple program to excercise the bcmem* functions - written to excercise
a bug  in the testing implementation of mem_realloc  

to build:

gcc testmem.c  `pkg-config glib-2.0 --cflags --libs` -o testmem -Wall -g

*/

#define MEM_TEST 1
#include "bcmem.c"

#define depth 2000



int main(int argc,char** argv)
{
   
   int x;
   void *p=NULL,*a,*c;
   
   printf("starting!!!\n");
   a=mem_alloc(100);
   c=mem_alloc(400);
   
   for(x=0;x<depth;x++)
     { void *b;
	     mem_checkboundaries();
		 mem_free(c);
	     p=mem_realloc(p,x*1000);
		 b=mem_alloc(x%50+7);
	     mem_free(a);
	     a=mem_alloc(x+1000);
		 c=mem_alloc(x%40*40);
		 mem_free(b);
     }
     
   mem_free(p);
   mem_free(c);
   mem_free(a);
       
   mem_display();  
   printf("done\n");
   return 0;
}
