#include "helper.h"

typedef struct {
	uint64_t allocated : 1;
	uint64_t app_id : 2;
	uint64_t size : 61 ;

} header;

int main(int argc, char** argv) {
    if (*(argv + 1) == NULL) {
        printf("You should provide name of the test file.\n");
        return 1;
    }
    void* ram = cse320_init(*(argv + 1));
    void* tmp_buf = cse320_tmp_buffer_init();
    int ret = 0;
    /*
     * You code goes below. Do not modify provided parts
     * of the code.
     */
	int size;
	int id;
	do{
	header *head= (header*)ram;
	size= (head->size)<<3;
	id= (head->app_id);
	printf("size of block is %d\n", size);
	printf("block is allocated is %d \n", head->allocated);
	printf("block id is %d \n\n", head->app_id);
	ram+=size;
	}while(id!=0);
    /*
     * Do not modify code below.
     */
    cse320_check();
    cse320_free();
    return ret;
}
