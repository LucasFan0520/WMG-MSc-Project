/* F038.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

typedef struct {
    char *name;
    char *encoded;
} EncodedStr;

static EncodedStr *strs = NULL;
static int scount = 0;
static int scap = 0;

static char *dup_str(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) memcpy(d, s, len + 1);
    return d;
}

static int find_str(const char *name) {
    for (int i = 0; i < scount; i++)
        if (strcmp(strs[i].name, name) == 0) return i;
    return -1;
}

static int is_valid_encoded(const char *s) {
    size_t len = strlen(s);
    size_t i = 0;
    while (i < len) {
        if (!isdigit((unsigned char)s[i])) return 0;
        long count = 0;
        while (i < len && isdigit((unsigned char)s[i])) {
            count = count * 10 + (s[i] - '0');
            if (count > 1000000) return 0;
            i++;
        }
        if (i >= len) return 0;
        i++;
    }
    return 1;
}

static void print_underscore(const char *s) {
    for (size_t i = 0; s[i]; i++) {
        putchar(s[i] == ' ' ? '_' : s[i]);
    }
    putchar('\n');
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[64] = {0};
        int offset = 0;
        if (sscanf(line, "%63s%n", cmd, &offset) != 1) continue;

        if (strcmp(cmd, "STORE") == 0) {
            char name[256] = {0};
            int off2 = 0;
            if (sscanf(line + offset, "%255s%n", name, &off2) != 1) continue;
            const char *enc = line + offset + off2;
            while (*enc == ' ') enc++;
            if (!is_valid_encoded(enc)) continue;
            if (scount >= scap) {
                scap = scap == 0 ? 8 : scap * 2;
                strs = realloc(strs, sizeof(EncodedStr) * scap);
            }
            strs[scount].name = dup_str(name);
            strs[scount].encoded = dup_str(enc);
            scount++;
        } else if (strcmp(cmd, "DECODE") == 0) {
            char name[256] = {0};
            if (sscanf(line + offset, "%255s", name) != 1) continue;
            int idx = find_str(name);
            if (idx < 0) { printf("NOT_FOUND\n"); continue; }
            const char *enc = strs[idx].encoded;
            size_t elen = strlen(enc);
            size_t i = 0;
            while (i < elen) {
                long count = 0;
                while (i < elen && isdigit((unsigned char)enc[i])) {
                    count = count * 10 + (enc[i] - '0');
                    i++;
                }
                if (i < elen) {
                    for (long j = 0; j < count; j++) {
                        char c = enc[i] == ' ' ? '_' : enc[i];
                        putchar(c);
                    }
                    i++;
                }
            }
            putchar('\n');
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256] = {0};
            if (sscanf(line + offset, "%255s", name) != 1) continue;
            int idx = find_str(name);
            if (idx < 0) continue;
            free(strs[idx].name);
            free(strs[idx].encoded);
            for (int i = idx; i < scount - 1; i++) strs[i] = strs[i + 1];
            scount--;
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < scount; i++) {
                printf("%s\n", strs[i].name);
            }
        }
    }
    for (int i = 0; i < scount; i++) {
        free(strs[i].name);
        free(strs[i].encoded);
    }
    free(strs);
    return 0;
}