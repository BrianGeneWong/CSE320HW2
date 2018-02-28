#include "helper.h"
#include "errno.h"
#include "string.h"
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
	if(ram==NULL){
		printf("INIT_ERROR");
		return errno;
	}
	if(tmp_buf==NULL){
		printf("INIT_ERROR");
		return errno;
	}
	int size;
	int id;
	int totalSize=0;
	void *ram_pntr=ram;
	header *head;
	do{
		head= (header*)ram_pntr;
		size= (head->size)<<3;
		id= (head->app_id);
	//	printf("size of block is %d\n", size);
	//	printf("block is allocated is %d \n", head->allocated);
	//	printf("block id is %d \n\n", head->app_id);
		ram_pntr+=size;
		totalSize+=size;
	}while(id!=0);
	if(totalSize>128){
		if(cse320_sbrk(totalSize-128)==NULL){
			printf("SBRK_ERROR\n");
			return errno;
			
		}
	}
	//FIRST SORY BY ID, IDs can only go up to 3 anyway
	// ID and allocated nested loops?
	void* tmp_pntr=tmp_buf;
	int currID=1;
	int allocated=1;
	ram_pntr=ram;
	while(currID<4){
		while(allocated>=0){
			printf("looking for id %d and allocated %d \n", currID,allocated);
			do{
				head= (header*)ram_pntr;
				size= (head->size)<<3;
				id= (head->app_id);
				int alloc=(head->allocated);
		//		printf("header is id %d and allocated %d \n", id, alloc);
				if(id==currID && alloc==allocated){
									
					//put into tempbuf
					memcpy(tmp_pntr,ram_pntr,size);
					tmp_pntr+=size;
					printf("size of block is %d\n", size);
					printf("block is allocated is %d \n", alloc);
					printf("block id is %d \n\n",id);
				}
				ram_pntr+=size;
			}while(id!=0);
		allocated--;
		ram_pntr=ram;
		}
	allocated=1;
	currID++;
	}
	//after sorting, copy things back into ram.
	printf("total size is %d\n", totalSize);
	memcpy(ram,tmp_buf,totalSize);



    //in the end, add your final block
   // ram_pntr=(ram)+totalSize;
    

    /*
     * Do not modify code below.
     */
    cse320_check();
    cse320_free();
    return ret;
}
