// F038.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DECODED 65536

typedef struct {
    char *name;
    char *encoded;
} Entry;

Entry *entries = NULL;
int ec = 0;

int find_entry(const char *name) {
    for (int i = 0; i < ec; i++) {
        if (strcmp(entries[i].name, name) == 0) return i;
    }
    return -1;
}

int decode_rle(const char *encoded, char *out, int out_size) {
    int oi = 0;
    const char *p = encoded;
    while (*p) {
        if (*p < '0' || *p > '9') return -1;
        long count = 0;
        while (*p >= '0' && *p <= '9') {
            count = count * 10 + (*p - '0');
            if (count > MAX_DECODED) return -1;
            p++;
        }
        if (*p == '\0') return -1;
        char ch = *p;
        p++;
        for (long i = 0; i < count; i++) {
            if (oi >= out_size - 1) return -1;
            out[oi++] = ch;
        }
    }
    out[oi] = '\0';
    return 0;
}

int main(void) {
    char line[16384];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char a1[256];
        char a2[8192];
        a1[0] = '\0';
        a2[0] = '\0';
        int n = sscanf(line, "%31s %255s %8191[^\n]", cmd, a1, a2);
        if (n < 1) continue;

        if (strcmp(cmd, "STORE") == 0 && n >= 3) {
            entries = realloc(entries, (ec + 1) * sizeof(Entry));
            if (!entries) return 1;
            entries[ec].name = strdup(a1);
            entries[ec].encoded = strdup(a2);
            ec++;
        } else if (strcmp(cmd, "DECODE") == 0 && n >= 2) {
            int ei = find_entry(a1);
            if (ei >= 0) {
                char *decoded = malloc(MAX_DECODED);
                if (!decoded) return 1;
                if (decode_rle(entries[ei].encoded, decoded, MAX_DECODED) == 0) {
                    for (char *p = decoded; *p; p++) {
                        putchar(*p == ' ' ? '_' : *p);
                    }
                    putchar('\n');
                }
                free(decoded);
            }
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            int ei = find_entry(a1);
            if (ei >= 0) {
                free(entries[ei].name);
                free(entries[ei].encoded);
                for (int k = ei; k < ec - 1; k++) {
                    entries[k] = entries[k + 1];
                }
                ec--;
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < ec; i++) {
                printf("%s\n", entries[i].name);
            }
        }
    }
    for (int i = 0; i < ec; i++) {
        free(entries[i].name);
        free(entries[i].encoded);
    }
    free(entries);
    return 0;
}