
#include <scootd.h>



//Used AI to generate example code I used.
//https://copilot.microsoft.com/sl/iEEDiJEa6PQ
int scootd_util_open_shared_memory(char * strFileName, scoot_device * pScoot)
{

	// Create shared memory ob	ject
	pScoot->shm_fd		= shm_open(strFileName, O_CREAT | O_RDWR, 0666);

	if (pScoot->shm_fd == -1)
	{
		perror("shm_open");
		return 1;
	}

	// Set the size of shared memory
	if (ftruncate(pScoot->shm_fd, sizeof(scoot_device)) == -1)
	{
		perror("ftruncate");
		return 1;
	}

	// Map the shared memory object into the process address space
	pScoot->pState		= mmap(0, sizeof(scoot_state), PROT_READ | PROT_WRITE, MAP_SHARED, pScoot->shm_fd, 0);

	if (pScoot->pState == MAP_FAILED)
	{
		perror("mmap");
		return 1;
	}

	return 0;


}



//AI: https://copilot.microsoft.com/sl/huDnvWgVZZY

pthread_t scootd_util_create_thread(void * (*thread_func) (void *), scootd_thread_config *pScootThreadConfig)
{
	pthread_t		thread;
	int 			result;

	// Create the thread
	result				= pthread_create(&thread, NULL, thread_func, pScootThreadConfig);

	if (result != 0)
	{
		fprintf(stderr, "Error creating thread\n");
		exit(EXIT_FAILURE);
	}
	else
	{
		printf("Running Thread %p\n", thread);
	}
	

	return thread;
}


//AI: https://copilot.microsoft.com/sl/jS6aPunFSKa

#define BUFFER_SIZE 512
char * scootd_util_run_command(scootd_thread_config *pScootThread, const char * command)
{
	scoot_device *pScootDevice;
	
	char			*buffer  ;
	char *			result = NULL; 
	size_t			result_size = 0;
	FILE *			pipe = popen(command, "r");
	int idx = pScootThread->thread_index;
	int count = 0;
	
	scootd_threads   *pThread;
	

	pScootDevice = &pScootThread->pScootDevice[idx];

	pThread = &pScootDevice->threads[idx]; 

	buffer = pThread->szBuffer;
	result = pThread->pOutBuffer;
	
	if(!pipe)
	{
		fprintf(stderr, "popen() failed!\n");
		return NULL;
	}

	while ((pThread->bRun == true) && (fgets(buffer, BUFFER_SIZE, pipe) != NULL))
	{
		size_t			buffer_len = strlen(buffer);
		char *			new_result = realloc(result, result_size + buffer_len + 1);

		if (!new_result)
		{
			free(result);
			fprintf(stderr, "realloc() failed!\n");
			return NULL;
		}

		result				= new_result;
		strcpy(result + result_size, buffer);
		result_size 		+= buffer_len;
		usleep(1000);
		count++;

		if((count % 1000) == 0)
		{
			printf("while %d\n", count);
		}
	}

	printf("********************************\n%s\n", result);

	pclose(pipe);
}



