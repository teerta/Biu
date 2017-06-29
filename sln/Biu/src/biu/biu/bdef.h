
#ifndef _BDEF_H_
#define _BDEF_H_


#ifdef _WINDOWS
#include <basetsd.h>
#else
#include <stdio.h>
#endif


#ifdef _WINDOWS
#ifndef __ssize_t_defined
#define ssize_t SSIZE_T
#endif
#endif // _WINDOWS


#endif // _BDEF_H_
