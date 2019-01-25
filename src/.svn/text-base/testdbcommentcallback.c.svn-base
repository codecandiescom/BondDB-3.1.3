#include <stdio.h>
#include <stdlib.h>

/*
To test:

gcc testdbcommentcallback.c dbcommentcallback.c dbmem.c \
 dbmethod.c safescanner.c \
 `glib-config --cflags --libs` -I/usr/include/postgresql/ -lbonddb -Wall

*/

/*#include "dbmem.h"*/
#include <glib.h>

#include "dbcommentcallback.h"


void test_callback_lang();
void test_old_casual_lang();
void test_api();


int idx;
gboolean res;

int main()
{
	
	test_old_casual_lang();
	puts("-----------------------------------\n");
	test_callback_lang();
	puts("-----------------------------------\n");

	test_api();

	return 0;
}

void test_api()
{
}

void test_old_casual_lang()
{

#define CORRECT 20
	gchar *correct[CORRECT] = {
		"1;rank.pay",
		"0; rank.pay",
		"2;rank.pay2",
		" 0;rank2.pay ",
		"0; rank2.pay",
		"0 ; rank2.pay",
		"0;rank2. pay",
		"0;rank2 .pay",
		"0;rank2 . pay",
		"0; rank2 .pay",
		"0; rank2. pay",
		"0; rank2 . pay",
		"0;          r_ank.pay",
		"0;       pg_class.pay",
		"2;       pg_class.pay",
		"1;  pg__class.pay_",
		"1;  pg_class._pay",
		"1;  pg_class_._pay",
		"0;  pg_class______.___",
		"1;  p1_2_ass._pay2",
	};

#define WRONG 4
	gchar *wrong[WRONG] = {
		"3;rank.pay",
		"1;ra2nk.asdf.pay",
		"1:ra2nk.asdf.pay",
		" 1:ra2nk.asdf.pay",
	};

	for (idx = 0; idx < CORRECT; idx++) {
		
		res = check_if_old_syntax(correct[idx]);
		
		
		if (res) {
			puts("Parse OK\n");
			
		} else {
			puts("Parse Failed\n");
		}


		if (res == TRUE) {
			printf("%d: PASSED\n\n", idx);
		} else {
			printf("%d: FAILED\n\n", idx);
		}
		
	}


	for (idx = 0; idx < WRONG; idx++) {
		
		res = check_if_old_syntax(wrong[idx]);
		
		
		if (res) {
			puts("Parse OK\n");
			
		} else {
			puts("Parse Failed\n");
		}


		if (res == FALSE) {
			printf("%d: PASSED\n\n", idx);
		}
		
	}
}





void test_callback_lang()
{
#define MAXTEST 5
	gchar *comm[MAXTEST] = {
		"soft: g_list_hello2(soft.hard, any.hard, new.new)",
		"hard : g_list_hello2(soft.hard, any.hard, _new.new)",
		"soft g_list_hello2(soft.hard, a.h, .new)",
		"any 1g_list_hello2(soft.hard, any.hard, new.new)",
		"soft g_list_hello2(soft.hard, any.hard, new,new)"		
	};
	gboolean validity[MAXTEST] = {
		TRUE,
		TRUE,
		FALSE,
		FALSE,
		FALSE
	};
	
	
	int mode;
	gchar * funcname = NULL;
	GList * args = NULL;
	GList * walk = NULL;
	
	mtrace();
	for (idx = 0; idx < MAXTEST; idx++) {
		
		funcname = NULL;
		args = NULL;
		
		res = parse_comment_callback(comm[idx], &mode, &funcname, &args);
		
		
		if (res) {
			puts("Parse OK\n");
			
			printf("f = %s\n", funcname);
			walk = args;
			for (walk = g_list_first(walk); walk; walk = g_list_next(walk)) {
				printf("%s.", (gchar*)walk->data);
				walk = g_list_next(walk);
				printf("%s\n", (gchar*)walk->data);
			}


			/* Free memeory allocation */
			printf("%p\n", funcname);
			puts(funcname);
			g_free(funcname);

			for (args = g_list_first(args); args; args = g_list_next(args)) {
				if (args->data) {
					g_free(args->data);
				}
			}
			
			
		} else {
			puts("Parse failed\n");
		}


		if (res == validity[idx]) {
			printf("%d: PASSED\n", idx);
		} else {
			printf("%d: FAILED\n", idx);
		}
		
	}
}
