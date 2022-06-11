#ifndef __EM_PRINT_H__
#define __EM_PRINT_H__


#define EM_LOG_TOP 6
#define EM_LOG_ERROR 5
#define EM_LOG_WARNING 4
#define EM_LOG_INFO 3
#define EM_LOG_DEBUG 2
#define EM_LOG_TRACE 1

#define em_printf(type, fmt, ...) _em_printf(__FILE__, __FUNCTION__, __LINE__, type, fmt, ##__VA_ARGS__)

void _em_printf(const char *file, const char *function, int line, int type, const char *fmt, ...);

#endif //__EM_PRINT_H__
