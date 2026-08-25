// F037.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 128;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) {
                free(buf);
                return NULL;
            }
            buf = new_buf;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *name;
    unsigned char *bytes;
    int size;
} ByteBuffer;

int main() {
    ByteBuffer *buffers = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "NEW ", 4) == 0) {
            char name[128], hex[256];
            if (sscanf(line + 4, "%127s %255s", name, hex) == 2) {
                buffers = realloc(buffers, (count + 1) * sizeof(ByteBuffer));
                buffers[count].name = strdup(name);
                int len = strlen(hex) / 2;
                buffers[count].bytes = malloc(len);
                buffers[count].size = len;
                for (int i = 0; i < len; i++) {
                    unsigned int b;
                    sscanf(hex + 2 * i, "%2x", &b);
                    buffers[count].bytes[i] = (unsigned char)b;
                }
                count++;
            }
        } else if (strncmp(line, "APPEND ", 7) == 0) {
            char name[128], hex[256];
            if (sscanf(line + 7, "%127s %255s", name, hex) == 2) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(buffers[i].name, name) == 0) {
                        int len = strlen(hex) / 2;
                        buffers[i].bytes = realloc(buffers[i].bytes, buffers[i].size + len);
                        for (int j = 0; j < len; j++) {
                            unsigned int b;
                            sscanf(hex + 2 * j, "%2x", &b);
                            buffers[i].bytes[buffers[i].size + j] = (unsigned char)b;
                        }
                        buffers[i].size += len;
                        break;
                    }
                }
            }
        } else if (strncmp(line, "SLICE ", 6) == 0) {
            char name[128];
            int offset, length;
            if (sscanf(line + 6, "%127s %d %d", name, &offset, &length) == 3) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(buffers[i].name, name) == 0) {
                        if (offset >= 0 && offset + length <= buffers[i].size) {
                            for (int j = 0; j < length; j++) {
                                printf("%02x", buffers[i].bytes[offset + j]);
                            }
                            printf("\n");
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
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
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %d\n", buffers[i].name, buffers[i].size);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(buffers[i].name);
        free(buffers[i].bytes);
    }
    free(buffers);
    return 0;
}
