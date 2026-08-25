// F037.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) { free(buf); return NULL; }
            buf = nb;
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
    size_t size;
} ByteBuffer;

int parse_hex(const char *hex, unsigned char **res, size_t *out_len) {
    size_t len = strlen(hex);
    size_t byte_count = len / 2;
    unsigned char *buf = malloc(byte_count + 1);
    if (!buf) return 0;
    for (size_t i = 0; i < byte_count; i++) {
        unsigned int val;
        char tmp[3];
        tmp[0] = hex[i * 2];
        tmp[1] = hex[i * 2 + 1];
        tmp[2] = '\0';
        if (sscanf(tmp, "%x", &val) != 1) {
            free(buf);
            return 0;
        }
        buf[i] = (unsigned char)val;
    }
    *res = buf;
    *out_len = byte_count;
    return 1;
}

int main(void) {
    ByteBuffer *list = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') { free(line); continue; }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) { free(line); break; }
        while (*p == ' ') p++;
        char *arg1 = p;
        if (strcmp(cmd, "NEW") == 0) {
            char *name = arg1;
            p = arg1;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *hex = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            unsigned char *parsed = NULL;
            size_t p_len = 0;
            if (parse_hex(hex, &parsed, &p_len)) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    ByteBuffer *nb = realloc(list, cap * sizeof(ByteBuffer));
                    if (nb) list = nb;
                }
                if (count < cap) {
                    list[count].name = strdup(name);
                    list[count].bytes = parsed;
                    list[count].size = p_len;
                    count++;
                } else {
                    free(parsed);
                }
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            char *name = arg1;
            p = arg1;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *hex = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            unsigned char *parsed = NULL;
            size_t p_len = 0;
            if (parse_hex(hex, &parsed, &p_len)) {
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(list[i].name, name) == 0) {
                        unsigned char *nb = realloc(list[i].bytes, list[i].size + p_len);
                        if (nb) {
                            list[i].bytes = nb;
                            memcpy(list[i].bytes + list[i].size, parsed, p_len);
                            list[i].size += p_len;
                        }
                        break;
                    }
                }
                free(parsed);
            }
        } else if (strcmp(cmd, "SLICE") == 0) {
            char *name = arg1;
            p = arg1;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *offset_str = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            int offset = atoi(offset_str);
            int length = atoi(p);
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, name) == 0) {
                    if (offset >= 0 && length >= 0 && (size_t)offset < list[i].size) {
                        size_t limit = (size_t)offset + (size_t)length;
                        if (limit > list[i].size) limit = list[i].size;
                        for (size_t j = (size_t)offset; j < limit; j++) {
                            printf("%02x", list[i].bytes[j]);
                        }
                        printf("\n");
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, arg1) == 0) {
                    free(list[i].name);
                    free(list[i].bytes);
                    for (size_t j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %zu\n", list[i].name, list[i].size);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].name);
        free(list[i].bytes);
    }
    free(list);
    return 0;
}
