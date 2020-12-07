#include "mbx_proto.h"

int main(void) {
	struct list_head *pos;
	printf("//Edge case tests for create//\n");
	printf("//Print without creating mailboxes//\n");
	list_for_each(pos, &mailboxes){
		mbx_t *m = NULL;

		//extract reference for this iteration
		m = list_entry(pos, mbx_t, list_node);

		printf("Mailbox {%s,%s}\n",m->name, m->data[0]);
		
	}
	printf("\n");	

	create_mbx_421("hola", 5);
	create_mbx_421("hola", 5);
	printf("//Print after creating two of the same mailboxes//\n");
	int counter = 0;
	list_for_each(pos, &mailboxes){
		mbx_t *m = NULL;

		//extract reference for this iteration
		m = list_entry(pos, mbx_t, list_node);

		printf("Mailbox %d {%s,%s}\n", counter, m->name, m->data[0]);
			
		counter++;	
	}
	printf("\n");	


	create_mbx_421("hola", -5);
	printf("//Print after giving negative size to mailbox//\n");
	counter = 0;
	list_for_each(pos, &mailboxes){
		mbx_t *m = NULL;

		//extract reference for this iteration
		m = list_entry(pos, mbx_t, list_node);

		printf("Mailbox %d {%s,%s}\n", counter, m->name, m->data[0]);
		
		counter++;
	}
	char** list = malloc(2);	
	create_mbx_421("tony", 5);
	list_mbx_421(list, 2);
	printf("list of names: %s,%s\n count of mailboxes:%d\n",list[0],list[1],count_mbx_421());
	delete_mbx_421("tony");
	resize_mbx_421("hola",10);
	printf("size of mailbox after resize: %d\n",size_mbx_421("hola"));
	unsigned char *d = {"hello"};
	write_mbx_421("hola", d, 0, 4); 
	//print mailbox
	printf("//Mailboxes after delete and write//\n");
	list_for_each(pos, &mailboxes){
		mbx_t *m = NULL;

		//extract reference for this iteration
		m = list_entry(pos, mbx_t, list_node);

		printf("Mailbox %d {%s,%s}\n",counter, m->name, m->data[0]);
		
		counter++;
	}
	d = malloc(4);
	read_mbx_421("hola",d,0,5);
	printf("value of read from hola: %s\n",d);
	free(d);
	free(list);
	delete_mbx_421("hola");
    	return 0;
}

