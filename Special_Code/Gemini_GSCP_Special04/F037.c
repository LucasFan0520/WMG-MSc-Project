/* F037.c */
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
        len--;
    }
    buf[len] = '\0';
    return buf;
}

char *mystrdup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (d) strcpy(d, s);
    return d;
}

typedef struct {
    char *name;
    unsigned char *bytes;
    size_t size;
    size_t cap;
} Buffer;

int parse_hex(const char *hex, unsigned char *out, size_t *out_len) {
    size_t len = strlen(hex);
    if (len % 2 != 0) return 0;
    size_t count = 0;
    for (size_t i = 0; i < len; i += 2) {
        unsigned int val;
        if (sscanf(hex + i, "%2x", &val) != 1) return 0;
        out[count++] = (unsigned char)val;
    }
    *out_len = count;
    return 1;
}

int main(void) {
    Buffer *buffers = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p == ' ') { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "NEW") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *hex = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') { *p = '\0'; p++; }
            size_t hex_len = strlen(hex);
            unsigned char *parsed = malloc(hex_len / 2 + 1);
            size_t out_len = 0;
            if (parsed && parse_hex(hex, parsed, &out_len)) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    Buffer *nb = realloc(buffers, cap * sizeof(Buffer));
                    if (nb) buffers = nb;
                }
                char *nn = mystrdup(name);
                if (nn) {
                    buffers[count].name = nn;
                    buffers[count].bytes = parsed;
                    buffers[count].size = out_len;
                    buffers[count].cap = hex_len / 2 + 1;
                    count++;
                } else {
                    free(parsed);
                }
            } else {
                free(parsed);
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *hex = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') { *p = '\0'; p++; }
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(buffers[i].name, name) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                Buffer *b = &buffers[found];
                size_t hex_len = strlen(hex);
                unsigned char *parsed = malloc(hex_len / 2 + 1);
                size_t out_len = 0;
                if (parsed && parse_hex(hex, parsed, &out_len)) {
                    if (b->size + out_len > b->cap) {
                        b->cap = b->size + out_len + 4;
                        unsigned char *nb = realloc(b->bytes, b->cap);
                        if (nb) b->bytes = nb;
                    }
                    memcpy(b->bytes + b->size, parsed, out_len);
                    b->size += out_len;
                }
                free(parsed);
            }
        } else if (strcmp(cmd, "SLICE") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *off_str = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *len_str = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') { *p = '\0'; p++; }
            long long offset = strtoll(off_str, NULL, 10);
            long long length = strtoll(len_str, NULL, 10);
            for (size_t i = 0; i < count; i++) {
                if (strcmp(buffers[i].name, name) == 0) {
                    if (offset >= 0 && length > 0 && (size_t)(offset + length) <= buffers[i].size) {
                        for (long long j = 0; j < length; j++) {
                            printf("%02x", buffers[i].bytes[offset + j]);
                        }
                        putchar('\n');
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') { *p = '\0'; p++; }
            size_t i = 0;
            while (i < count) {
                if (strcmp(buffers[i].name, name) == 0) {
                    free(buffers[i].name);
                    free(buffers[i].bytes);
                    for (size_t j = i; j < count - 1; j++) {
                        buffers[j] = buffers[j + 1];
                    }
                    count--;
                } else {
                    i++;
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
