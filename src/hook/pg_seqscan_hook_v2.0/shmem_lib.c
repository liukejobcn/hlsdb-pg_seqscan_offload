#include <sys/mman.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include "shmem_lib.h"

/*
 * for server use, create and free the share memory
 */
void *create_share_memory(char *name, int len)
{
        int fd;
        void *ptr;

        fd=shm_open(name, O_CREAT|O_TRUNC|O_RDWR, 0777);
        if(-1==fd){
                printf("Shared shm_open\n");
                return NULL;
        }

        if(ftruncate(fd, len)){
                printf("Shared ftruncate\n");
                return NULL;
        }

        ptr=mmap(NULL, len, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
        if(ptr==MAP_FAILED){
                printf("Shared MAP_FAILED\n");
                return NULL;
        }
        close(fd);

        memset(ptr, 0, len);
        return ptr;
}

int free_share_memory(char *name , void *ptr, int len)
{
        munmap(ptr, len);
        shm_unlink(name);
        return 0;
}

/*
 * for client use open and close the share memory
 */
void * open_share_memory(char *name, int len)
{
        int fd;
        void *ptr;
	fd=shm_open(name, O_RDWR, 0777);
        if(-1==fd){
                printf("Shared shm_open\n");
                return NULL;
        }

        ptr=mmap(NULL, len, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
        if(ptr==MAP_FAILED){
                printf("Shared MAP_FAILED\n");
                return NULL;
        }
        close(fd);

	return ptr;
}

int close_share_memory(char *name , void *ptr, int len)
{
        munmap(ptr, len);
        shm_unlink(name);
        return 0;
}


