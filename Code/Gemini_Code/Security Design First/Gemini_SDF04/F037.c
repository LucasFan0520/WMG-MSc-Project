// F037.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    unsigned char *bytes;
    int size;
} ByteBuffer;

ByteBuffer *buffers = NULL;
int count = 0;
int capacity = 0;

int find_buf(const char *name) {
    for (int i = 0; i < count; i++) {
        if (strcmp(buffers[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int hex_val(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

unsigned char *parse_hex(const char *hex, int *out_len) {
    int len = strlen(hex);
    if (len % 2 != 0) return NULL;
    int bcount = len / 2;
    unsigned char *res = malloc(bcount == 0 ? 1 : bcount);
    if (!res) return NULL;
    for (int i = 0; i < bcount; i++) {
        int v1 = hex_val(hex[i * 2]);
        int v2 = hex_val(hex[i * 2 + 1]);
        if (v1 < 0 || v2 < 0) {
            free(res);
            return NULL;
        }
        res[i] = (v1 << 4) | v2;
    }
    *out_len = bcount;
    return res;
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    while ((nread = getline(&line, &len, stdin)) != -1) {
        size_t l = strlen(line);
        if (l > 0 && line[l - 1] == '\n') {
            line[l - 1] = '\0';
        }
        char *cmd = line;
        char *space1 = strchr(cmd, ' ');
        if (space1) {
            *space1 = '\0';
            char *arg1 = space1 + 1;
            if (strcmp(cmd, "NEW") == 0) {
                char *space2 = strchr(arg1, ' ');
                if (space2) {
                    *space2 = '\0';
                    char *hex = space2 + 1;
                    int blen = 0;
                    unsigned char *b = parse_hex(hex, &blen);
                    if (b && find_buf(arg1) == -1) {
                        if (count >= capacity) {
                            capacity = capacity == 0 ? 4 : capacity * 2;
                            ByteBuffer *tmp = realloc(buffers, capacity * sizeof(ByteBuffer));
                            if (!tmp) {
                                free(b);
                                free(line);
                                return 1;
                            }
                            buffers = tmp;
                        }
                        buffers[count].name = strdup(arg1);
                        buffers[count].bytes = b;
                        buffers[count].size = blen;
                        if (buffers[count].name) {
                            count++;
                        } else {
                            free(b);
                        }
                    } else {
                        free(b);
                    }
                }
            } else if (strcmp(cmd, "APPEND") == 0) {
                char *space2 = strchr(arg1, ' ');
                if (space2) {
                    *space2 = '\0';
                    char *hex = space2 + 1;
                    int idx = find_buf(arg1);
                    if (idx != -1) {
                        int blen = 0;
                        unsigned char *b = parse_hex(hex, &blen);
                        if (b) {
                            int new_size = buffers[idx].size + blen;
                            unsigned char *nb = realloc(buffers[idx].bytes, new_size == 0 ? 1 : new_size);
                            if (nb) {
                                buffers[idx].bytes = nb;
                                memcpy(buffers[idx].bytes + buffers[idx].size, b, blen);
                                buffers[idx].size = new_size;
                            }
                            free(b);
                        }
                    }
                }
            } else if (strcmp(cmd, "SLICE") == 0) {
                char *space2 = strchr(arg1, ' ');
                if (space2) {
                    *space2 = '\0';
                    char *off_str = space2 + 1;
                    char *space3 = strchr(off_str, ' ');
                    if (space3) {
                        *space3 = '\0';
                        int offset = atoi(off_str);
                        int length = atoi(space3 + 1);
                        int idx = find_buf(arg1);
                        if (idx != -1) {
                            if (offset >= 0 && length >= 0 && offset + length <= buffers[idx].size) {
                                for (int i = 0; i < length; i++) {
                                    printf("%02X", buffers[idx].bytes[offset + i]);
                                }
                                printf("\n");
                            }
                        }
                    }
                }
            } else if (strcmp(cmd, "DELETE") == 0) {
                int idx = find_buf(arg1);
                if (idx != -1) {
                    free(buffers[idx].name);
                    free(buffers[idx].bytes);
                    for (int i = idx; i < count - 1; i++) {
                        buffers[i] = buffers[i + 1];
                    }
                    count--;
                }
            }
        } else {
            if (strcmp(cmd, "LIST") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s %d\n", buffers[i].name, buffers[i].size);
                }
            } else if (strcmp(cmd, "END") == 0) {
                break;
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(buffers[i].name);
        free(buffers[i].bytes);
    }
    free(buffers);
    return 0;
}