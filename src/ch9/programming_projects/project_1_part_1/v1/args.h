#ifndef __ARGS_H__
#define __ARGS_H__

int valid_int_sizes(void);
int valid_args (int argc, const char * const * const argv);
int get_arg_vaddrs_filename (int argc, const char * const * const argv, \
                             const char ** const fname);
#endif