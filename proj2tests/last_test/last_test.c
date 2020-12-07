#include "../test.h"

void main(){
	char **list = malloc(sizeof(char**)*2);
	list[0] = malloc(sizeof(char*)*10);
	list[1] = malloc(sizeof(char*)*10);

	printf("\n//Delete Tests: separate process//\n");
	printf("return of delete with separate process(sudo final->pass): %d \n",delete_mbx_421_syscall("world"));
	list_mbx_421_syscall(list,2);
	perror("Error value after delete");
	printf("list of mailboxes: %s %s\n", list[0], list[1]);

	printf("return of delete from invalid name: %d \n",delete_mbx_421_syscall("dont"));
	
	free(list[0]);
	free(list[1]);
	free(list);
}
