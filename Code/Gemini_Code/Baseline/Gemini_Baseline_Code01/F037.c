// F037.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    unsigned char *bytes;
    int size;
} ByteBuffer;

void parse_hex(const char *hex, unsigned char **out_bytes, int *out_size) {
    int len = strlen(hex);
    int count = len / 2;
    *out_bytes = malloc(count);
    *out_size = count;
    for (int i = 0; i < count; i++) {
        unsigned int b = 0;
        sscanf(hex + 2 * i, "%2x", &b);
        (*out_bytes)[i] = (unsigned char)b;
    }
}

int main() {
    ByteBuffer *buffers = NULL;
    int count = 0;
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[16];
        int read_bytes = 0;
        if (sscanf(line, "%15s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *args = line + read_bytes;
        while (*args == ' ') args++;
        if (strcmp(cmd, "NEW") == 0) {
            char name[256], hex[4096];
            if (sscanf(args, "%255s %4095s", name, hex) == 2) {
                unsigned char *b_arr = NULL;
                int b_size = 0;
                parse_hex(hex, &b_arr, &b_size);
                buffers = realloc(buffers, (count + 1) * sizeof(ByteBuffer));
                buffers[count].name = strdup(name);
                buffers[count].bytes = b_arr;
                buffers[count].size = b_size;
                count++;
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            char name[256], hex[4096];
            if (sscanf(args, "%255s %4095s", name, hex) == 2) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(buffers[i].name, name) == 0) {
                        unsigned char *b_arr = NULL;
                        int b_size = 0;
                        parse_hex(hex, &b_arr, &b_size);
                        buffers[i].bytes = realloc(buffers[i].bytes, buffers[i].size + b_size);
                        memcpy(buffers[i].bytes + buffers[i].size, b_arr, b_size);
                        buffers[i].size += b_size;
                        free(b_arr);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "SLICE") == 0) {
            char name[256];
            int offset = 0, length = 0;
            if (sscanf(args, "%255s %d %d", name, &offset, &length) == 3) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(buffers[i].name, name) == 0) {
                        if (offset >= 0 && offset < buffers[i].size) {
                            int end = offset + length;
                            if (end > buffers[i].size) end = buffers[i].size;
                            for (int j = offset; j < end; j++) {
                                printf("%02x", buffers[i].bytes[j]);
                            }
                            printf("\n");
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            if (sscanf(args, "%255s", name) == 1) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(buffers[i].name, name) == 0) {
                        free(buffers[i].name);
                        free(buffers[i].bytes);
                        for (int j = i; j < count - 1; j++) {
                            buffers[j] = buffers[j + 1];
                        }
                        count--;
                        buffers = realloc(buffers, count * sizeof(ByteBuffer));
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %d\n", buffers[i].name, buffers[i].size);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(buffers[i].name);
        free(buffers[i].bytes);
    }
    free(buffers);
    return 0;
}
