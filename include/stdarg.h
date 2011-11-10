/**
 * PICNIX project
 * 
 * stdarg.h
 * 
 * ANSI macros for variable parameter list
 */

#ifndef __PICNIX_STDARG_H
#define __PICNIX_STDARG_H 1

typedef unsigned char * va_list;

#define va_start(list, last)    list = (unsigned char *)&last + sizeof(last)
#define va_arg(list, type)      *((type *)((list += sizeof(type)) - sizeof(type)))
#define va_end(list)		    list = ((va_list) 0)

#endif // __PICNIX_STDARG_H
