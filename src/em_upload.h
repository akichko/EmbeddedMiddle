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
#ifndef __EM_UPLOAD_H__
#define __EM_UPLOAD_H__

#include "em_mempool.h"
#include "em_httpc.h"
#include "em_ringbuf.h"

typedef struct
{
	char data_name[32];
	int data_type;
	int data_ver;
	int data_num;
	em_buf_t buf; //データの区切りはデータ層で対応
} em_uldata_t;	  // fix size

typedef struct
{
	char *server_url;
	char *uldata_name;
	int uldata_ver;
	uint uldata_type;
	uint uldata_buf_capacity;
	em_uldata_t *uldata;
	em_ring_t rb_uldata;
	int num_sendbuf;
	int max_sendbuf;
	em_uldata_t **sendbuf;
	em_httpc_t hc;
	void *(*alloc_func)(size_t);
	void (*free_func)(void *);
} em_upload_t;

// uldata

int em_uldata_init(em_uldata_t *ud,
				   char *data_name,
				   int data_ver,
				   uint data_type,
				   uint buf_capacity,
				   void *(*alloc_func)(size_t),
				   void (*free_func)(void *));

int em_uldata_destroy(em_uldata_t *ud);

int em_uldata_append_buf(em_uldata_t *ud,
						 void *data,
						 int length);

// upload

int em_upload_init(em_upload_t *ul,
				   char *server_url,
				   char *uldata_name,
				   int uldata_ver,
				   uint uldata_type,
				   uint uldata_buf_capacity,
				   uint uldata_sendbuf_num,
				   void *(*alloc_func)(size_t),
				   void (*free_func)(void *));

int em_upload_destroy(em_upload_t *ul);

int em_upload_append_buf(em_upload_t *ul,
						 void *data,
						 int length);

int em_upload_add_uldata(em_upload_t *ul);

int em_upload_one(em_upload_t *ul,
				  em_uldata_t *uldata,
				  em_httpres_t *response);

int em_upload_multi(em_upload_t *ul,
					em_uldata_t **uldata,
					uint uldata_num,
					em_httpres_t *response);

int em_upload_sendbuf(em_upload_t *ul,
					  em_httpres_t *response);

#endif //__EM_UPLOAD_H__
