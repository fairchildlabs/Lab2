#include <scootd.h>





int main(int argc, char **argv)
{
	unsigned int old_state = 0;
	scoot_device aScootDevice;
	
	time_t t;
		struct tm *tmp;
		char formatted_time[50];

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
				
				printf("SCOOTD:State Change old_state = %d new_state = %d @ %s\n", old_state, aScootDevice.pState->state, formatted_time);
				old_state = aScootDevice.pState->state;
				sleep(1);
			}
			
			usleep(100);
		}

	}


	return 0;
}


