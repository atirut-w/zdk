#include "codegen_driver.h"
#include <stdlib.h>

CodeGenDriver *codegen_driver_new(Lexer *lexer) {
  CodeGenDriver *driver = malloc(sizeof(CodeGenDriver));
  if (!driver) {
    return NULL;
  }

  driver->lexer = lexer;

  return driver;
}

void codegen_driver_free(CodeGenDriver *driver) {
  if (driver) {
    free(driver);
  }
}
