/**
 * PICNIX project
 * 
 * interrupt.h
 * 
 * Interrupt handler macros
 */

#ifndef __PICNIX_INTERRUPT_H
#define __PICNIX_INTERRUPT_H 1

#define __CREATE_INTERRUPT_VECTOR(int_level, name) \
    void __intv_ ## name(void) __interrupt(int_level) __naked { \
        __asm goto _ ## name __endasm; \
    }

#define DEF_INT_HIGH(name) \
    __CREATE_INTERRUPT_VECTOR(1, name) \
    void name(void) __naked __interrupt { \
  
#define DEF_INT_LOW(name) \
    __CREATE_INTERRUPT_VECTOR(2, name) \
    void name(void) __naked __interrupt {

#define END_DEF_INT \
    __asm retfie __endasm; \
}

#define DEF_INT_HANDLER(intrp, handler) \
    __asm btfsc intrp __endasm; \
    __asm goto  _ ## handler __endasm;

#define INT_HANDLER(handler)                void handler(void) __interrupt
#define INT_HANDLER_NAKED(handler)          void handler(void) __naked __interrupt

#endif // __PICNIX_INTERRUPT_H
