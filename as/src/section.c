#include "section.h"

Section text = {NULL, 0};
Section data = {NULL, 0};
Section bss = {NULL, 0};

int section_write(Section *section, const void *data, size_t size) {
  if (section->file) {
    if (data) {
      size_t written = fwrite(data, 1, size, section->file);

      if (written != size) {
        return -1;
      }
    } else {
      uint8_t zero = 0;
      size_t i;

      for (i = 0; i < size; i++) {
        if (fwrite(&zero, 1, 1, section->file) != 1) {
          return -1;
        }
      }
    }
  } else if (data) {
    return -1;
  }

  section->pc += size;
  return 0;
}
