#ifndef __RETCODE_H__
#define __RETCODE_H__

#define CL_VIRUS 1
#define CL_SUCCESS 0
#define CL_EMEM -1
#define CL_EARG -2
#define CL_ENULLARG -3

#define cli_dbgmsg(...) fprintf(stderr, __VA_ARGS__)
#define cli_errmsg(...) fprintf(stderr, __VA_ARGS__)

#endif
