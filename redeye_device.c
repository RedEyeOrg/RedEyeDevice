#include <server_watchdog.h>
#include <recorder.h>
#include <alerter.h>
#include <stdio.h>
#include <lidar.h>


int main(int argc, char *argv[]) {
	puts("---------------------------------------");
	puts("------------ Redeye Device ------------");
	puts("---------------------------------------");


	init_recorder();
	init_server_watchdog(stop_record, start_record);
	start_record();
	start_server_watchdog();
	start_alerter();
	lidar_start();

	while (1) {

	}
}
