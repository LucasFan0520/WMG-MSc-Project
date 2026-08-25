// F037.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
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
    size_t size;
} ByteBuffer;

void parse_hex(const char *hex, unsigned char **out_bytes, size_t *out_size) {
    size_t len = strlen(hex);
    size_t count = len / 2;
    unsigned char *b = malloc(count + 1);
    for (size_t i = 0; i < count; i++) {
        unsigned int val;
        sscanf(hex + 2 * i, "%2x", &val);
        b[i] = (unsigned char)val;
    }
    *out_bytes = b;
    *out_size = count;
}

int main() {
    ByteBuffer *arr = NULL;
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
            char *name = line + 4;
            char *space = strchr(name, ' ');
            char *hex = "";
            if (space) {
                *space = '\0';
                hex = space + 1;
            }
            unsigned char *b = NULL;
            size_t sz = 0;
            parse_hex(hex, &b, &sz);
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                arr = realloc(arr, cap * sizeof(ByteBuffer));
            }
            arr[count].name = strdup(name);
            arr[count].bytes = b;
            arr[count].size = sz;
            count++;
        } else if (strncmp(line, "APPEND ", 7) == 0) {
            char *name = line + 7;
            char *space = strchr(name, ' ');
            char *hex = "";
            if (space) {
                *space = '\0';
                hex = space + 1;
            }
            unsigned char *b = NULL;
            size_t sz = 0;
            parse_hex(hex, &b, &sz);
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    arr[i].bytes = realloc(arr[i].bytes, arr[i].size + sz + 1);
                    memcpy(arr[i].bytes + arr[i].size, b, sz);
                    arr[i].size += sz;
                    break;
                }
            }
            free(b);
        } else if (strncmp(line, "SLICE ", 6) == 0) {
            char *p = line + 6;
            char *s1 = strchr(p, ' ');
            if (s1) {
                *s1 = '\0';
                char *name = p;
                char *p2 = s1 + 1;
                char *s2 = strchr(p2, ' ');
                if (s2) {
                    *s2 = '\0';
                    int offset = atoi(p2);
                    int length = atoi(s2 + 1);
                    for (size_t i = 0; i < count; i++) {
                        if (strcmp(arr[i].name, name) == 0) {
                            if (offset >= 0 && offset < (int)arr[i].size) {
                                int end = offset + length;
                                if (end > (int)arr[i].size) end = (int)arr[i].size;
                                for (int j = offset; j < end; j++) {
                                    printf("%02x", arr[i].bytes[j]);
                                }
                                printf("\n");
                            }
                            break;
                        }
                    }
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            size_t i = 0;
            while (i < count) {
                if (strcmp(arr[i].name, name) == 0) {
                    free(arr[i].name);
                    free(arr[i].bytes);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %zu\n", arr[i].name, arr[i].size);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].name);
        free(arr[i].bytes);
    }
    free(arr);
    return 0;
}
