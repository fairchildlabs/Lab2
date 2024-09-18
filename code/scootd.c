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


void * video0_run(void * pvScootDevice)
{
	char cmdbuf[512];
	
	sprintf(cmdbuf, "ffmpeg -f v4l2 -framerate 30 -video_size 640x480 -i /dev/video0 /var/www/html/video_13/640x480_%d.mp4", time(NULL));
	
	scootd_util_command_in_terminal(cmdbuf);

	return NULL;

}





void scootd_state_change(unsigned int old_state, scoot_device * pScootDevice)
{

	if(pScootDevice->pState->bits.video0)
	{
		if(pScootDevice->thread_handles[SCOOTD_THREAD_VIDEO_0])
		{
			SCOOTD_ASSERT(0);
		}
		else
		{
			pScootDevice->thread_handles[SCOOTD_THREAD_VIDEO_0] = scootd_util_create_thread(video0_run, pScootDevice);
		}
	}




}



int main(int argc, char **argv)
{
	unsigned int old_state = 0;
	scoot_device aScootDevice;
	
	time_t t;
	struct tm *tmp;
	char formatted_time[50];

	memset(&aScootDevice, 0, sizeof(scoot_device));

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

				scootd_state_change(old_state, &aScootDevice);





				old_state = aScootDevice.pState->state;
				sleep(1);
			}
			
			usleep(100);
		}

	}


	return 0;
}


