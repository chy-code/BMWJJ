#ifndef _DBGPRINT_H
#define _DBGPRINT_H

#ifdef USE_DBG_PRINT
void DbgPrint(const char *format, ...);
#else
#define DbgPrint(...);
#endif

#endif
