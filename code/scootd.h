#ifndef __SCOOTD_H_
#define __SCOOTD_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>


typedef struct
{
	int state;

} scoot_state;



typedef struct
{
	scoot_state *pState;
	
	int shm_fd;
	
} scoot_device;


int scootd_util_open_shared_memory(char *strFileName, scoot_device *pScoot);











#endif
