// F037.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    unsigned char *bytes;
    size_t size;
} HexBuffer;

int main(void) {
    HexBuffer *list = NULL;
    size_t count = 0;
    char *line = NULL;
    size_t line_len = 0;
    ssize_t read;
    while ((read = getline(&line, &line_len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (read > 1 && line[read - 2] == '\r') {
            line[read - 2] = '\0';
        }
        char *p = line;
        while (*p == ' ' || *p == '\t') {
            p++;
        }
        if (*p == '\0') {
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') {
            p++;
        }
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "NEW") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *hex = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            size_t hex_len = strlen(hex);
            size_t bytes_len = hex_len / 2;
            unsigned char *buf = malloc(bytes_len + 1);
            if (buf) {
                for (size_t i = 0; i < bytes_len; i++) {
                    unsigned int val = 0;
                    sscanf(hex + 2 * i, "%2x", &val);
                    buf[i] = (unsigned char)val;
                }
                HexBuffer *temp = realloc(list, (count + 1) * sizeof(HexBuffer));
                if (temp) {
                    list = temp;
                    list[count].name = strdup(name);
                    list[count].bytes = buf;
                    list[count].size = bytes_len;
                    count++;
                } else {
                    free(buf);
                }
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *hex = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, name) == 0) {
                    size_t hex_len = strlen(hex);
                    size_t add_len = hex_len / 2;
                    unsigned char *temp_b = realloc(list[i].bytes, list[i].size + add_len + 1);
                    if (temp_b) {
                        list[i].bytes = temp_b;
                        for (size_t k = 0; k < add_len; k++) {
                            unsigned int val = 0;
                            sscanf(hex + 2 * k, "%2x", &val);
                            list[i].bytes[list[i].size + k] = (unsigned char)val;
                        }
                        list[i].size += add_len;
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "SLICE") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *off_s = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *len_s = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            int offset = atoi(off_s);
            int length = atoi(len_s);
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, name) == 0) {
                    if (offset >= 0 && length >= 0 && (size_t)offset < list[i].size) {
                        size_t end = (size_t)offset + (size_t)length;
                        if (end > list[i].size) {
                            end = list[i].size;
                        }
                        for (size_t k = (size_t)offset; k < end; k++) {
                            printf("%02x", list[i].bytes[k]);
                        }
                        printf("\n");
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, name) == 0) {
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
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].name);
        free(list[i].bytes);
    }
    free(list);
    free(line);
    return 0;
}
