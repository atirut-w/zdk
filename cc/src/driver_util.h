#ifndef ZDK_DRIVER_UTIL_H
#define ZDK_DRIVER_UTIL_H

/* Small POSIX-ish helpers, C90-friendly. */

int zdk_is_path_sep(int c);
int zdk_file_exists(const char *path);
int zdk_mkdirs(const char *path);

/* Writes the directory portion of path into out (no trailing slash unless
 * root). */
int zdk_dirname(const char *path, char *out, unsigned long outcap);

/* Writes the last path component into out. */
int zdk_basename(const char *path, char *out, unsigned long outcap);

/* Searches PATH for argv0 (if no '/') and resolves to an absolute path. */
int zdk_resolve_exe_path(const char *argv0, char *out, unsigned long outcap);

/* Join a and b with a single '/'. */
int zdk_path_join(const char *a, const char *b, char *out,
                  unsigned long outcap);

char *zdk_strdup(const char *s);

/* Creates a temporary filename with suffix (does not create the file). */
int zdk_tmpname_with_suffix(char *out, unsigned long outcap,
                            const char *suffix);

/* Run a subprocess, returning its exit code (or 127 on exec failure). */
int zdk_run_argv(char *const *argv, int verbose);

#endif
