#ifndef ERRORREPORT_H
#define ERRORREPORT_H

#include <stdio.h>

/* Unified error reporting for parser and semantic analyzer.
   Prints: filename:line:column: error: message
   Then the source line with gutter and a caret line under the column. */
void error_report(const char *filename,
                  int line,
                  int column,
                  const char *msg,
                  const char *line_text,
                  int line_len);

#endif /* ERRORREPORT_H */
