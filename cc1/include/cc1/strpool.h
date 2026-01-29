#ifndef CC1_STRPOOL_H
#define CC1_STRPOOL_H

struct cc1_strpool;

struct cc1_strpool *cc1_strpool_new(void);
void cc1_strpool_free(struct cc1_strpool *p);

const char *cc1_strpool_intern(struct cc1_strpool *p, const char *s,
                               unsigned long n);

#endif
