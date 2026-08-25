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

unsigned char *parse_hex(const char *hex, size_t *out_len) {
    size_t len = strlen(hex);
    size_t count = len / 2;
    unsigned char *res = malloc(count == 0 ? 1 : count);
    if (!res) return NULL;
    for (size_t i = 0; i < count; i++) {
        unsigned int val;
        char tmp[3];
        tmp[0] = hex[i * 2];
        tmp[1] = hex[i * 2 + 1];
        tmp[2] = '\0';
        if (sscanf(tmp, "%x", &val) != 1) {
            free(res);
            return NULL;
        }
        res[i] = (unsigned char)val;
    }
    *out_len = count;
    return res;
}

int main(void) {
    ByteBuffer *buffers = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "NEW ", 4) == 0) {
            char *args = line + 4;
            char *space = strchr(args, ' ');
            if (space) {
                *space = '\0';
                char *name = args;
                char *hex = space + 1;
                size_t b_len = 0;
                unsigned char *b_data = parse_hex(hex, &b_len);
                if (b_data) {
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        ByteBuffer *next = realloc(buffers, capacity * sizeof(ByteBuffer));
                        if (!next) {
                            free(b_data);
                            break;
                        }
                        buffers = next;
                    }
                    buffers[count].name = strdup(name);
                    buffers[count].bytes = b_data;
                    buffers[count].size = b_len;
                    count++;
                }
            }
        } else if (strncmp(line, "APPEND ", 7) == 0) {
            char *args = line + 7;
            char *space = strchr(args, ' ');
            if (space) {
                *space = '\0';
                char *name = args;
                char *hex = space + 1;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(buffers[i].name, name) == 0) {
                        size_t b_len = 0;
                        unsigned char *b_data = parse_hex(hex, &b_len);
                        if (b_data) {
                            unsigned char *nxt = realloc(buffers[i].bytes, buffers[i].size + b_len);
                            if (nxt) {
                                buffers[i].bytes = nxt;
                                memcpy(buffers[i].bytes + buffers[i].size, b_data, b_len);
                                buffers[i].size += b_len;
                            }
                            free(b_data);
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "SLICE ", 6) == 0) {
            char *args = line + 6;
            char *sp1 = strchr(args, ' ');
            if (sp1) {
                *sp1 = '\0';
                char *name = args;
                char *rest = sp1 + 1;
                char *sp2 = strchr(rest, ' ');
                if (sp2) {
                    *sp2 = '\0';
                    int offset = atoi(rest);
                    int length = atoi(sp2 + 1);
                    for (size_t i = 0; i < count; i++) {
                        if (strcmp(buffers[i].name, name) == 0) {
                            if (offset >= 0 && length >= 0 && (size_t)offset < buffers[i].size) {
                                size_t limit = (size_t)offset + (size_t)length;
                                if (limit > buffers[i].size) limit = buffers[i].size;
                                for (size_t k = (size_t)offset; k < limit; k++) {
                                    printf("%02x", buffers[i].bytes[k]);
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
        } else if (strcmp(line, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %zu\n", buffers[i].name, buffers[i].size);
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(buffers[i].name);
        free(buffers[i].bytes);
    }
    free(buffers);
    free(line);
    return 0;
}
