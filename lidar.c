#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <tof.h> 
#include <pthread.h>

#define TOO_FAR_THRESHOLD 819
#define DANGEROUS_THRESHOLD 10

pthread_t lidar_thread;

static void* lidar_routine(void* param) {
	double distance = 0;
	double temp = 0;
	FILE *fp = NULL;

	while (1) {
		temp = tofReadDistance() / 10.0;

		if (temp != distance) {
                	fp = fopen("/home/pi/lidar.txt", "w");
			
			if (fp) {
				if (temp >= TOO_FAR_THRESHOLD) {
					fprintf(fp, "Too Far");
				} else if (temp < DANGEROUS_THRESHOLD) {
					fprintf(fp, "Dangerous");
				} else {
					fprintf(fp, "%.2lfcm", temp);
				}
				fclose(fp);
			}

			fp = NULL;
			usleep(2500);
			distance = temp;
		}
	}
}

int lidar_start() {
	int i;
	int iDistance;
	int model, revision;
	i = tofInit(3, 0x29, 1);
	if (i != 1)
	{
		return -1; // problem - quit
	}
	printf("VL53L0X device successfully opened.\n");
	i = tofGetModel(&model, &revision);
	printf("Model ID - %d\n", model);
	printf("Revision ID - %d\n", revision);

	pthread_create(&lidar_thread, NULL, lidar_routine, NULL);
	return 0;
}
