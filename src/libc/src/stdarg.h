#ifndef __STDARG_H
#define __STDARG_H

typedef void *va_list;
#define va_start(list, last) (list = (void *)((char *)&last + sizeof(last)))
#define va_arg(list, type) (*(type *)((list += sizeof(type)) - sizeof(type)))
#define va_end(list) (list = 0)

#endif
