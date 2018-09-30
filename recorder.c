#include <pthread.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <debug.h>
#include <recorder.h>
#include <avconv_util.h>

// 녹화 스레드
pthread_mutex_t recording_mutex = PTHREAD_MUTEX_INITIALIZER;
bool record_status = false;

static void _recorder_signal_handler() {
	int status = 0;

	pthread_mutex_lock(&recording_mutex);
	if (waitpid(-1, &status, WNOHANG) != -1) {
		DEBUG_PRINT("%s\n", "Recording Forced Stopped");
		record_status = false;
  }
	pthread_mutex_unlock(&recording_mutex);

	start_record();
}

bool init_recorder() {
	//signal(SIGCHLD, _recorder_signal_handler);

	return true;
}

bool start_record() {
	bool rc = false;

	pthread_mutex_lock(&recording_mutex);

	if (!record_status) {
		DEBUG_PRINT("%s\n", "Start Record!");
		avconv_record();
		record_status = true;
	}

ex:
	pthread_mutex_unlock(&recording_mutex);
	return rc;
}

bool stop_record() {
	bool rc = false;

	pthread_mutex_lock(&recording_mutex);

	if (record_status) {
		DEBUG_PRINT("%s\n", "Stop Record!");
		avconv_stop();
		record_status = false;
	}
ex:
	pthread_mutex_unlock(&recording_mutex);
	return rc;
}

bool is_recording() {
	bool rc;

	pthread_mutex_lock(&recording_mutex);
	rc = record_status;
	pthread_mutex_unlock(&recording_mutex);

	return rc;
}
