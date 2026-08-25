// F037.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    unsigned char *bytes;
    int size;
} ByteBuffer;

char *read_line(FILE *f) {
    size_t size = 32;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
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
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

int main() {
    ByteBuffer *list = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *p1 = strchr(line, ' ');
        if (!p1) {
            if (strcmp(line, "LIST") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s %d\n", list[i].name, list[i].size);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *cmd = line;
        char *rest = p1 + 1;
        if (strcmp(cmd, "NEW") == 0 || strcmp(cmd, "APPEND") == 0) {
            char *p2 = strchr(rest, ' ');
            if (p2) {
                *p2 = '\0';
                char *name = rest;
                char *hexbytes = p2 + 1;
                int hex_len = (int)strlen(hexbytes);
                int num_bytes = hex_len / 2;
                unsigned char *new_b = malloc(num_bytes);
                for (int i = 0; i < num_bytes; i++) {
                    char hex[3] = { hexbytes[2 * i], hexbytes[2 * i + 1], '\0' };
                    new_b[i] = (unsigned char)strtol(hex, NULL, 16);
                }
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(list[i].name, name) == 0) {
                        found = i;
                        break;
                    }
                }
                if (strcmp(cmd, "NEW") == 0) {
                    if (found != -1) {
                        free(list[found].bytes);
                        list[found].bytes = new_b;
                        list[found].size = num_bytes;
                    } else {
                        list = realloc(list, (count + 1) * sizeof(ByteBuffer));
                        list[count].name = strdup(name);
                        list[count].bytes = new_b;
                        list[count].size = num_bytes;
                        count++;
                    }
                } else {
                    if (found != -1) {
                        int old_sz = list[found].size;
                        list[found].bytes = realloc(list[found].bytes, old_sz + num_bytes);
                        memcpy(list[found].bytes + old_sz, new_b, num_bytes);
                        list[found].size += num_bytes;
                        free(new_b);
                    } else {
                        free(new_b);
                    }
                }
            }
        } else if (strcmp(cmd, "SLICE") == 0) {
            char *p2 = strchr(rest, ' ');
            if (p2) {
                *p2 = '\0';
                char *name = rest;
                char *rest2 = p2 + 1;
                char *p3 = strchr(rest2, ' ');
                if (p3) {
                    *p3 = '\0';
                    int offset = atoi(rest2);
                    int length = atoi(p3 + 1);
                    int found = -1;
                    for (int i = 0; i < count; i++) {
                        if (strcmp(list[i].name, name) == 0) {
                            found = i;
                            break;
                        }
                    }
                    if (found != -1) {
                        if (offset >= 0 && length > 0 && offset + length <= list[found].size) {
                            for (int i = 0; i < length; i++) {
                                printf("%02X", list[found].bytes[offset + i]);
                            }
                            printf("\n");
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = rest;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].name, name) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(list[found].name);
                free(list[found].bytes);
                for (int i = found; i < count - 1; i++) {
                    list[i] = list[i + 1];
                }
                count--;
                if (count == 0) {
                    free(list);
                    list = NULL;
                } else {
                    list = realloc(list, count * sizeof(ByteBuffer));
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(list[i].name);
        free(list[i].bytes);
    }
    free(list);
    return 0;
}