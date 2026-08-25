// F037.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *stream) {
    size_t capacity = 16;
    size_t len = 0;
    char *str = malloc(capacity);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(stream)) != EOF && c != '\n') {
        if (len + 1 >= capacity) {
            capacity *= 2;
            char *new_str = realloc(str, capacity);
            if (!new_str) {
                free(str);
                return NULL;
            }
            str = new_str;
        }
        str[len++] = (char)c;
    }
    if (len == 0 && c == EOF) {
        free(str);
        return NULL;
    }
    if (len > 0 && str[len - 1] == '\n') {
        len--;
    }
    str[len] = '\0';
    return str;
}

typedef struct {
    char *name;
    unsigned char *bytes;
    int size;
    int capacity;
} Buffer;

int parse_hex(const char *hex, unsigned char *out, int max_len) {
    int count = 0;
    while (*hex && count < max_len) {
        while (*hex == ' ') hex++;
        if (!*hex) break;
        unsigned int val = 0;
        if (sscanf(hex, "%2x", &val) == 1) {
            out[count++] = (unsigned char)val;
            hex += 2;
        } else {
            break;
        }
    }
    return count;
}

int main(void) {
    Buffer *buffers = NULL;
    int count = 0;
    int capacity = 0;
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
                int len_hex = (int)strlen(hex);
                unsigned char *buf_bytes = malloc(len_hex + 1);
                int parsed = parse_hex(hex, buf_bytes, len_hex);
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Buffer *new_b = realloc(buffers, capacity * sizeof(Buffer));
                    if (new_b) buffers = new_b;
                }
                if (count < capacity) {
                    buffers[count].name = strdup(name);
                    buffers[count].bytes = buf_bytes;
                    buffers[count].size = parsed;
                    buffers[count].capacity = len_hex + 1;
                    count++;
                } else {
                    free(buf_bytes);
                }
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
                        Buffer *b = &buffers[i];
                        int len_hex = (int)strlen(hex);
                        if (b->size + len_hex >= b->capacity) {
                            b->capacity = b->size + len_hex + 1;
                            unsigned char *new_bytes = realloc(b->bytes, b->capacity);
                            if (new_bytes) b->bytes = new_bytes;
                        }
                        int parsed = parse_hex(hex, b->bytes + b->size, len_hex);
                        b->size += parsed;
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
                char *offset_str = p + 1;
                while (*offset_str == ' ') offset_str++;
                char *endptr;
                long offset = strtol(offset_str, &endptr, 10);
                if (endptr != offset_str) {
                    p = endptr;
                    while (*p == ' ') p++;
                    long length = strtol(p, &endptr, 10);
                    if (endptr != p) {
                        for (int i = 0; i < count; i++) {
                            if (strcmp(buffers[i].name, name) == 0) {
                                Buffer *b = &buffers[i];
                                if (offset >= 0 && offset < b->size) {
                                    if (offset + length > b->size) {
                                        length = b->size - offset;
                                    }
                                    for (long j = 0; j < length; j++) {
                                        printf("%02x", b->bytes[offset + j]);
                                    }
                                    printf("\n");
                                }
                                break;
                            }
                        }
                    }
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            while (*name == ' ') name++;
            for (int i = 0; i < count; i++) {
                if (strcmp(buffers[i].name, name) == 0) {
                    free(buffers[i].name);
                    free(buffers[i].bytes);
                    for (int j = i; j < count - 1; j++) {
                        buffers[j] = buffers[j + 1];
                    }
                    count--;
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
