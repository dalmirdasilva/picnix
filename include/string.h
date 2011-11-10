/**
 * PICNIX project
 * 
 * string.h
 * 
 * Standard string functions
 * it is implemented in normal string.c library
 */

#ifndef __PICNIX_STRING_H
#define __PICNIX_STRING_H 1 

char *strcat(char *, char *);
char *strchr(char *, char);
int strcmp(char *, char *);
char *strcpy(char *, char *);
int strcspn(char *, char *);
int strlen(char *);
char *strlwr(char *);
char *strncat(char *, char *, size_t);
int strncmp(char *, char *, size_t);
char *strncpy(char *, char *, size_t);
char *strpbrk(char *, char *);
char *strrchr(char *, char);
int strspn(char *, char *);
char *strstr(char *, char *);
char *strtok(char *, char *);
char *strupr(char *);

#endif // __PICNIX_STRING_H
