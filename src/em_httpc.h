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
#ifndef __EM_HTTPC_H__
#define __EM_HTTPC_H__

#include <curl/curl.h>
#include "em_buf.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define EM_HTTP_PART_TYPE_STRING 1
#define EM_HTTP_PART_TYPE_BINARY 2
#define EM_HTTP_PART_TYPE_FILE 3

typedef struct
{
	long status_code;
	uint data_size;
	char *data;
} em_httpres_t;

typedef struct
{
	struct curl_slist *header_slist;
} em_httphdr_t;

typedef struct
{
	char *name;
	int data_type;
	char *data;
	int data_length;
} em_httppart_t;

typedef struct
{
	CURL *curl;
	struct curl_slist *header_slist;
	em_buf_t buf;
	void (*free_func)(void *);
} em_httpc_t;

// http client

int em_httpc_init(em_httpc_t *hc,
				  uint buf_size,
				  void *(*alloc_func)(size_t),
				  void (*free_func)(void *));

int em_httpc_destroy(em_httpc_t *hc);

int em_httpc_add_header(em_httpc_t *hc,
						char *header_str);

int em_httpc_reset_header(em_httpc_t *hc);

int em_httpc_setopt(em_httpc_t *hc,
					int opt,
					int opt_value);

int em_httpc_get(em_httpc_t *hc,
				 char *url,
				 em_httphdr_t *headers,
				 em_httpres_t *response);

int em_httpc_post(em_httpc_t *hc,
				  char *url,
				  em_httphdr_t *headers,
				  void *postdata,
				  int postdata_size,
				  em_httpres_t *response);

int em_httpc_post_multipart(em_httpc_t *hc,
							char *url,
							em_httphdr_t *headers,
							em_httppart_t *parts,
							uint part_num,
							em_httpres_t *response);

// http header

int em_httphdr_init(em_httphdr_t *hh);

int em_httphdr_add_header(em_httphdr_t *hh,
						  char *header_str);

int em_httphdr_destroy(em_httphdr_t *hh);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif //__EM_HTTPC_H__
