/* assignment3 customer_manager2.c 20190377 Yang Jin Woo */
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "customer_manager.h"

#define UNIT_ARRAY_SIZE 1024

/*struct which contains user's information*/
struct UserInfo {
  char *name;								// customer name
  char *id;									// customer id
  int purchase;							// purchase amount (> 0)
  struct UserInfo *nextname; //next customer's name in (name)linked list
  struct UserInfo *nextid;   //next customer's id in (id)linked list
};

/*hash Table*/
struct Table{
	struct UserInfo **array;  // pointer to the array
};

/*data base*/
struct DB {
  struct Table *name;  // pointer to the name Table
  struct Table *id;    // pointer to the id Table
  int Table_Size;      // bucket number of the table;
  int c;               // numbers of registered customer 
};

enum {HASH_MULTIPLIER = 65599};

/*--------------------------------------------------------------------*/
/*returns hash code(int) of the parameter pcKey(type : const char) 
in hash table which bucket number is BucketCount(parameter,type: int)
used global variable HASH_MULTIPLIER(type : int)*/
static int hash_function(const char *pcKey, int BucketCount)
{
   int i;
   unsigned int uiHash = 0U;
   for (i = 0; pcKey[i] != '\0'; i++){
   uiHash = uiHash *(unsigned int)HASH_MULTIPLIER+(unsigned int)pcKey[i];
	 }
   return (int)(uiHash % (unsigned int)BucketCount);
}

/*--------------------------------------------------------------------*/
/*returns a pointer of table which size is a*sizeof(struct UserInfo*)
paramter : a (type : int)*/
struct Table *Table_create(int a) {
   struct Table *t;
   t = (struct Table*)calloc(1, sizeof(struct Table));
t->array = (struct UserInfo **)calloc(1, a*sizeof(struct UserInfo*));
   return t;
}

/*--------------------------------------------------------------------*/
/*add customer's inforamation(parameter:id, name, purchase) 
database's two table which parameter d points.
returns 0 if it success adding, return -1 if it failed.*/
int Table_add
(DB_T d,const char *name, const char*id, int purchase, int bucket_size)
{
	struct Table *t1 = d->id;
	struct Table *t2 = d->name;
	
	if (d == NULL || id == NULL || name == NULL || purchase <= 0){
		return (-1);
  	}
struct UserInfo *p=(struct UserInfo*)calloc(1,sizeof(struct UserInfo));
   int h = hash_function((char*)id, bucket_size);
	struct UserInfo *q;
	for (q = t1->array[h]; q != NULL; q = q->nextid){
		if (strcmp(q->id, id) == 0 || strcmp(q->name,name) == 0) {
			free(p);
			return (-1);
		}
	}
	
	int h2 = hash_function((char*)name, bucket_size);
	for (q = t2->array[h2]; q != NULL; q = q->nextname){
		if (strcmp(q->id, id) == 0 || strcmp(q->name,name) == 0) {
			free(p);
			return (-1);
		}
	}
   p->name = strdup(name);
   p->id = strdup(id);
   p->purchase = purchase;
   p->nextid = t1->array[h];
	p->nextname = t2->array[h2];
   t1->array[h] = p;
	t2->array[h2] = p;
	return 0;
}


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
  	struct Table *name_table = Table_create(UNIT_ARRAY_SIZE);
  	struct Table *id_table = Table_create(UNIT_ARRAY_SIZE);
  	d->id = id_table;
  	d->name = name_table;
  	d->c = 0;
  	d->Table_Size = UNIT_ARRAY_SIZE;
  	return d;
}
/*--------------------------------------------------------------------*/
/*destroy customer DB which parameter d(type: DB_T) points*/
/*parameter :  d (type: DB_T) */
/*return value:   void */
void
DestroyCustomerDB(DB_T d)
{
  	if (d != NULL){
		struct Table *t1  = d->id;
		struct Table *t2  = d->name;
		struct UserInfo *p;
		struct UserInfo *p1;
		for (int i = 0; i < d->Table_Size; i++){
			p = t1->array[i];
			if(p == NULL){
				continue;
			}
			else{
				for (p = t1->array[i]; p != NULL; p = p1){
					free(p->name);
					p->name= NULL;
					free(p->id);
					p->id = NULL;
					p1 = p->nextid;
					free(p);
					p = NULL;
				}
			}
		}
		free(t1->array);
		free(t1);
		free(t2->array);
		free(t2);
		free(d);
	}
}
/*--------------------------------------------------------------------*/
/*Expand database(which parameter d points)'s 
hash table by double number of buckets. */
void
HashTableExpand(DB_T d)
{
	DB_T e = CreateCustomerDB();
	e->Table_Size = 2*(d->Table_Size);
	
	struct Table *t1 = Table_create(e->Table_Size);
	struct Table *t2 = Table_create(e->Table_Size);
	free(e->id->array);
	free(e->name->array);
	free(e->id);
	free(e->name);
	e->id = t1;
	e->name = t2;
	struct Table *t = d->id;
	struct UserInfo *p;
	for (int i = 0; i < d->Table_Size; i++){
		if(t->array[i] == NULL){
			continue;
		}
		else{
			for (p = t->array[i]; p != NULL; p = p->nextid){
				RegisterCustomer(e,p->id,p->name,p->purchase);
			}
		}
	}
	
	struct Table *t4  = d->name;
	struct UserInfo *p2;
	struct UserInfo *p1;
	for (int i = 0; i < d->Table_Size; i++){
		if(t->array[i] == NULL){
			continue;
		}
		else{
			for (p2 = t->array[i]; p2 != NULL; p2 = p1){
				free(p2->name);				
				free(p2->id);
				p1 = p2->nextid;
				free(p2);
			}
		}
	}
	
	free(t->array);
	free(t);
	free(t4->array);
	free(t4);
	*d = *e;
	free(e);
}
/*--------------------------------------------------------------------*/
/*register customer's information(parameter id, name, purchase) in DB 
which parameter d(type: DB_T) points
returns 0 if it registered successfully, returns -1 if it failed */
/*parameter :  d(type : DB_T), id(type: const char *), 
							 name(type: const char *),purchase(type: const int) */
/*return value:  -1,1(type: int) */
int
RegisterCustomer
(DB_T d,const char *id,const char *name, const int purchase)
{	
	
	int *c = &(d->c);
	if (id == NULL || name == NULL || purchase <= 0){
		return (-1);
  }
	
  int a = Table_add(d,name,id,purchase,d->Table_Size);
	
	if (a == 0){
		*c = *c + 1;
	}
	if (a == -1){
		return(-1);
	}

	if (*c >= (int)(0.75 * d->Table_Size) && d->Table_Size < 1048576){
		HashTableExpand(d);
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
	if (d == NULL || id == NULL){
		return (-1);
	}
	struct Table *t = d->id;
	struct Table *t2 = d->name;
	struct UserInfo *p;
	int h = hash_function((char*)id, d->Table_Size);
	int i = 0;
	int i1= 0;
	char *a = NULL;
	for (p = t->array[h]; p != NULL; p = p->nextid){
		if (strcmp(p->id, id) == 0) {
			i++;
			i1 = 1;
			a = p->name;
			break;
		}
		i++;
	}
	if (i1 == 0){
		return (-1);
	}

	int i2 = 0;
	int h2 = hash_function(a, d->Table_Size);
	for (p = t2->array[h2]; p != NULL; p = p->nextname){
		if (strcmp(p->name, a) == 0) {
			i2++;
			break;
		}
		i2++;
	}

	struct UserInfo *q;
	struct UserInfo *r;
	struct UserInfo *q2;
	struct UserInfo *r2;
	q = t->array[h];
	q2 = t2->array[h2];
	for (int j = 0; j < i; j++){
		if (j == i-2 && i != 1){
			r = q->nextid;
			q->nextid = r->nextid;
			break;
		}
		else if (i == 1){
			t->array[h] = q->nextid;
			break;
		}
		q = q->nextid;
	}
	for (int j = 0; j < i2; j++){
		if (j == i2-2 && i2 != 1){
			r2 = q2->nextname;
			q2->nextname = r2->nextname;
			free(r2->id);
			free(r2->name);
			free(r2);
			break;
		}
		else if (i2 == 1){
			t2->array[h2] = q2->nextname;
			free(q2->id);
			free(q2->name);
			free(q2);
			break;
		}
		q2 = q2->nextname;
	}

	return 0;

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
		return (-1);
	}
	struct Table *t = d->name;
	struct Table *t2 = d->id;
	struct UserInfo *p;
	int h = hash_function((char*)name, d->Table_Size);
	int i = 0;
	int i1= 0;
	char *a = NULL;
	
	
	
	for (p = t->array[h]; p != NULL; p = p->nextname){
		if (p->name != NULL && strcmp(p->name, name) == 0) {
			i++;
			i1 = 1;
			a = p->id;
			break;
		}
		i++;
	}
	if (i1 == 0){
		return (-1);
	}
	
	int i2 = 0;
	int h2 = hash_function(a, d->Table_Size);
	for (p = t2->array[h2]; p != NULL; p = p->nextid){
		if (strcmp(p->id, a) == 0) {
			i2++;
			break;
		}
		i2++;
	}
	
	struct UserInfo *q;
	struct UserInfo *r;
	struct UserInfo *q2;
	struct UserInfo *r2;
	q = t->array[h];
	q2 = t2->array[h2];
	for (int j = 0; j < i; j++){
		if (j == i-2 && i != 1){
			r = q->nextname;
			q->nextname = r->nextname;
			break;
		}
		else if (i == 1){
			t->array[h] = q->nextname;
			break;
		}
		q = q->nextname;
	}
	for (int j = 0; j < i2; j++){
		if (j == i2-2 && i2 != 1){
			r2 = q2->nextid;
			q2->nextid = r2->nextid;
			free(r2->id);
			free(r2->name);
			free(r2);
			break;
		}
		else if (i2 == 1){
			t2->array[h2] = q2->nextid;
			free(q2->id);
			free(q2->name);
			free(q2);
			break;
		}
		q2 = q2->nextid;
	}

	return 0;
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
	struct Table *t = d->id;
	struct UserInfo *p;
	int h = hash_function((char*)id, d->Table_Size);
	for (p = t->array[h]; p != NULL; p = p->nextid){
		if (strcmp(p->id, id) == 0) {
			return(p->purchase);
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
	struct Table *t = d->name;
	struct UserInfo *p;
	int h = hash_function((char*)name, d->Table_Size);
	for (p = t->array[h]; p != NULL; p = p->nextname){
		if (strcmp(p->name, name) == 0) {
			return(p->purchase);
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

	struct Table *t  = d->name;
	struct UserInfo *p;
	int e = 0;
	for (int i = 0; i < d->Table_Size; i++){
		p = t->array[i];
		if(p == NULL){
			continue;
		}
		else{
			for (p = t->array[i]; p != NULL; p = p->nextname){
				e += fp(p->id, p->name, p->purchase);
			}
		}

	}
	return e;
}
