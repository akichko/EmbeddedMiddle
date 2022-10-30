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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "em_mqttc.h"
#include "em_eventflag.h"
#include "em_print.h"

void em_mqtt_lib_init()
{
	mosquitto_lib_init();
}

void em_mqtt_lib_cleanup()
{
	mosquitto_lib_cleanup();
}

// Broker接続成功時callback
static void on_connect_sync(struct mosquitto *mosq, void *obj, int rc)
{
	em_mqttc_t *obj_mc = (em_mqttc_t *)obj;

	if(0 !=em_event_broadcast(&obj_mc->evt_connect)){
		em_printf(EM_LOG_ERROR, "error\n");
	}

	//if(0 !=em_sem_post(&obj_mc->sem_connect)){
	//	em_printf(EM_LOG_ERROR, "error\n");
	//}

	return;
}

// Broker切断時callback
static void on_disconnect(struct mosquitto *mosq, void *obj, int rc)
{
	// em_mqttc_t *obj_mc = (em_mqttc_t *)obj;
	em_printf(EM_LOG_DEBUG, "on_disconnect: rc = %d\n", rc);
}

// メッセージ送信callback
static void on_publish(struct mosquitto *mosq, void *obj, int mid)
{
	em_printf(EM_LOG_DEBUG, "published mid = %d\n", mid);
}

//メッセージ受信処理
static void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message)
{
	em_mqttc_t *obj_mc = (em_mqttc_t *)obj;

	em_mqbuf_t mqbuf = {0};
	if (message->mid > 0)
	{
		int mid_tmp = message->mid;
		mqbuf.mid = &mid_tmp;
	}
	mqbuf.topic = message->topic;
	mqbuf.payload = (char *)message->payload;
	mqbuf.payload_length = message->payloadlen;

	if (obj_mc->sub_callback != NULL)
	{
		obj_mc->sub_callback(&mqbuf);
	}
	else
	{
		em_printf(EM_LOG_WARNING, "no callback registered\n");
		if (message->payloadlen > 0)
		{
			printf("%s ", message->topic);
			fwrite(message->payload, 1, message->payloadlen, stdout);
			printf("\n");
		}
		else
		{
			printf("%s (null)\n", message->topic);
		}
		fflush(stdout);
	}
}

int em_mqttc_create(em_mqttc_t *mc,
					char *client_id, char *host, int port,
					char *cafile, char *certfile, char *keyfile, int keepalive,
					void *(*alloc_func)(size_t), void (*free_func)(void *))
{
	/* クライアント証明書とクライアント秘密鍵はどちらか一方を指定した場合は、他方の指定も必須 */
	if (((certfile == NULL) && (keyfile != NULL)) || ((certfile != NULL) && (keyfile == NULL)))
	{
		return -1;
	}
	memcpy(mc->env.client_id, client_id, strlen(client_id) + 1);
	memcpy(mc->env.host, host, strlen(host) + 1);
	mc->env.port = port;
	mc->env.cafile = cafile;
	mc->env.certfile = certfile;
	mc->env.keyfile = keyfile;
	mc->env.keepalive = keepalive;

	em_event_init(&mc->evt_connect);

	bool clean_session = true;
	mc->mosq = mosquitto_new(client_id, clean_session, mc);
	if (mc->mosq == NULL)
	{
		fprintf(stderr, "Cannot create mosquitto object\n");
		return (EXIT_FAILURE);
	}
	return 0;
}

int em_mqttc_destroy(em_mqttc_t *mc)
{
	mosquitto_destroy(mc->mosq);
	// mosquitto_lib_cleanup();
	return 0;
}

static int _em_mqttc_connect(em_mqttc_t *mc)
{
	int ret;
	if (mc->env.cafile != NULL)
	{
		ret = mosquitto_tls_set(mc->mosq, mc->env.cafile, NULL, mc->env.certfile, mc->env.keyfile, NULL);
		if (ret != MOSQ_ERR_SUCCESS)
		{
			printf("mosquitto_tls_set function is failed.\n");
		}
		mosquitto_tls_insecure_set(mc->mosq, true);
	}

	ret = mosquitto_connect_bind(mc->mosq, mc->env.host, mc->env.port, mc->env.keepalive, NULL);
	return ret;
}

int em_mqttc_connect(em_mqttc_t *mc)
{
	int ret;

	mosquitto_connect_callback_set(mc->mosq, on_connect_sync);
	mosquitto_disconnect_callback_set(mc->mosq, on_disconnect);
	mosquitto_publish_callback_set(mc->mosq, on_publish);

	if (0 != _em_mqttc_connect(mc))
	{
		fprintf(stderr, "failed to connect broker.\n");
		return -2;
	}

	ret = mosquitto_loop_start(mc->mosq);

	if (0 != em_event_wait(&mc->evt_connect, 10000)){
		em_printf(EM_LOG_WARNING, "on_connect func wait timeout\n");
		mosquitto_disconnect(mc->mosq);
	}

	return ret;
}

int em_mqttc_disconnect(em_mqttc_t *mc)
{
	int ret;
	ret = mosquitto_loop_stop(mc->mosq, TRUE);
	if (ret != 0)
	{
		em_printf(EM_LOG_ERROR, "mosquitto_loop_stop error. ret=%d\n", ret);
	}

	ret = mosquitto_disconnect(mc->mosq);
	if (ret != 0)
	{
		em_printf(EM_LOG_ERROR, "disconnect error. ret=%d\n", ret);
	}
	return ret;
}

int em_mqttc_subscribe(em_mqttc_t *mc, char *topic, void (*callback)(em_mqbuf_t *))
{
	int ret;
	// param check
	if (topic == NULL)
	{
		return -1;
	}
	mc->topic_subscribe = topic;
	mc->sub_callback = callback;
	mosquitto_message_callback_set(mc->mosq, on_message);

	ret = mosquitto_subscribe(mc->mosq, NULL, mc->topic_subscribe, 1);
	return ret;
}

int em_mqttc_unsubscribe(em_mqttc_t *mc, char *topic)
{
	int ret;
	// param check
	if (topic == NULL)
	{
		return -1;
	}
	mc->topic_subscribe = NULL;
	mc->sub_callback = NULL;
	
	ret = mosquitto_unsubscribe(mc->mosq, NULL, topic);
	return ret;
}

int em_mqttc_publish(em_mqttc_t *mc, int *mid, char *topic, char *payload, int payload_length)
{
	int ret;

	if ((topic == NULL) || (payload == NULL))
	{
		return -1;
	}

	ret = mosquitto_publish(mc->mosq, mid, topic, payload_length, payload, 1, true);

	return ret;
}

int em_mqttc_publish_txt(em_mqttc_t *mc, int *mid, char *topic, char *message)
{
	int ret = em_mqttc_publish(mc, mid, topic, message, strlen(message));
	return ret;
}
