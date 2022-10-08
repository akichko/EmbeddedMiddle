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

// cserver's IP should be written in /etc/hosts

int main(void)
{
	em_upload_t ul;
	em_uldata_t uldata;
	em_uldata_t uldata3[3];
	em_uldata_t *uldata3ptr[3];
	em_httpres_t response;
	int ret;
	char *url;
	url = "http://cserver:8080/ConnectedService/Fileupload";

	em_upload_init(&ul, url, "SendBuf", 100, EM_HTTP_PART_TYPE_BINARY, 1024 * 1024, 10, &malloc, &free);

	// single post test
	printf("** single post test **************************************\n");
	em_uldata_init(&uldata, "OnePartTxt", 100, EM_HTTP_PART_TYPE_STRING, 1024 * 1024, &malloc, &free);

	for (int i = 1; i < 100; i++)
	{
		em_buf_append(&uldata.buf, "test", 4);
		em_buf_append(&uldata.buf, (char *)&i, 1);

		em_upload_append_buf(&ul, "test", 4); // sendbuf
	}
	em_upload_add_uldata(&ul); // sendbuf登録

	if (0 != em_upload_one(&ul, &uldata, &response))
	{
		printf("upload error\n");
	}
	else
	{
		printf("%s\n", response.data);
	}

	// multi post test
	printf("** multi post test **************************************\n");
	em_uldata_init(&uldata3[0], "ThreeParts", 100, EM_HTTP_PART_TYPE_BINARY, 1024 * 1024, &malloc, &free);

	for (int i = 0; i < 2000; i++)
	{
		em_buf_append(&uldata3[0].buf, (char *)&i, 1);
		em_upload_append_buf(&ul, "abcdefghijklmnopqrstuvwxyz\n", 27); // sendbuf
	}
	em_upload_add_uldata(&ul); // sendbuf登録

	uldata3ptr[0] = &uldata3[0];
	uldata3ptr[1] = &uldata3[0];
	uldata3ptr[2] = &uldata3[0];

	if (0 != em_upload_multi(&ul, uldata3ptr, 3, &response))
	{
		printf("upload error\n");
	}
	else
	{
		printf("%s\n", response.data);
	}

	// send buf test
	printf("** send buf test **************************************\n");

	if (0 != em_upload_sendbuf(&ul, &response))
	{
		printf("upload error\n");
	}
	else
	{
		printf("%s\n", response.data);
	}

	em_upload_destroy(&ul);

	return EXIT_SUCCESS;
}
