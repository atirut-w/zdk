#ifndef SECTION_H
#define SECTION_H

#include <stdint.h>
#include <stdio.h>

typedef struct {
  FILE *file;
  uint16_t pc;
} Section;

extern Section text;
extern Section data;
extern Section bss;

/**
 * Writes data to the section and advances the program counter.
 * 
 * If the section has no backing file (e.g., BSS sections), only the program
 * counter is advanced. If data is NULL, zeros are written to the file.
 * 
 * Returns an error if the section has no backing file and data is non-NULL.
 */
int section_write(Section *section, const void *data, size_t size);

#endif /* SECTION_H */
