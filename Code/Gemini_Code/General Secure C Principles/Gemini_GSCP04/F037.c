// F037.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *stream) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(stream)) != EOF && c != '\n') {
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
    if (len > 0 && buf[len - 1] == '\r') {
        buf[len - 1] = '\0';
    }
    return buf;
}

typedef struct {
    char *name;
    unsigned char *bytes;
    size_t size;
} ByteBuffer;

size_t hex_to_bytes(const char *hex, unsigned char **res) {
    size_t len = strlen(hex);
    size_t count = len / 2;
    *res = malloc(count == 0 ? 1 : count);
    if (!*res) return 0;
    for (size_t i = 0; i < count; i++) {
        unsigned int val;
        sscanf(hex + 2 * i, "%2x", &val);
        (*res)[i] = (unsigned char)val;
    }
    return count;
}

int main(void) {
    ByteBuffer *buffers = NULL;
    size_t count = 0;
    size_t capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "NEW") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *hex = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(buffers[i].name, name) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found && *name != '\0') {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    ByteBuffer *new_buffers = realloc(buffers, capacity * sizeof(ByteBuffer));
                    if (new_buffers) buffers = new_buffers;
                }
                if (count < capacity) {
                    buffers[count].name = strdup(name);
                    buffers[count].size = hex_to_bytes(hex, &buffers[count].bytes);
                    count++;
                }
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *hex = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(buffers[i].name, name) == 0) {
                    unsigned char *add_bytes = NULL;
                    size_t add_size = hex_to_bytes(hex, &add_bytes);
                    if (add_size > 0) {
                        unsigned char *new_b = realloc(buffers[i].bytes, buffers[i].size + add_size);
                        if (new_b) {
                            buffers[i].bytes = new_b;
                            memcpy(buffers[i].bytes + buffers[i].size, add_bytes, add_size);
                            buffers[i].size += add_size;
                        }
                    }
                    free(add_bytes);
                    break;
                }
            }
        } else if (strcmp(cmd, "SLICE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *off_s = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *len_s = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            long long offset = atoll(off_s);
            long long length = atoll(len_s);
            for (size_t i = 0; i < count; i++) {
                if (strcmp(buffers[i].name, name) == 0) {
                    if (offset >= 0 && (size_t)offset < buffers[i].size && length > 0) {
                        size_t end = (size_t)offset + (size_t)length;
                        if (end > buffers[i].size) end = buffers[i].size;
                        for (size_t k = (size_t)offset; k < end; k++) {
                            printf("%02X", buffers[i].bytes[k]);
                        }
                        printf("\n");
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(buffers[i].name, name) == 0) {
                    free(buffers[i].name);
                    free(buffers[i].bytes);
                    for (size_t j = i; j < count - 1; j++) {
                        buffers[j] = buffers[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %zu\n", buffers[i].name, buffers[i].size);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(buffers[i].name);
        free(buffers[i].bytes);
    }
    free(buffers);
    return 0;
}
