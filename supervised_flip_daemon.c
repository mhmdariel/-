gcc -O2 -std=c11 -Wall -o supervised_flip_daemon supervised_flip_daemon.c
./supervised_flip_daemon /path/to/sandbox [--dry]
/*
 supervised_flip_daemon.c  (modified)

 Safe supervised watcher/worker for authorised bit-flips.

 CHANGES:
  - Requires sandbox/acknowledge.txt to contain EXACTLY the Arabic token:
        "الصلطانالله"
    (after trimming leading/trailing whitespace/newlines). If that token is
    missing or differs, the supervisor refuses to start and will not perform actions.
  - Re-checks acknowledgement before processing each approval file; if the
    acknowledgement is removed or altered later, that approval will be rejected.
  - All other behavior from the prior supervised daemon remains:
      - Watch the sandbox with inotify (IN_CREATE and IN_MOVED_TO)
      - Approvals are files named "<target>.approve" with first line "mask 0xFF" or "bits 0,3,7"
      - Optional second line "dry" to request a dry-run for that operation
      - Creates "<target>.bak" backup by renaming original -> .bak and writes result
      - Logs actions to stdout/stderr

 Safety: operator-controlled, sandbox-only, backup+dry-run, auditable.

 Build:
   gcc -O2 -std=c11 -Wall -o supervised_flip_daemon supervised_flip_daemon.c
*/

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <limits.h>
#include <dirent.h>
#include <ctype.h>

#define BUF_SZ 8192
#define EVENT_BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))

/* REQUIRED ACK TOKEN (UTF-8 sequence). Must match file contents exactly after trimming. */
static const char REQUIRED_ACK[] = "الصلطانالله";  /* exact Arabic token, no newline */

/* trim leading/trailing whitespace (space, tab, CR, LF, etc.) in-place */
/* returns pointer to trimmed string (within buf). The buffer is modified. */
static char * trim_inplace(char *buf) {
    if (!buf) return buf;
    char *s = buf;
    while (*s && isspace((unsigned char)*s)) ++s;
    char *end = s + strlen(s);
    while (end > s && isspace((unsigned char)*(end - 1))) --end;
    *end = '\0';
    /* if we advanced s, shift the trimmed content down to buf start for convenience */
    if (s != buf) memmove(buf, s, (end - s) + 1);
    return buf;
}

/* check acknowledgement file inside sandbox: sandbox/acknowledge.txt
   returns 1 if file exists and trimmed contents equal REQUIRED_ACK, else 0 */
static int check_acknowledgement(const char *sandbox_root) {
    char path[PATH_MAX];
    if (snprintf(path, sizeof(path), "%s/acknowledge.txt", sandbox_root) >= (int)sizeof(path)) {
        fprintf(stderr, "[ACK-CHECK] sandbox path too long\n");
        return 0;
    }

    FILE *f = fopen(path, "rb");
    if (!f) {
        fprintf(stderr, "[ACK-CHECK] acknowledgement file missing: %s\n", path);
        return 0;
    }

    /* read up to a safe number of bytes */
    char buf[1024];
    size_t n = fread(buf, 1, sizeof(buf)-1, f);
    fclose(f);
    if (n == 0) {
        fprintf(stderr, "[ACK-CHECK] acknowledgement file empty: %s\n", path);
        return 0;
    }
    buf[n] = '\0';

    char *trimmed = trim_inplace(buf);
    /* compare exact byte sequences */
    if (strcmp(trimmed, REQUIRED_ACK) == 0) {
        return 1;
    } else {
        fprintf(stderr, "[ACK-CHECK] acknowledgement invalid or altered. Expected exact token:\n    %s\n", REQUIRED_ACK);
        return 0;
    }
}

/* parse mask same as earlier tool */
static int parse_hex_mask(const char *s) {
    if (!s) return -1;
    char *end;
    unsigned long v = strtoul(s, &end, 0);
    if (end == s) return -1;
    return (int)(v & 0xFF);
}

/* parse bits list into mask byte; returns -1 on error */
static int parse_bitlist(const char *s) {
    if (!s) return -1;
    uint8_t mask = 0;
    const char *p = s;
    while (*p) {
        while (*p == ' ' || *p == '\t') ++p;
        if (!*p) break;
        char *end;
        long v = strtol(p, &end, 10);
        if (end == p || v < 0 || v > 7) return -1;
        mask |= (1u << (int)v);
        p = end;
        while (*p == ' ' || *p == '\t') ++p;
        if (*p == ',') ++p;
    }
    return (int)mask;
}

/* read approval file, produce mask and dry flag; return 0 on success */
static int read_approval(const char *approve_path, int *out_mask, int *out_dry, char *error, size_t errlen) {
    FILE *f = fopen(approve_path, "r");
    if (!f) {
        snprintf(error, errlen, "open approval file: %s", strerror(errno));
        return -1;
    }
    char line[256];
    if (!fgets(line, sizeof(line), f)) {
        snprintf(error, errlen, "empty approval file");
        fclose(f);
        return -1;
    }
    line[strcspn(line, "\r\n")] = 0;
    char mode[32], param[128];
    if (sscanf(line, "%31s %127[^\n\r]", mode, param) < 2) {
        snprintf(error, errlen, "approval file first line must be: <mode> <param>");
        fclose(f);
        return -1;
    }
    int mask = -1;
    if (strcmp(mode, "mask") == 0) {
        mask = parse_hex_mask(param);
        if (mask < 0) {
            snprintf(error, errlen, "invalid hex mask: %s", param);
            fclose(f);
            return -1;
        }
    } else if (strcmp(mode, "bits") == 0) {
        mask = parse_bitlist(param);
        if (mask < 0) {
            snprintf(error, errlen, "invalid bitlist: %s", param);
            fclose(f);
            return -1;
        }
    } else {
        snprintf(error, errlen, "unknown mode: %s", mode);
        fclose(f);
        return -1;
    }

    /* optional second line "dry" */
    *out_dry = 0;
    if (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "dry") == 0) *out_dry = 1;
    }
    fclose(f);
    *out_mask = mask & 0xFF;
    return 0;
}

/* ensure path is inside sandbox_root (no ../ escaping) */
static int path_in_sandbox(const char *sandbox_root, const char *path) {
    char real_root[PATH_MAX];
    char real_path[PATH_MAX];
    if (!realpath(sandbox_root, real_root)) return 0;
    if (!realpath(path, real_path)) return 0;
    size_t len = strlen(real_root);
    if (len == 0) return 0;
    if (strncmp(real_root, real_path, len) != 0) return 0;
    if (real_path[len] == '\0' || real_path[len] == '/') return 1;
    return 0;
}

/* flip operation: rename target->target.bak, read bak-> transform -> write target (unless dry) */
static int perform_flip(const char *sandbox_root, const char *target_relpath, int mask, int dry) {
    char target_path[PATH_MAX];
    char bak_path[PATH_MAX];

    if (snprintf(target_path, sizeof(target_path), "%s/%s", sandbox_root, target_relpath) >= (int)sizeof(target_path)) {
        fprintf(stderr, "[ERROR] target path too long: %s\n", target_relpath);
        return -1;
    }
    if (snprintf(bak_path, sizeof(bak_path), "%s/%s.bak", sandbox_root, target_relpath) >= (int)sizeof(bak_path)) {
        fprintf(stderr, "[ERROR] backup path too long: %s\n", target_relpath);
        return -1;
    }

    if (!path_in_sandbox(sandbox_root, target_path) || !path_in_sandbox(sandbox_root, bak_path)) {
        fprintf(stderr, "[ERROR] path escapes sandbox: %s\n", target_relpath);
        return -1;
    }

    struct stat st;
    if (stat(target_path, &st) != 0) {
        fprintf(stderr, "[ERROR] target stat failed %s: %s\n", target_path, strerror(errno));
        return -1;
    }
    if (!S_ISREG(st.st_mode)) {
        fprintf(stderr, "[ERROR] target not regular file: %s\n", target_path);
        return -1;
    }

    if (rename(target_path, bak_path) != 0) {
        fprintf(stderr, "[ERROR] failed to rename to backup %s -> %s: %s\n", target_path, bak_path, strerror(errno));
        return -1;
    }
    fprintf(stdout, "[INFO] created backup: %s\n", bak_path);

    int infd = open(bak_path, O_RDONLY);
    if (infd < 0) {
        fprintf(stderr, "[ERROR] open backup failed: %s\n", strerror(errno));
        rename(bak_path, target_path);
        return -1;
    }

    int outfd = -1;
    if (!dry) {
        outfd = open(target_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (outfd < 0) {
            fprintf(stderr, "[ERROR] create output failed: %s\n", strerror(errno));
            close(infd);
            rename(bak_path, target_path);
            return -1;
        }
    }

    unsigned char buf[BUF_SZ];
    ssize_t n;
    unsigned long long total=0, changed=0;

    while ((n = read(infd, buf, sizeof(buf))) > 0) {
        total += (unsigned long long)n;
        for (ssize_t i=0;i<n;++i) {
            unsigned char before = buf[i];
            unsigned char after  = before ^ (unsigned char)mask;
            if (before != after) ++changed;
            buf[i] = after;
        }
        if (!dry) {
            ssize_t towrite = n;
            unsigned char *p = buf;
            while (towrite > 0) {
                ssize_t w = write(outfd, p, towrite);
                if (w < 0) {
                    fprintf(stderr, "[ERROR] write failed: %s\n", strerror(errno));
                    free(NULL); /* no-op placeholder to keep function consistent */
                    if (outfd >= 0) close(outfd);
                    close(infd);
                    rename(bak_path, target_path);
                    return -1;
                }
                towrite -= w;
                p += w;
            }
        }
    }
    if (n < 0) {
        fprintf(stderr, "[ERROR] read failed: %s\n", strerror(errno));
        if (outfd >= 0) close(outfd);
        close(infd);
        rename(bak_path, target_path);
        return -1;
    }

    if (outfd >= 0) close(outfd);
    close(infd);

    if (dry) {
        if (rename(bak_path, target_path) != 0) {
            fprintf(stderr, "[WARN] failed to restore original after dry-run: %s\n", strerror(errno));
        }
        fprintf(stdout, "[DRY] target=%s total=%llu changed=%llu\n", target_relpath, total, changed);
    } else {
        fprintf(stdout, "[DONE] target=%s total=%llu changed=%llu backup=%s\n", target_relpath, total, changed, bak_path);
    }

    return 0;
}

/* helper to strip suffix ".approve" and return target relpath (malloc'd) */
static char * strip_approve_suffix(const char *name) {
    const char *suf = ".approve";
    size_t n = strlen(name);
    size_t suf_len = strlen(suf);
    if (n <= suf_len) return NULL;
    if (strcmp(name + n - suf_len, suf) != 0) return NULL;
    size_t out_len = n - suf_len;
    char *out = malloc(out_len + 1);
    if (!out) return NULL;
    memcpy(out, name, out_len);
    out[out_len] = 0;
    return out;
}

int main(int argc, char **argv) {
    if (argc < 2) { fprintf(stderr, "Usage: %s <sandbox_dir> [--dry]\n", argv[0]); return 1; }
    char sandbox[PATH_MAX];
    if (!realpath(argv[1], sandbox)) {
        fprintf(stderr, "Invalid sandbox path: %s\n", strerror(errno));
        return 1;
    }
    int global_dry = 0;
    for (int i=2;i<argc;++i) if (strcmp(argv[i],"--dry")==0) global_dry = 1;

    /* Check acknowledgement at startup and refuse to run if missing/incorrect */
    if (!check_acknowledgement(sandbox)) {
        fprintf(stderr, "[FATAL] acknowledgement missing/invalid in sandbox '%s'.\n"
                        "Place a file named 'acknowledge.txt' whose contents (trimmed) are exactly:\n    %s\n", sandbox, REQUIRED_ACK);
        return 2;
    }

    fprintf(stdout, "[INFO] supervised_flip_daemon starting, sandbox=%s global_dry=%d\n", sandbox, global_dry);

    int inotify_fd = inotify_init1(IN_NONBLOCK);
    if (inotify_fd < 0) { perror("inotify_init1"); return 2; }

    int wd = inotify_add_watch(inotify_fd, sandbox, IN_CREATE | IN_MOVED_TO);
    if (wd < 0) { perror("inotify_add_watch"); close(inotify_fd); return 2; }

    /* Also process any existing approval files present at startup */
    DIR *d = opendir(sandbox);
    if (d) {
        struct dirent *de;
        while ((de = readdir(d)) != NULL) {
            if (de->d_type != DT_REG) continue;
            if (strstr(de->d_name, ".approve")) {
                char *target_rel = strip_approve_suffix(de->d_name);
                if (!target_rel) continue;
                char approve_path[PATH_MAX];
                if (snprintf(approve_path, sizeof(approve_path), "%s/%s", sandbox, de->d_name) >= (int)sizeof(approve_path)) {
                    free(target_rel);
                    continue;
                }
                /* Re-check acknowledgement before acting on any approval */
                if (!check_acknowledgement(sandbox)) {
                    fprintf(stderr, "[WARN] acknowledgement invalid at startup; skipping approval: %s\n", approve_path);
                    free(target_rel);
                    continue;
                }
                int mask=0, dry=0;
                char err[256];
                if (read_approval(approve_path, &mask, &dry, err, sizeof(err))==0) {
                    int final_dry = global_dry || dry;
                    fprintf(stdout, "[INFO] processing approval (startup): %s -> mask=0x%02X dry=%d\n", target_rel, mask, final_dry);
                    perform_flip(sandbox, target_rel, mask, final_dry);
                } else {
                    fprintf(stderr, "[WARN] invalid approval %s: %s\n", approve_path, err);
                }
                free(target_rel);
            }
        }
        closedir(d);
    }

    char buf[EVENT_BUF_LEN];
    for (;;) {
        ssize_t len = read(inotify_fd, buf, sizeof(buf));
        if (len <= 0) {
            /* non-blocking: sleep briefly */
            usleep(200000); /* 200 ms */
            continue;
        }
        ssize_t i=0;
        while (i < len) {
            struct inotify_event *ev = (struct inotify_event *)(buf + i);
            if (ev->len > 0) {
                if (strstr(ev->name, ".approve")) {
                    char approve_path[PATH_MAX];
                    if (snprintf(approve_path, sizeof(approve_path), "%s/%s", sandbox, ev->name) >= (int)sizeof(approve_path)) {
                        goto next_event;
                    }
                    char *target_rel = strip_approve_suffix(ev->name);
                    if (!target_rel) {
                        fprintf(stderr, "[WARN] malformed approval filename: %s\n", ev->name);
                        goto next_event;
                    }
                    /* Re-check acknowledgement before every approved operation */
                    if (!check_acknowledgement(sandbox)) {
                        fprintf(stderr, "[WARN] acknowledgement missing/invalid; ignoring approval: %s\n", approve_path);
                        free(target_rel);
                        goto next_event;
                    }
                    int mask=0, dry=0;
                    char err[256];
                    if (read_approval(approve_path, &mask, &dry, err, sizeof(err)) != 0) {
                        fprintf(stderr, "[WARN] invalid approval %s: %s\n", approve_path, err);
                        free(target_rel);
                        goto next_event;
                    }
                    int final_dry = global_dry || dry;
                    fprintf(stdout, "[INFO] approval detected: target=%s mask=0x%02X dry=%d\n", target_rel, mask, final_dry);
                    if (perform_flip(sandbox, target_rel, mask, final_dry) != 0) {
                        fprintf(stderr, "[ERROR] perform_flip failed for %s\n", target_rel);
                    } else {
                        if (unlink(approve_path) != 0) {
                            fprintf(stderr, "[WARN] could not remove approval file %s: %s\n", approve_path, strerror(errno));
                        } else {
                            fprintf(stdout, "[INFO] approval file removed: %s\n", approve_path);
                        }
                    }
                    free(target_rel);
                }
            }
        next_event:
            i += sizeof(struct inotify_event) + ev->len;
        }
    }

    close(inotify_fd);
    return 0;
}
