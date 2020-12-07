#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/cred.h>
#include <linux/uidgid.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/string.h>

typedef struct component {
    int id;

    // This is used to link components together in their list
    struct list_head list_node;
} component_t;

typedef struct mbx {
	//char pointer to hold array
	const char *name;
	
	//int to hold name size
 	unsigned int name_size;

	//char pointer array to hold mbx data
 	unsigned char *data;

	//long to save size of data
	unsigned long data_size;

	//bool data_written
	bool data_written;

	// This is used to link players together in the players list
    	struct list_head list_node;

    	// This is used so each player can have their own list of components
    	struct list_head components;

	//PID of creator proccess
	pid_t PID;
} mbx_t;

//global list of mailboxes
LIST_HEAD(mailboxes);

//global count of mailboxes
static int mbx_count;


//leaks memory
const char* __user truncate_name(const char __user *name){
	unsigned int i = 0;
	char* n;

	while (name[i]){
		i++;
	}
	if(i > 64){
		n = (char*)kmalloc(65*sizeof(char), GFP_KERNEL);
		if(copy_from_user((void*)n,name,64*sizeof(char))!=0){
			return NULL;
		}
		n[64]='\0';
	} else {
		n = (char*)kmalloc((i+1)*sizeof(char), GFP_KERNEL);
		if(copy_from_user((void*)n,name,i*sizeof(char))!=0){
			return NULL;
		}
		n[i]='\0';

	}
	return n;
}

SYSCALL_DEFINE2(create_mbx_421, const char __user*, name, unsigned long, size) {
	//if name can't be read for 64 return fault
	if (strnlen_user(name,64)<=0) {return -EFAULT;}
	//if pid isn't valid return error
	if (!task_pid(current)) {return -ESRCH;}
	//if mem allocation is larger then kernel mem size return error
	if (size>4000000) {return -ENOMEM;}
	//first check if name exists
	const char * n = truncate_name(name);
	//if name is NULL copy failed so return invalid pointer error

	struct list_head *pos;
	//check if name already exists
	list_for_each(pos, &mailboxes){
		//extract reference for this iteration
		mbx_t* m = list_entry(pos, mbx_t, list_node);
		if(strcmp(m->name,n)==0) {
			return -EEXIST;
		}
	}
	
	if (n != NULL && access_ok(name,1)){
		mbx_count++;	
		mbx_t* new_mbx;
	       	new_mbx	= kmalloc(sizeof(mbx_t), GFP_KERNEL);
		new_mbx->name = n;
		//calc size of name for name_size
		unsigned int j;
		j=0;
		while(new_mbx->name[j]){ 
			j++;
		}	
		new_mbx->name_size = j;	
		new_mbx->data = (char*)kmalloc(sizeof(new_mbx->data)*size, GFP_KERNEL);
		new_mbx->data_written = false;
//		new_mbx->data[size-1] = "\0";
		new_mbx->data_size = size;
		new_mbx->PID = task_pid(current);
		list_add_tail(&new_mbx->list_node, &mailboxes);
	}
	else{
		return -EFAULT;
	}
	return 0;
}

SYSCALL_DEFINE2(resize_mbx_421, const char __user*, name, unsigned long, size) {
	//if size is too large return error
	if (size>4000000) {return -ENOMEM;}
	int i = 0;
	struct list_head *pos;
	list_for_each(pos, &mailboxes){
		//extract reference for this iteration
		mbx_t* m = list_entry(pos, mbx_t, list_node);
		
		if(access_ok(name,m->name_size)){
			if(strcmp(m->name,name)==0) {
				m->data = krealloc((void*)m->data,sizeof(char*)*size,GFP_KERNEL);
	//			m->data[size-1] = "\0";
				m->data_size = size;
				i++;
			}
		}
	}	
	if (i == 0){
		//if resize never took place invalid mailbox name error
		return -ENOENT;
	}
	return 0;
}

SYSCALL_DEFINE1(delete_mbx_421, const char __user*, name) {
	const char * n = truncate_name(name);
	struct list_head *pos = NULL;
	struct list_head *tmp;
	int i=0;
       	list_for_each_safe(pos, tmp, &mailboxes) {
        	mbx_t* m = list_entry(pos, mbx_t, list_node);
		if (strcmp(m->name,n)==0) {
			//if pid != creator and permissions!=root, Access error 
			if((int)task_pid(current)!=(int)m->PID && from_kgid_munged(current_user_ns(), current_egid())!=0) {
			       	return -EACCES; 
			}
			kfree(m->data);	
               		//delete position at pos 
			list_del(pos);
               		kfree(m);
			mbx_count--;
        		i++;
		}
	
        }
	//if mailbox was deleted return 0
	if ( i == 1) {return 0;}
	//else return invalid mailbox name error
	else {return -ENOENT;}
}

SYSCALL_DEFINE4(write_mbx_421, const char __user*, name, const unsigned char __user*, data, long, offset, long, n) {
	bool noName = true;
	bool noSpace = true;
	//if n bytes can be accessed from data
	if(access_ok(data,n)){
		struct list_head *pos;
	
		list_for_each(pos, &mailboxes){
			//extract reference for this iteration
			mbx_t* m = list_entry(pos, mbx_t, list_node);
			
		
			if(access_ok(name,m->name_size)){
				//if a mailbox is found and offset+n<m->dsize
				if(strcmp(m->name,name)==0){
				       if(offset+n<=m->data_size){
						if(copy_from_user(m->data+offset,data,n)!=0){
							return -EFAULT;
						}
						m->data_written = true;
						noSpace = false;
				       	}	
					noName = false;	
				}	
			}
		}
		if(noName) {;return -ENOENT;}
		if(noSpace) {;return -ENOSPC;}
		return(n);
	} else {
		//if copy fails return bad address error
		return -EFAULT;
	}
}

SYSCALL_DEFINE4(read_mbx_421, const char __user*, name, unsigned char __user*, data, long, offset, long, n) {
	//if n bytes can be accessed from data
	if(access_ok(data,n)){
		struct list_head *pos;
	
		list_for_each(pos, &mailboxes){
			//extract reference
			mbx_t* m = list_entry(pos, mbx_t, list_node);
		
			//if name can be accessed for name_size bytes
			if(access_ok(name,m->name_size)){
				//if a mailbox is named "name"
				if(strcmp(m->name,name)==0){
					if(m->data_written==false){
						return copy_to_user(data,"\0",1);
					} else if (m->data_size < offset+n){
						int i = m->data_size - offset-1;
						copy_to_user(data,m->data+offset,i);
						return i;
					} else {
						copy_to_user(data,(char*) m->data+offset,n)	;
						return n;
					}
				}	
			}	
		}
		return -ENOENT;
	} else {
		//data can't be accessed for n bytes return error
		return -EFAULT;
	}
}

SYSCALL_DEFINE1(size_mbx_421, const char __user*, name) {
	
	if(name != NULL){	
		struct list_head *pos;
	
		list_for_each(pos, &mailboxes){
			//extract reference
			mbx_t* m = list_entry(pos, mbx_t, list_node);
			//if name can be accessed for name_size bytes
			if(access_ok(name,m->name_size)){
				//if a mailbox is named "name"
				if(strcmp(m->name,name)==0){
					return m->data_size;
				}
			}
			
		}
	//if size isn't returned, error that name doesn't exist

		return -ENOENT;
	} else {
		//return bad pointer if name is NULL
		return -EFAULT;
	}
}

SYSCALL_DEFINE2(list_mbx_421, char __user**, list, unsigned long, max_entries) {
	//check that pointer is valid
	if (max_entries == 0){return 0;}
	if(max_entries > mbx_count) {
		if(!access_ok(list,mbx_count)){return -EFAULT;}	
	} else {
		if(!access_ok(list,max_entries)){return -EFAULT;}	
	}
	struct list_head *pos;
	//used to keep track of entries
	unsigned int i = 0;
	
	list_for_each(pos, &mailboxes){
		//extract reference
		mbx_t* m = list_entry(pos, mbx_t, list_node);
		if(i < max_entries && i < mbx_count){
			if(copy_to_user(list[i],(const void*)m->name,m->name_size+1)!=0){
				//if copy fails return bad address error
				return -EFAULT;
			}	
			//increment list count for return	
			i++;
		}
	}
	//return amount of names copied
	return i;
}

SYSCALL_DEFINE0(count_mbx_421) {
	if (mbx_count>0){
		return mbx_count;
	} else {
		//if mbx_count is <1 return 0
		return 0;
	}
}
