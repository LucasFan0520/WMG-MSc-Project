// F037.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    if (len > 0 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
    } else {
        buf[len] = '\0';
    }
    return buf;
}

char *safe_dup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (d) {
        strcpy(d, s);
    }
    return d;
}

typedef struct {
    char *name;
    unsigned char *bytes;
    size_t size;
    size_t cap;
} ByteBuffer;

int main(void) {
    ByteBuffer *buffers = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "NEW ", 4) == 0) {
            char *p = line + 4;
            char *name = p;
            while (*p && *p != ' ') p++;
            char *hex = "";
            if (*p == ' ') {
                *p = '\0';
                hex = p + 1;
            }
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                ByteBuffer *nb = realloc(buffers, cap * sizeof(ByteBuffer));
                if (!nb) {
                    free(line);
                    break;
                }
                buffers = nb;
            }
            buffers[count].name = safe_dup(name);
            size_t hlen = strlen(hex);
            size_t bcount = hlen / 2;
            buffers[count].bytes = malloc(bcount + 1);
            buffers[count].size = bcount;
            buffers[count].cap = bcount;
            if (buffers[count].bytes) {
                for (size_t i = 0; i < bcount; i++) {
                    unsigned int bval;
                    sscanf(hex + 2 * i, "%2x", &bval);
                    buffers[count].bytes[i] = (unsigned char)bval;
                }
            }
            count++;
        } else if (strncmp(line, "APPEND ", 7) == 0) {
            char *p = line + 7;
            char *name = p;
            while (*p && *p != ' ') p++;
            char *hex = "";
            if (*p == ' ') {
                *p = '\0';
                hex = p + 1;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(buffers[i].name, name) == 0) {
                    ByteBuffer *buf = &buffers[i];
                    size_t hlen = strlen(hex);
                    size_t bcount = hlen / 2;
                    if (buf->size + bcount > buf->cap) {
                        buf->cap = buf->size + bcount + 4;
                        unsigned char *nb = realloc(buf->bytes, buf->cap);
                        if (!nb) break;
                        buf->bytes = nb;
                    }
                    for (size_t j = 0; j < bcount; j++) {
                        unsigned int bval;
                        sscanf(hex + 2 * j, "%2x", &bval);
                        buf->bytes[buf->size++] = (unsigned char)bval;
                    }
                    break;
                }
            }
        } else if (strncmp(line, "SLICE ", 6) == 0) {
            char *p = line + 6;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            char *offset_str = p;
            while (*p && *p != ' ') p++;
            char *len_str = "";
            if (*p == ' ') {
                *p = '\0';
                len_str = p + 1;
            }
            long long offset = strtoll(offset_str, NULL, 10);
            long long length = strtoll(len_str, NULL, 10);
            for (size_t i = 0; i < count; i++) {
                if (strcmp(buffers[i].name, name) == 0) {
                    ByteBuffer *buf = &buffers[i];
                    if (offset >= 0 && (size_t)offset < buf->size && length > 0) {
                        size_t end = (size_t)offset + (size_t)length;
                        if (end > buf->size) end = buf->size;
                        for (size_t j = (size_t)offset; j < end; j++) {
                            printf("%02x", buf->bytes[j]);
                        }
                        printf("\n");
                    }
                    break;
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
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
        } else if (strcmp(line, "LIST") == 0) {
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
