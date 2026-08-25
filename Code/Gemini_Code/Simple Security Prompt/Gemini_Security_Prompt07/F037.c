// F037.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    unsigned char *bytes;
    int size;
} ByteBuffer;

void append_hex(ByteBuffer *bb, const char *hex) {
    int len = strlen(hex);
    for (int i = 0; i < len - 1; i += 2) {
        char pair[3] = { hex[i], hex[i+1], '\0' };
        unsigned char val = (unsigned char)strtol(pair, NULL, 16);
        bb->bytes = realloc(bb->bytes, bb->size + 1);
        bb->bytes[bb->size] = val;
        bb->size++;
    }
}

int main() {
    ByteBuffer *buffers = NULL;
    int count = 0;
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "NEW") == 0) {
            char *name = strtok(NULL, " ");
            char *hex = strtok(NULL, " ");
            if (name && hex) {
                buffers = realloc(buffers, (count + 1) * sizeof(ByteBuffer));
                buffers[count].name = strdup(name);
                buffers[count].bytes = NULL;
                buffers[count].size = 0;
                append_hex(&buffers[count], hex);
                count++;
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            char *name = strtok(NULL, " ");
            char *hex = strtok(NULL, " ");
            if (name && hex) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(buffers[i].name, name) == 0) {
                        append_hex(&buffers[i], hex);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "SLICE") == 0) {
            char *name = strtok(NULL, " ");
            char *off_str = strtok(NULL, " ");
            char *len_str = strtok(NULL, " ");
            if (name && off_str && len_str) {
                int offset = atoi(off_str);
                int length = atoi(len_str);
                for (int i = 0; i < count; i++) {
                    if (strcmp(buffers[i].name, name) == 0) {
                        if (offset >= 0 && offset < buffers[i].size && length > 0) {
                            int end = offset + length;
                            if (end > buffers[i].size) end = buffers[i].size;
                            for (int j = offset; j < end; j++) {
                                printf("%02X", buffers[i].bytes[j]);
                            }
                            printf("\n");
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = strtok(NULL, " ");
            if (name) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(buffers[i].name, name) == 0) {
                        free(buffers[i].name);
                        free(buffers[i].bytes);
                        for (int j = i; j < count - 1; j++) {
                            buffers[j] = buffers[j + 1];
                        }
                        count--;
                        i--;
                    }
                }
                if (count == 0) {
                    free(buffers);
                    buffers = NULL;
                } else {
                    buffers = realloc(buffers, count * sizeof(ByteBuffer));
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
