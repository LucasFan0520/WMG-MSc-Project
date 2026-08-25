// F038.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *encoded;
} RLE;

RLE *items = NULL;
int i_count = 0;

int find_name(const char *name) {
    for (int i = 0; i < i_count; i++) {
        if (strcmp(items[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int decode_rle(const char *encoded, char *out, int out_size) {
    int pos = 0;
    int oi = 0;
    int len = (int)strlen(encoded);
    while (pos < len) {
        if (encoded[pos] < '0' || encoded[pos] > '9') {
            return 0;
        }
        int count = 0;
        while (pos < len && encoded[pos] >= '0' && encoded[pos] <= '9') {
            count = count * 10 + (encoded[pos] - '0');
            if (count > 100000) {
                return 0;
            }
            pos++;
        }
        if (pos >= len) {
            return 0;
        }
        char ch = encoded[pos];
        pos++;
        for (int i = 0; i < count; i++) {
            if (oi >= out_size - 1) {
                return 0;
            }
            out[oi++] = ch;
        }
    }
    out[oi] = '\0';
    return 1;
}

int main() {
    char cmd[32];
    char name[1024];
    char encoded[102400];

    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "STORE") == 0) {
            scanf("%s %s", name, encoded);
            int idx = find_name(name);
            if (idx != -1) {
                free(items[idx].encoded);
                items[idx].encoded = strdup(encoded);
            } else {
                items = realloc(items, sizeof(RLE) * (i_count + 1));
                items[i_count].name = strdup(name);
                items[i_count].encoded = strdup(encoded);
                i_count++;
            }
        } else if (strcmp(cmd, "DECODE") == 0) {
            scanf("%s", name);
            int idx = find_name(name);
            if (idx != -1) {
                char *decoded = malloc(200000);
                if (decode_rle(items[idx].encoded, decoded, 200000)) {
                    for (int i = 0; decoded[i]; i++) {
                        if (decoded[i] == ' ') {
                            putchar('_');
                        } else {
                            putchar(decoded[i]);
                        }
                    }
                    putchar('\n');
                }
                free(decoded);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", name);
            int idx = find_name(name);
            if (idx != -1) {
                free(items[idx].name);
                free(items[idx].encoded);
                for (int i = idx; i < i_count - 1; i++) {
                    items[i] = items[i + 1];
                }
                i_count--;
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < i_count; i++) {
                printf("%s\n", items[i].name);
            }
        }
    }

    for (int i = 0; i < i_count; i++) {
        free(items[i].name);
        free(items[i].encoded);
    }
    free(items);

    return 0;
}