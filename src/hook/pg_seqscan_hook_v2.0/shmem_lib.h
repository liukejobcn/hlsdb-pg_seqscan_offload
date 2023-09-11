#ifndef __SHMEM_LIB_H__
#define __SHMEM_LIB_H__

/*
 * for server use, create and free the share memory
 */
void * create_share_memory(char *name, int len);
int free_share_memory(char *name , void *ptr, int len);

/*
 * for client use open and close the share memory
 */
void * open_share_memory(char *name, int len);
int close_share_memory(char *name , void *ptr, int len);

#endif

