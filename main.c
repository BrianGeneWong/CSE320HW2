#include "helper.h"
#include "errno.h"
#include "string.h"
typedef struct {
	uint64_t allocated : 1;
	uint64_t app_id : 2;
	uint64_t size : 61 ;

}__attribute__((packed)) header;


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
	int alloc;
	void *ram_pntr=ram;
	void* tmp_pntr=tmp_buf;
	
	int ramSize=0;
	header *head;
	//we can get rid of gaps here also
		if(cse320_sbrk(1024-128)==NULL){
			printf("SBRK_ERROR\n");
			return errno;
			
		}
	while(ramSize<1024){
		head= (header*)ram_pntr;
		size= (head->size)<<3;
		id= (head->app_id);
//		printf("block is allocated is %d \n", head->allocated);
//		printf("block id is %d \n", head->app_id);
//		printf("size of block is %d\n\n", size);
		if(size==0){
			ramSize+=8;
			ram_pntr+=8;
		}
		else{
			memcpy(tmp_pntr,ram_pntr,size);
//			printf("copy block of id %d and size %d\n",id,size);
			tmp_pntr+=size;
			ram_pntr+=size;
			totalSize+=size;
			ramSize+=size;
		}
	}
//	printf("ramSize: %d totalSize %d \n", ramSize,totalSize);
	memcpy(ram,tmp_buf,1024);	
	//lets traverse through temp buf
	
		if(totalSize>(1024-16)){
			printf("SBRK_ERROR\n");
			return ENOMEM;
			
		}
	
	//FIRST SORY BY ID, IDs can only go up to 3 anyway
	// ID and allocated nested loops?
//	printf("\n\n SORY BY ID AND ALLOCATED \n");
	tmp_pntr=tmp_buf;
	int currID=1;
	int allocated=1;
	ram_pntr=ram;
	while(currID<4){
		while(allocated>=0){
		//	printf("looking for id %d and allocated %d \n", currID,allocated);
			do{
				head= (header*)ram_pntr;
				size= (head->size)<<3;
				id= (head->app_id);
				alloc=(head->allocated);
//				printf("header is id %d and allocated %d \n", id, alloc);
				if(id==currID && alloc==allocated){
									
					//put into tempbuf
					memcpy(tmp_pntr,ram_pntr,size);
					tmp_pntr+=size;
//					printf("id: %d, alloc: %d, size:  %d \n", id, alloc, size);
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
//	printf("total size is %d\n", totalSize);
	memcpy(ram,tmp_buf,totalSize);

    //SORT BY SIZE
    ram_pntr=ram;
    void* break_pnt=ram_pntr;
    tmp_pntr=tmp_buf;
    head= (header*)ram_pntr;
    currID= (head->app_id);
    allocated= (head->allocated); 
    header *bestSize=NULL;
    int prevLowest=0;
    int currLowest=0;
  //  printf("\n\nbeginning of sorting by size\n\n");
    while(currID<4){
		do{
				
//			printf("current id: %d and curralloc: %d\n",currID,allocated);
			//if we run out of id and allocated type, do, the next one
			head=(header*)ram_pntr;
			alloc= (head->allocated);
			size= (head->size)<<3;
			id= (head->app_id);
//			printf("current block id: %d, alloc: %d, size:  %d \n", id, alloc, size);
			if(alloc==allocated && id==currID){
//				printf(" FOUND A BLOCK id %d alloc %d size: %d\n",id,alloc,size);
//				printf(" prevlowest: %d  currlowest %d\n",prevLowest,currLowest);
			        if(currLowest==0){
					if(size>prevLowest){
					currLowest=size;
					bestSize=head;
					}
				}
				else{
					if(size<currLowest && size>prevLowest){
							currLowest=size;
							bestSize=head;
					
					}

				}
								
			}
			ram_pntr+=size;
		}while(id!=0);
//			printf("FOUND ID IS ZERO,bestHeader is %p bestsize %d, prev lowest %d\n\n",bestSize,currLowest,prevLowest);
			if(bestSize!=NULL){
//				printf("WE GON COPY STUFF NOW\n");
				prevLowest=currLowest;
				memcpy(tmp_pntr,bestSize,currLowest);
				tmp_pntr+=prevLowest;
				alloc= (bestSize->allocated);
				size= (bestSize->size)<<3;
				id= (bestSize->app_id);
				bestSize=NULL;
//				printf("COPY BLOCK id: %d, alloc: %d, size:  %d \n", id, alloc, size);
			}
			else{
//				printf("DIDNT FIND ANYTHING \n");
				allocated--;
				if(allocated==-1){
					allocated=1;
					currID++;
				}
				prevLowest=0;
			}
	ram_pntr=ram;
	currLowest=0;

    }

	memcpy(ram,tmp_buf,totalSize);


    //COALESCE
  //  printf("\n COALESCCE HERE\n");
    ram_pntr=ram;
    tmp_pntr=tmp_buf;
    header* nextHeader;
    int change=0;
    head=(header*)ram_pntr;
    while(head->app_id!=0){
   	 head=(header*)ram_pntr;
	size= (head->size)<<3;
	id= (head->app_id);	
	alloc=(head->allocated);
//	printf("id: %d, alloc: %d, size:  %d \n", id, alloc, size);
        nextHeader=(header*)(ram_pntr+size);
	if(nextHeader->app_id==0)
		break;
        if(head->app_id==nextHeader->app_id && head->allocated==0 && nextHeader->allocated==0){
//		printf("FOUND TWO FREE BLOCKS\n");
		size=size+((nextHeader->size)<<3);
		head->size=(size>>3);
//		printf("new size is %d\n",size);
		nextHeader=(header*)(ram_pntr+size-8);
//		printf("header: %p, footer: %p \n", head,nextHeader);
		memcpy(nextHeader,head,8);
		change=1;
	}
	if(change){
     	  ram_pntr=ram;
	  change=0;
	}
	else
		ram_pntr+=size;
	
    }
  
    //in the end, add your final block
    ram_pntr=(ram)+totalSize;
    head= (header*)ram_pntr;
    head->allocated=0; 
    head->app_id=0;
    head->size=(16>>3);
    //footer 
    ram_pntr+=8;
    memcpy(ram_pntr,(ram_pntr-8),8);
    //traverse one more time to check
	
//	printf("\n\n FINAL TRAVERSAL TO CHECK\n");
	ram_pntr=ram;
	do{
		head= (header*)ram_pntr;
		size= (head->size)<<3;
		id= (head->app_id);
		alloc=(head->allocated);
//		printf("header :id: %d, alloc: %d, size:  %d \n", id, alloc, size);
		ram_pntr+=size-8;			
		head= (header*)ram_pntr;
		size= (head->size)<<3;
		id= (head->app_id);
		alloc=(head->allocated);

//		printf("footer id: %d, alloc: %d, size:  %d \n\n", id, alloc, size);
		ram_pntr+=8;
	}while(id!=0);
	ram_pntr+=8;
/*		head= (header*)ram_pntr;
		size= (head->size)<<3;
		alloc=(head->allocated);
		id= (head->app_id);
		printf("id: %d, alloc: %d, size:  %d \n", id, alloc, size);
*/	
    /*
     * Do not modify code below.
     */
    cse320_check();
    cse320_free();
    return ret;
}
