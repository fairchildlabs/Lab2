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
#include <ctype.h>
#include <stdbool.h>

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

#define SCOOTD_THREAD_UTIL_BUFFER_SIZE 4096


typedef struct
{

	pthread_t thread_handle;
	char      *pOutBuffer;
	char      szBuffer[SCOOTD_THREAD_UTIL_BUFFER_SIZE];
	bool      bRun;

} scootd_threads;




typedef struct
{
	scoot_state *pState;
	
	int shm_fd;

	scootd_threads threads[SCOOTD_MAX_THREADS];
	
	
} scoot_device;

typedef struct
{
	int thread_index;
	scoot_device *pScootDevice;

} scootd_thread_config;

int scootd_util_open_shared_memory(char *strFileName, scoot_device *pScoot);

pthread_t scootd_util_create_thread(void * (*thread_func) (void *), scootd_thread_config *pScootThreadConfig);

char * scootd_util_run_command(scootd_thread_config *pScootThread, const char * command);







#endif
