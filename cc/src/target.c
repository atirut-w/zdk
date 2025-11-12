#include "target.h"
#include <stdio.h>
#include <string.h>

/* List of all available targets */
static struct Target *all_targets[] = {
  &target_z80,
  NULL
};

struct Target *target_find(const char *name) {
  int i;
  if (!name) {
    return NULL;
  }
  
  for (i = 0; all_targets[i] != NULL; i++) {
    if (strcmp(all_targets[i]->name, name) == 0) {
      return all_targets[i];
    }
  }
  
  return NULL;
}

void target_list(void) {
  int i;
  printf("Available targets:\n");
  for (i = 0; all_targets[i] != NULL; i++) {
    printf("  %-10s - %s\n", all_targets[i]->name, all_targets[i]->description);
  }
}
