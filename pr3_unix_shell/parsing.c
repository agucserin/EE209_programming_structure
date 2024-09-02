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
#include "parsing.h"

int parse(DynArray_T b2, DynArray_T barinx,
 DynArray_T ineql, char* a, char * argv){
	int i = 0;
	int b = 0;
	int i2 = 0;
	int check = 0;
	int check2 = 0;
	
	while(a[i] != '\0'){
		if(isspace(a[i]) && check2 == 0){
			if(b == 0){
				i++;
				continue;
			}
			else{
				char * c = (char*)malloc(sizeof(char)*4096);
				int j = i2;
				for(j = i2; j < i; j++){
					if(a[j] == '"'){
						if(check == 0){
							check = 1;
							i2++;
						}
						else{
							check = 0;
							i2++;
						}
					}
					else{
						c[j-i2] = a[j];
					}
				}
				c[j-i2] = '\0';
				i++;
				DynArray_add(b2, c);
				b = 0;
				continue;
			}
		}
		else if(check2 == 0 && a[i] == '|'){
			if(b == 0){
				char *bar = (char*)malloc(sizeof(char)*4096);
				bar[0] = '|';
				bar[1] = '\0';
				
				DynArray_add(b2, bar);
				int *a3 = (int*)malloc(sizeof(char)*4096);
				*a3 = DynArray_getLength(b2) - 1;
				DynArray_add(barinx , a3);
				i++;
				continue;
			}
			char * c = (char*)malloc(sizeof(char)*4096);
			int j = i2;
			for(j = i2; j < i; j++){
				if(a[j] == '"'){
					if(check == 0){
						check = 1;
						i2++;
					}
					else{
						check = 0;
						i2++;
					}
				}
				else{
					c[j-i2] = a[j];
				}
			}
			c[j-i2] = '\0';
			i++;
			DynArray_add(b2, c);
			char *bar = (char*)malloc(sizeof(char)*4096);
			bar[0] = '|';
			bar[1] = '\0'; 
			DynArray_add(b2, bar);
			int *a3 = (int*)malloc(sizeof(char)*4096);
			*a3 = DynArray_getLength(b2)-1;
			DynArray_add(barinx , a3);
			b = 0;
		}
		else if(check2 == 0 && (a[i] == '>' || a[i] == '<')){
			if(b == 0){
				if(a[i] == '>'){
					char *bar = (char*)malloc(sizeof(char)*4096);
					bar[0] = '>';
					bar[1] = '\0';
					
					DynArray_add(b2, bar);
					int *a3 = (int*)malloc(sizeof(char)*4096);
					*a3 = DynArray_getLength(b2) - 1;
					DynArray_add(ineql , a3);
					i++;
					continue;
				}
				if(a[i] == '<'){
					char *bar = (char*)malloc(sizeof(char)*4096);
					bar[0] = '<';
					bar[1] = '\0';
					
					DynArray_add(b2, bar);
					int *a3 = (int*)malloc(sizeof(char)*4096);
					*a3 = DynArray_getLength(b2) - 1;
					DynArray_add(ineql , a3);
					i++;
					continue;
				}
			}
			char * c = (char*)malloc(sizeof(char)*4096);
			int j = i2;
			for(j = i2; j < i; j++){
				if(a[j] == '"'){
					if(check == 0){
						check = 1;
						i2++;
					}
					else{
						check = 0;
						i2++;
					}
				}
				else{
					c[j-i2] = a[j];
				}
			}
			c[j-i2] = '\0';
			i++;
			DynArray_add(b2, c);
			char *bar = (char*)malloc(sizeof(char)*4096);
			bar[0] = a[i-1];
			bar[1] = '\0'; 
			DynArray_add(b2, bar);
			int *a3 = (int*)malloc(sizeof(char)*4096);
			*a3 = DynArray_getLength(b2)-1;
			DynArray_add(ineql , a3);
			b = 0;
		}
		else{
			if(a[i] == '"'){
				if(check2 == 0){
					check2 = 1;
				}
				else{
					check2 = 0;
				}
			}
			if(b == 1){
				i++;
				continue;
			}
			b = 1;
			i2 = i;
			i++;
			continue;
		}
		
		
	}
	if(check == 1 || check2 == 1){
			fprintf(stderr,"%s: Could not find quote pair\n",argv);
			for (int j = 0; j < DynArray_getLength(b2); j++){
				char *pipe = DynArray_get(b2, j);
				free(pipe);
			}
			DynArray_free(ineql);
			DynArray_free(b2);
			DynArray_free(barinx);
			return 1;
	}
	return 0;
}
