#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <curl/curl.h>

#include <debug.h>
#include <config.h>
#include <server_watchdog.h>

pthread_t *watchdog_thread = NULL;
pthread_mutex_t watchdog_mutex = PTHREAD_MUTEX_INITIALIZER;

bool (*server_disconnect_cb)();
bool (*server_reconnect_cb)();

bool init_server_watchdog(bool (*disconnect_cb)(), bool(*reconnect_cb)()) {
  bool rc = false;

  pthread_mutex_lock(&watchdog_mutex);
  watchdog_thread = (pthread_t*)malloc(sizeof(pthread_t));

  if (watchdog_thread) {
    server_disconnect_cb = disconnect_cb;
    server_reconnect_cb = reconnect_cb;
    rc = true;
  }

ex:
  pthread_mutex_unlock(&watchdog_mutex);
  return rc;
}


static size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp)
{
   return size * nmemb;
}

static bool has_server_available() {
  CURL* context = NULL;
  bool rc = false;

  context = curl_easy_init();
  curl_easy_setopt(context, CURLOPT_NOPROGRESS, 1L);
  curl_easy_setopt(context, CURLOPT_CONNECTTIMEOUT, 1);
  curl_easy_setopt(context, CURLOPT_TIMEOUT, 1);
  curl_easy_setopt(context, CURLOPT_NOSIGNAL, 1L);
  curl_easy_setopt(context, CURLOPT_DNS_SERVERS, "168.126.63.1,168.126.63.2,8.8.8.8");
  curl_easy_setopt(context, CURLOPT_URL, "http://"SERVER_ADDRESS"/check");
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


static void* watchdog_thread_routine(void *param) {
  int disconnected = 0;
  bool status = false;

  while(1) {

    status = has_server_available();

    DEBUG_PRINT("%s : %d\n", "Connectivity Status", status);

    pthread_mutex_lock(&watchdog_mutex);
    if (!status) {
      if (disconnected == 0) {
        if (server_disconnect_cb != NULL) {
          server_disconnect_cb();
        }
      }
      disconnected = 1;
    } else if (disconnected == 1) {
      disconnected = 0;
      if (server_reconnect_cb != NULL) {
        server_reconnect_cb();
      }

    }
    pthread_mutex_unlock(&watchdog_mutex);

    sleep(1);
  }
}


bool start_server_watchdog() {
  bool rc = false;

  pthread_mutex_lock(&watchdog_mutex);

  if (watchdog_thread) {
    pthread_create(watchdog_thread, NULL, watchdog_thread_routine, NULL);
    rc = true;
  }

ex:
  pthread_mutex_unlock(&watchdog_mutex);
  return rc;
}
