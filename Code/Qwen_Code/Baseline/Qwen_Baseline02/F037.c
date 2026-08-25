// F037.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name; unsigned char *bytes; int len; } Buf;
Buf bufs[1000]; int nbufs = 0;

int find_buf(char *name) {
    for (int i = 0; i < nbufs; i++) if (!strcmp(bufs[i].name, name)) return i;
    return -1;
}

int hex_val(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

int parse_hex(char *hex, unsigned char **out) {
    int l = strlen(hex);
    if (l % 2 != 0) return 0;
    *out = malloc(l / 2);
    for (int i = 0; i < l; i += 2) {
        int h = hex_val(hex[i]), l_val = hex_val(hex[i+1]);
        if (h == -1 || l_val == -1) { free(*out); return 0; }
        (*out)[i/2] = (h << 4) | l_val;
    }
    return l / 2;
}

int main() {
    char cmd[20], name[100], hex[10000]; int offset, length;
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "NEW")) {
            scanf("%s %s", name, hex);
            unsigned char *bytes;
            int len = parse_hex(hex, &bytes);
            if (len > 0) {
                bufs[nbufs].name = strdup(name);
                bufs[nbufs].bytes = bytes;
                bufs[nbufs++].len = len;
            }
        } else if (!strcmp(cmd, "APPEND")) {
            scanf("%s %s", name, hex);
            unsigned char *bytes;
            int len = parse_hex(hex, &bytes);
            int idx = find_buf(name);
            if (idx != -1 && len > 0) {
                bufs[idx].bytes = realloc(bufs[idx].bytes, bufs[idx].len + len);
                memcpy(bufs[idx].bytes + bufs[idx].len, bytes, len);
                bufs[idx].len += len;
                free(bytes);
            } else if (len > 0) {
                free(bytes);
            }
        } else if (!strcmp(cmd, "SLICE")) {
            scanf("%s %d %d", name, &offset, &length);
            int idx = find_buf(name);
            if (idx != -1 && offset >= 0 && length >= 0 && offset + length <= bufs[idx].len) {
                for (int i = 0; i < length; i++) printf("%02x", bufs[idx].bytes[offset + i]);
                printf("\n");
            }
        } else if (!strcmp(cmd, "DELETE")) {
            scanf("%s", name);
            int idx = find_buf(name);
            if (idx != -1) {
                free(bufs[idx].name); free(bufs[idx].bytes);
                for (int i = idx; i < nbufs - 1; i++) bufs[i] = bufs[i + 1];
                nbufs--;
            }
        } else if (!strcmp(cmd, "LIST")) {
            for (int i = 0; i < nbufs; i++) {
                printf("%s %d\n", bufs[i].name, bufs[i].len);
            }
        }
    }
    for (int i = 0; i < nbufs; i++) { free(bufs[i].name); free(bufs[i].bytes); }
    return 0;
}