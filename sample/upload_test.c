#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <curl/curl.h>
#include "em_upload.h"

typedef struct
{
	char sdata[32];
	int idata1;
	int idata2;
	int idata3;
} ultest_t;

//cserver's IP should be written in /etc/hosts

int main(void)
{
	em_upload_t ul;
	em_uldata_t uldata;
	em_uldata_t uldatas[3];
	em_uldata_t *uldatasp[3];
	em_httpres_t response;
	int ret;
	char *url;
	url = "http://cserver:8080/ConnectedService/FileuploadServlet";

	em_uldata_init(&uldata, 100, EM_HTTP_PART_TYPE_STRING, 1024 * 1024, &malloc, &free);
	strcpy(uldata.name, "OnePart.txt");

	em_upload_init(&ul, url, 100, EM_HTTP_PART_TYPE_BINARY, 1024 * 1024, 10, &malloc, &free);

	for (int i = 1; i < 100; i++)
	{
		em_buf_append(&uldata.buf, "test", 4);
		em_buf_append(&uldata.buf, (void *)&i, 1);
		// em_upload_addbuf(&ul, ulrecord[i]);

		em_upload_append_buf(&ul, "test", 4);
		em_upload_append_buf(&ul, (void *)&i, 1);
	}
	em_upload_add_uldata(&ul);

	ret = em_upload_one(&ul, &uldata, &response);
	if (ret == 0)
	{
		printf("%s\n", response.data);
	}

	// multi post test

	em_uldata_init(&uldatas[0], 100, EM_HTTP_PART_TYPE_BINARY, 1024 * 1024, &malloc, &free);
	strcpy(uldatas[0].name, "ThreeParts.txt");

	for (int i = 0; i < 2000; i++)
	{
		em_buf_append(&uldatas[0].buf, (void *)&i, 1);
		em_upload_append_buf(&ul, (void *)&i, 1);
	}

	uldatasp[0] = &uldatas[0];
	uldatasp[1] = &uldatas[0];
	uldatasp[2] = &uldatas[0];

	ret = em_upload_multi(&ul, uldatasp, 3, &response);
	if (ret == 0)
	{
		printf("%s\n", response.data);
	}

	// send buf test

	em_upload_add_uldata(&ul);
	ret = em_upload_sendbuf(&ul, &response);
	if (ret == 0)
	{
		printf("%s\n", response.data);
	}

	em_upload_destroy(&ul);

	return EXIT_SUCCESS;
}
