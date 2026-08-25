/* F037.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    unsigned char *bytes;
    int size;
} HexBuffer;

int parse_hex(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

int main(void) {
    HexBuffer *buffers = NULL;
    int count = 0;
    int capacity = 0;
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
            if (!space) continue;
            *space = '\0';
            char *name = args;
            char *hex = space + 1;
            int hex_len = strlen(hex);
            if (hex_len % 2 != 0) { *space = ' '; continue; }
            int b_size = hex_len / 2;
            unsigned char *b_bytes = malloc(b_size > 0 ? b_size : 1);
            char *n_name = strdup(name);
            *space = ' ';
            if (b_bytes && n_name) {
                int ok = 1;
                for (int i = 0; i < b_size; i++) {
                    int h1 = parse_hex(hex[2 * i]);
                    int h2 = parse_hex(hex[2 * i + 1]);
                    if (h1 == -1 || h2 == -1) { ok = 0; break; }
                    b_bytes[i] = (h1 << 4) | h2;
                }
                if (ok) {
                    if (count >= capacity) {
                        int new_cap = capacity == 0 ? 4 : capacity * 2;
                        HexBuffer *tmp = realloc(buffers, new_cap * sizeof(HexBuffer));
                        if (tmp) {
                            buffers = tmp;
                            capacity = new_cap;
                        } else {
                            free(n_name); free(b_bytes);
                            continue;
                        }
                    }
                    buffers[count].name = n_name;
                    buffers[count].bytes = b_bytes;
                    buffers[count].size = b_size;
                    count++;
                } else {
                    free(n_name); free(b_bytes);
                }
            } else {
                free(n_name); free(b_bytes);
            }
        } else if (strncmp(line, "APPEND ", 7) == 0) {
            char *args = line + 7;
            char *space = strchr(args, ' ');
            if (!space) continue;
            *space = '\0';
            char *name = args;
            char *hex = space + 1;
            int hex_len = strlen(hex);
            if (hex_len % 2 != 0) { *space = ' '; continue; }
            int add_size = hex_len / 2;
            for (int i = 0; i < count; i++) {
                if (strcmp(buffers[i].name, name) == 0) {
                    if (add_size > 0) {
                        unsigned char *new_b = realloc(buffers[i].bytes, buffers[i].size + add_size);
                        if (new_b) {
                            buffers[i].bytes = new_b;
                            int ok = 1;
                            for (int j = 0; j < add_size; j++) {
                                int h1 = parse_hex(hex[2 * j]);
                                int h2 = parse_hex(hex[2 * j + 1]);
                                if (h1 == -1 || h2 == -1) { ok = 0; break; }
                                buffers[i].bytes[buffers[i].size + j] = (h1 << 4) | h2;
                            }
                            if (ok) {
                                buffers[i].size += add_size;
                            }
                        }
                    }
                    break;
                }
            }
            *space = ' ';
        } else if (strncmp(line, "SLICE ", 6) == 0) {
            char *args = line + 6;
            char *p1 = strchr(args, ' ');
            if (!p1) continue;
            *p1 = '\0';
            char *name = args;
            char *rest = p1 + 1;
            char *p2 = strchr(rest, ' ');
            if (!p2) { *p1 = ' '; continue; }
            *p2 = '\0';
            int offset = atoi(rest);
            int slen = atoi(p2 + 1);
            for (int i = 0; i < count; i++) {
                if (strcmp(buffers[i].name, name) == 0) {
                    if (offset >= 0 && slen >= 0 && offset + slen <= buffers[i].size) {
                        for (int j = 0; j < slen; j++) {
                            printf("%02X", buffers[i].bytes[offset + j]);
                        }
                        printf("\n");
                    }
                    break;
                }
            }
            *p1 = ' '; *p2 = ' ';
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
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
