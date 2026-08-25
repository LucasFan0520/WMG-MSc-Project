// F037.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    unsigned char *bytes;
    int size;
} ByteBuffer;

int hex_to_bytes(const char *hex, unsigned char **res) {
    int len = strlen(hex);
    int count = len / 2;
    unsigned char *buf = malloc(count + 1);
    if (!buf) return 0;
    for (int i = 0; i < count; i++) {
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
    return count;
}

int main(void) {
    ByteBuffer *buffers = NULL;
    int count = 0;
    char *line = NULL;
    size_t cap = 0;
    while (getline(&line, &cap, stdin) != -1) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "NEW ", 4) == 0) {
            char name[256], hex[4096];
            if (sscanf(line + 4, "%255s %4095s", name, hex) == 2) {
                unsigned char *res = NULL;
                int sz = hex_to_bytes(hex, &res);
                if (sz >= 0) {
                    ByteBuffer *tmp = realloc(buffers, (count + 1) * sizeof(ByteBuffer));
                    if (tmp) {
                        buffers = tmp;
                        buffers[count].name = strdup(name);
                        buffers[count].bytes = res;
                        buffers[count].size = sz;
                        count++;
                    } else {
                        free(res);
                    }
                }
            }
        } else if (strncmp(line, "APPEND ", 7) == 0) {
            char name[256], hex[4096];
            if (sscanf(line + 7, "%255s %4095s", name, hex) == 2) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(buffers[i].name, name) == 0) {
                        unsigned char *res = NULL;
                        int sz = hex_to_bytes(hex, &res);
                        if (sz > 0) {
                            unsigned char *nb = realloc(buffers[i].bytes, buffers[i].size + sz);
                            if (nb) {
                                buffers[i].bytes = nb;
                                memcpy(buffers[i].bytes + buffers[i].size, res, sz);
                                buffers[i].size += sz;
                            }
                            free(res);
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "SLICE ", 6) == 0) {
            char name[256];
            int offset, length;
            if (sscanf(line + 6, "%255s %d %d", name, &offset, &length) == 3) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(buffers[i].name, name) == 0) {
                        if (offset >= 0 && length >= 0 && offset + length <= buffers[i].size) {
                            for (int j = 0; j < length; j++) {
                                printf("%02x", buffers[i].bytes[offset + j]);
                            }
                            printf("\n");
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            while (*name == ' ') name++;
            for (int i = 0; i < count; i++) {
                if (strcmp(buffers[i].name, name) == 0) {
                    free(buffers[i].name);
                    free(buffers[i].bytes);
                    for (int j = i; j < count - 1; j++) {
                        buffers[j] = buffers[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %d\n", buffers[i].name, buffers[i].size);
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
