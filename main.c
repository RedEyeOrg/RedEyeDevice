#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#define READ 0
#define WRITE 1

#define RECORD_COMMAND "./ffmpeg -f v4l2 -r 25 -s 640x480 -i /dev/video0 %s.avi"

pid_t recording_pid;

pid_t parent_pid;

void sigquit_handler (int sig)
{
    pid_t self = getpid();
    if (parent_pid != self)
    {
        printf("recording done");
        _exit(0);
    }
}

void* recording_routine(void* param) {
	char *command = NULL;
	char *file_name = (char*)param;
	int rc = 0;

	if (!param) {
		fprintf(stderr, "Failed to get record file name\n");
		rc = -1;
		goto ex;
	}

	asprintf(&command, RECORD_COMMAND, file_name);

	if (!command) {
		rc = -1;
		goto ex;
	}
	
	printf("Command : %s\n", command);
	printf("file name : %s\n", file_name);
	recording_pid = fork();

	if (recording_pid == 0) {
		execlp("./ffmpeg", "-f v4l2 -r 25", "-s", "640x480", "-i", "/dev/video0", file_name, NULL); 
	}

ex:
	if (command) {
		free(command);
		command = NULL;
	}

	if (file_name) {
		free(file_name);
		file_name = NULL;
	}
	return (void*)rc;
}

pthread_t recording_thread;
pthread_mutex_t recording_mutex = PTHREAD_MUTEX_INITIALIZER;
int is_recording;

void start_recording(char *file_name) {
	char *f = strdup(file_name);

	if (!f) {
		fprintf(stderr, "Failed to copy fille name\n");
		return;
	}

	pthread_mutex_lock(&recording_mutex);
	if (!is_recording) {
		pthread_create(&recording_thread, NULL, recording_routine, f);
		is_recording = 1;
	}

	pthread_mutex_unlock(&recording_mutex);
}

void stop_recording() {
	pthread_mutex_lock(&recording_mutex);

	printf("is_recording : %d\n", is_recording);
	if (is_recording) {
		kill(recording_pid, 2);
		int status;
		wait(&status);
		is_recording = 0;
	}
	pthread_mutex_unlock(&recording_mutex);
}

int main() {
	signal(SIGKILL, sigquit_handler);
    	parent_pid = getpid();

	start_recording("test123.avi");
	sleep(360);
	stop_recording();	

}
