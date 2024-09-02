#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dynarray.h"
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <assert.h>
#include "execution.h"
#include "parsing.h"

void (*old_fun)( int);

void (*pfRet1)(int);
void (*pfRet2)(int);
void (*pfRet3)(int);

/*void mySigintHandler(int iSignal)
{
   printf("\nType Ctrl-\\ again within 5 seconds to exit.\n");
   signal( SIGQUIT, SIG_DFL);
}*/

int ctrl = 0;
int ctrl1 = 0;
void handler(int signo){
	exit(0);
}

void sigint_handler( int signo)
{
	 ctrl1 = 1;
   printf("\nType Ctrl-\\ again within 5 seconds to exit.\n");
   alarm(5);
   signal( SIGQUIT, handler);  // 또는 signal( SIGINT, SIG_DFL);
}

void handler2(int signo){
	ctrl = 1;
	signal( SIGQUIT, sigint_handler);
	signal( SIGALRM, handler2);
	return;
}

/*static void mySigintHandler2(int iSignal)
{
   
}*/

int main(int argc, char *argv[], char *envp[]){
  /*sigset_t sSet;
  int iRet;

  sigemptyset(&sSet);
  sigaddset(&sSet, SIGALRM);
  iRet = sigprocmask(SIG_UNBLOCK, &sSet, NULL);
  assert(iRet == 0);

	sigemptyset(&sSet);
  sigaddset(&sSet, SIGINT);
  iRet = sigprocmask(SIG_UNBLOCK, &sSet, NULL);
  assert(iRet == 0);
  
  sigemptyset(&sSet);
  sigaddset(&sSet, SIGQUIT);
  iRet = sigprocmask(SIG_UNBLOCK, &sSet, NULL);
  assert(iRet == 0);*/
  char front[4096];
  char * home1 = getenv("HOME");
  
  char * ishrc = "/.ishrc";
  strcat(front,home1);
  strcat(front,ishrc);

	
  FILE * fp = fopen(front,"r");
  printf("abcd\n");
	char a[4096];
  while(fgets(a,sizeof(a),fp) != NULL){
  	
  	DynArray_T b2 = DynArray_new(0);
		DynArray_T barinx = DynArray_new(0);
		DynArray_T ineql = DynArray_new(0);
  	char a5 = '%';
		printf("%c %s",a5,a);
		
		if(strcmp(a,"\n") == 0){
			DynArray_free(ineql);
			DynArray_free(b2);
			DynArray_free(barinx);
			continue;
		}
		
		int a3 = parse(b2,barinx,ineql,a,argv[0]);
		if (a3 == 1){
			continue;
		}
		
		
		if(DynArray_getLength(b2) > 0){
			char * a33 = DynArray_get(b2, 0);
			if(strcmp(a33,"exit") == 0){
				if (DynArray_getLength(b2) > 1){
fprintf(stderr,"%s: exit does not take any parameters\n",argv[0]);
					for (int j = 0; j < DynArray_getLength(b2); j++){
						printf("a\n");
						char *pipe = DynArray_get(b2, j);
						free(pipe);
					}
					if(DynArray_getLength(ineql) > 0){
						for (int j = 0; j < DynArray_getLength(ineql); j++){
							char *pipe = DynArray_get(ineql, j);
							free(pipe);
						}
					}
					if(DynArray_getLength(barinx) > 0){
						for (int j = 0; j < DynArray_getLength(barinx); j++){
							char *pipe = DynArray_get(barinx, j);
							free(pipe);
						}
					}
					DynArray_free(ineql);
					DynArray_free(b2);
					DynArray_free(barinx);
					continue;
				}
				else{
					free(a33);
					DynArray_free(ineql);
					DynArray_free(b2);
					DynArray_free(barinx);
					goto free3;
				}
				
			}
		}

		execute(b2,barinx,ineql,argv[0],home1);
  }
  
  fclose(fp);
  signal(SIGALRM, handler2);
	signal( SIGQUIT, sigint_handler);
	signal(SIGINT,SIG_IGN);
  
	while(1){
		char a5 = '%';
		printf("%c ",a5);
		char a111[4096];
		char * b5 = fgets(a111,sizeof(a111),stdin);
		if(b5 == NULL && ctrl == 0 && ctrl1 == 1){
			continue;
		}
		else if(b5 == NULL && ctrl == 1){
			char *b5 = fgets(a111,sizeof(a111),stdin);
			ctrl = 0;
			ctrl1 = 0;
			if(b5 == NULL && ctrl1 == 0){
				continue;
			}
		}
		else if(b5 == NULL){
			printf("\n");
			goto free3;
		}
		
		
		if(strcmp(a111,"\n") == 0){
			continue;
		}
		
		DynArray_T b2 = DynArray_new(0);
		DynArray_T barinx = DynArray_new(0);
		DynArray_T ineql = DynArray_new(0);
		
		int a3 = parse(b2,barinx,ineql,a111,argv[0]);
		if (a3 == 1){
			continue;
		}
		if(DynArray_getLength(b2) > 0){
			char * a33 = DynArray_get(b2, 0);
			if(strcmp(a33,"exit") == 0){
				if (DynArray_getLength(b2) > 1){
fprintf(stderr,"%s: exit does not take any parameters\n",argv[0]);
					for (int j = 0; j < DynArray_getLength(b2); j++){
						char *pipe = DynArray_get(b2, j);
						free(pipe);
					}
					if(DynArray_getLength(ineql) > 0){
						for (int j = 0; j < DynArray_getLength(ineql); j++){
							char *pipe = DynArray_get(ineql, j);
							free(pipe);
						}
					}
					if(DynArray_getLength(barinx) > 0){
						for (int j = 0; j < DynArray_getLength(barinx); j++){
							char *pipe = DynArray_get(barinx, j);
							free(pipe);
						}
					}
					DynArray_free(ineql);
					DynArray_free(b2);
					DynArray_free(barinx);
					continue;
				}
				else{
					printf("\n");
					free(a33);
					DynArray_free(ineql);
					DynArray_free(b2);
					DynArray_free(barinx);
					goto free3;
				}
				
			}
		}
		execute(b2,barinx,ineql,argv[0],home1);
		
   }
   free3:
   return 0;
}


