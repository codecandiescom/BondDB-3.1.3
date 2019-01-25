#include <glib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "bc.h"
#include "advstring.h"

#define TEST 1
/* WARNING TO READERS OF THIS CODE: (dru's disclaimer!)
 * I did not write it. In all pratice i don't condone the use of goto's. In all likelyhold
 * this is evil sent code. But it works so i'm not touching it. I'm welcome for new
 * version of this code. oh please someone code it again.
 *
 * Needs to be improved, with maybe an additional layer so it can tell the difference
 * between blahjobtype and blahjob.  See if comparing with job, blahjob is a better mix
 * but at the moment it thinks blahjobtype is so rewrite it some point someone.
 * 
 * OK -> update to solve blahjobtype and blahjob problem.  Add an extra level of matching,
 * so that start and end have a better (lower) return value than in the middle, i.e. so that
 * blahjob has a better match than blahjobtype -> Liam.
 */

/* returns the strength of a match */

/* first checks to see if a direct match between the 2 strings, which in that case it returns 0. (this direct
 * match is non-case specific.)
 * If contains more than one occurance of a string in the other string then return 1.
 * Then check to see if the string which is smallest occurs in the other string, and is sourrounded
 * by non alpbetical charaters If this is so, return 2.  (an exmaple of this would the occrance of "apple"
 * in the string "red apples".
 * Finial check is to see if the string which is smallest occurs in the other string surrourned by alphabetical
 * charaters.  so you get the return value of 3 for "apple" and "applepie". 
 */

/** above doco wrote b4 code - this code doesn't care about alphabetic / non-alphabetic. I don't either.  Now returning
 * 3 if the string appearns in the middle of the other, 2 if at the start or end, 4 worst. 8/12/2001, liam */

gint
adv_strcmp(gchar * str1, gchar * str2)
   {
      gchar *a, *b, *tmp;
      gint l1, l2, retval = ( -1);
      
      
      if (g_strcasecmp(str1, str2) == 0)
	return (0);
      
   /* make temp. buffers, so we don't corrupt the input */
   a = mem_strdup(str1);
   l1 = strlen(a);
   b = mem_strdup(str2);
   l2 = strlen(b);
	/* remove stars at the start and end of strings */
	if(a[0] == '*')
		{
			tmp = mem_strdup(a+1);
			mem_free(a);	
			a = tmp;
		}
	if(b[0] == '*')
		{
			tmp = mem_strdup(b+1);
			mem_free(b);	
			b = tmp;
		}
	if(a[l1-1] == '*')
		{
/*			debugmsg("* removed in compare"); */
			a[l1-1] = '\0';
		}
	if(b[l2-1] == '*')
		{
/*			debugmsg("* removed in compare"); */
			b[l2-1] = '\0';
		}
   tmp = a;
   while (*tmp != '\0')
      {
      *tmp = tolower(*tmp);
      tmp++;
      }
   tmp = b;
   while (*tmp != '\0')
      {
      *tmp = tolower(*tmp);
      tmp++;
      }

   /* make 'a' the longer string */
   if (l1 < l2)
      {
      int ti;

      tmp = a;
      a = b;
      b = tmp;

      ti = l1;
      l1 = l2;
      l2 = ti;
      }
   tmp = strstr(a, b);
   retval = ( -1);
   if (!tmp)
      goto done;

   /* it's in there somewhere ;) */

   /* if more than once, return 1 */
   if (strstr(tmp + 1, b) != NULL)
      {
      retval = 1;
      goto done;
      }

   /* cases: at start, at end, in middle */
   if (tmp == a)
      {
      retval = 2;
      goto done;
      }
   if (tmp == a + l1 - l2)
      {
      retval = 2;
      goto done;
      }

   /* in middle, if 1st is non-alpha then return 1 */
   if (!isalpha(*(tmp - 1)))
      {
      retval = 3;
      goto done;
      }

   /* in middle, if last is non-alpha then return 1 */
   if (!isalpha(*(tmp + l2)))
      {
      retval = 3;
      goto done;
      }

   retval = 4;

done:
   /* free temp. buffers */
   mem_free(a);
   mem_free(b);

   return (retval);
   }

/* This should not be here, but in dbtest */
#ifdef ADVSTRING_TEST
int
main(void)
   {
   char *a, *b;

   a = "apple";
   b = "apple";
   printf("[%s] [%s] %d (0 - exact match)\n", a, b, adv_strcmp(a, b));

   a = "apple";
   b = "red apple";
   printf("[%s] [%s] %d (2 - shorter at end)\n", a, b, adv_strcmp(a, b));

   a = "apple";
   b = "red apples";
   printf("[%s] [%s] %d (2 - space before shorter)\n", a, b, adv_strcmp(a, b));

   a = "apple";
   b = ":apple:";
   printf("[%s] [%s] %d (2 - ':'s around shorter)\n", a, b, adv_strcmp(a, b));

   a = "apple";
   b = "apple pie";
   printf("[%s] [%s] %d (2 - shorter at start)\n", a, b, adv_strcmp(a, b));

   a = "xAPPLEy";
   b = "apple";
   printf("[%s] [%s] %d (3 - alphabetical charatcers around shorter)\n", a, b, adv_strcmp(a, b));

   a = "APPLE";
   b = "appleapplez";
   printf("[%s] [%s] %d (1)\n", a, b, adv_strcmp(a, b));

   a = "APPLE";
   b = "appleappleapple";
   printf("[%s] [%s] %d (1)\n", a, b, adv_strcmp(a, b));
   
   a = "n*";
   b = "n";
   printf("[%s] [%s] %d (2)\n", a, b, adv_strcmp(a, b));

   
      printf("Liam's tests - ignore the rest\n");
      
      a = "blah";
      b = "xblahx";
      printf("[%s] [%s] %d (3)\n", a, b, adv_strcmp(a, b));
      
      a = "blah";
      b = "xblahx";
      printf("[%s] [%s] %d (3)\n", a, b, adv_strcmp(a, b));
      
      a = "blah";
      b = "xblah";
      printf("[%s] [%s] %d (2)\n", a, b, adv_strcmp(a, b));
   }
#endif
