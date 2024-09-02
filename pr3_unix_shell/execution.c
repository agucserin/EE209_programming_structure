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

int execute(DynArray_T b2, DynArray_T barinx, DynArray_T ineql, 
char * argv, char * home){
		int checkexit = 0;
		if(DynArray_getLength(barinx) == 0){
			int i = 0;
			char * a3 = DynArray_get(b2, i);
			if(strcmp(a3,"exit") == 0){
				if (DynArray_getLength(b2) > 1){
					fprintf(stderr,"%s: exit does not take any parameters\n",argv);
					goto free;
				}
				printf("\n");
				/*free(a3);
				DynArray_free(ineql);
				DynArray_free(barinx);
				DynArray_free(b2);*/
				checkexit = 1;
				goto free;
			}
			else if(strcmp(a3 ,"setenv") == 0){
				if (DynArray_getLength(b2) == 1){
fprintf(stderr,"%s: setenv takes one or two parameters\n",argv);
					goto free;
				}
				else if (DynArray_getLength(b2) > 3){
fprintf(stderr,"%s: setenv takes one or two parameters\n",argv);
					goto free;
				}
				const char * a4 = DynArray_get(b2, i + 1);
				if (DynArray_getLength(b2) < i + 3){
					setenv(a4, "", 1);
					goto free;
				}
				else if (strcmp(DynArray_get(b2, i + 2) ,"|") == 0){
					setenv(a4, "", 1);
					goto free;
				}
				else{
					setenv(a4, DynArray_get(b2, i + 2), 1);
					goto free;
				}
			}
			else if(strcmp(a3 ,"unsetenv") == 0){
				if (DynArray_getLength(b2) == 1){
fprintf(stderr,"%s: unsetenv takes one or two parameters\n",argv);
					goto free;
				}
				else if (DynArray_getLength(b2) > 3){
fprintf(stderr,"%s: unsetenv takes one or two parameters\n",argv);
					goto free;
				}
				const char * a4 = DynArray_get(b2, i + 1);
				char * key = getenv(a4);
				if (key != NULL){
					unsetenv(a4);
				}
				goto free;
			}
			else if(strcmp(a3 ,"cd") == 0){
				if(DynArray_getLength(b2) == 1){
					chdir(home);
				}
				else{
					char * a4 = DynArray_get(b2, i + 1);
					if(strcmp(a4 ,".") == 0){
						char name[4096];
						getcwd(name,4096);
						chdir(name);
					}
					else if(strcmp(a4 ,"..") == 0){
						char name[4096];
						getcwd(name,4096);
						for(int i = strlen(name) - 1; i >= 0 ; i--){
							if(name[i] == '/'){
								name[i] = '\0';
								break;
							}
						}
						chdir(name);
					}
					else{
						int b3 = chdir(a4);
						if(b3 == -1){
							fprintf(stderr,"%s: No such file or directory\n",argv);
							goto free;
		  			}
					}
				}
			}
			else{
				pid_t iPid;
				int a4;
				int a3 = -1;
				if(DynArray_getLength(barinx) == 0){					
					a4 = DynArray_getLength(b2);
				}
				else{
					int *a5 = DynArray_get(barinx, 0);
					a4 = *a5;
				}
				
				int count = 0;
				int a35 = 0;
				for(int i = 0; i < DynArray_getLength(ineql); i++ ){
					int *a6 = DynArray_get(ineql,i);
					if(*a6 > a3 && *a6 < a4){
						a35 = *a6;
						count++;
					}
				}

				if(count == 2){
					int output = -1;
					int input = -1;
					for(int i = 0; i < DynArray_getLength(ineql); i++ ){
						int *a6 = DynArray_get(ineql,i);
						if(strcmp(">",DynArray_get(b2,*a6)) == 0){
							output = *a6;
						}
						else if(strcmp("<",DynArray_get(b2,*a6)) == 0){
							input = *a6;
						}
					}
					if (input == 0 || output == 0){
						fprintf(stderr,"%s: Missing command name\n",argv);
						goto free;
					}
					if (input == -1 || output == -1){
fprintf(stderr,"%s: Multiple redirection of standard in/out\n",argv);
						goto free;
					}
					if (output > input){
						if(output - input == 1 || output == DynArray_getLength(b2) - 1){
fprintf(stderr,"%s: Pipe or redirection destination not specified\n",argv);
							goto free;
						}
						iPid = fork();
						if(iPid == 0){
							signal(SIGINT, SIG_DFL);
							char *a[1023];
							for(int i = 0; i < input; i++){
								a[i] = DynArray_get(b2, i);
							}
							
							if( DynArray_getLength(b2) - output > 2){
								for(int i = output + 2; i < DynArray_getLength(b2); i++){
									a[input + i - output - 2] = DynArray_get(b2, i);
								}
								a[input + DynArray_getLength(b2) - output - 2] = NULL;
							}
							else{
								a[input] = NULL;
							}
							
							for(int i = input + 1; i < output; i++){
								char *b = DynArray_get(b2, i);
								int b3;
								b3 = open(b, O_RDONLY);
								dup2(b3,0);
								close(b3);
								
								if(b3 == -1){
									perror(a[0]);
									exit(-1);
								}
							}
							
							char *b12 = DynArray_get(b2, output + 1);
							int b31;
							b31 = creat(b12, 0600);
							dup2(b31,1);
							close(b31);
							
							execvp(a[0],a);
							perror(a[0]);
							exit(-1);
							
						}
						close(3);
						iPid = wait(NULL);
					}
					else if (input > output){
						if(output - input == -1 || input == DynArray_getLength(b2) - 1){
							
							fprintf(stderr,"%s: Pipe or redirection destination not specified\n",argv);
							goto free;
						}
						iPid = fork();
						if(iPid == 0){
							signal(SIGINT, SIG_DFL);
							char *a[1023];
							for(int i = 0; i < output; i++){
								a[i] = DynArray_get(b2, i);
							}
							
							if(input - output > 2){
								for(int i = output + 2; i < input; i++){
									a[output + i - output - 2] = DynArray_get(b2, i);
								}
								a[input - 2] = NULL;
							}
							else{
								a[output] = NULL;
							}
							
							for(int i = input + 1; i < DynArray_getLength(b2); i++){
								char *b = DynArray_get(b2, i);
								int b3;
								b3 = open(b, O_RDONLY);
								dup2(b3,0);
								close(b3);
								
								if(b3 == -1){
									fprintf(stderr,"%s: No such file or directory\n",argv);
									exit(0);
								}
							}
							
							char *b12 = DynArray_get(b2, output + 1);
							int b31;
							b31 = creat(b12, 0600);
							dup2(b31,1);
							close(b31);
							
							execvp(a[0],a);
							perror(a[0]);
							exit(-1);
						}
						close(3);
						iPid = wait(NULL);
					}
					goto free;
				}
				
				if(count > 2){
					fprintf(stderr,"%s: Multiple redirection of standard in/out\n",argv);
					goto free;
				}

				if(a35 == 0 && DynArray_getLength(ineql) == 0){
					iPid = fork();                                           
					if(iPid == 0){
						signal(SIGINT, SIG_DFL);
						char *a[a4 - a3];
						for(int i = a3 + 1; i < a4; i++ ){
							a[i - a3 - 1] = DynArray_get(b2, i);
						}
						a[a4 - a3-1] = NULL;
						
						execvp(a[0],a);
						perror(a[0]);
						exit(-1);
					}
					close(3);
					iPid = wait(NULL);
					goto free;
				}
				else{
					iPid = fork();
					if(iPid == 0){
						signal(SIGINT, SIG_DFL);
						if(strcmp(DynArray_get(b2, a35),">") == 0){
							if(a35 == 0){
								fprintf(stderr,"%s: Missing command name\n",argv);
								exit(0);
							}
							
							if( a35 + 1 == DynArray_getLength(b2)){
								fprintf(stderr,"%s: Pipe or redirection destination not specified\n",argv);
								exit(-1);
							}
							
							char *b = DynArray_get(b2, a35 + 1);
							int b3;
							b3 = creat(b, 0600);
							dup2(b3,1);
							close(b3);
							
							char *a[a35 - a3];
							for(int i = a3 + 1; i < a35; i++ ){
								a[i - a3 - 1] = DynArray_get(b2, i);
							}
							
							if( DynArray_getLength(b2) > a35 + 2 ){
								for(int i = a35 + 2; i < DynArray_getLength(b2); i++){
									a[a35 - a3 - 1 + i - a35 - 2 ] = DynArray_get(b2, i);
								}
								a[a35 - a3 - 1 + DynArray_getLength(b2) - a35 - 2 ] = NULL;
							}
							else{
								a[a35 - a3 - 1 ] = NULL;
							}
							
							execvp(a[0],a);
							perror(a[0]);
							exit(-1);
						}
						else{
							if(a35 == 0){
								fprintf(stderr,"%s: Missing command name\n",argv);
								exit(0);
							}
							
							if( a35 + 1 == DynArray_getLength(b2)){
								fprintf(stderr,"%s: Pipe or redirection destination not specified\n",argv);
								exit(0);
							}
							
							for(int i = a35 + 1; i < DynArray_getLength(b2); i++){
								char *b = DynArray_get(b2, a35 + 1);
								int b3;
								b3 = open(b, O_RDONLY);
								dup2(b3,0);
								close(b3);
							
								if(b3 == -1){
									fprintf(stderr,"%s: No such file or directory\n",argv);
									exit(0);
								}
							}

							char *a[a35 - a3];
							for(int i = a3 + 1; i < a35; i++ ){
								a[i - a3 - 1] = DynArray_get(b2, i);
							}
							a[a35 - a3 - 1 ] = NULL;
							
							execvp(a[0],a);
							perror(a[0]);
							exit(-1);
						}
					}
					close(3);
					iPid = wait(NULL);
				}
			}
			free:
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
			DynArray_free(ineql);
			DynArray_free(b2);
			DynArray_free(barinx);
			if(checkexit == 0){
				return 0;
			}
			else{
				return 0;
			}
		}
		
		
		
		else{

		for(int i = 0; i < DynArray_getLength(barinx) ; i++){
			int *pipe1 = DynArray_get(barinx, i);
			int *pipe2 = pipe1;
			if(i < DynArray_getLength(barinx) - 1){
				pipe2 = DynArray_get(barinx, i+1);
			}
			if(*pipe1 == 0){
				fprintf(stderr,"%s: Missing command name\n",argv);
				for (int i = 0; i < DynArray_getLength(b2); i++){
					char *pipe = DynArray_get(b2, i);
					free(pipe);
				}
				DynArray_free(ineql);
				DynArray_free(b2);
				DynArray_free(barinx);
				return 0;
			}
			else if(*pipe2 - *pipe1 == 1){
				fprintf(stderr,"%s: Pipe or redirection destination not specified\n",argv);
				for (int i = 0; i < DynArray_getLength(b2); i++){
					char *pipe = DynArray_get(b2, i);
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
				return 0;
			}
			else if(*pipe1 == DynArray_getLength(b2) - 1){
				fprintf(stderr,"%s: Pipe or redirection destination not specified\n",argv);
				for (int i = 0; i < DynArray_getLength(b2); i++){
					char *pipe = DynArray_get(b2, i);
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
				return 0;
			}
		}

		
		

		/*for(int i = 0; i < 4 ; i++){
			int *pipe1 = DynArray_get(pipelist, i);
			printf("%d\n",pipe1[0]);
			printf("%d\n",pipe1[1]);
		}*/
		
		if(strcmp(DynArray_get(b2,0),"cd") == 0 || strcmp(DynArray_get(b2,0),"setenv") == 0 || strcmp(DynArray_get(b2,0),"unsetenv") == 0 || strcmp(DynArray_get(b2,0),"exit") == 0){
			int *a = DynArray_get(barinx,DynArray_getLength(barinx)-1);
			if(strcmp(DynArray_get(b2, DynArray_getLength(b2) - 1),"|") == 0 &&  *a == DynArray_getLength(b2) - 1){
				fprintf(stderr,"%s: Pipe or redirection destination not specified\n",argv);
				for (int i = 0; i < DynArray_getLength(b2); i++){
					char *pipe = DynArray_get(b2, i);
					free(pipe);
				}
				DynArray_free(ineql);
				DynArray_free(b2);
				DynArray_free(barinx);
				return 0;
			}
			else{
				char * a = DynArray_get(b2,0);
				fprintf(stderr,"%s: %s takes one parameter\n",argv, a);
				for (int i = 0; i < DynArray_getLength(b2); i++){
					char *pipe = DynArray_get(b2, i);
					free(pipe);
				}
				DynArray_free(ineql);
				DynArray_free(b2);
				DynArray_free(barinx);
				return 0;
			}
		}
		
		
		DynArray_T pipelist = DynArray_new(0);
		for(int i = 0; i < DynArray_getLength(barinx) + 1; i++){
			int * pipe1 = (int *)malloc(sizeof(int)*2) ;
			if(pipe(pipe1) == -1){
				exit(-1);
			}
			DynArray_add(pipelist , pipe1);
		}
		int chk = 0;
		for(int i = 0; i < DynArray_getLength(barinx) + 2; i++){
			pid_t iPid;
			if(i == 0){
				int a4;
				int a3 = -1;
				if(DynArray_getLength(barinx) == i){					
					a4 = DynArray_getLength(b2);
				}
				else{
					int *a5 = DynArray_get(barinx, i);
					a4 = *a5;
				}
				
				int a35 = -1;
				int count = 0;
				for(int j = 0; j < DynArray_getLength(ineql); j++ ){
					int *a6 = DynArray_get(ineql,j);
					if(*a6 > a3 && *a6 < a4){
						a35 = *a6;
						count++;
					}
				}
				
				if (count > 1){
					fprintf(stderr,"%s: Multiple redirection of standard in/out\n",argv);
					goto free2;
				}
				
				if(a35 == -1){
					iPid = fork();
					if(iPid == 0){
						signal(SIGINT, SIG_DFL);
						int *pipe = DynArray_get(pipelist, i);
						
						close(pipe[0]);
						dup2(pipe[1],1);
						close(pipe[1]);

						char *a[a4 - a3];
						for(int i = a3 + 1; i < a4; i++ ){
							a[i - a3 - 1] = DynArray_get(b2, i);
						}
						a[a4 - a3-1] = NULL;
						
						execvp(a[0],a);
						perror(a[0]);
						exit(-1);
					}
				}
				else{
					if(a35 == 0){
					  	fprintf(stderr,"%s: Missing command name\n",argv);
					  	goto free2;
					}
					else if(strcmp(DynArray_get(b2, a35),">") == 0){
							fprintf(stderr,"%s: Multiple redirection of standard output\n",argv);
							goto free2;
					}
					char *b = DynArray_get(b2, a35 + 1);
					if(strcmp(b,"|") == 0){
						fprintf(stderr,"%s: Pipe or redirection destination not specified\n",argv);
						goto free2;
					}
					iPid = fork();
					if(iPid == 0){
						signal(SIGINT, SIG_DFL);
							int *pipe = DynArray_get(pipelist, i);
							
							int b3;
							b3 = open(b, O_RDONLY);
							if(b3 == -1){
								printf("%s\n",b);
								fprintf(stderr,"%s: No such file or directory\n",argv);
								exit(-1);
							}
							dup2(b3,0);
							close(b3);
							
							close(pipe[0]);
							dup2(pipe[1],1);
							close(pipe[1]);



							char *a[a35 - a3];
							for(int i = a3 + 1; i < a35; i++ ){
								a[i - a3 - 1] = DynArray_get(b2, i);
							}
							a[a35 - a3 - 1 ] = NULL;
							
							execvp(a[0],a);
							perror(a[0]);
							exit(-1);
					}
				}
			}
			else if(i == DynArray_getLength(barinx) + 1 ){
				if (chk == 0){
					iPid = fork();
					if(iPid == 0){
						signal(SIGINT, SIG_DFL);
						for(int j = 0; j < i - 1; j++ ){
							int *pipe = DynArray_get(pipelist, j);
							close(pipe[0]);
							close(pipe[1]);
						}
						int *pipe = DynArray_get(pipelist, i-1);

						close(pipe[1]);
						dup2(pipe[0],0);
						close(pipe[0]);

						char *a[3];
						a[0] = "grep";
						a[1] = "";
						a[2] = NULL;
						execvp(a[0],a);
						perror(a[0]);
						exit(-1);
						
					}
				}
			}
			else{
				int a4;
				int *a3 = DynArray_get(barinx, i - 1);
				if(DynArray_getLength(barinx) == i){					
					a4 = DynArray_getLength(b2);
				}
				else{
					int *a5 = DynArray_get(barinx, i);
					a4 = *a5;
				}

				int a35 = -1;
				int count = 0;
				for(int i = 0; i < DynArray_getLength(ineql); i++ ){
					int *a6 = DynArray_get(ineql,i);
					if(*a6 > *a3 && *a6 < a4){
						a35 = *a6;
						count++;
					}
				}

				if (count > 1){
					fprintf(stderr,"%s: Multiple redirection of standard in/out\n",argv);
					goto free2;
				}

				if(a35 == -1){
					iPid = fork();
					if(iPid == 0){
						signal(SIGINT, SIG_DFL);
						for(int j = 0; j < i - 1; j++ ){
							int *pipe = DynArray_get(pipelist, j);
							close(pipe[0]);
							close(pipe[1]);
						}
						
						int *pipe = DynArray_get(pipelist, i-1);
						int *pipeBC = DynArray_get(pipelist, i);

						close(pipe[1]);
						dup2(pipe[0],0);
						close(pipe[0]);

						close(pipeBC[0]);
						dup2(pipeBC[1],1);
						close(pipeBC[1]);
						
						char *a[a4 - *a3];
						for(int i = *a3 + 1; i < a4; i++ ){
							a[i - *a3 - 1] = DynArray_get(b2, i);
						}
						a[a4 - *a3-1] = NULL;
						
						execvp(a[0],a);
						perror(a[0]);
						exit(-1);
					}	
				}
				else{
					if(a35 == 0){
						fprintf(stderr,"%s: Pipe or redirection destination not specified\n",argv);
						goto free2;
					}
					if(strcmp(DynArray_get(b2, a35),"<") == 0){
						fprintf(stderr,"%s: Multiple redirection of standard in/out\n",argv);
						goto free2;
					}
					if(i != DynArray_getLength(barinx)){
						fprintf(stderr,"%s: Multiple redirection of standard in/out\n",argv);
						goto free2;
					}
					iPid = fork();
					if(iPid == 0){
						signal(SIGINT, SIG_DFL);
						for(int j = 0; j < i - 1; j++ ){
							int *pipe = DynArray_get(pipelist, j);
							close(pipe[0]);
							close(pipe[1]);
						}
						chk = 1;
						int *pipe = DynArray_get(pipelist, i-1);

						close(pipe[1]);
						dup2(pipe[0],0);
						close(pipe[0]);
						
						char *b = DynArray_get(b2, a35 + 1);
						
						int b3;
						b3 = creat(b, 0600);
						dup2(b3,1);
						close(b3);
						
						char *a[a35 - *a3];
						for(int i = *a3 + 1; i < a35; i++ ){
							a[i - *a3 - 1] = DynArray_get(b2, i);
						}
						
						if( DynArray_getLength(b2) > a35 + 2 ){
							for(int i = a35 + 2; i < DynArray_getLength(b2); i++){
								a[a35 - *a3 - 1 + i - a35 - 2 ] = DynArray_get(b2, i);
							}
							a[a35 - *a3 - 1 + DynArray_getLength(b2) - a35 - 2 ] = NULL;
						}
						else{
							a[a35 - *a3 - 1 ] = NULL;
						}
						
						execvp(a[0],a);
						perror(a[0]);
						exit(-1);
					}	
				}
			}
			if(i < DynArray_getLength(barinx) + 1 ){
				int *pipe = DynArray_get(pipelist, i);
				close(pipe[1]);	
			}
			iPid = wait(NULL);
		}
		free2:
			for (int i = 0; i < DynArray_getLength(pipelist); i++){
				int *pipe = DynArray_get(pipelist, i);
				close(pipe[0]);
				close(pipe[1]);
			}
			for (int i = 0; i < DynArray_getLength(pipelist); i++){
				int *pipe = DynArray_get(pipelist, i);
				free(pipe);
			}
			for (int i = 0; i < DynArray_getLength(barinx); i++){
				int *pipe = DynArray_get(barinx, i);
				free(pipe);
			}
			for (int i = 0; i < DynArray_getLength(ineql); i++){
				int *pipe = DynArray_get(ineql, i);
				free(pipe);
			}
			for (int i = 0; i < DynArray_getLength(b2); i++){
				char *pipe = DynArray_get(b2, i);
				free(pipe);
			}
			DynArray_free(ineql);
			DynArray_free(b2);
			DynArray_free(pipelist);
			DynArray_free(barinx);
			return 0;
		}
	}

