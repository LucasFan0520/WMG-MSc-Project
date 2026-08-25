// F037.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 1024;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) { free(buf); return NULL; }
            buf = new_buf;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *name;
    unsigned char *bytes;
    int size;
} ByteBuffer;

int hex_to_bytes(char *hex, unsigned char **res) {
    int len = strlen(hex);
    int count = len / 2;
    *res = malloc(count);
    for (int i = 0; i < count; i++) {
        unsigned int b;
        sscanf(hex + 2 * i, "%2x", &b);
        (*res)[i] = (unsigned char)b;
    }
    return count;
}

int main() {
    ByteBuffer *buffers = NULL;
    int count = 0;
    int capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char cmd[64];
        int n = 0;
        if (sscanf(line, "%63s%n", cmd, &n) < 1) {
            free(line);
            continue;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %d\n", buffers[i].name, buffers[i].size);
            }
            free(line);
            continue;
        }
        char *p = line + n;
        while (*p == ' ' || *p == '\t') p++;
        if (strcmp(cmd, "NEW") == 0) {
            char name[256], hex[1024];
            if (sscanf(p, "%255s %1023s", name, hex) == 2) {
                unsigned char *res = NULL;
                int b_size = hex_to_bytes(hex, &res);
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    buffers = realloc(buffers, capacity * sizeof(ByteBuffer));
                }
                buffers[count].name = strdup(name);
                buffers[count].bytes = res;
                buffers[count].size = b_size;
                count++;
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            char name[256], hex[1024];
            if (sscanf(p, "%255s %1023s", name, hex) == 2) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(buffers[i].name, name) == 0) {
                        unsigned char *res = NULL;
                        int add_size = hex_to_bytes(hex, &res);
                        buffers[i].bytes = realloc(buffers[i].bytes, buffers[i].size + add_size);
                        memcpy(buffers[i].bytes + buffers[i].size, res, add_size);
                        buffers[i].size += add_size;
                        free(res);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            if (sscanf(p, "%255s", name) == 1) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(buffers[i].name, name) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    free(buffers[found].name);
                    free(buffers[found].bytes);
                    for (int i = found; i < count - 1; i++) {
                        buffers[i] = buffers[i + 1];
                    }
                    count--;
                }
            }
        } else if (strcmp(cmd, "SLICE") == 0) {
            char name[256];
            int offset = 0, length = 0;
            if (sscanf(p, "%255s %d %d", name, &offset, &length) == 3) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(buffers[i].name, name) == 0) {
                        if (offset >= 0 && offset < buffers[i].size && length > 0) {
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
