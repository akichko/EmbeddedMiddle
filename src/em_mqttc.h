/*============================================================================
MIT License

Copyright (c) 2022 akichko

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
============================================================================*/
#ifndef _EM_MQTTC_H_
#define _EM_MQTTC_H_

#include <mosquitto.h>
#include "em_cmndefs.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct
{
	char client_id[128];
	char host[256];
	int port;
	char *cafile;
	char *certfile;
	char *keyfile;
	int keepalive;
} em_mqttenv_t;

typedef struct
{
	int *mid;
	char *topic;
	char *payload;
	int payload_length;
} em_mqbuf_t;

typedef struct
{
	struct mosquitto *mosq;
	em_mqttenv_t env;
	em_mqbuf_t buf_publish;
	char *topic_subscribe;
	char connect_desire;
	void (*sub_callback)(em_mqbuf_t *);
	void (*free_func)(void *);
} em_mqttc_t;

void em_mqtt_lib_init();

void em_mqtt_lib_cleanup();


int em_mqttc_create(em_mqttc_t *mc,
					char *client_id,
					char *host,
					int port,
					char *cafile,
					char *certfile,
					char *keyfile,
					int keepalive,
					void *(*alloc_func)(size_t),
					void (*free_func)(void *));

int em_mqttc_destroy(em_mqttc_t *mc);

int em_mqttc_subscribe_start(em_mqttc_t *mc,
							 char *topic,
							 void (*callback)(em_mqbuf_t *));

int em_mqttc_subscribe_stop(em_mqttc_t *mc);

int em_mqttc_publish(em_mqttc_t *mc,
					 int *mid,
					 char *topic,
					 char *payload,
					 int payload_length);

int em_mqttc_publish_txt(em_mqttc_t *mc,
						 int *mid,
						 char *topic,
						 char *message);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif //_EM_MQTTC_H_
