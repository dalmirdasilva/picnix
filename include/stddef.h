/**
 * PICNIX project
 * 
 * stddef.h
 * 
 * Standard definitions
 */

#ifndef __PICNIX_STDDEF_H
#define __PICNIX_STDDEF_H 1 

#define __expand_macro(x)           x
#define __concat(a,b)               a ## b

#ifndef NULL
    #define NULL                    0
#endif

#define TRUE                        1
#define FALSE                       0

#endif // __PICNIX_STDDEF_H
