/* F037.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    unsigned char *bytes;
    size_t size;
} BufferRec;

int hex_val(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

int main(void) {
    BufferRec *buffers = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;

    while (getline(&line, &len, stdin) != -1) {
        size_t str_len = strlen(line);
        while (str_len > 0 && (line[str_len - 1] == '\n' || line[str_len - 1] == '\r')) {
            line[str_len - 1] = '\0';
            str_len--;
        }
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') continue;
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) break;
        while (*p == ' ') p++;

        if (strcmp(cmd, "NEW") == 0) {
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *hex = p;

            size_t h_len = strlen(hex);
            size_t b_size = h_len / 2;
            unsigned char *buf = malloc(b_size + 1);
            if (buf) {
                size_t idx = 0;
                int valid = 1;
                for (size_t i = 0; i < b_size; i++) {
                    int v1 = hex_val(hex[2 * i]);
                    int v2 = hex_val(hex[2 * i + 1]);
                    if (v1 < 0 || v2 < 0) { valid = 0; break; }
                    buf[idx++] = (unsigned char)((v1 << 4) | v2);
                }
                if (valid) {
                    if (count >= capacity) {
                        size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                        BufferRec *temp = realloc(buffers, new_cap * sizeof(BufferRec));
                        if (!temp) { free(buf); break; }
                        buffers = temp;
                        capacity = new_cap;
                    }
                    buffers[count].name = strdup(name);
                    buffers[count].bytes = buf;
                    buffers[count].size = b_size;
                    if (buffers[count].name) count++;
                    else free(buf);
                } else {
                    free(buf);
                }
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *hex = p;

            for (size_t i = 0; i < count; i++) {
                if (strcmp(buffers[i].name, name) == 0) {
                    size_t h_len = strlen(hex);
                    size_t add_size = h_len / 2;
                    unsigned char *temp = realloc(buffers[i].bytes, buffers[i].size + add_size + 1);
                    if (temp) {
                        buffers[i].bytes = temp;
                        size_t idx = buffers[i].size;
                        for (size_t j = 0; j < add_size; j++) {
                            int v1 = hex_val(hex[2 * j]);
                            int v2 = hex_val(hex[2 * j + 1]);
                            if (v1 >= 0 && v2 >= 0) {
                                buffers[i].bytes[idx++] = (unsigned char)((v1 << 4) | v2);
                            }
                        }
                        buffers[i].size += add_size;
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "SLICE") == 0) {
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *off_str = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            int offset = atoi(off_str);
            int slen = atoi(p);

            for (size_t i = 0; i < count; i++) {
                if (strcmp(buffers[i].name, name) == 0) {
                    if (offset >= 0 && (size_t)offset < buffers[i].size && slen > 0) {
                        size_t end = (size_t)offset + (size_t)slen;
                        if (end > buffers[i].size) end = buffers[i].size;
                        for (size_t j = (size_t)offset; j < end; j++) {
                            printf("%02x", buffers[i].bytes[j]);
                        }
                        printf("\n");
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = p;
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
    }
    free(line);
    for (size_t i = 0; i < count; i++) {
        free(buffers[i].name);
        free(buffers[i].bytes);
    }
    free(buffers);
    return 0;
}
