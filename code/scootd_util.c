
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
	pThread->bRun = true;
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

//I found this here : https://stackoverflow.com/questions/548063/kill-a-process-started-with-popen

#define READ 0
#define WRITE 1


pid_t
popen2(const char *command, int *infp, int *outfp)
{
    int p_stdin[2], p_stdout[2];
    pid_t pid;

    if (pipe(p_stdin) != 0 || pipe(p_stdout) != 0)
        return -1;

    pid = fork();

    if (pid < 0)
        return pid;
    else if (pid == 0)
    {
        dup2(p_stdin[READ], STDIN_FILENO);
        dup2(p_stdout[WRITE], STDOUT_FILENO);

     //close unuse descriptors on child process.
     close(p_stdin[READ]);
     close(p_stdin[WRITE]);
     close(p_stdout[READ]);
     close(p_stdout[WRITE]);

        //can change to any exec* function family.
        execl("/bin/bash", "bash", "-c", command, NULL);
        perror("execl");
        exit(1);
    }

    // close unused descriptors on parent process.
    close(p_stdin[READ]);
    close(p_stdout[WRITE]);

    if (infp == NULL)
        close(p_stdin[WRITE]);
    else
        *infp = p_stdin[WRITE];

    if (outfp == NULL)
        close(p_stdout[READ]);
    else
        *outfp = p_stdout[READ];

    return pid;
}

int
pclose2(pid_t pid) {
    int internal_stat;
    waitpid(pid, &internal_stat, 0);
    return WEXITSTATUS(internal_stat);
}

#if 1
//AI: https://copilot.microsoft.com/sl/jS6aPunFSKa
int scootd_util_run_command(scootd_thread_config *pScootThread, const char * command)
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

	//int outfd, infd;
	int pid = popen2(command, &pThread->infd, &pThread->outfd);

	printf("POPEN PID = %d INFD = %d OUTFD %d \n", pid, pThread->infd, pThread->outfd);
	
	pThread->outpipe  = fdopen(pThread->outfd, "r");
	pThread->inpipe = fdopen(pThread->infd, "w");
	
	buffer = pThread->szBuffer;
	result = pThread->pOutBuffer;
	pipe = pThread->outpipe;
	
	if(!pipe)
	{
		fprintf(stderr, "popen() failed!\n");
		return -1;
	}

	while ((fgets(buffer, SCOOTD_THREAD_UTIL_BUFFER_SIZE, pipe) != NULL))
	{
		size_t			buffer_len = strlen(buffer);
		char *			new_result = realloc(result, result_size + buffer_len + 1);

		if (!new_result)
		{
			free(result);
			fprintf(stderr, "realloc() failed!\n");
			return -2;
		}

		result				= new_result;
		strcpy(result + result_size, buffer);
		result_size 		+= buffer_len;
		count++;

		if((count % 1000) == 0)
		{
		//	printf("while %d\n", count);
		}
	}

	printf("********************************\n%s\n", result);
	pThread->bDone = true;
	return 0;
}



#else
//AI: https://copilot.microsoft.com/sl/jS6aPunFSKa
int scootd_util_run_command(scootd_thread_config *pScootThread, const char * command)
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
		return -1;
	}

	while ((pThread->bRun == true) && (fgets(buffer, SCOOTD_THREAD_UTIL_BUFFER_SIZE, pipe) != NULL))
	{
		size_t			buffer_len = strlen(buffer);
		char *			new_result = realloc(result, result_size + buffer_len + 1);

		if (!new_result)
		{
			free(result);
			fprintf(stderr, "realloc() failed!\n");
			return -2;
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
	return 0;
}
#endif



void closeProcess (int pid)
{
    printf("killing %d\n", pid);
    kill(pid, SIGKILL);
    return;
}


int scootd_util_kill_thread(scoot_device *pScootDevice, scootd_threads	 *pThread)
{
	printf("scootd_util_kill_thread(%d) \n", pThread->idx);

	if(scootd_util_character_to_pipe(pThread, 'q'))
	{
		printf("ERROR - CHAR 'q' to pioe(%d)\n", pThread->idx);
		return -1;
	}
	else
	{
		printf("GOOD -  CHAR 'q' to piope(%d)\n", pThread->idx);
	}

	printf("Sending SIGTERM\n");
	kill(pThread->pid, SIGTERM);
	printf("Sending SIGTERM kill done\n");
	usleep(100);
	waitpid(pThread->pid, NULL, 0);
	usleep(100);
	printf("Done WAIT PID");
	

#if 0
	if(pThread->pid > 0)
	{
		printf("scootd_util_kill_thread(%d) SENDING KILL\n", pThread->pid);
		kill(pThread->pid, SIGTERM);
		printf("scootd_util_kill_thread(%d) KILL DONE\n", pThread->pid);
		waitpid(pThread->pid, NULL, 0);
		printf("scootd_util_kill_thread(%d) WAIDPID DONE\n", pThread->pid);
	
	}

	pThread->bRun = false;
	usleep(100);

	if(0)//if(pThread->pipe > 0)
	{
		printf("CLosing PIPE\n");
		pclose(pThread->pipe);
	}

	printf("Cancel Thread \n");
	
	pthread_cancel(pThread->thread_handle);
#endif

	
	printf("scootd_util_kill_thread(%p) AFTER SLEEP\n", pThread);

	return 0;
}



#if 0

int scootd_util_run_command_nonblocking(scootd_thread_config *pScootThread, const char * command)
{
	scoot_device *pScootDevice;
	FILE			*pipe;
	char			*buffer  ;
	char *			result = NULL; 
	size_t			result_size = 0;
	int idx = pScootThread->thread_index;
	int count = 0;
//	const int       usSelectTimeout = 1000;	
	const int       usSelectTimeout = 500000;	
	scootd_threads	 *pThread;
	pid_t pid;	

//	fork();

	pScootDevice = &pScootThread->pScootDevice[idx];

	pThread = &pScootDevice->threads[idx]; 

	printf("scootd_util_run_command_nonblocking() POPEN usSelectTimeout = %d (%s)\n", usSelectTimeout, command);

	pThread->pipe = popen2(command, "r");

	printf("scootd_util_run_command_nonblocking() POPEN POST = %d (%p)\n", usSelectTimeout, pThread->pipe);

	buffer = pThread->szBuffer;
	result = pThread->pOutBuffer;
	pipe = pThread->pipe;

	if (!pipe)
	{
		fprintf(stderr, "popen() failed!\n");
		return -1;
	}

//	int 			pipe_fd = fileno(pipe);

//	int flags = fcntl(pipe_fd, F_GETFL, 0);

//	fcntl(pipe_fd, F_SETFL, flags | O_NONBLOCK);

//	pid = fcntl(pipe_fd, F_GETOWN);

	pThread->pid = pid;

	printf("pThread(%p) PID = %d\n", pThread, pid);
	
	
	fd_set			read_fds;

	struct timeval timeout;

	while (true == pThread->bRun) 
	{
		FD_ZERO(&read_fds);
		FD_SET(pipe_fd, &read_fds);

		timeout.tv_sec		= 0;					// 1 second timeout
		timeout.tv_usec 	= usSelectTimeout;

		//printf("scootd_util_run_command_nonblocking() SELECT = %d \n", count);

		int 			ret = select(pipe_fd + 1, &read_fds, NULL, NULL, &timeout);

		//printf("scootd_util_run_command_nonblocking() SELECT = %d ret = %d\n", count, ret);

		count++;


		
		if (ret == -1)
		{
			fprintf(stderr, "select() failed!\n");
			free(result);
			pclose(pipe);
			return -2;
		}
		else if (ret == 0)
		{

			usleep(usSelectTimeout);
			printf("scootd_util_run_command_nonblocking() NO DATA usSelectTimeout = %d bRun = %d\n", usSelectTimeout, pThread->bRun);

			// Timeout, no data available
			continue;
		}
		else
		{
			printf("UNEXPECTED RETURN %d bRun =%d pid = %d\n", ret, pThread->bRun, pid);
		}

		if (FD_ISSET(pipe_fd, &read_fds))
		{
			if (fgets(buffer, SCOOTD_THREAD_UTIL_BUFFER_SIZE, pipe) == NULL)
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
				return -3;
			}

			result				= new_result;
			strcpy(result + result_size, buffer);
			result_size 		+= buffer_len;
		}

		usleep(usSelectTimeout);
		//printf("scootd_util_run_command_nonblocking() usSelectTimeout = %d\n", usSelectTimeout);

	}

	if(false == pThread->bRun)
	{
		printf("Non-blocking bRun == false\n");
	}

	//printf("CLOSING PIPE (%d)\n\n", count);
	
	//pclose(pipe);
	return 0;
}

#endif



int scootd_util_character_to_pipe(scootd_threads * pThread, char character)
{
	//int 			idx = pThread->idx;
	//scoot_device *	pScootDevice;
	//scootd_threads * pThread;

	//pScootDevice		= &pScootThread->pScootDevice[idx];
	//pThread 			= &pScootDevice->threads[idx];
	FILE *			pipe;

	

	pipe				= pThread->inpipe;

	printf("scootd_util_character_to_pipe(%p) char = %c\n", pipe, character);

	if (pipe == NULL)
	{
		printf("Invalid pipe!\n");
		return - 2;
	}

	if (fputc(character, pipe) == EOF)
	{
		printf("Failed to write character to pipe!\n");
		return - 1;
	}
	else
	{
		printf("CHAR %c sent\n", character);
	}

	printf("scootd_util_character_to_pipe(%p) RETURN char = %c\n", pipe, character);

	return 0;
}




