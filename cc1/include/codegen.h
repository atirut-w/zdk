#ifndef CODEGEN_H
#define CODEGEN_H

#include "parser.h"
#include <stdio.h>

void codegen_generate(TranslationUnit *unit, FILE *output);

#endif
