#ifndef __EM_PRINT_H__
#define __EM_PRINT_H__


#define EM_PRINTF(type, fmt, ...) em_printf(__FILE__, __FUNCTION__, __LINE__, type, fmt, ##__VA_ARGS__)

void em_printf(const char *file, const char *function, int line, int type, const char *fmt, ...);

#endif //__EM_PRINT_H__
