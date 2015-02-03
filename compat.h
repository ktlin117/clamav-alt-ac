#ifndef __RETCODE_H__
#define __RETCODE_H__

#define CL_VIRUS 1
#define CL_SUCCESS 0
#define CL_EMEM -1
#define CL_EARG -2
#define CL_ENULLARG -3

#define cli_dbgmsg(...) fprintf(stderr, __VA_ARGS__)
#define cli_errmsg(...) fprintf(stderr, __VA_ARGS__)

#define cli_malloc(size) malloc(size)
#define cli_calloc(count, size) calloc(count, size)
#define cli_realloc(ptr, size) realloc(ptr, size)

#endif
