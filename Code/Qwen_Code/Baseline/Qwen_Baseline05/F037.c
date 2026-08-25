// F037.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_NAME 100
#define MAX_BUF 10000

typedef struct {
    char name[MAX_NAME];
    unsigned char data[MAX_BUF];
    int size;
} Buffer;

Buffer buffers[100];
int buf_count = 0;

int find_buf(const char *name) {
    for (int i = 0; i < buf_count; i++) if (strcmp(buffers[i].name, name) == 0) return i;
    return -1;
}

int hex_to_byte(char c1, char c2) {
    if (!isxdigit(c1) || !isxdigit(c2)) return -1;
    int v1 = isdigit(c1) ? c1 - '0' : tolower(c1) - 'a' + 10;
    int v2 = isdigit(c2) ? c2 - '0' : tolower(c2) - 'a' + 10;
    return (v1 << 4) | v2;
}

int parse_hex(const char *hex, unsigned char *out) {
    int len = strlen(hex);
    if (len % 2 != 0) return -1;
    for (int i = 0; i < len; i += 2) {
        int b = hex_to_byte(hex[i], hex[i+1]);
        if (b == -1) return -1;
        out[i/2] = b;
    }
    return len / 2;
}

int main() {
    char cmd[20], name[MAX_NAME], hex[MAX_BUF * 2 + 10];
    int offset, length;
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "NEW") == 0) {
            scanf("%s %s", name, hex);
            int idx = find_buf(name);
            if (idx == -1) {
                idx = buf_count++;
                strcpy(buffers[idx].name, name);
                buffers[idx].size = 0;
            }
            int len = parse_hex(hex, buffers[idx].data);
            if (len != -1) buffers[idx].size = len;
        } else if (strcmp(cmd, "APPEND") == 0) {
            scanf("%s %s", name, hex);
            int idx = find_buf(name);
            if (idx != -1) {
                unsigned char temp[MAX_BUF];
                int len = parse_hex(hex, temp);
                if (len != -1 && buffers[idx].size + len <= MAX_BUF) {
                    memcpy(buffers[idx].data + buffers[idx].size, temp, len);
                    buffers[idx].size += len;
                }
            }
        } else if (strcmp(cmd, "SLICE") == 0) {
            scanf("%s %d %d", name, &offset, &length);
            int idx = find_buf(name);
            if (idx != -1 && offset >= 0 && length >= 0 && offset + length <= buffers[idx].size) {
                for (int i = 0; i < length; i++) {
                    printf("%02x", buffers[idx].data[offset + i]);
                }
                printf("\n");
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", name);
            int idx = find_buf(name);
            if (idx != -1) {
                for (int i = idx; i < buf_count - 1; i++) buffers[i] = buffers[i + 1];
                buf_count--;
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < buf_count; i++) printf("%s %d\n", buffers[i].name, buffers[i].size);
        }
    }
    return 0;
}