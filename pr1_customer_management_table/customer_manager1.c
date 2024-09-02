/* assignment3 customer_manager1.c 20190377 Yang Jin Woo */
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "customer_manager.h"

#define UNITARRAYSIZE 1024

/*struct which contains user's information*/
struct UserInfo {
  char *name;                // customer name
  char *id;                  // customer id
  int purchase;              // purchase amount (> 0)
};

/*data base*/
struct DB {
  struct UserInfo **pArray;   // pointer to the array
  int curArrSize;            // current array size (max # of elements)
  int numItems;              // # of stored items, needed to determine
};



/*--------------------------------------------------------------------*/
/*create customer DB  */
/*parameter : void  */
/*return value: d (DB_T : pointer which points created database  */
/*global variable :  UNITARRAYSIZE (int) */
DB_T
CreateCustomerDB(void)
{
  DB_T d;
  d = (DB_T) calloc(1, sizeof(struct DB));
  if (d == NULL) {
    fprintf(stderr, "Can't allocate a memory for DB_T\n");
    return NULL;
  }
  d->numItems = 0;
  d->curArrSize = UNITARRAYSIZE; // start with 1024 elements
  d->pArray =
(struct UserInfo **)calloc(1,(UNITARRAYSIZE)*sizeof(struct UserInfo*));
  if (d->pArray == NULL) {
	int a = d->curArrSize;
fprintf(stderr, "Can't allocate a memory for array of size %d\n",a);   
    free(d);
    return NULL;
  }
  return d;
  
}

/*--------------------------------------------------------------------*/
/*destroy customer DB which parameter d(type: DB_T) points*/
/*parameter :  d (type: DB_T) */
/*return value:   void */
void
DestroyCustomerDB(DB_T d)
{
	struct UserInfo **b = d->pArray;
	for(int i = 0; i < d->curArrSize; i++){
		if(b[i] != NULL){
			if(b[i]->name != NULL){
				free(b[i]->name);
			}
			if(b[i]->id != NULL){
				free(b[i]->id);
			}
			free(b[i]);
		}
	}
	free(d->pArray);
  free(d);
}
/*--------------------------------------------------------------------*/
/*register customer's information(parameter:id, name, purchase) in DB 
which parameter d(type: DB_T) points
returns 0 if it registered successfully, returns -1 if it failed */
/*parameter :  d(type : DB_T), id(type: const char *), 
							 name(type: const char *),purchase(type: const int) */
/*return value:  -1,1(type: int) */
int
RegisterCustomer
(DB_T d, const char *id,const char *name, const int purchase)
{
  if (id == NULL || name == NULL || purchase <= 0){
	return (-1);
  }
  
  struct UserInfo *a =
	(struct UserInfo*)calloc(1,sizeof(struct UserInfo));
  a->name = strdup(name);
  a->id = strdup(id);
  a->purchase = (int)purchase;
  
  
  //struct UserInfo **b = d->pArray;
  int i = 0;
  int l = 0;
  while(1){
	if (i >= d->curArrSize){
		break;
	}
	else if 
(i < d->curArrSize &&(d->pArray[i]==NULL||d->pArray[i]->name==NULL)){
		d->pArray[i] = a;
		d->numItems += 1;
		l = 1;
		break;
	}
else if(d->pArray[i]!=NULL&&sizeof(d->pArray[i]->id)>1&&
sizeof(d->pArray[i]->id)>1){
		if(strcmp(d->pArray[i]->id, (char*)id) == 0 ||
 strcmp(d->pArray[i]->name, (char*)name) == 0){
			free(a->name);
			free(a->id);
			free(a);
			l = 1;
  			return(-1);
		}
	}
	i++;
  }
  if( l == 0 && d->numItems >= d->curArrSize ){
d->pArray = realloc(d->pArray,(size_t)(d->curArrSize+1024)*
sizeof(struct UserInfo*));
		d->curArrSize += 1024;
		d->pArray[i] = a;
		for (int k = 1; k < 1024; k++){
			d->pArray[d->curArrSize - 1024 + k] = NULL;
		}
		d->numItems += 1;
  }
  
  return (0);
}
/*--------------------------------------------------------------------*/
/*unregister customer's information(id, name, purchase) 
  from DB which parameter d(type: DB_T) points
by parameter id(type : const char*). 
returns 0 if it unregistered successfully, returns -1 if it failed   */
/*parameter :  d(type : DB_T), id(type: const char *)  */
/*return value: -1,1(type: int) */
int
UnregisterCustomerByID(DB_T d, const char *id)
{
	if (d == NULL && id == NULL){
		return -1;
	}
  	struct UserInfo **b = d->pArray;
  	int *c = &(d->numItems);
  	
  	int i = 0;
  	int l = 0;
	if(*c == 0){
		return -1;
	}
  	while(1){
if
(b[i] != NULL && b[i]->name != NULL && strcmp(b[i]->id,(char*)id)==0){
  			l = 1;
  			break;
		}
		else if (i == d->curArrSize-1){
			break;
		}
		i++;
	}
	if (l == 0){
		return -1;
	}
	
	free(b[i]->name);
	b[i]->name = NULL;
	free(b[i]->id);
	b[i]->id = NULL;
  	b[i]->purchase = 0;
  	free(b[i]);
	b[i] = NULL;
  	*c = *c - 1;
  	
  	return (0);
  	
}

/*--------------------------------------------------------------------*/
/*unregister customer's information(id, name, purchase) 
from DB which parameter d(type: DB_T) points
by parameter name(type : const char*). 
returns 0 if it unregistered successfully, returns -1 if it failed   */
/*parameter :  d(type : DB_T), name(type: const char *)  */
/*return value: -1,1(type: int) */

int
UnregisterCustomerByName(DB_T d, const char *name)
{	
	if (d == NULL || name == NULL){
		return -1;
	}
  	struct UserInfo **b = d->pArray;
  	int *c = &(d->numItems);
	
  	if(*c == 0){
		return -1;
	}
  	int i = 0;
  	int l = 0;
  	while(1){
if (b[i]!=NULL &&b[i]->name!=NULL&&strcmp(b[i]->name,(char*)name)==0){
  			l = 1;
  			break;
		}
		else if (i == d->curArrSize -1){
			break;
		}
		i++;
	}
	if (l == 0){
		return -1;
	}
	
	free(b[i]->name);
	b[i]->name = NULL;
	free(b[i]->id);
	b[i]->id = NULL;
  	b[i]->purchase = 0;
  	free(b[i]);
	b[i] = NULL;
	
  	*c = *c - 1;
  	
  	return (0);
}
/*--------------------------------------------------------------------*/
/*returns customer(whose id is parameter id)'s purchase amount
from DB which parameter d(type: DB_T) points. 
returns -1 if it fails to find customer. */
/*parameter :  d(type : DB_T), id(type: const char *)  */
/*return value:  purchase ,-1 (type: int) */
int
GetPurchaseByID(DB_T d, const char* id)
{
  	if (d == NULL || id == NULL){
		return (-1);
	}
	struct UserInfo **b = d->pArray;
	
  	int i = 0;
  	for (i = 0; i < d->curArrSize; i++){
if(b[i]!=NULL&&b[i]->id!=NULL&&strcmp(b[i]->id,(char*)id) == 0){
  			return b[i]->purchase;	
		}	
	}
	
  	return (-1);
}


/*--------------------------------------------------------------------*/
/* returns customer(whose name is parameter name)'s purchase amount
from DB which parameter d(type: DB_T) points. 
returns -1 if it fails to find customer.*/
/*parameter :   d(type : DB_T), name(type: const char *) */
/*return value:   purchase ,-1 (type: int) */
int
GetPurchaseByName(DB_T d, const char* name)
{
	if (d == NULL || name == NULL){
		return (-1);
	}
	struct UserInfo **b = d->pArray;
  	
  	int i = 0;
  	for (i = 0; i < d->curArrSize; i++){
if(b[i]!=NULL&&b[i]->name!=NULL&&strcmp(b[i]->name,(char*)name)==0){
  			return b[i]->purchase;	
		}	
	}
	
  	return (-1);
}
/*--------------------------------------------------------------------*/
/*returns the sum of the integers returned by 
parameter fp(type : FUNCPTR_T) 
by calling fp for each user information in DB 
which which parameter d(type: DB_T) points.
returns -1 if the parameters are NULL*/
/*parameter :  d(type : DB_T),  fp(type : FUNCPTR_T)   */
/*return value:  sum, -1(type:int)*/
int
GetSumCustomerPurchase(DB_T d, FUNCPTR_T fp)
{
  	if (d == NULL || fp == NULL){
		return (-1);
	}
	
	struct UserInfo **b = d->pArray;
	
	int sum = 0;
	int i = 0;
	for (i = 0; i < d->curArrSize; i++){
		if( b[i] != NULL && b[i]->name != NULL){
			sum += fp(b[i]->id, b[i]->name, b[i]->purchase);
		}
  		
	}
	
  	return sum;
}
