#include "errorreport.h"
#include <stdio.h>
#include <stdlib.h>

/* All code C90 compliant: no // comments, no mixed declarations */

static int er_num_width(int n)
{
    int w;
    if (n <= 0) return 1;
    w = 0;
    while (n > 0) {
        w++;
        n /= 10;
    }
    return w;
}

static void er_print_gutter(FILE *out, int width_line, int shown_line)
{
    int width, i;
    width = er_num_width(width_line);
    if (shown_line > 0) {
        fprintf(out, "%*d | ", width, shown_line);
    } else {
        for (i = 0; i < width; i++) fputc(' ', out);
        fputs(" | ", out);
    }
}

static void er_print_caret_line(FILE *out, int width_line, int column, const char *line_text, int line_len)
{
    int vc, target, idx;
    er_print_gutter(out, width_line, 0);
    vc = 0;
    target = column > 0 ? column - 1 : 0;
    for (idx = 0; idx < line_len && vc < target; idx++) {
        if (line_text[idx] == '\t') {
            fputc('\t', out);
            vc += 8 - (vc % 8);
        } else {
            fputc(' ', out);
            vc++;
        }
    }
    fputc('^', out);
    fputc('\n', out);
}

/* Try to read a specific line (1-based) from a file. Returns 1 on success. */
static int er_fetch_line_from_file(const char *filename, int line_no, char **buf_out, int *len_out)
{
    FILE *f;
    int cur;
    int cap;
    int len;
    int ch;
    char *buf;
    if (!filename || line_no <= 0 || !buf_out || !len_out) return 0;
    f = fopen(filename, "rb");
    if (!f) return 0;
    cur = 1;
    while (cur < line_no) {
        ch = fgetc(f);
        if (ch == EOF) {
            fclose(f);
            return 0;
        }
        if (ch == '\n') cur++;
    }
    cap = 128;
    buf = (char *)malloc((size_t)cap);
    if (!buf) {
        fclose(f);
        return 0;
    }
    len = 0;
    while (1) {
        ch = fgetc(f);
        if (ch == EOF || ch == '\n') break;
        if (len + 1 >= cap) {
            int new_cap = cap * 2;
            char *nb = (char *)realloc(buf, (size_t)new_cap);
            if (!nb) {
                free(buf);
                fclose(f);
                return 0;
            }
            buf = nb;
            cap = new_cap;
        }
        buf[len++] = (char)ch;
    }
    *buf_out = buf;
    *len_out = len;
    fclose(f);
    return 1;
}

void error_report(const char *filename,
                  int line,
                  int column,
                  const char *msg,
                  const char *line_text,
                  int line_len)
{
    const char *fname;
    const char *shown_text;
    int shown_len;
    char *loaded;
    fname = filename ? filename : "<stdin>";
    fprintf(stderr, "%s:%d:%d: error: %s\n", fname, line, column, msg);

    shown_text = line_text;
    shown_len = line_len;
    loaded = 0;
    if ((!shown_text || shown_len <= 0) && filename && line > 0) {
        if (er_fetch_line_from_file(filename, line, &loaded, &shown_len)) {
            shown_text = loaded;
        }
    }

    if (shown_text && shown_len > 0) {
        er_print_gutter(stderr, line, line);
        fwrite(shown_text, 1, (size_t)shown_len, stderr);
        fputc('\n', stderr);
    }

    er_print_caret_line(stderr, line, column, shown_text ? shown_text : "", shown_text ? shown_len : 0);
    if (loaded) free(loaded);
}
