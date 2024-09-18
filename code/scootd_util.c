
#include <scootd.h>



//Used AI to generate example code I sued.
//https://copilot.microsoft.com/sl/iEEDiJEa6PQ


int scootd_util_open_shared_memory(char *strFileName, scoot_device *pScoot)
{

	// Create shared memory ob	ject
	pScoot->shm_fd = shm_open(strFileName, O_CREAT | O_RDWR, 0666);
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
	pScoot->pState = mmap(0, sizeof(scoot_state), PROT_READ | PROT_WRITE, MAP_SHARED, pScoot->shm_fd, 0);
	if (pScoot->pState == MAP_FAILED) 
	{
			perror("mmap");
			return 1;
	}

	return 0;


}




