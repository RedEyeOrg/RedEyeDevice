#include <pthread.h>
#include <wiringPi.h>
#include <curl/curl.h>
#include <signal.h>
#include <stdlib.h>
#include <config.h>
#include <alerter.h>

pthread_mutex_t alert_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_t *alert_thread = NULL;

static size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp)
{
   return size * nmemb;
}

static bool send_alert() {
  CURL* context = NULL;
  bool rc = false;

  context = curl_easy_init();
  curl_easy_setopt(context, CURLOPT_NOPROGRESS, 1L);
  curl_easy_setopt(context, CURLOPT_CONNECTTIMEOUT, 1);
  curl_easy_setopt(context, CURLOPT_TIMEOUT, 1);
  curl_easy_setopt(context, CURLOPT_NOSIGNAL, 1L);
  curl_easy_setopt(context, CURLOPT_DNS_SERVERS, "168.126.63.1,168.126.63.2,8.8.8.8");
  curl_easy_setopt(context, CURLOPT_URL, "http://"SERVER_ADDRESS"/record");
  curl_easy_setopt(context, CURLOPT_WRITEFUNCTION, write_data);

  if (curl_easy_perform(context) == CURLE_OK) {
    rc = true;
  }

ex:
  if (context) {
    curl_easy_cleanup(context);
  }
  return rc;
}

static void on_button_clicked() {
	send_alert();
}

static void *alerter_routine(void *param) {
	bool status = NULL;

	wiringPiISR(26, INT_EDGE_FALLING, on_button_clicked);
}

bool start_alerter() {
	bool rc = false;


	pthread_mutex_lock(&alert_mutex);
	if (!alert_thread) {
		wiringPiSetup();
		alert_thread = (pthread_t*)malloc(sizeof(alert_thread));
		if (alert_thread) {
			pthread_create(alert_thread, NULL, alerter_routine, NULL);
			rc = true;
		}
	}
	pthread_mutex_unlock(&alert_mutex);

	return rc;
}

bool stop_alerter() {
	bool rc = false;
	pthread_mutex_lock(&alert_mutex);
	pthread_kill(*alert_thread, 9);
	pthread_mutex_unlock(&alert_mutex);

	return rc;
}
