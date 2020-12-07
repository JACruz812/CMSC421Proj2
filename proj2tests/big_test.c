#include "test.h"

void main(){
	char* d1 = malloc(sizeof(char*)*10);
	char* d2 = malloc(sizeof(char*)*10);
	
	char** list = malloc(sizeof(char**)*2);
	list[1] = malloc(10*sizeof(char*));
	list[0] = malloc(10*sizeof(char*));

	char* hi = NULL;

	
	printf("\n//Create Tests//\n");
	printf("return value of create with invalid name: %ld\n", create_mbx_421_syscall(hi,5));
	perror("Error value after invalid name");
	list_mbx_421_syscall(list,1);
	printf("mailbox name(s): %s\n", list[0]);
	
	printf("return value of valid create: %ld\n",create_mbx_421_syscall("hello",5));
	list_mbx_421_syscall(list,1);
	printf("mailbox name(s): %s\n", list[0]);

	printf("return value of create with duplicate name: %ld\n", create_mbx_421_syscall("hello",5));
	perror("Error value after duplicate name");
	list_mbx_421_syscall(list,1);
	printf("mailbox name(s): %s\n", list[0]);

	printf("return value of create with 65 char name: %ld\n", create_mbx_421_syscall("01234567890123456789012345678901234567890123456789012345678901234",5));
	list_mbx_421_syscall(list,2);
	printf("mailbox name(s): %s %s\n\n", list[0], list[1]);	
	
	delete_mbx_421_syscall("0123456789012345678901234567890123456789012345678901234567890123");
	free(list[1]);
	list[1] = malloc(sizeof(char*)*10);

	printf("\n//Resize Tests//\n");
	printf("mailbox name and size: %s->%d\n",list[0],size_mbx_421_syscall("hello"));
	printf("return value of valid resize: %ld\n", resize_mbx_421_syscall("hello",10));
	printf("mailbox name and size: %s->%d\n",list[0],size_mbx_421_syscall("hello"));
	printf("return value of resize with invalid name: %ld\n\n", resize_mbx_421_syscall("hey",10));
	perror("Error value after invalid name");
	
	
	
	printf("\n//Write Tests//\n");
	read_mbx_421_syscall("hello",d1,0,5);
	printf("mailbox name and data: %s->%s\n",list[0],d1);
	printf("return value of write with endline and null: %ld\n", write_mbx_421_syscall("hello","hi\0b\nye",0,7));
	read_mbx_421_syscall("hello",d1,0,3);
	read_mbx_421_syscall("hello",d2,3,4);
	printf("mailbox name and data: %s->%s%s\n",list[0],d1,d2);
	printf("return value of partial overwrite: %ld\n", write_mbx_421_syscall("hello","world",2,5));
	read_mbx_421_syscall("hello",d1,0,7);
	printf("mailbox name and data: %s->%s\n",list[0],d1);
	printf("return value write with oversized offset/size: %ld\n", write_mbx_421_syscall("hello","world",5,10));
	perror("Error value after oversized offset/size");
	printf("mailbox name and data: %s->%s\n",list[0],d1);
	printf("return value of write with oversized data(size 5): %ld\n", write_mbx_421_syscall("hello","worldgoodbeae",2,5));
	read_mbx_421_syscall("hello",d1,0,7);
	printf("mailbox name and data: %s->%s\n",list[0],d1);
	printf("return value of write with invalid name: %ld\n", write_mbx_421_syscall("nonoo","world",2,5));
	perror("Error value after invalid name");
	read_mbx_421_syscall("hello",d1,0,7);
	printf("mailbox name and data: %s->%s\n\n",list[0],d1);	


	printf("\n//Read Tests//\n");
	printf("return value of valid read: %d\n",read_mbx_421_syscall("hello",d1,0,7));
	create_mbx_421_syscall("world",5);
	free(list[0]);
	free(list[1]);
	list[0] = malloc(sizeof(char*)*10);
	list[1] = malloc(sizeof(char*)*10);
	list_mbx_421_syscall(list,2);
	printf("return value of empty mailbox: %d\n",read_mbx_421_syscall("world",d2,0,5));
	printf("mailbox names and data: %s->%s, %s->%s\n",list[0],d1,list[1],d2);
	printf("return value of invalid mailbox name: %d\n",read_mbx_421_syscall("noName",d2,0,5));
	perror("Error value after invalid name");
	char* f = malloc(2);
	printf("return value of invalid data pointer size: %d\n",read_mbx_421_syscall("noName",f,0,5));
	free(f);
	perror("Error value after invalid data pointer size");
	printf("return value of invalid offset: %d\n",read_mbx_421_syscall("noName",f,33,5));
	perror("Error value after invalid offset");
	printf("return value of invalid size: %d\n\n",read_mbx_421_syscall("noName",f,3,50));
	perror("Error value after invalid size");
	
	
	printf("\n//Size Tests//\n");
	printf("return size of mailbox: %d\n",size_mbx_421_syscall("hello"));
	printf("return size of invalid name: %d\n",size_mbx_421_syscall("hey"));
	perror("Error value after invalid name");
	printf("return size of empty mailbox: %d\n\n",size_mbx_421_syscall("world"));
	
	
	printf("\n//List Tests//\n");
	
	char** list3 = malloc(2);
	printf("return value of unallocated list: %d\n",list_mbx_421_syscall(list3,2));
	perror("Error value after unallocated list");
	printf("list values: %s %s\n",list3[0],list3[1]);
	free(list3);

	printf("return value of with allocated list: %d\n",list_mbx_421_syscall(list,2));
	printf("list values: %s %s\n",list[0],list[1]);	
	
	char** list2 = malloc(1);
	list2[1] = malloc(10);
	printf("return value with undersized list: %d\n",list_mbx_421_syscall(list2,2));
	perror("Error value after undersized list");
	free(list2);
	free(list2[1]);
	printf("list values: %s %s\n",list[0],list[1]);
	
	printf("return value of oversized max_entries(10): %d\n",list_mbx_421_syscall(list,10));
	printf("list values: %s %s\n\n",list[0],list[1]);
	

	printf("\n//Count Tests//\n");
	printf("return value of count: %d\n",count_mbx_421_syscall());
	delete_mbx_421_syscall("hello");
	delete_mbx_421_syscall("world");
	printf("return value of count(empty mailbox): %d\n\n",count_mbx_421_syscall());

	
	
	

	create_mbx_421_syscall("world",5);

	delete_mbx_421_syscall("hello");
	free(list[0]);
	free(list[1]);
	free(list);
	free(d1);
	free(d2);

}
