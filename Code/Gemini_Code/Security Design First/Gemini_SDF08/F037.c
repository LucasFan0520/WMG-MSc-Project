// F037.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    unsigned char *bytes;
    int size;
} Buffer;

int hex_val(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

unsigned char *parse_hex(const char *hex, int *out_len) {
    int len = strlen(hex);
    int count = len / 2;
    if (count <= 0) { *out_len = 0; return NULL; }
    unsigned char *buf = malloc(count);
    if (!buf) { *out_len = 0; return NULL; }
    for (int i = 0; i < count; i++) {
        int h1 = hex_val(hex[2 * i]);
        int h2 = hex_val(hex[2 * i + 1]);
        if (h1 == -1 || h2 == -1) {
            free(buf);
            *out_len = 0;
            return NULL;
        }
        buf[i] = (unsigned char)((h1 << 4) | h2);
    }
    *out_len = count;
    return buf;
}

int main(void) {
    Buffer *buffers = NULL;
    int count = 0;
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
        if (strncmp(line, "NEW ", 4) == 0) {
            char *p = line + 4;
            char *space = strchr(p, ' ');
            if (space) {
                *space = '\0';
                char *name = p;
                char *hex = space + 1;
                int b_len = 0;
                unsigned char *b_arr = parse_hex(hex, &b_len);
                if (b_arr || b_len == 0) {
                    Buffer *tmp = realloc(buffers, sizeof(Buffer) * (count + 1));
                    if (tmp) {
                        buffers = tmp;
                        buffers[count].name = strdup(name);
                        buffers[count].bytes = b_arr;
                        buffers[count].size = b_len;
                        if (buffers[count].name) {
                            count++;
                        } else {
                            free(b_arr);
                        }
                    } else {
                        free(b_arr);
                    }
                }
            }
        } else if (strncmp(line, "APPEND ", 7) == 0) {
            char *p = line + 7;
            char *space = strchr(p, ' ');
            if (space) {
                *space = '\0';
                char *name = p;
                char *hex = space + 1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(buffers[i].name, name) == 0) {
                        int b_len = 0;
                        unsigned char *b_arr = parse_hex(hex, &b_len);
                        if (b_arr && b_len > 0) {
                            unsigned char *nt = realloc(buffers[i].bytes, buffers[i].size + b_len);
                            if (nt) {
                                buffers[i].bytes = nt;
                                memcpy(buffers[i].bytes + buffers[i].size, b_arr, b_len);
                                buffers[i].size += b_len;
                            }
                            free(b_arr);
                        }
                        break;
                    }
                }
            }
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
                    char *off_str = p2;
                    char *len_str = s2 + 1;
                    long offset = strtol(off_str, NULL, 10);
                    long length = strtol(len_str, NULL, 10);
                    for (int i = 0; i < count; i++) {
                        if (strcmp(buffers[i].name, name) == 0) {
                            if (offset >= 0 && length > 0 && offset + length <= buffers[i].size) {
                                for (long j = 0; j < length; j++) {
                                    printf("%02X", buffers[i].bytes[offset + j]);
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
            int idx = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(buffers[i].name, name) == 0) {
                    idx = i;
                    break;
                }
            }
            if (idx != -1) {
                free(buffers[idx].name);
                free(buffers[idx].bytes);
                for (int i = idx; i < count - 1; i++) {
                    buffers[i] = buffers[i + 1];
                }
                count--;
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %d\n", buffers[i].name, buffers[i].size);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(buffers[i].name);
        free(buffers[i].bytes);
    }
    free(buffers);
    free(line);
    return 0;
}
