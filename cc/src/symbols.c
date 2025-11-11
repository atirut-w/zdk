#include <stdlib.h>
#include <string.h>
#include "symbols.h"

void symbols_init(struct Symbols *s) {
    s->typedefs = 0;
}

void symbols_free(struct Symbols *s) {
    struct TypedefEntry *e = s->typedefs;
    while (e) {
        struct TypedefEntry *n = e->next;
        free(e->name);
        free(e);
        e = n;
    }
    s->typedefs = 0;
}

int symbols_add_typedef(struct Symbols *s, const char *name) {
    struct TypedefEntry *e;
    if (symbols_is_typedef(s, name)) {
        return 0; /* already there */
    }
    e = (struct TypedefEntry*)malloc(sizeof(struct TypedefEntry));
    if (!e) return 0;
    e->name = (char*)malloc(strlen(name)+1);
    if (!e->name) { free(e); return 0; }
    strcpy(e->name, name);
    e->next = s->typedefs;
    s->typedefs = e;
    return 1;
}

int symbols_is_typedef(const struct Symbols *s, const char *name) {
    struct TypedefEntry const *e = s->typedefs;
    while (e) {
        if (strcmp(e->name, name) == 0) return 1;
        e = e->next;
    }
    return 0;
}
