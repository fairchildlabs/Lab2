#ifndef __SCOOTD_H_
#define __SCOOTD_H_

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <assert.h>


#define SCOOTD_ASSERT(_X) assert(_X)


#define SCOOTD_THREAD_VIDEO_0 0

typedef union
{
	unsigned int state;
	struct
	{
		unsigned int video0 : 1;
		unsigned int video1 : 1;



	} bits;

} scoot_state;


#define SCOOTD_MAX_THREADS 16
	

typedef struct
{
	scoot_state *pState;
	
	int shm_fd;
	pthread_t thread_handles[SCOOTD_MAX_THREADS];
	
} scoot_device;


int scootd_util_open_shared_memory(char *strFileName, scoot_device *pScoot);

pthread_t scootd_util_create_thread(void * (*thread_func) (void *), scoot_device *pScootDevice);



void scootd_util_command_in_terminal(const char *command);









#endif
