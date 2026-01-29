#include "driver_util.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>
#include <sys/types.h>

#include <sys/wait.h>

/* mkstemp is POSIX; declare for strict C90 builds. */
int mkstemp(char *tpl);
#include <unistd.h>

int zdk_is_path_sep(int c) { return c == '/' || c == '\\'; }

int zdk_file_exists(const char *path) {
  struct stat st;
  if (!path || !path[0])
    return 0;
  return stat(path, &st) == 0;
}

int zdk_dirname(const char *path, char *out, unsigned long outcap) {
  unsigned long n;
  unsigned long i;
  if (!path || !out || outcap == 0)
    return 0;
  n = (unsigned long)strlen(path);
  if (n == 0)
    return 0;
  i = n;
  while (i > 0 && !zdk_is_path_sep((unsigned char)path[i - 1]))
    i--;
  if (i == 0) {
    if (outcap < 2)
      return 0;
    out[0] = '.';
    out[1] = 0;
    return 1;
  }
  while (i > 1 && zdk_is_path_sep((unsigned char)path[i - 1]))
    i--;
  if (i + 1 > outcap)
    return 0;
  memcpy(out, path, i);
  out[i] = 0;
  return 1;
}

int zdk_basename(const char *path, char *out, unsigned long outcap) {
  const char *base;
  unsigned long n;
  if (!path || !out || outcap == 0)
    return 0;
  base = path + strlen(path);
  while (base > path && !zdk_is_path_sep((unsigned char)base[-1]))
    base--;
  n = (unsigned long)strlen(base);
  if (n + 1u > outcap)
    return 0;
  memcpy(out, base, n + 1u);
  return 1;
}

int zdk_path_join(const char *a, const char *b, char *out,
                  unsigned long outcap) {
  unsigned long na;
  unsigned long nb;
  int need_slash;
  if (!a || !b || !out || outcap == 0)
    return 0;
  na = (unsigned long)strlen(a);
  nb = (unsigned long)strlen(b);
  if (na == 0 || nb == 0)
    return 0;
  need_slash = !zdk_is_path_sep((unsigned char)a[na - 1]);
  if (na + (need_slash ? 1u : 0u) + nb + 1u > outcap)
    return 0;
  memcpy(out, a, na);
  if (need_slash) {
    out[na] = '/';
    memcpy(out + na + 1u, b, nb + 1u);
  } else {
    memcpy(out + na, b, nb + 1u);
  }
  return 1;
}

static int zdk_is_exe_like(const char *path) {
  (void)path;
  /* On unix, execve checks permissions; we only check existence here. */
  return zdk_file_exists(path);
}

static int zdk_has_slash(const char *s) {
  unsigned long i;
  unsigned long n;
  if (!s)
    return 0;
  n = (unsigned long)strlen(s);
  for (i = 0; i < n; i++) {
    if (zdk_is_path_sep((unsigned char)s[i]))
      return 1;
  }
  return 0;
}

int zdk_resolve_exe_path(const char *argv0, char *out, unsigned long outcap) {
  char tmp[4096];
  const char *path_env;
  const char *p;
  unsigned long argv0n;

  if (!argv0 || !out || outcap == 0)
    return 0;

  if (zdk_has_slash(argv0)) {
    if ((unsigned long)strlen(argv0) + 1u > outcap)
      return 0;
    strcpy(out, argv0);
    return 1;
  }

  path_env = getenv("PATH");
  if (!path_env)
    return 0;

  argv0n = (unsigned long)strlen(argv0);
  p = path_env;
  while (*p) {
    const char *seg = p;
    unsigned long seglen = 0;
    while (p[seglen] && p[seglen] != ':')
      seglen++;

    if (seglen == 0) {
      /* empty segment means current directory */
      if (argv0n + 1u < sizeof(tmp)) {
        strcpy(tmp, argv0);
        if (zdk_is_exe_like(tmp)) {
          if ((unsigned long)strlen(tmp) + 1u > outcap)
            return 0;
          strcpy(out, tmp);
          return 1;
        }
      }
    } else {
      if (seglen + 1u + argv0n + 1u < sizeof(tmp)) {
        memcpy(tmp, seg, seglen);
        tmp[seglen] = '/';
        memcpy(tmp + seglen + 1u, argv0, argv0n + 1u);
        if (zdk_is_exe_like(tmp)) {
          if ((unsigned long)strlen(tmp) + 1u > outcap)
            return 0;
          strcpy(out, tmp);
          return 1;
        }
      }
    }

    p += seglen;
    if (*p == ':')
      p++;
  }

  return 0;
}

char *zdk_strdup(const char *s) {
  unsigned long n;
  char *p;
  if (!s)
    return 0;
  n = (unsigned long)strlen(s);
  p = (char *)malloc(n + 1u);
  if (!p)
    return 0;
  memcpy(p, s, n + 1u);
  return p;
}

int zdk_tmpname_with_suffix(char *out, unsigned long outcap,
                            const char *suffix) {
  const char *td;
  char base[4096];
  int fd;
  unsigned long bn;
  unsigned long sn;

  if (!out || outcap == 0 || !suffix)
    return 0;

  td = getenv("TMPDIR");
  if (!td || !td[0])
    td = "/tmp";

  if (!zdk_path_join(td, "zdkXXXXXX", base, sizeof(base)))
    return 0;

  fd = mkstemp(base);
  if (fd < 0)
    return 0;
  close(fd);
  unlink(base);

  bn = (unsigned long)strlen(base);
  sn = (unsigned long)strlen(suffix);
  if (bn + sn + 1u > outcap)
    return 0;
  memcpy(out, base, bn);
  memcpy(out + bn, suffix, sn + 1u);
  return 1;
}

static int zdk_mkdir_one(const char *path) {
  if (mkdir(path, 0777) == 0)
    return 1;
  if (errno == EEXIST)
    return 1;
  return 0;
}

int zdk_mkdirs(const char *path) {
  char buf[4096];
  unsigned long n;
  unsigned long i;
  if (!path)
    return 0;
  n = (unsigned long)strlen(path);
  if (n + 1u > sizeof(buf))
    return 0;
  strcpy(buf, path);

  /* Strip trailing slashes */
  while (n > 1 && zdk_is_path_sep((unsigned char)buf[n - 1])) {
    buf[n - 1] = 0;
    n--;
  }

  for (i = 1; i < n; i++) {
    if (zdk_is_path_sep((unsigned char)buf[i])) {
      buf[i] = 0;
      if (!zdk_mkdir_one(buf))
        return 0;
      buf[i] = '/';
    }
  }
  return zdk_mkdir_one(buf);
}

int zdk_run_argv(char *const *argv, int verbose) {
  pid_t pid;
  int status;
  int i;

  if (!argv || !argv[0])
    return 127;

  if (verbose) {
    for (i = 0; argv[i]; i++) {
      if (i)
        fputc(' ', stderr);
      fputs(argv[i], stderr);
    }
    fputc('\n', stderr);
  }

  pid = fork();
  if (pid == 0) {
    execvp(argv[0], argv);
    _exit(127);
  }
  if (pid < 0)
    return 127;

  for (;;) {
    if (waitpid(pid, &status, 0) < 0)
      return 127;
    break;
  }
  if (WIFEXITED(status))
    return WEXITSTATUS(status);
  return 127;
}
