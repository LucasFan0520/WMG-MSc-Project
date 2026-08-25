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
    buf[len] = '\0';
    if (len > 0 && buf[len-1] == '\r') {
        buf[len-1] = '\0';
    }
    return buf;
}

typedef struct {
    char *name;
    unsigned char *bytes;
    int size;
    int cap;
} ByteBuffer;

int main(void) {
    ByteBuffer *buffers = NULL;
    int count = 0;
    int cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "NEW ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *hex = p + 1;
                while (*hex == ' ') hex++;
                if (count >= cap) {
                    cap = cap == 0 ? 16 : cap * 2;
                    buffers = realloc(buffers, cap * sizeof(ByteBuffer));
                }
                buffers[count].name = strdup(name);
                buffers[count].bytes = NULL;
                buffers[count].size = 0;
                buffers[count].cap = 0;
                ByteBuffer *b = &buffers[count];
                int len = strlen(hex);
                for (int i = 0; i < len - 1; i += 2) {
                    char tmp[3] = { hex[i], hex[i+1], '\0' };
                    unsigned char byte = (unsigned char)strtol(tmp, NULL, 16);
                    if (b->size >= b->cap) {
                        b->cap = b->cap == 0 ? 16 : b->cap * 2;
                        b->bytes = realloc(b->bytes, b->cap);
                    }
                    b->bytes[b->size++] = byte;
                }
                count++;
            }
        } else if (strncmp(line, "APPEND ", 7) == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *hex = p + 1;
                while (*hex == ' ') hex++;
                for (int i = 0; i < count; i++) {
                    if (strcmp(buffers[i].name, name) == 0) {
                        ByteBuffer *b = &buffers[i];
                        int len = strlen(hex);
                        for (int j = 0; j < len - 1; j += 2) {
                            char tmp[3] = { hex[j], hex[j+1], '\0' };
                            unsigned char byte = (unsigned char)strtol(tmp, NULL, 16);
                            if (b->size >= b->cap) {
                                b->cap = b->cap == 0 ? 16 : b->cap * 2;
                                b->bytes = realloc(b->bytes, b->cap);
                            }
                            b->bytes[b->size++] = byte;
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "SLICE ", 6) == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *endptr;
                long offset = strtol(p + 1, &endptr, 10);
                long length = strtol(endptr, NULL, 10);
                for (int i = 0; i < count; i++) {
                    if (strcmp(buffers[i].name, name) == 0) {
                        ByteBuffer *b = &buffers[i];
                        if (offset >= 0 && offset < b->size && length > 0) {
                            long end = offset + length;
                            if (end > b->size) end = b->size;
                            for (long j = offset; j < end; j++) {
                                printf("%02X", b->bytes[j]);
                            }
                            printf("\n");
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            while (*name == ' ') name++;
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
