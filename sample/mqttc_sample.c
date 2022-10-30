#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "em_mqttc.h"
#include "em_print.h"

// char *broker_name = "test.mosquitto.org";
char *broker_name = "localhost";

void subscribe_callback(em_mqbuf_t *mqbuf)
{
	int mid;
	if (mqbuf->mid == NULL)
	{
		mid = -1;
	}
	else
	{
		mid = *mqbuf->mid;
	}
	printf("subscribe_callback: mid=%d, topic=%s, payload=%s\n",
		   mid, mqbuf->topic, mqbuf->payload);
}

int main(void)
{
	em_print_set_loglevel(EM_LOG_DEBUG);
	em_mqttc_t mqc_pub;
	em_mqttc_t mqc_sub;
	em_mqtt_lib_init();

	if (0 != em_mqttc_create(&mqc_pub, "pub", broker_name, 1883, NULL, NULL, NULL, 60, &malloc, &free))
	{
		em_printf(EM_LOG_ERROR, "em_mqttc_create error\n");
	}
	em_printf(EM_LOG_INFO, "mqttc_pub created\n");
	if (0 != em_mqttc_connect(&mqc_pub))
	{
		em_printf(EM_LOG_ERROR, "em_mqttc_connect error\n");
	}
	em_printf(EM_LOG_INFO, "mqttc_pub connected\n");

	if (0 != em_mqttc_create(&mqc_sub, "sub", broker_name, 1883, NULL, NULL, NULL, 60, &malloc, &free))
	{
		em_printf(EM_LOG_ERROR, "em_mqttc_create error\n");
	}
	em_printf(EM_LOG_INFO, "mqttc_sub created\n");
	if (0 != em_mqttc_connect(&mqc_sub))
	{
		em_printf(EM_LOG_ERROR, "em_mqttc_connect error\n");
	}
	em_printf(EM_LOG_INFO, "mqttc_sub connected\n");

	em_printf(EM_LOG_INFO, "mqttc_subscribe_start\n");
	if (0 != em_mqttc_subscribe(&mqc_sub, "testtopic10", &subscribe_callback))
	{
		em_printf(EM_LOG_ERROR, "em_mqttc_subscribe error\n");
	}

	sleep(2);

	int mid = 10;
	//pub:public
	em_printf(EM_LOG_INFO, "publish 1\n");
	if (0 != em_mqttc_publish_txt(&mqc_pub, &mid, "testtopic10", "hello1"))
	{
		em_printf(EM_LOG_ERROR, "em_mqttc_publish_once_txt error\n");
	}

	mid = 100;
	if (0 != em_mqttc_publish_txt(&mqc_pub, &mid, "testtopic10", "hello2"))
	{
		em_printf(EM_LOG_ERROR, "em_mqttc_publish_once_txt error\n");
	}

	//sub:public
	mid = 200;
	if (0 != em_mqttc_publish_txt(&mqc_sub, &mid, "testtopic10", "hello_from_sub"))
	{
		em_printf(EM_LOG_ERROR, "em_mqttc_publish_txt error\n");
	}

	sleep(3);

	//sub:
	em_printf(EM_LOG_INFO, "mqttc_unsubscribe\n");
	if (0 != em_mqttc_unsubscribe(&mqc_sub, "testtopic10"))
	{
		em_printf(EM_LOG_ERROR, "em_mqttc_subscribe error\n");
	}
	sleep(3);

	mid = 300;
	if (0 != em_mqttc_publish_txt(&mqc_pub, &mid, "testtopic10", "hello3"))
	{
		em_printf(EM_LOG_ERROR, "em_mqttc_publish_once_txt error\n");
	}

	sleep(3);

	em_printf(EM_LOG_INFO, "mqttc_sub_disconnect\n");
	if (0 != em_mqttc_disconnect(&mqc_sub))
	{
		em_printf(EM_LOG_ERROR, "em_mqttc_subscribe_stop error\n");
	}
	em_printf(EM_LOG_INFO, "mqttc_pub_disconnect\n");
	if (0 != em_mqttc_disconnect(&mqc_pub))
	{
		em_printf(EM_LOG_ERROR, "em_mqttc_subscribe_stop error\n");
	}

	sleep(1);

	em_mqttc_destroy(&mqc_pub);
	em_mqttc_destroy(&mqc_sub);

	em_printf(EM_LOG_INFO, "mqttc test ended\n");
	return EXIT_SUCCESS;
}
