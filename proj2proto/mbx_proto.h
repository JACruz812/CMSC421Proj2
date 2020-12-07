#include "sys/ipc.h"
#include "sys/shm.h"
#include "linkedList/list.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "stddef.h"

typedef struct component {
    int id;

    // This is used to link components together in their list
    struct list_head list_node;
} component_t;

typedef struct mbx {
	//char pointer to hold array
	const char* name;
	
	//char pointer array to hold mbx data
 	const unsigned char **data;

	//int to save size of data
	unsigned long data_size;

	// This is used to link players together in the players list
    	struct list_head list_node;

    	// This is used so each player can have their own list of components
    	struct list_head components;
} mbx_t;

const char* truncate_name(const char* name);
long create_mbx_421(const char *name, unsigned long size);
long resize_mbx_421(const char *name, unsigned long size);
long delete_mbx_421(const char *name);
long write_mbx_421(const char *name, const unsigned char *data, long offset, long n);
long read_mbx_421(const char *name, unsigned char *data, long offset, long n);
long size_mbx_421(const char *name);
long list_mbx_421(char **list, unsigned long max_entries);
long count_mbx_421(void);


//global list of mailboxes
LIST_HEAD(mailboxes);

//global count of mailboxes
static int mbx_count;

long create_mbx_421(const char *name, unsigned long size){
	//first check if name exists
	struct list_head *pos;
	list_for_each(pos, &mailboxes){
		//extract reference for this iteration
		mbx_t* m = list_entry(pos, mbx_t, list_node);
		if(strcmp(name,m->name)==0){//name == m->name) {
		//	return -EEXIST;
			return 1;
		}
	}


	if (name != NULL){ 
		mbx_count++;	
		mbx_t* new_mbx = (mbx_t*)malloc(sizeof(mbx_t));
		new_mbx->name = truncate_name(name);
		new_mbx->data = malloc(sizeof(char)*size);
		new_mbx->data_size = size;
		list_add_tail(&new_mbx->list_node, &mailboxes);
	}
	else{
//		return -EINVAL;
		return 1;
	}
	return 0;
}



const char* truncate_name(const char *name){
	int i = 0;
	char* n;

	while (name[i]){
		i++;
	}
	if(i > 64){
		n = (char*)malloc(65*sizeof(char));
		memcpy(n,name,64*sizeof(char));
		n[64]='\0';
	} else {
		n = (char*)malloc((i+1)*sizeof(char));
		memcpy(n,name,i*sizeof(char));
		n[i]='\0';
	}
	return n;
}

long resize_mbx_421(const char *name, unsigned long size){
	
	struct list_head *pos;
	list_for_each(pos, &mailboxes){
		//extract reference for this iteration
		mbx_t* m = list_entry(pos, mbx_t, list_node);
		
		if(strcmp(name,m->name)==0) {
		//m->data=(char*)krealloc(m->data,sizeof(char*)*size,GFP_KERNEL);
			m->data = realloc((void*)m->data,sizeof(char)*size);
			m->data_size = size;
		}
	}	
	return 0;	
}

long delete_mbx_421(const char *name){
	
	struct list_head *pos = NULL;
	struct list_head *tmp;
	list_for_each_safe(pos, tmp, &mailboxes) {
            mbx_t* m = list_entry(pos, mbx_t, list_node);
            if (strcmp(name,m->name) == 0){
    	    	free((char*)m->name);	
		free((void*)m->data);	
	        // pos has been modified here. If you need the original node, use tmp
                list_del(pos);
                // kfree(p)
                free(m);
		mbx_count--;
            }
        }
 

	return 0;
}	

long write_mbx_421(const char *name, const unsigned char *data, long offset, long n){
	
	struct list_head *pos;
	
	list_for_each(pos, &mailboxes){
		//extract reference for this iteration
		mbx_t* m = list_entry(pos, mbx_t, list_node);
			
		if(strcmp(name,m->name)==0 && m->data_size > offset) {
		//	m->data[offset] = realloc((void*)m->data[offset],sizeof(char)*n);
			m->data[offset] = data;
		}
	}

	return 0;
}

long read_mbx_421(const char *name, unsigned char *data, long offset, long n){
	struct list_head *pos;
	int i = 0;
	
	list_for_each(pos, &mailboxes){
		//extract reference
		mbx_t* m = list_entry(pos, mbx_t, list_node);
		
		//if a mailbox is named "name"
		if(strcmp(name,m->name)==0 && offset<m->data_size){
			while(m->data[offset][i]!=NULL && i<n){
				data[i] = m->data[offset][i];
				i++;
			}	
		}
	}	
	//return number of copied bytes
	return i;
}

long size_mbx_421(const char *name){
	
	struct list_head *pos;
	
	list_for_each(pos, &mailboxes){
		//extract reference
		mbx_t* m = list_entry(pos, mbx_t, list_node);
		
		//if a mailbox is named "name"
		if(strcmp(name,m->name)==0){
			return m->data_size;
		}
	}
	//should return error
	return 0;
}

long list_mbx_421(char **list, unsigned long max_entries){
	//check that pointer is valid
	if( list != NULL ){		
		struct list_head *pos = NULL;
		//used to keep track of entries
		int i = 0;
	
		list_for_each(pos, &mailboxes){
			//extract reference
			mbx_t* m = list_entry(pos, mbx_t, list_node);
			int j=0;
		       	while(m->name[j]){ 
				j++;
			}
			printf("\n%d\n",j);
			if(i < max_entries && i < mbx_count){
				list[i] = (char*)m->name;
			}
			i++;
		}

		return i;
	} else {
		//return -EFAULT
		return -1;
	}
}

long count_mbx_421(void){
	return mbx_count;
}

