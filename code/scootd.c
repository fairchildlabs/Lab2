#include <scootd.h>


void * dummy_thread( void * pvScootDevice)
{
	int count = 0;

	while(1)
	{	
		printf("dummy_thread(%d)\n", count);
		count++;
		sleep(1);
	}

	return NULL;

}

//#define CMD_NONBLOCKING
void * video0_run(void * pvScootdThreads)
{
	char cmdbuf[512];
	scootd_thread_config *pScootThread = pvScootdThreads;


	sprintf(cmdbuf, "ffmpeg -f v4l2 -framerate 30 -video_size 640x480 -i /dev/video0 /var/www/html/video_13/00%10d_640x480.mp4", time(NULL));

#ifdef CMD_NONBLOCKING
	scootd_util_run_command_nonblocking(pScootThread, cmdbuf);
#else
	scootd_util_run_command(pScootThread, cmdbuf);
#endif
	return NULL;

}





void scootd_state_change(unsigned int old_state, scootd_thread_config *pScootdThreads)
{
	scoot_device *pScootDevice = pScootdThreads->pScootDevice;
	scoot_state  *pOldState = (scoot_state  *)&old_state;
	scootd_threads	 *pThread;
	

	printf("scootd_state_change = 0x%08x  old_state = 0x%08x\n", pScootDevice->pState->state, pOldState->state);		

	if(pOldState->bits.video0)
	{
		pThread = &pScootDevice->threads[SCOOTD_THREAD_VIDEO_0]; 
		
		if(pThread->thread_handle)
		{
			scootd_util_kill_thread(pScootDevice, pThread);
		}
	}
	
	if(pScootDevice->pState->bits.video0)
	{

		pThread = &pScootDevice->threads[SCOOTD_THREAD_VIDEO_0]; 


		pThread->thread_handle = scootd_util_create_thread(video0_run, &pScootdThreads[SCOOTD_THREAD_VIDEO_0] );
	}
	


}



int main(int argc, char **argv)
{
	unsigned int old_state = 0;
	scoot_device aScootDevice;
	int i;
	time_t t;
	struct tm *tmp;
	char formatted_time[50];
	scootd_thread_config scdThreadConfig[SCOOTD_MAX_THREADS];

	memset(&aScootDevice, 0, sizeof(scoot_device));
	for(i = 0; i < SCOOTD_MAX_THREADS; i++)
	{
		scdThreadConfig[i].pScootDevice = &aScootDevice;
		scdThreadConfig[i].thread_index = 0;

		aScootDevice.threads[i].idx = i;
		aScootDevice.threads[i].pvScootDevice = &aScootDevice;

		
	}


	printf("scootd - Lab1\n");


	if(	scootd_util_open_shared_memory("scootd_shared.mem", &aScootDevice))
	{
		printf("Error opening shared memory\n");
	}
	else
	{
		while(1)
		{
			if(old_state != aScootDevice.pState->state)
			{
				usleep(10);
				 time(&t);
    			tmp = localtime(&t);
				strftime(formatted_time, sizeof(formatted_time), "%a %b %d %H:%M:%S %Y", tmp);
				
				printf("SCOOTD:State Change old_state = %d new_state = %d @ %s video0 = %d video1 = %d\n", old_state, aScootDevice.pState->state, formatted_time, aScootDevice.pState->bits.video0, aScootDevice.pState->bits.video1);

				scootd_state_change(old_state, &scdThreadConfig[0]);

				printf("SCOOTD: State Return\n");	



				old_state = aScootDevice.pState->state;
				sleep(1);
			}
			else
			{
			//	printf("SCOOTD: NO Change\n");
			}
			
			usleep(100);
		}

	}


	return 0;
}


