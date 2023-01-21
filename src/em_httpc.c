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

#include <curl/curl.h>
#include "em_httpc.h"
#include "em_print.h"

static size_t _em_buffer_writer(char *buffer, size_t size, size_t nitems, void *outstream)
{
	em_buf_t *buf = (em_buf_t *)outstream;
	int block = size * nitems;

	int ret = em_buf_append(buf, buffer, block);
	if (ret != 0)
	{
		return CURL_WRITEFUNC_PAUSE;
	}

	em_printf(EM_LOG_TRACE, "buffer = %p, size = %ld, nitems = %ld, outstream = %p\n",
			  buffer, size, nitems, outstream);

	buf->data[buf->data_size] = '\0';

	return block;
}

static int _em_curl_init(em_httpc_t *hc)
{

	hc->curl = curl_easy_init();
	hc->buf.data_size = 0;
	curl_easy_setopt(hc->curl, CURLOPT_WRITEDATA, &hc->buf);
	curl_easy_setopt(hc->curl, CURLOPT_WRITEFUNCTION, &_em_buffer_writer);
	curl_easy_setopt(hc->curl, CURLOPT_TIMEOUT, 10);
	return 0;
}

int em_httpc_init(em_httpc_t *hc, uint buf_size,
				  void *(*alloc_func)(size_t), void (*free_func)(void *))
{
	hc->header_slist = NULL;
	em_buf_init(&hc->buf, buf_size, alloc_func, free_func);
	hc->free_func = free_func;

	em_printf(EM_LOG_TRACE, "outstream addr = %p\n", &hc->buf);

	return 0;
}

int em_httpc_destroy(em_httpc_t *hc)
{
	curl_slist_free_all(hc->header_slist);
	em_buf_destroy(&hc->buf);
	return 0;
}

// common http header
int em_httpc_add_header(em_httpc_t *hc, char *header_str)
{
	// struct curl_slist *headers = NULL;
	// headers = curl_slist_append(headers, "Hoge: Fuga");
	hc->header_slist = curl_slist_append(hc->header_slist, header_str);
	curl_easy_setopt(hc->curl, CURLOPT_HTTPHEADER, hc->header_slist);
	return 0;
}

int em_httpc_reset_header(em_httpc_t *hc)
{
	curl_slist_free_all(hc->header_slist);
	hc->header_slist = NULL;
	// curl_easy_setopt(hc->curl, CURLOPT_HTTPHEADER, hc->header_slist);
	return 0;
}

int em_httpc_setopt(em_httpc_t *hc, int opt, int opt_value)
{
	switch (opt)
	{
	case CURLOPT_SSL_VERIFYPEER:
		curl_easy_setopt(hc->curl, CURLOPT_SSL_VERIFYPEER, opt_value);
		break;
	default:
		break;
	}
	return 0;
}

int em_httpc_get(em_httpc_t *hc, char *url, em_httphdr_t *headers, em_httpres_t *response)
{
	CURLcode curlRet;
	if (response == NULL)
	{
		em_printf(EM_LOG_ERROR, "param error\n");
		return -1;
	}

	_em_curl_init(hc);
	curl_easy_setopt(hc->curl, CURLOPT_URL, url);

	if (headers != NULL)
	{
		curl_easy_setopt(hc->curl, CURLOPT_HTTPHEADER, headers->header_slist);
	}
	else if (hc->header_slist != NULL)
	{
		curl_easy_setopt(hc->curl, CURLOPT_HTTPHEADER, hc->header_slist);
	}

	curlRet = curl_easy_perform(hc->curl);
	if (curlRet != CURLE_OK)
	{
		em_printf(EM_LOG_ERROR, "http error ret=%d\n", curlRet);
		return -1;
	}

	curl_easy_getinfo(hc->curl, CURLINFO_RESPONSE_CODE, &response->status_code);
	response->data_size = hc->buf.data_size;
	response->data = hc->buf.data;

	curl_easy_cleanup(hc->curl);
	return 0;
}

int em_httpc_post(em_httpc_t *hc, char *url, em_httphdr_t *headers,
				  void *postdata, int postdata_size, em_httpres_t *response)
{
	if (postdata == NULL || postdata_size <= 0)
	{
		return -1;
	}

	CURLcode curlRet;

	_em_curl_init(hc);
	curl_easy_setopt(hc->curl, CURLOPT_URL, url);
	curl_easy_setopt(hc->curl, CURLOPT_POST, 1);
	curl_easy_setopt(hc->curl, CURLOPT_POSTFIELDS, postdata);
	curl_easy_setopt(hc->curl, CURLOPT_POSTFIELDSIZE, postdata_size);

	if (headers != NULL)
	{
		curl_easy_setopt(hc->curl, CURLOPT_HTTPHEADER, headers->header_slist);
	}
	else if (hc->header_slist != NULL)
	{
		curl_easy_setopt(hc->curl, CURLOPT_HTTPHEADER, hc->header_slist);
	}

	curlRet = curl_easy_perform(hc->curl);
	if (curlRet != CURLE_OK)
	{
		em_printf(EM_LOG_ERROR, "http error ret=%d\n", curlRet);
		return -1;
	}

	curl_easy_getinfo(hc->curl, CURLINFO_RESPONSE_CODE, &response->status_code);
	response->data_size = hc->buf.data_size;
	response->data = hc->buf.data;

	curl_easy_cleanup(hc->curl);
	return 0;
}

int em_httpc_post_multipart(em_httpc_t *hc, char *url, em_httphdr_t *headers,
							em_httppart_t *parts, uint part_num, em_httpres_t *response)
{
	CURLcode curlRet;

	_em_curl_init(hc);
	curl_easy_setopt(hc->curl, CURLOPT_URL, url);

	// struct curl_slist *headerlist = NULL;
	/* initialize custom header list (stating that Expect: 100-continue is not
	   wanted */
	// headerlist = curl_slist_append(headerlist, "Expect:");

	/* Create the form */
	curl_mime *form = curl_mime_init(hc->curl);
	curl_mimepart *field = NULL;
	for (uint i = 0; i < part_num; i++)
	{
		switch (parts[i].data_type)
		{
		case EM_HTTP_PART_TYPE_STRING:
			field = curl_mime_addpart(form);
			/* Fill in the filename field */
			curl_mime_name(field, parts[i].name);
			curl_mime_type(field, "text/plain");
			curl_mime_data(field, parts[i].data, CURL_ZERO_TERMINATED);
			break;
		case EM_HTTP_PART_TYPE_BINARY:
			field = curl_mime_addpart(form);
			curl_mime_name(field, parts[i].name);
			curl_mime_type(field, "binary/octet-stream");
			curl_mime_data(field, parts[i].data, parts[i].data_length);
			break;
		case EM_HTTP_PART_TYPE_FILE:
			field = curl_mime_addpart(form);
			curl_mime_name(field, parts[i].name);
			curl_mime_filedata(field, parts[i].data);
			break;
		default:
			em_printf(EM_LOG_ERROR, "unknown type %d\n", parts[i].data_type);
			break;
		}
	}
	curl_easy_setopt(hc->curl, CURLOPT_MIMEPOST, form);

	/* Perform the request, res will get the return code */
	curlRet = curl_easy_perform(hc->curl);
	if (curlRet != CURLE_OK)
	{
		em_printf(EM_LOG_ERROR, "http error ret=%d\n", curlRet);
		return -1;
	}

	curl_easy_getinfo(hc->curl, CURLINFO_RESPONSE_CODE, &response->status_code);
	response->data_size = hc->buf.data_size;
	response->data = hc->buf.data;

	/* then cleanup the form */
	curl_mime_free(form);
	curl_easy_cleanup(hc->curl);
	return 0;
}

int em_httphdr_init(em_httphdr_t *hh)
{
	hh->header_slist = NULL;
	return 0;
}

int em_httphdr_add_header(em_httphdr_t *hh, char *header_str)
{
	hh->header_slist = curl_slist_append(hh->header_slist, header_str);
	return 0;
}

int em_httphdr_destroy(em_httphdr_t *hh)
{
	curl_slist_free_all(hh->header_slist);
	return 0;
}
