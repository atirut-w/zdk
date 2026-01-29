#include "driver_util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ZDK_MAX_ARGS 4096
#define ZDK_MAX_INPUTS 256

static void zdk_die(const char *msg) {
  fprintf(stderr, "cc: %s\n", msg);
  exit(1);
}

static const char *zdk_getenv_nonempty(const char *k) {
  const char *v = getenv(k);
  if (!v || !v[0])
    return 0;
  return v;
}

static int zdk_starts_with(const char *s, const char *pfx) {
  unsigned long i;
  if (!s || !pfx)
    return 0;
  for (i = 0; pfx[i]; i++) {
    if (s[i] != pfx[i])
      return 0;
  }
  return 1;
}

static int zdk_split_Wcomma(const char *arg, const char *flag, char **outv,
                            int outcap) {
  /* arg like -Wl,a,b,c -> outv: a b c */
  const char *p;
  int n;
  if (!zdk_starts_with(arg, flag))
    return 0;
  p = arg + strlen(flag);
  if (*p == 0)
    return 0;
  n = 0;
  while (*p) {
    const char *seg = p;
    unsigned long seglen = 0;
    char *s;
    while (seg[seglen] && seg[seglen] != ',')
      seglen++;
    s = (char *)malloc(seglen + 1u);
    if (!s)
      zdk_die("out of memory");
    memcpy(s, seg, seglen);
    s[seglen] = 0;
    if (n + 1 >= outcap)
      zdk_die("too many -Wl/-Wa args");
    outv[n++] = s;
    if (seg[seglen] == ',')
      p = seg + seglen + 1u;
    else
      p = seg + seglen;
  }
  outv[n] = 0;
  return n;
}

static const char *zdk_basename_noext(const char *path, char *buf,
                                      unsigned long cap) {
  const char *base;
  const char *dot;
  unsigned long n;
  if (!path || !buf || cap == 0)
    return 0;
  base = path + strlen(path);
  while (base > path && !zdk_is_path_sep((unsigned char)base[-1]))
    base--;
  dot = base + strlen(base);
  while (dot > base && dot[-1] != '.')
    dot--;
  if (dot > base)
    n = (unsigned long)(dot - base - 1);
  else
    n = (unsigned long)strlen(base);
  if (n + 1u > cap)
    return 0;
  memcpy(buf, base, n);
  buf[n] = 0;
  return buf;
}

static int zdk_has_suffix(const char *s, const char *suf) {
  unsigned long ns;
  unsigned long nf;
  if (!s || !suf)
    return 0;
  ns = (unsigned long)strlen(s);
  nf = (unsigned long)strlen(suf);
  if (nf > ns)
    return 0;
  return memcmp(s + (ns - nf), suf, nf) == 0;
}

static int zdk_find_tool(const char *name, const char *env_key,
                         const char *driver_dir, const char *prefix,
                         const char *target, const char *tool_prefix,
                         const char *libexec_rel, char *out,
                         unsigned long outcap) {
  char buf[4096];
  const char *ev;

  ev = zdk_getenv_nonempty(env_key);
  if (ev) {
    if ((unsigned long)strlen(ev) + 1u > outcap)
      return 0;
    strcpy(out, ev);
    return 1;
  }

  /* prefer sibling tools in driver dir */
  if (driver_dir) {
    if (strcmp(name, "cc1") == 0) {
      if (zdk_path_join(driver_dir, "cc1", buf, sizeof(buf)) &&
          zdk_file_exists(buf)) {
        if ((unsigned long)strlen(buf) + 1u > outcap)
          return 0;
        strcpy(out, buf);
        return 1;
      }
    }
    if (zdk_path_join(driver_dir, name, buf, sizeof(buf)) &&
        zdk_file_exists(buf)) {
      if ((unsigned long)strlen(buf) + 1u > outcap)
        return 0;
      strcpy(out, buf);
      return 1;
    }
  }

  /* libexec cc1 */
  if (prefix && libexec_rel && strcmp(name, "cc1") == 0) {
    if (zdk_path_join(prefix, libexec_rel, buf, sizeof(buf)) &&
        zdk_file_exists(buf)) {
      if ((unsigned long)strlen(buf) + 1u > outcap)
        return 0;
      strcpy(out, buf);
      return 1;
    }
  }

  /* ZDK tools: prefix/bin/<name> */
  if (prefix) {
    if (zdk_path_join(prefix, "bin", buf, sizeof(buf)) &&
        zdk_path_join(buf, name, buf, sizeof(buf)) && zdk_file_exists(buf)) {
      if ((unsigned long)strlen(buf) + 1u > outcap)
        return 0;
      strcpy(out, buf);
      return 1;
    }
  }

  /* GNU tools: prefix/bin/<tool_prefix><name> */
  if (prefix && tool_prefix) {
    char tname[256];
    unsigned long n1;
    unsigned long n2;
    n1 = (unsigned long)strlen(tool_prefix);
    n2 = (unsigned long)strlen(name);
    if (n1 + n2 + 1u < sizeof(tname)) {
      memcpy(tname, tool_prefix, n1);
      memcpy(tname + n1, name, n2 + 1u);
      if (zdk_path_join(prefix, "bin", buf, sizeof(buf)) &&
          zdk_path_join(buf, tname, buf, sizeof(buf)) && zdk_file_exists(buf)) {
        if ((unsigned long)strlen(buf) + 1u > outcap)
          return 0;
        strcpy(out, buf);
        return 1;
      }
    }
  }

  /* target/bin/<tool_prefix><name> */
  if (prefix && target && tool_prefix) {
    char troot[4096];
    char tname[256];
    unsigned long n1;
    unsigned long n2;
    if (zdk_path_join(prefix, target, troot, sizeof(troot)) &&
        zdk_path_join(troot, "bin", troot, sizeof(troot))) {
      n1 = (unsigned long)strlen(tool_prefix);
      n2 = (unsigned long)strlen(name);
      if (n1 + n2 + 1u < sizeof(tname)) {
        memcpy(tname, tool_prefix, n1);
        memcpy(tname + n1, name, n2 + 1u);
        if (zdk_path_join(troot, tname, buf, sizeof(buf)) &&
            zdk_file_exists(buf)) {
          if ((unsigned long)strlen(buf) + 1u > outcap)
            return 0;
          strcpy(out, buf);
          return 1;
        }
      }
    }
  }

  /* fall back to PATH */
  if (tool_prefix && tool_prefix[0] && strcmp(name, "cc1") != 0) {
    char tname[256];
    unsigned long n1;
    unsigned long n2;
    n1 = (unsigned long)strlen(tool_prefix);
    n2 = (unsigned long)strlen(name);
    if (n1 + n2 + 1u < sizeof(tname)) {
      memcpy(tname, tool_prefix, n1);
      memcpy(tname + n1, name, n2 + 1u);
      if (n1 + n2 + 1u > outcap)
        return 0;
      strcpy(out, tname);
      return 1;
    }
  }
  if ((unsigned long)strlen(name) + 1u > outcap)
    return 0;
  strcpy(out, name);
  return 1;
}

int main(int argc, char **argv) {
  int i;
  int verbose = 0;
  int save_temps = 0;

  int opt_E = 0;
  int opt_S = 0;
  int opt_c = 0;

  const char *target = "z80-unknown-none-elf";
  char tool_prefix[128];

  const char *out_path = 0;
  const char *sysroot = 0;

  char driver_path[4096];
  char driver_dir[4096];
  char prefix[4096];
  char cc_name[256];
  char tmp[4096];

  char cc1_path[4096];
  char as_path[4096];
  char ld_path[4096];

  const char *inputs[ZDK_MAX_INPUTS];
  int n_inputs = 0;

  char *ld_inputs[ZDK_MAX_INPUTS];
  int n_ld_inputs = 0;

  char *ld_extra[ZDK_MAX_INPUTS];
  int n_ld_extra = 0;
  char *as_extra[ZDK_MAX_INPUTS];
  int n_as_extra = 0;

  char sysroot_default[4096];
  char sysroot_named[4096];
  char sysroot_lib[4096];

  strcpy(tool_prefix, "z80-unknown-none-elf-");

  for (i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "-v")) {
      verbose = 1;
      continue;
    }
    if (!strcmp(argv[i], "-save-temps")) {
      save_temps = 1;
      continue;
    }
    if (!strcmp(argv[i], "-E")) {
      opt_E = 1;
      continue;
    }
    if (!strcmp(argv[i], "-S")) {
      opt_S = 1;
      continue;
    }
    if (!strcmp(argv[i], "-c")) {
      opt_c = 1;
      continue;
    }
    if (!strcmp(argv[i], "-o")) {
      if (i + 1 >= argc)
        zdk_die("-o requires a value");
      out_path = argv[++i];
      continue;
    }
    if (zdk_starts_with(argv[i], "--target=")) {
      target = argv[i] + strlen("--target=");
      continue;
    }
    if (zdk_starts_with(argv[i], "--tool-prefix=")) {
      const char *p = argv[i] + strlen("--tool-prefix=");
      if (strlen(p) + 1u >= sizeof(tool_prefix))
        zdk_die("tool prefix too long");
      strcpy(tool_prefix, p);
      continue;
    }
    if (zdk_starts_with(argv[i], "--sysroot=")) {
      sysroot = argv[i] + strlen("--sysroot=");
      continue;
    }

    if (!strcmp(argv[i], "-Wl"))
      zdk_die("use -Wl,ARG[,ARG...] form");
    if (!strcmp(argv[i], "-Wa"))
      zdk_die("use -Wa,ARG[,ARG...] form");

    if (zdk_starts_with(argv[i], "-Wl,")) {
      char *parts[256];
      int n;
      n = zdk_split_Wcomma(argv[i], "-Wl,", parts, 256);
      if (n <= 0)
        continue;
      for (n = 0; parts[n]; n++) {
        if (n_ld_extra + 1 >= ZDK_MAX_INPUTS)
          zdk_die("too many linker args");
        ld_extra[n_ld_extra++] = parts[n];
      }
      continue;
    }
    if (zdk_starts_with(argv[i], "-Wa,")) {
      char *parts[256];
      int n;
      n = zdk_split_Wcomma(argv[i], "-Wa,", parts, 256);
      if (n <= 0)
        continue;
      for (n = 0; parts[n]; n++) {
        if (n_as_extra + 1 >= ZDK_MAX_INPUTS)
          zdk_die("too many assembler args");
        as_extra[n_as_extra++] = parts[n];
      }
      continue;
    }

    if (argv[i][0] != '-') {
      if (n_inputs >= ZDK_MAX_INPUTS)
        zdk_die("too many inputs");
      inputs[n_inputs++] = argv[i];
      continue;
    }

    /* Forward a few common linker flags directly. */
    if (argv[i][0] == '-' && (argv[i][1] == 'L' || argv[i][1] == 'l')) {
      if (n_ld_extra + 1 >= ZDK_MAX_INPUTS)
        zdk_die("too many linker args");
      ld_extra[n_ld_extra++] = zdk_strdup(argv[i]);
      continue;
    }

    /* ignore other flags for now */
  }

  if (n_inputs == 0)
    zdk_die("no input files");

  if (out_path && (opt_S || opt_c) && n_inputs != 1)
    zdk_die("-o with -S/-c requires exactly one input");

  if (opt_E)
    zdk_die("-E not implemented (no preprocessor yet)");

  if (!zdk_resolve_exe_path(argv[0], driver_path, sizeof(driver_path))) {
    if ((unsigned long)strlen(argv[0]) + 1u > sizeof(driver_path))
      zdk_die("driver path too long");
    strcpy(driver_path, argv[0]);
  }
  if (!zdk_dirname(driver_path, driver_dir, sizeof(driver_dir)))
    zdk_die("failed to determine driver dir");

  if (!zdk_basename(driver_path, cc_name, sizeof(cc_name)))
    zdk_die("failed to determine driver name");
  if (!zdk_dirname(driver_dir, prefix, sizeof(prefix)))
    zdk_die("failed to determine prefix");
  if (!zdk_dirname(prefix, prefix, sizeof(prefix)))
    zdk_die("failed to determine prefix");

  /* Sysroot default: PREFIX/<target>/<cc_name>/sysroot, then
   * PREFIX/<target>/sysroot */
  if (!sysroot) {
    if (!zdk_path_join(prefix, target, tmp, sizeof(tmp)))
      zdk_die("failed to build target prefix");

    if (zdk_path_join(tmp, cc_name, sysroot_named, sizeof(sysroot_named)) &&
        zdk_path_join(sysroot_named, "sysroot", sysroot_named,
                      sizeof(sysroot_named)) &&
        zdk_file_exists(sysroot_named)) {
      sysroot = sysroot_named;
    } else {
      if (!zdk_path_join(tmp, "sysroot", sysroot_default,
                         sizeof(sysroot_default)))
        zdk_die("failed to build default sysroot path");
      if (zdk_file_exists(sysroot_default))
        sysroot = sysroot_default;
    }
  }
  if (sysroot) {
    if (!zdk_path_join(sysroot, "usr/lib", sysroot_lib, sizeof(sysroot_lib)))
      zdk_die("failed to build sysroot lib path");
  }

  {
    char libexec_cc1[4096];
    char libexec_dir[4096];
    if (!zdk_path_join("libexec", cc_name, libexec_dir, sizeof(libexec_dir)) ||
        !zdk_path_join(libexec_dir, "cc1", libexec_cc1, sizeof(libexec_cc1)))
      zdk_die("failed to build libexec path");

    if (!zdk_find_tool("cc1", "ZDK_CC1", driver_dir, prefix, target,
                       tool_prefix, libexec_cc1, cc1_path, sizeof(cc1_path))) {
      if (!zdk_find_tool("cc1", "ZDK_CC1", driver_dir, prefix, target,
                         tool_prefix, "libexec/zdk/cc1", cc1_path,
                         sizeof(cc1_path)))
        zdk_die("failed to locate cc1");
    }
  }
  if (!zdk_find_tool("as", "ZDK_AS", driver_dir, prefix, target, tool_prefix, 0,
                     as_path, sizeof(as_path)))
    zdk_die("failed to locate assembler");
  if (!zdk_find_tool("ld", "ZDK_LD", driver_dir, prefix, target, tool_prefix, 0,
                     ld_path, sizeof(ld_path)))
    zdk_die("failed to locate linker");

  /* Compile each .c to .s or .o */
  for (i = 0; i < n_inputs; i++) {
    const char *in = inputs[i];
    char base[256];
    char s_path[4096];
    char o_path[4096];
    int st;

    if (zdk_has_suffix(in, ".c")) {
      if (!zdk_basename_noext(in, base, sizeof(base)))
        zdk_die("input name too long");

      if (opt_S) {
        if (out_path) {
          strcpy(s_path, out_path);
        } else {
          if ((unsigned long)strlen(base) + 3u > sizeof(s_path))
            zdk_die("path too long");
          sprintf(s_path, "%s.s", base);
        }
      } else {
        if (save_temps) {
          if ((unsigned long)strlen(base) + 3u > sizeof(s_path))
            zdk_die("path too long");
          sprintf(s_path, "%s.s", base);
        } else {
          if (!zdk_tmpname_with_suffix(s_path, sizeof(s_path), ".s"))
            zdk_die("failed to create temp name");
        }
      }

      {
        char *cc1_argv[16];
        int ai = 0;
        cc1_argv[ai++] = cc1_path;
        cc1_argv[ai++] = (char *)in;
        cc1_argv[ai++] = (char *)"-o";
        cc1_argv[ai++] = s_path;
        cc1_argv[ai] = 0;
        st = zdk_run_argv(cc1_argv, verbose);
        if (st != 0)
          return st;
      }

      if (opt_S) {
        continue;
      }

      /* assemble to .o */
      if (opt_c) {
        if (out_path) {
          strcpy(o_path, out_path);
        } else {
          if ((unsigned long)strlen(base) + 3u > sizeof(o_path))
            zdk_die("path too long");
          sprintf(o_path, "%s.o", base);
        }
      } else {
        if (save_temps) {
          if ((unsigned long)strlen(base) + 3u > sizeof(o_path))
            zdk_die("path too long");
          sprintf(o_path, "%s.o", base);
        } else {
          if (!zdk_tmpname_with_suffix(o_path, sizeof(o_path), ".o"))
            zdk_die("failed to create temp name");
        }
      }

      {
        char *as_argv[ZDK_MAX_ARGS];
        int ai = 0;
        int j;
        as_argv[ai++] = as_path;
        for (j = 0; j < n_as_extra; j++)
          as_argv[ai++] = as_extra[j];
        as_argv[ai++] = s_path;
        as_argv[ai++] = (char *)"-o";
        as_argv[ai++] = o_path;
        as_argv[ai] = 0;
        st = zdk_run_argv(as_argv, verbose);
        if (st != 0)
          return st;
      }

      if (!save_temps)
        remove(s_path);

      if (opt_c) {
        continue;
      }

      if (n_ld_inputs >= ZDK_MAX_INPUTS)
        zdk_die("too many link inputs");
      ld_inputs[n_ld_inputs++] = zdk_strdup(o_path);

      continue;
    }

    if (zdk_has_suffix(in, ".s") || zdk_has_suffix(in, ".S")) {
      /* assemble input .s */
      if (!zdk_basename_noext(in, base, sizeof(base)))
        zdk_die("input name too long");
      if (opt_c && out_path && n_inputs == 1) {
        strcpy(o_path, out_path);
      } else if (save_temps) {
        sprintf(o_path, "%s.o", base);
      } else {
        if (!zdk_tmpname_with_suffix(o_path, sizeof(o_path), ".o"))
          zdk_die("failed to create temp name");
      }
      {
        char *as_argv[ZDK_MAX_ARGS];
        int ai = 0;
        int j;
        as_argv[ai++] = as_path;
        for (j = 0; j < n_as_extra; j++)
          as_argv[ai++] = as_extra[j];
        as_argv[ai++] = (char *)in;
        as_argv[ai++] = (char *)"-o";
        as_argv[ai++] = o_path;
        as_argv[ai] = 0;
        st = zdk_run_argv(as_argv, verbose);
        if (st != 0)
          return st;
      }
      if (opt_c)
        continue;
      ld_inputs[n_ld_inputs++] = zdk_strdup(o_path);
      continue;
    }

    /* object/archive: pass to linker */
    if (n_ld_inputs >= ZDK_MAX_INPUTS)
      zdk_die("too many link inputs");
    ld_inputs[n_ld_inputs++] = (char *)in;
  }

  if (opt_c || opt_S)
    return 0;

  /* link */
  {
    char *ld_argv[ZDK_MAX_ARGS];
    int ai = 0;
    int j;
    ld_argv[ai++] = ld_path;
    if (sysroot) {
      char libarg[4096];
      unsigned long sl;
      sl = (unsigned long)strlen(sysroot_lib);
      if (sl + 3u > sizeof(libarg))
        zdk_die("sysroot lib path too long");
      strcpy(libarg, "-L");
      strcat(libarg, sysroot_lib);
      ld_argv[ai++] = libarg;
    }
    for (j = 0; j < n_ld_extra; j++)
      ld_argv[ai++] = ld_extra[j];
    for (j = 0; j < n_ld_inputs; j++)
      ld_argv[ai++] = (char *)ld_inputs[j];
    if (out_path) {
      ld_argv[ai++] = (char *)"-o";
      ld_argv[ai++] = (char *)out_path;
    }
    ld_argv[ai] = 0;
    return zdk_run_argv(ld_argv, verbose);
  }
}
