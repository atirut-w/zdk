#ifndef CODEGEN_H
#define CODEGEN_H

#include "parser.h"
#include <stdio.h>

void codegen_generate(ASTNode *ast, FILE *output);

#endif
