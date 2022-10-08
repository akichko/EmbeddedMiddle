#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "em_mqttc.h"
#include "em_print.h"

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
	em_mqttc_t mqc_pub;
	em_mqttc_t mqc_sub;
	em_mqtt_lib_init();

	em_printf(EM_LOG_INFO, "mqttc_pub create\n");
	if (0 != em_mqttc_create(&mqc_pub, "pub", "test.mosquitto.org", 1883, NULL, NULL, NULL, 60, &malloc, &free))
	{
		em_printf(EM_LOG_ERROR, "em_mqttc_create error\n");
	}

	em_printf(EM_LOG_INFO, "mqttc_sub create\n");
	if (0 != em_mqttc_create(&mqc_sub, "sub", "test.mosquitto.org", 1883, NULL, NULL, NULL, 60, &malloc, &free))
	{
		em_printf(EM_LOG_ERROR, "em_mqttc_create error\n");
	}

	em_printf(EM_LOG_INFO, "mqttc_subscribe_start\n");
	if (0 != em_mqttc_subscribe_start(&mqc_sub, "testtopic10", &subscribe_callback))
	{
		em_printf(EM_LOG_ERROR, "em_mqttc_subscribe_start error\n");
	}

	em_printf(EM_LOG_INFO, "publish\n");
	if (0 != em_mqttc_publish_txt(&mqc_pub, NULL, "testtopic10", "hello"))
	{
		em_printf(EM_LOG_ERROR, "em_mqttc_publish_txt error\n");
	}
	int mid = 100;
	if (0 != em_mqttc_publish_txt(&mqc_pub, &mid, "testtopic10", "hello2"))
	{
		em_printf(EM_LOG_ERROR, "em_mqttc_publish_txt error\n");
	}

	sleep(4);

	em_printf(EM_LOG_INFO, "mqttc_subscribe_stop\n");
	if (0 != em_mqttc_subscribe_stop(&mqc_sub))
	{
		em_printf(EM_LOG_ERROR, "em_mqttc_subscribe_stop error\n");
	}

	sleep(1);

	em_mqttc_destroy(&mqc_pub);
	em_mqttc_destroy(&mqc_sub);

	em_printf(EM_LOG_INFO, "mqttc test ended\n");
	return EXIT_SUCCESS;
}
