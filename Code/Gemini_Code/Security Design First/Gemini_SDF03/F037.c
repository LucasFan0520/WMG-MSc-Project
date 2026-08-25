// F037.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    unsigned char *bytes;
    size_t size;
} ByteBuffer;

int main(void) {
    ByteBuffer *arr = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "NEW ", 4) == 0 || strncmp(line, "APPEND ", 7) == 0) {
            int is_new = (strncmp(line, "NEW ", 4) == 0);
            char *p = line + (is_new ? 4 : 7);
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            char *hex = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                hex = p;
            }
            size_t hex_len = strlen(hex);
            size_t byte_len = hex_len / 2;
            unsigned char *parsed = malloc(byte_len == 0 ? 1 : byte_len);
            if (!parsed) exit(1);
            for (size_t i = 0; i < byte_len; i++) {
                unsigned int val = 0;
                sscanf(hex + i * 2, "%2x", &val);
                parsed[i] = (unsigned char)val;
            }
            if (is_new) {
                int found = 0;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, name) == 0) {
                        free(arr[i].bytes);
                        arr[i].bytes = parsed;
                        arr[i].size = byte_len;
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        ByteBuffer *tmp = realloc(arr, capacity * sizeof(ByteBuffer));
                        if (!tmp) exit(1);
                        arr = tmp;
                    }
                    arr[count].name = strdup(name);
                    arr[count].bytes = parsed;
                    arr[count].size = byte_len;
                    if (!arr[count].name) exit(1);
                    count++;
                }
            } else {
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, name) == 0) {
                        unsigned char *tmp = realloc(arr[i].bytes, arr[i].size + byte_len);
                        if (!tmp) exit(1);
                        arr[i].bytes = tmp;
                        memcpy(arr[i].bytes + arr[i].size, parsed, byte_len);
                        arr[i].size += byte_len;
                        break;
                    }
                }
                free(parsed);
            }
        } else if (strncmp(line, "SLICE ", 6) == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            char *off_str = "0";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                off_str = p;
            }
            while (*p && *p != ' ') p++;
            char *len_str = "0";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                len_str = p;
            }
            int offset = atoi(off_str);
            int slen = atoi(len_str);
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    if (offset >= 0 && (size_t)offset < arr[i].size && slen >= 0) {
                        size_t limit = (size_t)offset + slen;
                        if (limit > arr[i].size) limit = arr[i].size;
                        for (size_t j = (size_t)offset; j < limit; j++) {
                            printf("%02x", arr[i].bytes[j]);
                        }
                        printf("\n");
                    }
                    break;
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            while (*name == ' ') name++;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    free(arr[i].name);
                    free(arr[i].bytes);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %zu\n", arr[i].name, arr[i].size);
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].name);
        free(arr[i].bytes);
    }
    free(arr);
    free(line);
    return 0;
}
