#ifndef __ARGS_H__
#define __ARGS_H__

int get_arg_pi(const int argc, const char * const * const argv, int * const pi, const char * const arg_name);
int get_arg_bool(const int argc, const char * const * const argv, int * const b, const char * const arg_name);

#endif