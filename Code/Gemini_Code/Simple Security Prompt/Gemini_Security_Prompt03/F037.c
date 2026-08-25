// F037.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    unsigned char *bytes;
    int size;
} ByteBuffer;

char *read_line(FILE *fp) {
    size_t size = 32;
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

int hex_val(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

int main(void) {
    ByteBuffer *list = NULL;
    int count = 0;
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
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "NEW") == 0 || strcmp(cmd, "APPEND") == 0) {
            int is_new = (strcmp(cmd, "NEW") == 0);
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *hex = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            int hex_len = strlen(hex);
            int byte_len = hex_len / 2;
            unsigned char *parsed_bytes = malloc(byte_len + 1);
            int valid_bytes = 0;
            for (int j = 0; j < byte_len; j++) {
                int b1 = hex_val(hex[2 * j]);
                int b2 = hex_val(hex[2 * j + 1]);
                if (b1 != -1 && b2 != -1) {
                    parsed_bytes[valid_bytes++] = (b1 << 4) | b2;
                } else {
                    break;
                }
            }
            if (is_new) {
                list = realloc(list, (count + 1) * sizeof(ByteBuffer));
                list[count].name = strdup(name);
                list[count].bytes = parsed_bytes;
                list[count].size = valid_bytes;
                count++;
            } else {
                for (int i = 0; i < count; i++) {
                    if (list[i].name && strcmp(list[i].name, name) == 0) {
                        list[i].bytes = realloc(list[i].bytes, list[i].size + valid_bytes);
                        memcpy(list[i].bytes + list[i].size, parsed_bytes, valid_bytes);
                        list[i].size += valid_bytes;
                        break;
                    }
                }
                free(parsed_bytes);
            }
        } else if (strcmp(cmd, "SLICE") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *off_s = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            int offset = atoi(off_s);
            int length = atoi(p);
            for (int i = 0; i < count; i++) {
                if (list[i].name && strcmp(list[i].name, name) == 0) {
                    if (offset >= 0 && offset < list[i].size) {
                        int limit = (offset + length > list[i].size) ? list[i].size - offset : length;
                        for (int j = 0; j < limit; j++) {
                            printf("%02x", list[i].bytes[offset + j]);
                        }
                        printf("\n");
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < count; i++) {
                if (list[i].name && strcmp(list[i].name, name) == 0) {
                    free(list[i].name);
                    free(list[i].bytes);
                    list[i].name = NULL;
                    list[i].bytes = NULL;
                    list[i].size = 0;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                if (list[i].name) {
                    printf("%s %d\n", list[i].name, list[i].size);
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        if (list[i].name) {
            free(list[i].name);
            free(list[i].bytes);
        }
    }
    free(list);
    return 0;
}
