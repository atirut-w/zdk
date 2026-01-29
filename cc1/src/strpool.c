#include "cc1/strpool.h"

#include <stdlib.h>
#include <string.h>

struct cc1_sp_node {
  char *s;
  unsigned long n;
  struct cc1_sp_node *next;
};

struct cc1_strpool {
  struct cc1_sp_node *head;
};

struct cc1_strpool *cc1_strpool_new(void) {
  struct cc1_strpool *p = (struct cc1_strpool *)calloc(1, sizeof(*p));
  return p;
}

void cc1_strpool_free(struct cc1_strpool *p) {
  struct cc1_sp_node *n;
  struct cc1_sp_node *nx;
  if (!p)
    return;
  n = p->head;
  while (n) {
    nx = n->next;
    free(n->s);
    free(n);
    n = nx;
  }
  free(p);
}

const char *cc1_strpool_intern(struct cc1_strpool *p, const char *s,
                               unsigned long n) {
  struct cc1_sp_node *it;
  char *c;
  for (it = p->head; it; it = it->next) {
    if (it->n == n && !memcmp(it->s, s, n) && it->s[n] == 0)
      return it->s;
  }
  c = (char *)malloc(n + 1);
  if (!c)
    return 0;
  memcpy(c, s, n);
  c[n] = 0;
  it = (struct cc1_sp_node *)calloc(1, sizeof(*it));
  it->s = c;
  it->n = n;
  it->next = p->head;
  p->head = it;
  return c;
}
