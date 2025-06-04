#include<stdio.h>
#include<string.h>
#include <pthread.h>
#include<unistd.h>

/** use sbrk system call to allcate memory.  */


// struct block{
//     size_t size;
//     unsigned free;
//     struct block *next
// };

/** make block  16 byte addressable */
typedef char ALIGN[16];
union header{
    struct {
    size_t size;
    unsigned is_free;
    union header *next;
}s;
    ALIGN stub;

};

typedef union header header_t;
header_t *head,*tail;
pthread_mutex_t global_malloc_lock;

header_t* freeSlotAvailable(size_t size){
    header_t *curr = head;
    while(curr!=NULL){
        if(curr->s.is_free == 1 && curr->s.size >= size){
            
            return curr;
        }
        curr = curr->s.next;
    }
    return NULL;
}

void* custom_malloc(size_t size){


    
    if(!size){
        return NULL;
    }

    /** check if free slots are available ? If so then assign those slots */
    pthread_mutex_lock(&global_malloc_lock); 
    header_t *temp = freeSlotAvailable(size);
    if(temp!=NULL){
        // found a free slot
        temp->s.is_free = 0;
        pthread_mutex_unlock(&global_malloc_lock); 
        return (void *)(temp + 1);
    }


    size_t total_size = size + sizeof(header_t);
    header_t* ptr = sbrk(total_size);
    if(ptr == (void*) -1){
            pthread_mutex_unlock(&global_malloc_lock); 
            return NULL;
    }
    ptr->s.size = size;
    ptr->s.is_free = 0;
    ptr->s.next = NULL;
    if(head == NULL){
        head = ptr;
        
    }
    if(tail){
        tail->s.next = ptr;
    }
    tail = ptr;

    pthread_mutex_unlock(&global_malloc_lock); 

       

    return (void *)(ptr + 1);

    
}

void custom_free(void *ptr){
    /** if pointer points to end of block, then remove the block */
    /** if pointer is somewhere in the middle, then mark the block as free.  */

    /** heap grows in LIFO fashion, there may be free blocks in the middle which can't be freed. 
     * Mark those blocks as free and assign those blocks when malloc is requested.
     */
     header_t *header;
     void *block;
     if(!ptr){
        return ;
     }
     pthread_mutex_lock(&global_malloc_lock);
     header = (header_t *)(ptr - 1);

     block= sbrk(0);
     if((char *)ptr + header->s.size == block){
        /** block points to end of heap, delete the block */
        if(head == tail){
            head = NULL;
            tail = NULL;

        }else{
            header_t *temp = head;
            while(temp->s.next!=tail){
               
                temp = temp->s.next;
                



            }
            tail = temp;
            tail->s.next = NULL;

        }
        sbrk(0 - header->s.size -sizeof(header_t));
        pthread_mutex_unlock(&global_malloc_lock);
        // size_t size = ptr->s.size;
        // size = size * -1;
        // void *block = sbrk(size);
        return ;
     }

     header->s.is_free = 1;
     pthread_mutex_unlock(&global_malloc_lock);
     return ;
}

void* custom_calloc(size_t n_num, size_t size){

    size_t new_size = n_num * size;
    if(!size || !n_num){
        return NULL;
    }

    void* block = custom_malloc(new_size);
    if(!block){
        return NULL;
    }
    /** initialize this block with default values */
    memset(block,0,new_size);
    return block;
}

void* custom_realloc(void *old_block, size_t size){
    if(!old_block){
        return custom_malloc(size);
    }
    if(size == 0){
        custom_free(old_block);
        return NULL;
    }
    header_t *temp = (header_t *)(old_block - 1);
    if(temp->s.size >= size){
        return old_block;
    }
    void *new_block = custom_malloc(size);
    if(!new_block){
        return NULL;
    }
    memcpy(new_block,old_block,temp->s.size);
    custom_free(old_block);
    
    return new_block;
    

}


int main(){

    char *str1 = (char *)custom_malloc(20);
    if (str1) {
        strcpy(str1, "Hello");
        printf("str1: %s @ %p\n", str1, (void*)str1);
    }

    custom_free(str1);    
}

