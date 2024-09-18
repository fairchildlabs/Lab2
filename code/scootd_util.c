
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

pthread_t scootd_util_create_thread(void * (*thread_func) (void *), scootd_thread_config *pScootThread)
{
	pthread_t		thread;
	scootd_threads   *pThread;
	int 			result;
	scoot_device *pScootDevice;
	int idx = pScootThread->thread_index;

	pScootDevice = &pScootThread->pScootDevice[idx];
	pThread = &pScootDevice->threads[idx]; 

	// Create the thread
	result				= pthread_create(&thread, NULL, thread_func, pScootThread);

	if (result != 0)
	{
		fprintf(stderr, "Error creating thread\n");
		exit(EXIT_FAILURE);
	}
	else
	{	
		pThread->thread_handle = thread;
		printf("Running Thread %p\n", thread);
	}
	

	return thread;
}


//AI: https://copilot.microsoft.com/sl/jS6aPunFSKa

void scootd_util_run_command(scootd_thread_config *pScootThread, const char * command)
{
	scoot_device *pScootDevice;
	FILE            *pipe;
	char			*buffer  ;
	char *			result = NULL; 
	size_t			result_size = 0;
	int idx = pScootThread->thread_index;
	int count = 0;
	
	scootd_threads   *pThread;
	

		

	pScootDevice = &pScootThread->pScootDevice[idx];

	pThread = &pScootDevice->threads[idx]; 

	pThread->pipe = popen(command, "r");


	buffer = pThread->szBuffer;
	result = pThread->pOutBuffer;
	pipe = pThread->pipe;
	
	if(!pipe)
	{
		fprintf(stderr, "popen() failed!\n");
		return NULL;
	}

	while ((pThread->bRun == true) && (fgets(buffer, SCOOTD_THREAD_UTIL_BUFFER_SIZE, pipe) != NULL))
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







char * run_command_nonblocking(const char * command)
{
	char			buffer[BUFFER_SIZE];
	char *			result = NULL;
	size_t			result_size = 0;
	FILE *			pipe = popen(command, "r");

	if (!pipe)
	{
		fprintf(stderr, "popen() failed!\n");
		return NULL;
	}

	int 			pipe_fd = fileno(pipe);
	fd_set			read_fds;

	struct timeval timeout;

	while (1)
	{
		FD_ZERO(&read_fds);
		FD_SET(pipe_fd, &read_fds);

		timeout.tv_sec		= 1;					// 1 second timeout
		timeout.tv_usec 	= 0;

		int 			ret = select(pipe_fd + 1, &read_fds, NULL, NULL, &timeout);

		if (ret == -1)
		{
			fprintf(stderr, "select() failed!\n");
			free(result);
			pclose(pipe);
			return NULL;
		}
		else if (ret == 0)
		{
			// Timeout, no data available
			continue;
		}

		if (FD_ISSET(pipe_fd, &read_fds))
		{
			if (fgets(buffer, BUFFER_SIZE, pipe) == NULL)
			{
				break;								// End of file or error
			}

			size_t			buffer_len = strlen(buffer);
			char *			new_result = realloc(result, result_size + buffer_len + 1);

			if (!new_result)
			{
				free(result);
				fprintf(stderr, "realloc() failed!\n");
				pclose(pipe);
				return NULL;
			}

			result				= new_result;
			strcpy(result + result_size, buffer);
			result_size 		+= buffer_len;
		}
	}

	pclose(pipe);
	return result;
}






int scootd_util_character_to_pipe(scootd_thread_config * pScootThread, char character)
{
	int 			idx = pScootThread->thread_index;
	scoot_device *	pScootDevice;
	scootd_threads * pThread;

	pScootDevice		= &pScootThread->pScootDevice[idx];
	pThread 			= &pScootDevice->threads[idx];
	FILE *			pipe;


	pipe				= pThread->pipe;

	if (pipe == NULL)
	{
		fprintf(stderr, "Invalid pipe!\n");
		return - 1;
	}

	if (fputc(character, pipe) == EOF)
	{
		fprintf(stderr, "Failed to write character to pipe!\n");
		return - 1;
	}
	else
	{
		printf("CHAR %c sent\n", character);
	}

	return 0;
}




