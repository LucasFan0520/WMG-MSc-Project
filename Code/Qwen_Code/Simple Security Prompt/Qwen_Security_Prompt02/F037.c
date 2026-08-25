// F037.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    unsigned char *buf;
    int size;
    int cap;
} Buffer;

Buffer *buffers = NULL;
int b_count = 0;

int find_name(const char *name) {
    for (int i = 0; i < b_count; i++) {
        if (strcmp(buffers[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int hex_to_byte(const char *s) {
    int val = 0;
    for (int i = 0; i < 2; i++) {
        char c = s[i];
        val *= 16;
        if (c >= '0' && c <= '9') {
            val += c - '0';
        } else if (c >= 'a' && c <= 'f') {
            val += c - 'a' + 10;
        } else if (c >= 'A' && c <= 'F') {
            val += c - 'A' + 10;
        } else {
            return -1;
        }
    }
    return val;
}

int parse_hex(const char *hex, unsigned char **out, int *out_len) {
    int len = (int)strlen(hex);
    if (len % 2 != 0) {
        return 0;
    }
    int byte_count = len / 2;
    unsigned char *data = malloc(byte_count);
    for (int i = 0; i < byte_count; i++) {
        int b = hex_to_byte(hex + i * 2);
        if (b < 0) {
            free(data);
            return 0;
        }
        data[i] = (unsigned char)b;
    }
    *out = data;
    *out_len = byte_count;
    return 1;
}

int main() {
    char cmd[32];
    char name[1024];
    char hex[102400];
    int offset;
    int length;

    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "NEW") == 0) {
            scanf("%s %s", name, hex);
            unsigned char *data = NULL;
            int data_len = 0;
            if (!parse_hex(hex, &data, &data_len)) {
                continue;
            }
            int idx = find_name(name);
            if (idx != -1) {
                free(buffers[idx].buf);
                buffers[idx].buf = data;
                buffers[idx].size = data_len;
                buffers[idx].cap = data_len;
            } else {
                buffers = realloc(buffers, sizeof(Buffer) * (b_count + 1));
                buffers[b_count].name = strdup(name);
                buffers[b_count].buf = data;
                buffers[b_count].size = data_len;
                buffers[b_count].cap = data_len;
                b_count++;
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            scanf("%s %s", name, hex);
            int idx = find_name(name);
            if (idx == -1) {
                continue;
            }
            unsigned char *data = NULL;
            int data_len = 0;
            if (!parse_hex(hex, &data, &data_len)) {
                continue;
            }
            int new_size = buffers[idx].size + data_len;
            if (new_size > buffers[idx].cap) {
                buffers[idx].cap = new_size;
                buffers[idx].buf = realloc(buffers[idx].buf, buffers[idx].cap);
            }
            memcpy(buffers[idx].buf + buffers[idx].size, data, data_len);
            buffers[idx].size = new_size;
            free(data);
        } else if (strcmp(cmd, "SLICE") == 0) {
            scanf("%s %d %d", name, &offset, &length);
            int idx = find_name(name);
            if (idx == -1) {
                continue;
            }
            if (offset < 0 || offset >= buffers[idx].size || length <= 0) {
                continue;
            }
            if (offset + length > buffers[idx].size) {
                length = buffers[idx].size - offset;
            }
            for (int i = 0; i < length; i++) {
                printf("%02x", buffers[idx].buf[offset + i]);
            }
            printf("\n");
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", name);
            int idx = find_name(name);
            if (idx != -1) {
                free(buffers[idx].name);
                free(buffers[idx].buf);
                for (int i = idx; i < b_count - 1; i++) {
                    buffers[i] = buffers[i + 1];
                }
                b_count--;
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < b_count; i++) {
                printf("%s %d\n", buffers[i].name, buffers[i].size);
            }
        }
    }

    for (int i = 0; i < b_count; i++) {
        free(buffers[i].name);
        free(buffers[i].buf);
    }
    free(buffers);

    return 0;
}