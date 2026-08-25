// F037.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_BUF 1000
#define MAX_SIZE 10000

typedef struct {
    char name[100];
    unsigned char data[MAX_SIZE];
    int size;
} Buf;

Buf bufs[MAX_BUF];
int bcount = 0;

int find_buf(const char *name) {
    for (int i = 0; i < bcount; i++) if (strcmp(bufs[i].name, name) == 0) return i;
    return -1;
}

int hex_to_bytes(const char *hex, unsigned char *out) {
    int len = strlen(hex);
    if (len % 2 != 0) return -1;
    for (int i = 0; i < len; i += 2) {
        if (!isxdigit(hex[i]) || !isxdigit(hex[i+1])) return -1;
        unsigned int b;
        sscanf(hex + i, "%2x", &b);
        out[i/2] = (unsigned char)b;
    }
    return len / 2;
}

int main() {
    char cmd[20], name[100], hex[MAX_SIZE * 2];
    int offset, length;
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "NEW") == 0) {
            scanf("%s %s", name, hex);
            if (find_buf(name) == -1 && bcount < MAX_BUF) {
                int bytes = hex_to_bytes(hex, bufs[bcount].data);
                if (bytes >= 0) {
                    strcpy(bufs[bcount].name, name);
                    bufs[bcount].size = bytes;
                    bcount++;
                }
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            scanf("%s %s", name, hex);
            int idx = find_buf(name);
            if (idx != -1) {
                unsigned char temp[MAX_SIZE];
                int bytes = hex_to_bytes(hex, temp);
                if (bytes >= 0 && bufs[idx].size + bytes <= MAX_SIZE) {
                    memcpy(bufs[idx].data + bufs[idx].size, temp, bytes);
                    bufs[idx].size += bytes;
                }
            }
        } else if (strcmp(cmd, "SLICE") == 0) {
            scanf("%s %d %d", name, &offset, &length);
            int idx = find_buf(name);
            if (idx != -1 && offset >= 0 && length >= 0 && offset + length <= bufs[idx].size) {
                for (int i = 0; i < length; i++) {
                    printf("%02x", bufs[idx].data[offset + i]);
                }
                printf("\n");
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", name);
            int idx = find_buf(name);
            if (idx != -1) {
                for (int i = idx; i < bcount - 1; i++) bufs[i] = bufs[i + 1];
                bcount--;
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < bcount; i++) {
                printf("%s %d\n", bufs[i].name, bufs[i].size);
            }
        }
    }
    return 0;
}