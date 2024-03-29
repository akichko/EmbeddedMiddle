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
#ifndef __EM_SEMAPHORE_H__
#define __EM_SEMAPHORE_H__

#include <semaphore.h>
#include "em_cmndefs.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct
{
	sem_t smf;
} em_sem_t;

int em_sem_init(em_sem_t *sem, int value);

int em_sem_destroy(em_sem_t *sem);

int em_sem_wait(em_sem_t *sem, int timeout_ms);

int em_sem_post(em_sem_t *sem);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif //__EM_SEMAPHORE_H__
