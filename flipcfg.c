/*
 * flipcfg.c
 *
 * Purpose:
 *   Read an input file, flip bits in every byte according to either:
 *     - an 8-bit XOR mask (mode "mask", param e.g. 0xFF or FF)
 *     - a comma-separated list of bit positions 0..7 (mode "bits", param e.g. 0,3,7)
 *   Create an automatic backup "<infile>.bak" (rename original -> .bak), then write
 *   the transformed bytes to the original filename (created anew). If --dry is set,
 *   do not write the output — only report statistics about how many bytes would change.
 *
 * Safety:
 *   - This tool is operator-invoked only (no autonomous searching).
 *   - It creates a .bak backup of the original file before writing.
 *   - Use on files you own and for lawful testing only.
 *
 * Build:
 *   gcc -O2 -std=c11 -Wall -Wextra -o flipcfg flipcfg.c
 *
 * Usage:
 *   ./flipcfg <infile> mask <hex_mask>
 *   ./flipcfg <infile> bits <bitlist>
 *   optional arg: --dry (place anywhere)
 *
 * Examples:
 *   ./flipcfg config.bin mask 0xFF
 *   ./flipcfg settings.bin bits 0,3,7 --dry
 */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

#define BUF_SZ 8192

static void usage(const char *prog) {
    fprintf(stderr,
        "Usage: %s <infile> <mode> <param> [--dry]\n"
        "Modes:\n"
        "  mask <hex_mask>   - XOR every byte with hex_mask (e.g. 0x0F or FF)\n"
        "  bits <bitlist>    - flip specified bit positions (0..7), e.g. 0,3,7\n"
        "Optional flag:\n"
        "  --dry             - dry run: do not write changes, only report\n", prog);
}

/* parse hex string into 0..255; returns -1 on parse error */
static int parse_hex_mask(const char *s) {
    if (!s) return -1;
    char *end;
    unsigned long v = strtoul(s, &end, 0); /* base 0 supports 0x prefix */
    if (end == s) return -1;
    return (int)(v & 0xFF);
}

/* parse bitlist "0,3,7" into mask byte; returns -1 on error */
static int parse_bitlist(const char *s) {
    if (!s) return -1;
    uint8_t mask = 0;
    const char *p = s;
    while (*p) {
        /* skip spaces */
        while (*p == ' ' || *p == '\t') ++p;
        if (!*p) break;
        char *end;
        long v = strtol(p, &end, 10);
        if (end == p || v < 0 || v > 7) return -1;
        mask |= (1u << (int)v);
        p = end;
        /* skip spaces */
        while (*p == ' ' || *p == '\t') ++p;
        if (*p == ',') ++p;
    }
    return (int)mask;
}

/* build backup name as "<infile>.bak" (caller must free) */
static char *make_backup_name(const char *infile) {
    size_t L = strlen(infile) + 4 + 1;
    char *bak = malloc(L);
    if (!bak) return NULL;
    strcpy(bak, infile);
    strcat(bak, ".bak");
    return bak;
}

int main(int argc, char **argv) {
    if (argc < 4) {
        usage(argv[0]);
        return 1;
    }

    const char *infile = argv[1];
    const char *mode   = argv[2];
    const char *param  = argv[3];
    int dry = 0;

    /* optional --dry can appear as argv[4] or argv[3] if param is last; detect anywhere */
    for (int i = 4; i < argc; ++i) {
        if (strcmp(argv[i], "--dry") == 0) { dry = 1; break; }
    }
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--dry") == 0) { dry = 1; break; }
    }

    fprintf(stderr, "WARNING: operate only on files you own. A backup will be created: <infile>.bak\n");

    int mask = 0;
    if (strcmp(mode, "mask") == 0) {
        mask = parse_hex_mask(param);
        if (mask < 0) {
            fprintf(stderr, "Invalid mask parameter: %s\n", param);
            return 2;
        }
    } else if (strcmp(mode, "bits") == 0) {
        mask = parse_bitlist(param);
        if (mask < 0) {
            fprintf(stderr, "Invalid bitlist parameter: %s\n", param);
            return 2;
        }
    } else {
        usage(argv[0]);
        return 2;
    }

    /* stat infile to ensure it exists and is a regular file */
    struct stat st;
    if (stat(infile, &st) != 0) {
        fprintf(stderr, "stat(%s) failed: %s\n", infile, strerror(errno));
        return 3;
    }
    if (!S_ISREG(st.st_mode)) {
        fprintf(stderr, "Error: %s is not a regular file\n", infile);
        return 3;
    }

    /* Create backup name, and rename infile -> backup */
    char *bakname = make_backup_name(infile);
    if (!bakname) {
        fprintf(stderr, "Out of memory\n");
        return 4;
    }

    if (rename(infile, bakname) != 0) {
        fprintf(stderr, "Failed to create backup (rename): %s -> %s : %s\n", infile, bakname, strerror(errno));
        free(bakname);
        return 5;
    }

    /* Open backup for reading, and create new output file with same name as original */
    int infd = open(bakname, O_RDONLY);
    if (infd < 0) {
        fprintf(stderr, "Failed to open backup %s: %s\n", bakname, strerror(errno));
        /* attempt to restore original name by renaming back */
        rename(bakname, infile);
        free(bakname);
        return 6;
    }

    int outfd = -1;
    if (!dry) {
        outfd = open(infile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (outfd < 0) {
            fprintf(stderr, "Failed to create output %s: %s\n", infile, strerror(errno));
            close(infd);
            /* attempt to restore original */
            rename(bakname, infile);
            free(bakname);
            return 7;
        }
    }

    unsigned char *buf = malloc(BUF_SZ);
    if (!buf) {
        fprintf(stderr, "Out of memory\n");
        close(infd);
        if (outfd >= 0) close(outfd);
        rename(bakname, infile);
        free(bakname);
        return 8;
    }

    ssize_t n;
    unsigned long long total = 0;
    unsigned long long changed = 0;

    while ((n = read(infd, buf, BUF_SZ)) > 0) {
        total += (unsigned long long)n;
        /* process */
        for (ssize_t i = 0; i < n; ++i) {
            unsigned char before = buf[i];
            unsigned char after  = before ^ (unsigned char)mask;
            if (before != after) ++changed;
            buf[i] = after;
        }

        if (!dry) {
            ssize_t wrote = 0;
            ssize_t towrite = n;
            unsigned char *p = buf;
            while (towrite > 0) {
                wrote = write(outfd, p, towrite);
                if (wrote < 0) {
                    fprintf(stderr, "Write error: %s\n", strerror(errno));
                    free(buf);
                    close(infd);
                    if (outfd >= 0) close(outfd);
                    /* attempt to restore original name */
                    rename(bakname, infile);
                    free(bakname);
                    return 9;
                }
                towrite -= wrote;
                p += wrote;
            }
        }
    }

    if (n < 0) {
        fprintf(stderr, "Read error from %s: %s\n", bakname, strerror(errno));
        free(buf);
        if (outfd >= 0) close(outfd);
        close(infd);
        rename(bakname, infile);
        free(bakname);
        return 10;
    }

    /* close files */
    if (outfd >= 0) close(outfd);
    close(infd);

    if (dry) {
        printf("DRY RUN: bytes scanned: %llu\n", total);
        printf("DRY RUN: bytes that would change: %llu\n", changed);
        /* since dry-run didn't create output, restore original name by renaming backup back */
        if (rename(bakname, infile) != 0) {
            fprintf(stderr, "Warning: failed to restore original file name from backup: %s\n", strerror(errno));
            /* not fatal; but user must fix manually */
        }
    } else {
        printf("Completed. Backup saved as: %s\n", bakname);
        printf("Bytes processed: %llu; Bytes changed: %llu\n", total, changed);
    }

    free(buf);
    free(bakname);
    return 0;
}
