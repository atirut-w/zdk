#ifndef SYMBOLS_H
#define SYMBOLS_H

/* Simple typedef name table for distinguishing TYPE_NAME tokens. */

struct TypedefEntry {
    char *name;
    struct TypedefEntry *next;
};

struct Symbols {
    struct TypedefEntry *typedefs;
};

void symbols_init(struct Symbols *s);
void symbols_free(struct Symbols *s);
int symbols_add_typedef(struct Symbols *s, const char *name);
int symbols_is_typedef(const struct Symbols *s, const char *name);

#endif /* SYMBOLS_H */
