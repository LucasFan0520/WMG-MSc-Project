// F037.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    char *name;
    unsigned char *bytes;
    int size;
} Buffer;

Buffer *bufs = NULL;
int bcount = 0;

int find_buf(const char *name) {
    for (int i = 0; i < bcount; i++) {
        if (strcmp(bufs[i].name, name) == 0) return i;
    }
    return -1;
}

int parse_hex(const char *hex, unsigned char **out) {
    int len = strlen(hex);
    if (len % 2 != 0) return 0;
    *out = malloc(len / 2);
    for (int i = 0; i < len; i += 2) {
        if (!isxdigit(hex[i]) || !isxdigit(hex[i+1])) {
            free(*out);
            return 0;
        }
        unsigned int val;
        sscanf(hex + i, "%2x", &val);
        (*out)[i/2] = val;
    }
    return len / 2;
}

int main() {
    char cmd[20];
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "NEW") == 0) {
            char name[100], hex[2000];
            scanf("%s %s", name, hex);
            unsigned char *bytes;
            int size = parse_hex(hex, &bytes);
            if (size > 0) {
                if (find_buf(name) == -1) {
                    bufs = realloc(bufs, sizeof(Buffer) * (bcount + 1));
                    bufs[bcount].name = strdup(name);
                    bufs[bcount].bytes = bytes;
                    bufs[bcount].size = size;
                    bcount++;
                } else {
                    free(bytes);
                }
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            char name[100], hex[2000];
            scanf("%s %s", name, hex);
            unsigned char *bytes;
            int size = parse_hex(hex, &bytes);
            int idx = find_buf(name);
            if (idx != -1 && size > 0) {
                bufs[idx].bytes = realloc(bufs[idx].bytes, bufs[idx].size + size);
                memcpy(bufs[idx].bytes + bufs[idx].size, bytes, size);
                bufs[idx].size += size;
            }
            if (bytes) free(bytes);
        } else if (strcmp(cmd, "SLICE") == 0) {
            char name[100];
            int offset, length;
            scanf("%s %d %d", name, &offset, &length);
            int idx = find_buf(name);
            if (idx != -1 && offset >= 0 && length >= 0 && offset + length <= bufs[idx].size) {
                for (int i = 0; i < length; i++) {
                    printf("%02x", bufs[idx].bytes[offset + i]);
                }
                printf("\n");
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[100];
            scanf("%s", name);
            int idx = find_buf(name);
            if (idx != -1) {
                free(bufs[idx].name);
                free(bufs[idx].bytes);
                for (int i = idx; i < bcount - 1; i++) bufs[i] = bufs[i+1];
                bcount--;
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < bcount; i++) {
                printf("%s %d\n", bufs[i].name, bufs[i].size);
            }
        }
    }
    for (int i = 0; i < bcount; i++) {
        free(bufs[i].name);
        free(bufs[i].bytes);
    }
    free(bufs);
    return 0;
}