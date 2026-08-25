// F037.c
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

int find_buffer(const char *name) {
    for (int i = 0; i < count; i++) {
        if (strcmp(buffers[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int parse_hex(const char *hex, unsigned char *buf) {
    int len = 0;
    const char *p = hex;
    while (*p && *(p + 1)) {
        unsigned int b;
        if (sscanf(p, "%2x", &b) == 1) {
            buf[len++] = (unsigned char)b;
        }
        p += 2;
    }
    return len;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "NEW ", 4) == 0) {
            char *p1 = line + 4;
            char *sp = strchr(p1, ' ');
            if (sp) {
                *sp = 0;
                char *hex = sp + 1;
                int hex_len = strlen(hex) / 2;
                unsigned char *buf = malloc(hex_len + 1);
                int bytes_len = parse_hex(hex, buf);
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    buffers = realloc(buffers, capacity * sizeof(ByteBuffer));
                }
                buffers[count].name = strdup(p1);
                buffers[count].bytes = buf;
                buffers[count].size = bytes_len;
                count++;
            }
        } else if (strncmp(line, "APPEND ", 7) == 0) {
            char *p1 = line + 7;
            char *sp = strchr(p1, ' ');
            if (sp) {
                *sp = 0;
                char *hex = sp + 1;
                int idx = find_buffer(p1);
                if (idx != -1) {
                    int hex_len = strlen(hex) / 2;
                    unsigned char *buf = malloc(hex_len + 1);
                    int bytes_len = parse_hex(hex, buf);
                    buffers[idx].bytes = realloc(buffers[idx].bytes, buffers[idx].size + bytes_len);
                    memcpy(buffers[idx].bytes + buffers[idx].size, buf, bytes_len);
                    buffers[idx].size += bytes_len;
                    free(buf);
                }
            }
        } else if (strncmp(line, "SLICE ", 6) == 0) {
            char *p1 = line + 6;
            char *sp1 = strchr(p1, ' ');
            if (sp1) {
                *sp1 = 0;
                char *p2 = sp1 + 1;
                char *sp2 = strchr(p2, ' ');
                if (sp2) {
                    *sp2 = 0;
                    int offset = atoi(p1);
                    int length = atoi(p2);
                    int idx = find_buffer(sp2 + 1);
                    if (idx != -1) {
                        ByteBuffer *b = &buffers[idx];
                        if (offset >= 0 && offset < b->size) {
                            int end = offset + length;
                            if (end > b->size) {
                                end = b->size;
                            }
                            for (int i = offset; i < end; i++) {
                                printf("%02x", b->bytes[i]);
                            }
                            printf("\n");
                        }
                    }
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            int idx = find_buffer(name);
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
    return 0;
}
