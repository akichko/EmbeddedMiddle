#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <curl/curl.h>
#include "em_httpc.h"

int main(void)
{
	em_httpc_t hc;
	em_httpres_t response;
	int ret;
	char *url;

	// GET test
	printf("** GET TEST ***********************************\n");
	// url = "http://192.168.3.4:8080/ConnectedService/RequestInfoServlet?aaa=123&bbb=ccc";
	url = "http://127.0.0.1:8080/ConnectedService/RequestInfoServlet?aaa=123&bbb=ccc";

	em_httpc_init(&hc, 1024 * 1024, &malloc, &free);
	em_httpc_setopt(&hc, CURLOPT_SSL_VERIFYPEER, 0);

	char *header_str = "Hoge: Fuga";
	em_httpc_add_header(&hc, header_str);

	em_httpc_reset_header(&hc);
	em_httpc_add_header(&hc, "Hoge2: Fuga2");
	em_httpc_add_header(&hc, "Hoge2: Fuga2-2"); // skip
	em_httpc_add_header(&hc, "Hoge3: Fuga3");

	printf("request to %s\n", url);

	ret = em_httpc_get(&hc, url, NULL, &response);
	if (ret == 0)
	{
		printf("%s\n", response.data);
	}

	// POST test
	printf("\n** POST TEST ***********************************\n");
	url = "http://127.0.0.1:8080/ConnectedService/RequestInfoServlet";
	char *post_str = "name=edo&age=20";

	printf("request to %s\n", url);

	ret = em_httpc_post(&hc, url, NULL, post_str, strlen(post_str), &response);
	if (ret == 0)
	{
		printf("%s\n", response.data);
	}

	// POST multi form test
	printf("\n** POST multiform TEST ***********************************\n");
	url = "http://127.0.0.1:8080/ConnectedService/RequestInfoServlet";

	em_httppart_t parts[3] = {
		{"part1", EM_HTTP_PART_TYPE_STRING, "body data", strlen("body data")},
		{"part2", EM_HTTP_PART_TYPE_STRING, "abcde", strlen("abcde")},
		{"part3", EM_HTTP_PART_TYPE_STRING, "name=edo&age=20", strlen("name=edo&age=20")}};

	printf("request to %s\n", url);

	ret = em_httpc_post_multipart(&hc, url, NULL, parts, 3, &response);
	if (ret == 0)
	{
		printf("%s\n", response.data);
	}

	// printf("StatusCode = %ld, body length = %ld\n", response.status_code, strlen(response.data));

	em_httpc_destroy(&hc);

	return EXIT_SUCCESS;
}
