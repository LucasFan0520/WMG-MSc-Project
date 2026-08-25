// F037.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *name;
    unsigned char *bytes;
    int size;
} Buffer;

int hex_to_bytes(char *hex, unsigned char **res) {
    int len = (int)strlen(hex);
    int count = len / 2;
    *res = malloc(count + 1);
    if (!*res) return 0;
    for (int i = 0; i < count; i++) {
        unsigned int val;
        sscanf(hex + 2 * i, "%2x", &val);
        (*res)[i] = (unsigned char)val;
    }
    return count;
}

int main() {
    Buffer *buffers = NULL;
    int count = 0;
    int cap = 0;
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
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %d\n", buffers[i].name, buffers[i].size);
            }
            free(line);
            continue;
        }
        while (*p == ' ') p++;
        if (strcmp(cmd, "NEW") == 0) {
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *hex = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            unsigned char *res = NULL;
            int sz = hex_to_bytes(hex, &res);
            if (count >= cap) {
                cap = cap == 0 ? 16 : cap * 2;
                Buffer *nb = realloc(buffers, cap * sizeof(Buffer));
                if (nb) buffers = nb;
            }
            buffers[count].name = strdup(name);
            buffers[count].bytes = res;
            buffers[count].size = sz;
            count++;
        } else if (strcmp(cmd, "APPEND") == 0) {
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *hex = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            unsigned char *res = NULL;
            int sz = hex_to_bytes(hex, &res);
            for (int i = 0; i < count; i++) {
                if (strcmp(buffers[i].name, name) == 0) {
                    unsigned char *nb = realloc(buffers[i].bytes, buffers[i].size + sz);
                    if (nb) {
                        buffers[i].bytes = nb;
                        memcpy(buffers[i].bytes + buffers[i].size, res, sz);
                        buffers[i].size += sz;
                    }
                    break;
                }
            }
            free(res);
        } else if (strcmp(cmd, "SLICE") == 0) {
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *off_str = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *len_str = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            int offset = atoi(off_str);
            int length = atoi(len_str);
            for (int i = 0; i < count; i++) {
                if (strcmp(buffers[i].name, name) == 0) {
                    if (offset >= 0 && offset < buffers[i].size && length > 0) {
                        int actual_len = length;
                        if (offset + length > buffers[i].size) {
                            actual_len = buffers[i].size - offset;
                        }
                        for (int j = 0; j < actual_len; j++) {
                            printf("%02x", buffers[i].bytes[offset + j]);
                        }
                        printf("\n");
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
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
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(buffers[i].name);
        free(buffers[i].bytes);
    }
    free(buffers);
    return 0;
}
