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
#ifndef EM_CMNDEFS_H
#define EM_CMNDEFS_H

#include <stdio.h>
#include <stdlib.h>
#include "em_print.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef unsigned char		uchar;
typedef unsigned int		uint;
typedef unsigned short int	ushort;
typedef unsigned long int	ulong;

#define TRUE 1
#define FALSE 0

#define EM_SUCCESS 0
#define EM_ERROR -1
#define EM_ERR_TIMEOUT -2
#define EM_ERR_PARAM -11
#define EM_ERR_SYS -51
#define EM_ERR_OTHER -99


#define EM_WAIT -1
#define EM_NO_TIMEOUT -1
#define EM_NO_WAIT 0


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // EM_CMNDEFS_H
