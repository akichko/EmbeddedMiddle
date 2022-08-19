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
#include <stdlib.h>
#include <string.h>
#include "em_upload.h"
#include "em_print.h"

// upload data

int em_uldata_init(em_uldata_t *ud, int data_ver, uint data_type, uint buf_capacity,
				   void *(*alloc_func)(size_t), void (*free_func)(void *))
{
	strcpy(ud->name, "NoName.txt");
	ud->data_type = data_type;
	ud->data_ver = data_ver;
	ud->data_num = 0;
	em_buf_init(&ud->buf, buf_capacity, alloc_func, free_func);

	return 0;
}

int em_uldata_destroy(em_uldata_t *ud)
{
	em_buf_destroy(&ud->buf);
	return 0;
}

int em_uldata_append_buf(em_uldata_t *ud, void *data, int length)
{
	int remain = em_buf_get_remain_size(&ud->buf);
	if (remain < length)
	{
		em_printf(EM_LOG_ERROR, "buffer full\n");
		return -1;
	}
	em_buf_append(&ud->buf, data, length);

	return 0;
}

// upload

int em_upload_init(em_upload_t *ul, char *server_url,
				   int uldata_ver, uint uldata_type, uint uldata_buf_capacity, uint uldata_max_num,
				   void *(*alloc_func)(size_t), void (*free_func)(void *))
{
	ul->server_url = server_url;
	ul->uldata_ver = uldata_ver;
	ul->uldata_type = uldata_type;
	ul->uldata_buf_capacity = uldata_buf_capacity;
	ul->num_sendbuf = 0;
	ul->max_sendbuf = uldata_max_num;
	ul->sendbuf = (em_uldata_t **)alloc_func(sizeof(em_uldata_t *) * ul->max_sendbuf);
	ul->alloc_func = alloc_func;
	ul->free_func = free_func;

	em_httpc_init(&ul->hc, 16 * 1024, alloc_func, free_func);
	em_mpool_create(&ul->mp_uldata, sizeof(em_uldata_t), uldata_max_num, alloc_func, free_func);
	em_mpool_alloc_block(&ul->mp_uldata, (void **)&ul->uldata, EM_NO_WAIT);
	em_uldata_init(ul->uldata, ul->uldata_ver, ul->uldata_type, ul->uldata_buf_capacity, ul->alloc_func, ul->free_func);

	return 0;
}

int em_upload_destroy(em_upload_t *ul)
{
	ul->free_func(ul->sendbuf);
	em_uldata_destroy(ul->uldata);
	em_mpool_delete(&ul->mp_uldata);
	em_httpc_destroy(&ul->hc);
	return 0;
}

int em_upload_append_buf(em_upload_t *ul, void *data, int length)
{
	return em_uldata_append_buf(ul->uldata, data, length);
}

int em_upload_add_uldata(em_upload_t *ul)
{
	if (ul->num_sendbuf >= ul->max_sendbuf)
	{
		em_printf(EM_LOG_ERROR, "buf max %d/%d\n", ul->num_sendbuf, ul->max_sendbuf);
		return -1;
	}

	ul->sendbuf[ul->num_sendbuf] = ul->uldata;
	em_mpool_alloc_block(&ul->mp_uldata, (void **)&ul->uldata, EM_NO_WAIT);
	
	em_uldata_init(ul->uldata, ul->uldata_ver, ul->uldata_type, ul->uldata_buf_capacity, ul->alloc_func, ul->free_func);
	ul->num_sendbuf++;
	return 0;
}

int em_upload_one(em_upload_t *ul, em_uldata_t *uldata, em_httpres_t *response)
{
	em_httppart_t part;

	part.name = uldata->name;
	part.data_type = uldata->data_type;
	part.data = uldata->buf.data;
	part.data_length = uldata->buf.data_size;

	em_httpc_post_multipart(&ul->hc, ul->server_url, NULL, &part, 1, response);

	return 0;
}

int em_upload_multi(em_upload_t *ul, em_uldata_t **uldata, uint uldata_num, em_httpres_t *response)
{
	// fix binary
	// int data_num = em_queue_getnum(&ul->qu_uldata);

	em_httppart_t parts[uldata_num];

	for (int i = 0; i < uldata_num; i++)
	{
		parts[i].name = uldata[i]->name;
		parts[i].data_type = uldata[i]->data_type;
		parts[i].data = uldata[i]->buf.data;
		parts[i].data_length = uldata[i]->buf.data_size;
	}

	em_httpc_post_multipart(&ul->hc, ul->server_url, NULL, parts, uldata_num, response);

	return 0;
}

int em_upload_sendbuf(em_upload_t *ul, em_httpres_t *response)
{
	em_httppart_t parts[ul->num_sendbuf];

	for (int i = 0; i < ul->num_sendbuf; i++)
	{
		parts[i].name = ul->sendbuf[i]->name;
		parts[i].data_type = ul->sendbuf[i]->data_type;
		parts[i].data = ul->sendbuf[i]->buf.data;
		parts[i].data_length = ul->sendbuf[i]->buf.data_size;
	}

	int ret = em_httpc_post_multipart(&ul->hc, ul->server_url, NULL, parts, ul->num_sendbuf, response);
	if (ret != 0)
	{
		em_printf(EM_LOG_ERROR, "upload error\n");
		return -1;
	}

	for (int i = 0; i < ul->num_sendbuf; i++)
	{
		em_uldata_destroy(ul->sendbuf[i]);
		em_mpool_free_block(&ul->mp_uldata, ul->sendbuf[i]);
	}
	ul->num_sendbuf = 0;

	return 0;
}
